/*
 * @Author: Qiong Li
 * @Date: 2024-12-01 14:38:27
 * @LastEditors: Qiong Li
 * @LastEditTime: 2024-12-01 15:19:09
 * @FilePath: \SoftPositE\source\gen_data\gen_posit_data_v2.c
 * @Description: 读取FP32数据并将其转化为Posit格式
 * @Reference: 
 * Copyright (c) 2024 by ICAIS Lab @ Nanjing University, All Rights Reserved. 
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <time.h>
#include <string.h>

#include "../SoftPositE.h"
#include "gen_posit_data_v2_stimuli.h"

// posit32_t convertDoubleToPosit(double f32, int n, int es);

// Custom Union Type
union FloatIntUnion{
    float floatValue;
    int32_t intValue;
};

// 读取fp32data并将其转化为Posit格式
void generate_and_write_to_file(const char *filename, int count, int n, int es) {
    // 打开文件以读取数据
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file %s!\n", filename);
        return 1;
    }

    union FloatIntUnion FP32_value[count];
    // 读取FP32 data
    int i;
    for(i=0; i<count; i++){
        FP32_value[i].intValue = Softmax_FP32_data[i];
    }

    // 读取FP32数据并转化为Posit格式，再将其写入文件
    for (i = 0; i < count; i++) {
        float normal_value = FP32_value[i].floatValue;
        double d = normal_value;
        posit32_t posit_value = convertDoubleToPosit(d, n, es);
        uint32_t p32 = posit_value.v;

        // 用于存储十六进制字符串
        char hex_str[11];
        if(n==32){
            sprintf(hex_str, "0x%08X", p32);
        }
        else if(n==16){
            sprintf(hex_str, "0x%04X", p32>>16);
        }
        else if(n==8){
            sprintf(hex_str, "0x%02X", p32>>24);
        }
        else{
            printf("Unsupported posit format!\n");
            return 0;
        }

        // fprintf(file, "%f %s\n", normal_value, hex_str);
        fprintf(file, "%s,\n", hex_str);
    }

    // 关闭文件
    fclose(file);
    printf("Generated %d normal values and wrote to %s\n", count, filename);
}

int main() {

    // 分别生成浮点数并保存到两个文件中
    generate_and_write_to_file("data/P16_0_Softmax.txt", 128, 16, 0);
    generate_and_write_to_file("data/P16_1_Softmax.txt", 128, 16, 1);
    generate_and_write_to_file("data/P16_2_Softmax.txt", 128, 16, 2);
    generate_and_write_to_file("data/P16_3_Softmax.txt", 128, 16, 3);

    generate_and_write_to_file("data/P8_0_Softmax.txt", 128, 8, 0);
    generate_and_write_to_file("data/P8_1_Softmax.txt", 128, 8, 1);
    generate_and_write_to_file("data/P8_2_Softmax.txt", 128, 8, 2);
    generate_and_write_to_file("data/P8_3_Softmax.txt", 128, 8, 3);

    return 0;
}