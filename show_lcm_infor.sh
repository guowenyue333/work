#!/bin/bash

name=$1
upper_name="${name^^}"
echo "$uppername"
#########获取内核版本########
#1.找到projectconfig.mk
project_config_file=$(find device/ -type f -name "ProjectConfig.mk" | grep -i "$upper_name" | head -n 1)

#2.获取内核版本
kernel_version=$(grep "LINUX_KERNEL_VERSION" "$project_config_file" | awk -F '=' '{print $2}' | tr -d '[:space:]')
#3.获取目录
project_dir_name=$(basename $(dirname "$project_config_file"))

#########获取路径##########
defconfig_file="$kernel_version/arch/arm/configs/${project_dir_name}_defconfig"

#########提取LCM配置########
lcm_config=$(grep "CONFIG_CUSTOM_KERNEL_LCM=" "$defconfig_file" | awk -F '"' '{print $2}')


########格式输出#########
for lcm in $lcm_config; do
    echo "LCM_NAME is: $lcm"
    IFS='_' read -r ic module lane resolution <<< "$lcm"
    echo "IC is : $ic"
    echo "Module : $module"
    echo "lane is: $lane"
    echo "resolution: $resolution"
    echo
done



 
