#include <linux/of.h>          
#include <linux/of_device.h>   
#include <linux/of_platform.h>
#include <linux/input.h>
#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>

#include "gpio_key.h"




/*ID匹配*/
static const struct platform_device_id gpio_key_id_table[] = {
    { "gpio_keys", 0 },  // 名称匹配
    { }
};
MODULE_DEVICE_TABLE(platform, gpio_key_id_table);


/*设备树匹配*/
static const struct of_device_id gpio_key_of_match[] = {
    { .compatible = "agn.gpio_key" },
    {}
};
MODULE_DEVICE_TABLE(of, gpio_key_of_match);

static int gpio_key_probe(struct platform_device *pdev)
{   
    int i,ret = 0;
    struct device_node *np;
    struct gpio_key_data *data;
    np = of_find_compatible_node(NULL, NULL, "agn.gpio_key");//获取设备树节点
    if(!np) {
        pr_err("Cannot find device tree code\n");
        return -ENODEV;
    }
    pdev = of_platform_device_create(np, DRV_NAME, NULL);//创建平台设备
    if(!pdev) {
        pr_err("Failed to create platform device\n");
        return -ENODEV;
    }
    data = devm_kzalloc(&pdev->dev, sizeof(*data),GFP_KERNEL);// 分配驱动数据结构空间
    if(!data) return -ENODEV;
    data->input = input_allocate_device(); // 初始化输入设备，分配一个新的结构体给input
    data->input->name = "GPIO Keys"; // 输入设备名称
    data->input->id.bustype = BUS_HOST; // 设备的总线类型
    set_bit(EV_KEY, data->input->evbit);
    set_bit(KEY_F1, data->input->keybit);
    set_bit(KEY_F2, data->input->keybit);
    ret = input_register_device(data->input);
    if(ret) {
        pr_err("Failed to register input device\n");
        input_free_device(data->input);
        return ret;
    }

    //配置GPIO和中断
    for(i = 0; i < MAX_KEYS; i++) {
        data->gpio[i] = devm_gpiod_get_index(&pdev->dev, "key-gpios", i, GPIOD_IN);// 获取GPIO描述符
        data->irq[i] = gpiod_to_irq(data->gpio[i]);// 把GPIO描述符转化为中断号存入数组
        ret = devm_request_irq(
            &pdev->dev, 
            data->irq[i], 
            gpio_key_isr, 
            IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING, 
            devm_kasprintf(&pdev->dev, GFP_KERNEL, "gpio_key%d", i), data);// 中断注册函数 
        if(ret) {
            dev_err(&pdev->dev, "Failed to request IRQ %d: %d\n", data->irq[i], ret);
            return ret;
        }
    }
    platform_set_drvdata(pdev, data); // 关联驱动程序数据与平台设备
    return 0;
}

static irqreturn_t gpio_key_isr(int irq, void *dev_id)
{
    struct gpio_key_data *data = dev_id; // 获得驱动数据
    int i,val;
    for(i = 0; i < MAX_KEYS; i++) {
        if(irq == data->irq[i]) {
            val = gpiod_get_value(data->gpio[i]); // 当中断服务号等于IO口的中断服务号 读取该io口的值
            input_report_key(data->input, KEY_F1 + i, !val); // 低电平有效!val
            input_sync(data->input);
            break;
        }
    }
    return IRQ_HANDLED; // 中断已被处理
}
static int gpio_key_remove(struct platform_device *pdev)
{   
    struct gpio_key_data *data;
    data = platform_get_drvdata(pdev);
        /* 检查驱动数据是否存在 */
    if (!data) {
        dev_warn(&pdev->dev, "No driver data found\n");
        return -ENOMEM;
    }   
    platform_set_drvdata(pdev, data);
    if(data && data->input) {
        input_unregister_device(data->input); //删除注册的输入设备
        input_free_device(data->input);  //释放输入设备
    }
    for(int i = 0; i < MAX_KEYS; i++) {
        if(data->irq[i] > 0) {
            devm_free_irq(&pdev->dev,data->irq[i], data);          //释放中断

        }
        if(data->gpio[i]) {
            devm_gpiod_put(&pdev->dev, data->gpio[i]); //释放GPIO
            data->gpio[i] = NULL;
        }
    }
    pr_info("GPIO Key driver removed\n");
    return 0;
}

struct platform_driver gpio_key_driver = {
    .probe = gpio_key_probe,
    .remove = gpio_key_remove,
    .id_table = gpio_key_id_table,
    .driver = {
        .name = DRV_NAME,
        .of_match_table = gpio_key_of_match,
    },
};
MODULE_LICENSE("GPL");
module_platform_driver(gpio_key_driver);


