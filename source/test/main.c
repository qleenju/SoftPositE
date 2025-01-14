/*
 * @Author: Qiong Li
 * @Date: 2023-03-18 12:28:20
 * @LastEditors: Qiong Li
 * @LastEditTime: 2025-01-14 18:47:25
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
void test_posit_fma();

int main(){
    // test_posit_muladd();
    // test_convertPositToDouble();
    // test_posit_muladd_mixed();
    test_posit_fma();
    
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
    n = 16;
    es = 6;
    uiA = 0x7f5a << (32-n);
    uiB = 0x6864 << (32-n);
    uiC = 0xf701 << (32-n);
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

void test_posit_fma(){
    posit32_t pA, pB, pC, pZ;
    int n_i, es_i;
    int n_o, es_o;
    uint32_t uiA, uiB, uiC, uiZ;
    
    // test instance
    n_i = 6;
    es_i = 0;
    n_o = 16;
    es_o = 1;
    // pA.v = 0x02 << (32-n_i);
    // pB.v = 0x1f << (32-n_i);
    // pC.v = 0x361d << (32-n_o);
    pA.v = 0x1c << (32-n_i);
    pB.v = 0x2f << (32-n_i);
    pC.v = 0x3595 << (32-n_o);
    // convert to double
    double dA, dB, dC;
    dA = convertPositToDouble(pA, n_i, es_i);
    dB = convertPositToDouble(pB, n_i, es_i);
    dC = convertPositToDouble(pC, n_o, es_o);
    printf("dA = %.15f\n", dA);
    printf("dB = %.15f\n", dB);
    printf("dC = %.15f\n", dC);
    // double-based fma
    double dZ = dA*dB+dC;
    // convert to posit again
    pZ = convertDoubleToPosit(dZ, n_o, es_o);
    // valid output data
    uiZ = pZ.v >> (32-n_o);
    printf("uiZ = ");
    printBinary((uint64_t*)&uiZ, n_o);
}