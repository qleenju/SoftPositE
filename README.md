# SoftPositE
C-based Function Extension of SoftPosit Library

### About SoftPositE
**Extension of SoftPosit**, aiming to address the challenges of SoftPosit Library.

Currently, we have implemented the following extended functions. We are still working on..., according to our own requirements.

```
// posit-based fma unit, supporting arbitrary posit format
uint32_t posit_muladd(
	uint_fast32_t uiA, uint_fast32_t uiB, uint_fast32_t uiC, uint_fast32_t op, int n, int es);

// convert arbitrary posit format to double format
double convertPositToDouble(posit32_t pA, int n, int es);

// convert double format to arbitrary posit format
posit32_t convertDoubleToPosit(double f32, int n, int es);
```

### Appendix: About SoftPosit
[SoftPosit](https://gitlab.com/cerlane/SoftPosit) is a comprehensive C library for posits based on Berkeley [SoftFloat](https://github.com/ucb-bar/berkeley-softfloat-3), developed by NGA reseach team.

The library offers an array of posit-based operations, such as basic addition and multiplication, fused multiply-add, and format consersion. Additionally, it provides support for the quire format.

#### Features
The main functions provided are as follows (take posit16_t as an example):

##### Main Posit Functionalities
- Add: `posit16_t p16_add(posit16_t, posit16_t)`
- Subtract: `posit16_t p16_sub(posit16_t, posit16_t)`
- Divide: `posit16_t p16_div(posit16_t, posit16_t)`
- Multiply: `posit16_t p16_mul(posit16_t, posit16_t)`
- Fused Multiply Add: `posit16_t p16_mulAdd(posit16_t, posit16_t, posit16_t)`

##### Main Quire Functionalities
- Fused Dot Product-Add: `quire16_t q16_fdp_add(quire16_t, posit16_t, posit16_t)`
- Fused Dot Product-Subtract: `quire16_t q16_fdp_sub(quire16_t, posit16_t, posit16_t)`
- Set Quire Variable to Zero: `quire16_t q16_clr(quire16_t)`
- Convert Quire to Posit: `posit16_t q16_to_p16(quire16_t)`
  
##### Functionalities in Posit Standard
- Square Root: `posit16_t p16_sqrt(posit16_t)`
- Round to Nearest Integer: `posit16_t p16_roundToInt(posit16_t)`
- Check Equal: `bool p16_eq(posit16_t, posit16_t)`
- Check Less than Equal: `bool p16_le(posit16_t, posit16_t)`
- Check Less than: `bool p16_lt(posit16_t, posit16_t)`
- Convert Posit to Integer (32-bit): `int_fast32_t p16_to_i32(posit16_t)`
- Convert Posit to Long Long Integer (64-bit): `int_fast64_t p16_to_i64(posit16_t)`
- Convert Unsigned Integer (32-bit) to Posit: `posit16_t ui32_to_p16(uint32_t a)`
- Convert Unsigned Long Long Int (64-bit) to Posit: `posit16_t ui64_to_p16(uint64_t a)`
- Convert Integer (32-bit) to Posit: `posit16_t i32_to_p16(int32_t a)`
- Convert Long Integer (64 bits) to Posit: `posit16_t i64_to_p16(int64_t a)`
- Convert Posit to Unsigned Integer (32-bit): `uint_fast32_t p16_to_ui32(posit16_t)`
- Convert Posit to Unsigned Long Long Integer (64-bit): `uint_fast64_t p16_to_ui64(posit16_t)`
- Convert Posit to Posit of Another Size: `posit8_t p16_to_p8(posit16_t)`

##### Helper Functionalities (Not in Posit Standard)
- Convert Posit to Double (64-bit): `double convertP16ToDouble(posit16_t)`
- Convert Double (64-bit) to Posit: `posit16_t convertDoubleToP16(double)`
- Convert Binary Expressed in Unsigned Integer to Posit: `posit16_t castP16(uint16_t)`
- Cast Posit into Binary expressed in Unsigned Integer: `uint16_t castUI(posit16_t)`


#### Challenges
- **Limited Format Supports**: The functions listed above may only be available in a specific posit format, which can limit flexibility. The main supported formats are as follows:
  - `posit32_t`: 32-bit with two exponent bits (n=32, es=2)
  - `posit16_t`: 16-bit with one exponent bit (n=16, es=1)
  - `posit8_t`: 8-bit with zero exponent bit (n=8, es=0)
  - `posit_2_t`: 2-bit to 32-bit with two exponent bits (es=2)
  - `posit_1_t`: 2-bit to 32-bit with one exponent bit (es=1)
- **Limited Function Capabilities**: The function can be further extended, e.g., adding support for mixed-precision arithmetic.
- **Potential Bugs**: As mentioned in README, some functions have not been exhaustively tested, that is, there may be unknown bugs.