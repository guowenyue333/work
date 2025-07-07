#!/bin/bash

Dir=(
"kernel-4.19/drivers/misc/mediatek/imgsensor/src/common/v1"
"vendor/mediatek/proprietary/custom/mt6739/hal/imgsensor"
"vendor/mediatek/proprietary/custom/mt6739/hal/imgsensor_metadata"
)

camera_model=$1
uppercamera_model="${camera_model^^}"
camera_new_model=$1"s"
upper_new_model=$uppercamera_model"S"
echo $camera_new_model
echo $upper_new_model

#############拷贝文件#############
#######有其他尾缀文件加if语句#####
for dir in "${Dir[@]}"; do
    if [ -d "$dir/${camera_model}_mipi_raw" ]; then
        cp -r "$dir/${camera_model}_mipi_raw" "$dir/${camera_new_model}_mipi_raw"
    fi
    if [ -d "$dir/${camera_model}_mipi_mono" ]; then
        cp -r "$dir/${camera_model}_mipi_mono" "$dir/${camera_new_model}_mipi_mono"
    fi
#############修改文件名###########
    if [ -d "$dir/${camera_new_model}_mipi_raw" ]; then
        find "$dir/${camera_new_model}_mipi_raw" -type f -name "*${camera_model}*" | while read -r file; do
        new_file=$(echo "$file" | sed "s/\(.*\)${camera_model}\(.*\)/\1${camera_new_model}\2/")
        mv "$file" "$new_file"
        done
#########修改文件内容#############
        find "$dir/${camera_new_model}_mipi_raw" -type f \( -name "*.c" -o -name "*.h" -o -name "Makefile" \) | while read -r file; do
        sudo sed -i "s/${camera_model}/${camera_new_model}/g" "$file"
        sudo sed -i "s/${uppercamera_model}/${upper_new_model}/g" "$file"
        done
    fi
done
