/*
 * @Author: Qiong Li
 * @Date: 2023-03-17 17:01:56
 * @LastEditors: Qiong Li
 * @LastEditTime: 2023-03-17 17:05:38
 * @FilePath: \posit_tensor_core\SoftPositE\SoftPositE.h
 * @Description: 
 * @Reference: 
 * Copyright (c) 2023 by Qiong Li qleenju@163.com, All Rights Reserved. 
 */
#include "include/platform.h"
#include "include/internals.h"

uint32_t posit_muladd(
	uint_fast32_t uiA, uint_fast32_t uiB, uint_fast32_t uiC, uint_fast32_t op, int n, int es);