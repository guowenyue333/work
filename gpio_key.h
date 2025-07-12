#ifndef GPIO_KEY_H
#define GPIO_KEY_H


#define MAX_KEYS 2
#define DRV_NAME "gpio_key"

static irqreturn_t gpio_key_isr(int irq, void *dev_id);
static int gpio_key_probe(struct platform_device *pdev);
static int gpio_key_remove(struct platform_device *pdev);

static const struct platform_device_id gpio_key_id_table[];
static const struct of_device_id gpio_key_of_match[];

/*平台驱动结构*/
struct gpio_key_data {
    struct gpio_desc *gpio[MAX_KEYS]; // gpio端口
    int irq[MAX_KEYS];   // 中断端口号
    struct input_dev *input;// 输入设备结构
    u8 init_stage;  // 添加初始化阶段标记
};




#endif