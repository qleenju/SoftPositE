/*
 * @Author: Qiong Li
 * @Date: 2023-04-16 14:05:18
 * @LastEditors: Qiong Li
 * @LastEditTime: 2023-04-19 20:41:41
 * @FilePath: \SoftPositE\source\posit_muladd_mixed.c
 * @Description: Posit-based FMA function implemented by C, supporting mixed-precision strategy
 *      - Support arbitrary posit formats, i.e., posit(n, es)
 *          - when n<32, only the MSB n-bits are valid, while remaining bits will be 0
 *      - Mixed-precision Strategy: (n_i, es_i) for uiA & uiB, (n_o, es_o) for uiC & result_o
 * @Reference: revised from posit_muladd.c
 * Copyright (c) 2023 by Qiong Li qleenju@163.com, All Rights Reserved. 
 */

#include "SoftPositE.h"

uint32_t
posit_muladd_mixed(
	uint_fast32_t uiA, uint_fast32_t uiB, uint_fast32_t uiC, uint_fast32_t op, 
    int n_i, int es_i, int n_o, int es_o) {

	uint32_t uZ;
	int regZ;
	uint_fast32_t fracA, fracB, fracZ, regime, tmp;
	bool signA, signB, signC, signZ, regSA, regSB, regSC, regSZ, bitNPlusOne = 0, bitsMore = 0, rcarry;
	int_fast32_t expA, expC, expZ;
	int_fast16_t kA = 0, kC = 0, kZ = 0, shiftRight;
	uint_fast64_t frac64C, frac64Z;
	// valid exponent of product of A & B
	int_fast32_t rg_exp_AB;
	// the maximum value that can be represented by es-bit exponent field
	// Note: subtraction has higher priority than left shift
	int exp_max_i = (0x00000001<<es_i) - 1;
    int exp_max_o = (0x00000001<<es_o) - 1;
	int count;

	// Ensure 2<=n<=32
	if (n_i<2 || n_i>32 || n_o<2 || n_o>32) {
		uZ = 0x80000000;
		return uZ;
	}

	// Set the LSB (32-n)-bits 0
	uiA = uiA & (0xffffffff << (32 - n_i));
	uiB = uiB & (0xffffffff << (32 - n_i));
    // n_o for uiC, instread of n_i
	uiC = uiC & (0xffffffff << (32 - n_o));

	// NaR Handing
	if (uiA == 0x80000000 || uiB == 0x80000000 || uiC == 0x80000000) {
		uZ = 0x80000000;
		return uZ;
	}
	else if (uiA == 0 || uiB == 0) {
		if (op == softposit_mulAdd_subC)
			uZ = ~uiC + 1;
		else
			uZ = uiC;
		return uZ;
	}

	// op handling
	if (op == softposit_mulAdd_subC)
		uiC = ~uiC + 1;
	else if (op == softposit_mulAdd_subProd)
		uiA = ~uiA + 1;
	else;

	// Sign of operand
	signA = signP32UI(uiA);
	signB = signP32UI(uiB);
	signC = signP32UI(uiC);
	signZ = signA ^ signB;

	// Obtain absolute values, since posit uses two's complement for negative numbers
	if (signA) uiA = ~uiA + 1;
	if (signB) uiB = ~uiB + 1;
	if (signC) uiC = ~uiC + 1;

	// MSB bit of regime field
	regSA = signregP32UI(uiA);
	regSB = signregP32UI(uiB);
	regSC = signregP32UI(uiC);

    // Actually there is no need to apply 2-bit posit format?
	if (n_i == 2) {
		// if regSA=1 && regSB=1, then |a|=|b|=1
		// if regSA=0 or regSB=0, then the value is 0 or NaN (return)
		uZ = (regSA & regSB) ? (0x40000000) : (0x0);
		// i.e., negative product
		if (signZ) {
			// both ab and c are negative, i.e., (-uiZ) + (-uiC) = -(uiZ + uiC)
			if (signC) {
				// 0x40.. + 0x40.. = 0x40.. (consider overflow)
				uZ |= uiC;
				// invert sign
				uZ = uZ ^ 0x80000000;
			}
			// ab is negative, and c is positive
			else {
				if (uiC == uZ) uZ = 0;
				else uZ = (uZ > 0) ? (0xC0000000) : (0x40000000);
			}
		}
		else {
			// ab is positive, and c is negative
			if (signC) {
				if (uiC == uZ)  uZ = 0;
				else uZ = (uZ > 0) ? (0x40000000) : (0xC0000000);
			}
			// both ab and c are positive, i.e., (+ uiZ) + (+uiC)
			else {
				uZ |= uiC;
			}
		}
		return uZ;
	}
	// n: [2, 32]
	else {
		// left shift Sign bit and MSB bit of regime field out
		tmp = (uiA << 2) & 0xFFFFFFFF;
		// introduce "count" to limit the shift amount to (n-2)
		count = 0;
		// regSA=1, means the regime field starts with consecutive 1
		if (regSA) {
			while (tmp >> 31 && count<(n_i-2)) {
				// the initial value of KA is 0
				kA++;
				tmp = (tmp << 1) & 0xFFFFFFFF;
				count++;
			}
		}
		else {
			kA = -1;
			while (!(tmp >> 31) && count<n_i-2) {
				kA--;
				tmp = (tmp << 1) & 0xFFFFFFFF;
				count++;
			}
			tmp &= 0x7FFFFFFF;
		}
		// The "tmp" obtained above is 32-bit, where the MSB bit is the the LSB-bit of regime field, and is forced to 0.
		// The exponent field start from the 2nd bit, with a bit-width of es
		expA = tmp >> (31-es_i);
		// The MSB bit of 32-bit fracA is implicit bit, and is forced to 1 (since effective zero has been handled in special cases)
		fracA = ((tmp << es_i) | 0x80000000) & 0xFFFFFFFF;

		// Handle regime field of uiB, identical to the process of uiA
		tmp = (uiB << 2) & 0xFFFFFFFF;
		count = 0;
		if (regSB) {
			while (tmp >> 31 && count<n_i-2) {
				kA++;
				tmp = (tmp << 1) & 0xFFFFFFFF;
				count++;
			}
		}
		else {
			kA--;
			while (!(tmp >> 31) && count<n_i-2) {
				kA--;
				tmp = (tmp << 1) & 0xFFFFFFFF;
				count++;
			}
			tmp &= 0x7FFFFFFF;
		}
		// addition of exponents of a, b
		expA += tmp >> (31-es_i);
		fracB = ((tmp << es_i) | 0x80000000) & 0xFFFFFFFF;
		// multiplication of mantissa of a, b (include implicit bit), to obtain 64-bit frac64Z
		frac64Z = (uint_fast64_t)fracA * fracB;

		// Mixed-Precision Strategy, (n_i, es_i) --> (n_o, es_o)
		rg_exp_AB = (kA<<es_i) + expA;
		kA = rg_exp_AB >> es_o;
		expA = rg_exp_AB & (~(0xFFFFFFFF<<es_o));

		rcarry = frac64Z >> 63;
		// 1-bit normalization
		if (rcarry) {
			expA++;
			if (expA > exp_max_o) {
				kA++;
				expA -= (exp_max_o+1);
			}
			// keep the decimal point at the 2nd bit, and now the MSB-bit of frac64Z is 0
			frac64Z >>= 1;
		}

		if (uiC != 0) {
			tmp = (uiC << 2) & 0xFFFFFFFF;
			count = 0;
			if (regSC) {
				while (tmp >> 31 && count<=n_o-2) {
					kC++;
					tmp = (tmp << 1) & 0xFFFFFFFF;
					count++;
				}
			}
			else {
				kC = -1;
				while (!(tmp >> 31) && count<=n_o-2) {
					kC--;
					tmp = (tmp << 1) & 0xFFFFFFFF;
					count++;
				}
				tmp &= 0x7FFFFFFF;
			}

			expC = tmp >> (31-es_o);
			// Also keep the decimal point of frac64C at the 2nd bit
			if (es_o > 0)
				frac64C = (((tmp << (es_o - 1)) | 0x40000000ULL) & 0x7FFFFFFFULL) << 32;
			else
				frac64C = (((tmp >> 1) | 0x40000000ULL) & 0x7FFFFFFFULL) << 32;
			
			// the exponent difference of ab and c, i.e., e_ab - e_c
			shiftRight = ((kA - kC) << es_o) + (expA - expC);

			// |uiC| > |prod|, right shift frac64Z
			if (shiftRight < 0) {
				if (shiftRight <= -63) {
					bitsMore = 1;
					frac64Z = 0;
				}
				else if ((frac64Z << (64 + shiftRight)) != 0)
					bitsMore = 1;

				if (signZ == signC)
					frac64Z = frac64C + (frac64Z >> -shiftRight);
				// different signs
				else {
					frac64Z = frac64C - (frac64Z >> -shiftRight);
					// since |uiC| > |prod|, set sign to signC
					signZ = signC;
					if (bitsMore)
						frac64Z -= 1;
				}
				kZ = kC;
				expZ = expC;
			}
			// |uiC| < |Prod|
			else if (shiftRight > 0) {
				if (shiftRight >= 63) {
					bitsMore = 1;
					frac64C = 0;
				}
				else if ((frac64C << (64 - shiftRight)) != 0)
					bitsMore = 1;
				
				if (signZ == signC)
					frac64Z = frac64Z + (frac64C >> shiftRight);
				else {
					frac64Z = frac64Z - (frac64C >> shiftRight);
					if (bitsMore)
						frac64Z -= 1;
				}
				kZ = kA;
				expZ = expA;
			}
			// shiftRight=0, i.e., the exponents of ab and c are equivalent
			else {
				// Check if the same number
				if (frac64C == frac64Z && signZ != signC) {
					uZ = 0;
					return uZ;
				}
				else {
					if (signZ == signC)
						frac64Z += frac64C;
					else {
						if (frac64Z < frac64C) {
							frac64Z = frac64C - frac64Z;
							signZ = signC;
						}
						else {
							frac64Z -= frac64C;
						}
					}
				}
				// actually can be kC too, no diff
				kZ = kA;
				// same here
				expZ = expA;
			}

			rcarry = (uint64_t)frac64Z >> 63;
			// 1-bit normalization
			if (rcarry) {
				expZ++;
				if (expZ > exp_max_o) {
					kZ++;
					expZ -= (exp_max_o+1);
				}
				if (frac64Z & 0x1)
					bitsMore = 1;
				frac64Z = (frac64Z >> 1) & 0x7FFFFFFFFFFFFFFF;
			}
			else {
				// for subtract cases, large normalization
				if (frac64Z != 0) {
					// e.g., es=1 --> exp_max=1 --> frac64Z>>61, leave the MSB 3-bits
					// Since the MSB bit has been 0, so if the remaining 2-bits are 0, it can be normalized
					
					// [NOTE][FIX] If exp_max_o > 62 (i.e., es_o>=6), it will fall into an INFITE LOOP!!!
					/*
					while ((frac64Z >> (62-exp_max_o)) == 0) {
						kZ--;
						frac64Z <<= (exp_max_o+1);
					}
					*/
					while ((frac64Z >> 62) == 0) {
						expZ--;
						frac64Z <<= 1;
						if (expZ < 0) {
							kZ--;
							expZ = exp_max_o;
						}
					}
				}

			}

		}
		// uiC==0
		else {
			kZ = kA;
			expZ = expA;
		}

		// compute regime field, according to kZ
		// regZ is the count of consecutive identical 1s or 0s of regime field
		// regSZ is the MSB bit of regime field
		if (kZ < 0) {
			regZ = -kZ;
			regSZ = 0;
			regime = 0x40000000 >> regZ;
		}
		else {
			regZ = kZ + 1;
			regSZ = 1;
			regime = 0x7FFFFFFF - (0x7FFFFFFF >> regZ);
		}

		if (regZ > (n_o - 2)) {
			// max or min posit number. exp and frac does not matter.
			uZ = (regSZ) ? (0x7FFFFFFF & ((int32_t)0x80000000 >> (n_o - 1))) : (0x1 << (32 - n_o));
		}
		else {
			// remove hidden bits
			frac64Z &= 0x3FFFFFFFFFFFFFFF;
			// the total bit-width of sign & regime & exponent is 1+(regZ+1) + es, i.e., (regZ+es+2)
			// Note: the MSB 2-bit of frac64Z has been set to 0
			// So, after the following shift, the MSB (regZ+es+2)-bits are forced to 0, to save sign & regime & exponent fields
			fracZ = frac64Z >> (regZ + es_o + 32);

			// The shifted bits all come from mantissa field
			if (regZ <= (n_o-es_o-2)) {
				bitNPlusOne = (((uint64_t)0x8000000000000000 >> (n_o - regZ - es_o)) & frac64Z);
				bitsMore |= (((uint64_t)0x7FFFFFFFFFFFFFFF >> (n_o - regZ - es_o)) & frac64Z);
				// set the LSB (32-n) bits to 0 (since the valid bit-width is n)
				fracZ &= ((int32_t)0x80000000 >> (n_o - 1));
				expZ <<= (30 - es_o - regZ);	
			}
			else {
				// (n-es-1) <= regZ <= (n-2), partially the shifted bits come from exponent field
				if (regZ - n_o + es_o + 1 > 0)
					bitNPlusOne = expZ & (0x00000001 << (regZ - n_o + es_o + 1));
				else
					bitNPlusOne = expZ & 0x1;
					
				if (regZ - n_o + es_o > 0)
					// bitsMore = expZ & (0x00000001 << (regZ - n + es));
					bitsMore |= expZ & ~(0xFFFFFFFF<<(regZ-n_o+es_o+1));
				else if (regZ - n_o + es_o == 0)
					bitsMore |= expZ & 0x1;
				if (frac64Z > 0) {
					fracZ = 0;
					bitsMore = 1;
				}

				expZ = expZ >> (es_o + 2 - n_o + regZ);
				expZ = expZ << (32 - n_o);
			}

			uZ = packToP32UI(regime, expZ, fracZ);

 			if (bitNPlusOne) {
				uZ += (uint32_t)(((uZ >> (32 - n_o)) & 1) | bitsMore) << (32 - n_o);
			}

		}

		// if negative, using two's complement
		if (signZ) uZ = ~uZ + 1;
		return uZ;
	}
}