/*
 * @Author: Qiong Li
 * @Date: 2024-05-14 22:08:53
 * @LastEditors: Qiong Li
 * @LastEditTime: 2024-05-18 15:34:18
 * @FilePath: \SoftPositE\source\gen_data\gen_posit_data.c
 * @Description: 
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

// posit32_t convertDoubleToPosit(double f32, int n, int es);

// 生成(0, 1)正态分布的单精度浮点数
float generate_normal() {
    float u1 = ((float)rand() / RAND_MAX);
    float u2 = ((float)rand() / RAND_MAX);
    float z0 = sqrt(-2.0f * log(u1)) * cos(2.0f * M_PI * u2);
    return z0;
}

// 将生成的浮点数和十六进制表示写入文件
void generate_and_write_to_file(const char *filename, int count, int n, int es) {
    // 打开文件以写入
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file %s!\n", filename);
        return;
    }

    // 生成符合(0, 1)正态分布的单精度浮点数并写入文件
    for (int i = 0; i < count; i++) {
        float normal_value = generate_normal();
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
    // 设置随机数种子
    srand((unsigned int)time(NULL));

    // 分别生成浮点数并保存到两个文件中
    // generate_and_write_to_file("p16_gemm_a.txt", 1024, 16, 1);
    // generate_and_write_to_file("p16_gemm_b.txt", 1024, 16, 1);
    generate_and_write_to_file("p8_gemm_a.txt", 1024, 8, 0);
    generate_and_write_to_file("p8_gemm_b.txt", 1024, 8, 0);

    return 0;
}