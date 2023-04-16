/*
 * @Author: Qiong Li
 * @Date: 2023-03-18 12:28:20
 * @LastEditors: Qiong Li
 * @LastEditTime: 2023-04-16 16:20:59
 * @FilePath: \SoftPositE\source\test\main.c
 * @Description: 
 * @Reference: 
 * Copyright (c) 2023 by Qiong Li qleenju@163.com, All Rights Reserved. 
 */

#include <stdio.h>
#include "../SoftPositE.h"

void test_posit_muladd();
void test_convertPositToDouble();
void test_posit_muladd_mixed();

int main(){
    // test_posit_muladd();
    // test_convertPositToDouble();
    test_posit_muladd_mixed();
    
}

void test_posit_muladd_mixed(){
    uint32_t uiZ;
    uint_fast32_t uiA, uiB, uiC;
    uint_fast32_t op;
    int n_i, es_i;
    int n_o, es_o;

    // test instances
    n_i = 8;
    es_i = 3;
    n_o = 8;
    es_o = 3;
    uiA = 0x5a << (32-n_i);
    uiB = 0x64 << (32-n_i);
    uiC = 0x01 << (32-n_o);
    op = 0;
    
    uiZ = posit_muladd_mixed(uiA, uiB, uiC, op, n_i, es_i, n_o, es_o);
    uiZ = uiZ >> (32-n_o);
    printf("uiZ = ");
    printBinary((uint64_t*)&uiZ, n_o);
    
}

void test_posit_muladd(){
    uint32_t uiZ;
    uint_fast32_t uiA, uiB, uiC;
    uint_fast32_t op;
    int n;
    int es;

    // test instances
    n = 8;
    es = 3;
    uiA = 0x5a << (32-n);
    uiB = 0x64 << (32-n);
    uiC = 0x01 << (32-n);
    op = 0;
    
    uiZ = posit_muladd(uiA, uiB, uiC, op, n, es);
    uiZ = uiZ >> (32-n);
    printf("uiZ = ");
    printBinary((uint64_t*)&uiZ, n);

}

void test_convertPositToDouble(){
    posit32_t pA, pZ;
    int n, es;
    double f64;
    uint32_t uiA, uiZ;
    
    // test instance
    pA.v = 0xff121901;
    n = 16;
    es = 2;
    // valid input data
    uiA = pA.v >> (32-n);
    printf("uiA = ");
    printBinary((uint64_t*)&uiA, n);
    // convert to double
    f64 = convertPositToDouble(pA, n, es);
    printf("f64 = %.15f\n", f64);
    // convert to posit again
    pZ = convertDoubleToPosit(f64, n, es);
    // valid output data
    uiZ = pZ.v >> (32-n);
    printf("uiZ = ");
    printBinary((uint64_t*)&uiZ, n);
    
    if(uiA==uiZ){
        printf("Congratulations! Validation Successful!\n");
    }
    else{
        printf("Uh-oh, Validation Failed!\n");
    }
}