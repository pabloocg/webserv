#include <stdint.h>

void
wsv_str_cpy_2(char* dst, const char* src)
{
	*((uint16_t*)dst) = *((uint16_t*)src);
}

void
wsv_str_cpy_3(char* dst, const char* src)
{
	*((uint16_t*)dst) = *((uint16_t*)src);
	dst[2] = src[2];
}

void
wsv_str_cpy_4(char* dst, const char* src)
{
	*((uint32_t*)dst) = *((uint32_t*)src);
}

void
wsv_str_cpy_5(char* dst, const char* src)
{
	*((uint32_t*)dst) = *((uint32_t*)src);
	dst[4] = src[4];
}

void
wsv_str_cpy_6(char* dst, const char* src)
{
	((uint32_t*)dst)[0] = ((uint32_t*)src)[0];
	((uint16_t*)dst)[2] = ((uint16_t*)src)[2];
}

void
wsv_str_cpy_8(char* dst, const char* src)
{
	((uint32_t*)dst)[0] = ((uint32_t*)src)[0];
	((uint32_t*)dst)[1] = ((uint32_t*)src)[1];
}

void
wsv_str_cpy_9(char* dst, const char* src)
{
	((uint32_t*)dst)[0] = ((uint32_t*)src)[0];
	((uint32_t*)dst)[1] = ((uint32_t*)src)[1];
	dst[8] = src[8];
}

void
wsv_str_cpy_10(char* dst, const char* src)
{
	((uint32_t*)dst)[0] = ((uint32_t*)src)[0];
	((uint32_t*)dst)[1] = ((uint32_t*)src)[1];
	((uint16_t*)dst)[4] = ((uint16_t*)src)[4];
}

void
wsv_str_cpy_12(char* dst, const char* src)
{
	((uint32_t*)dst)[0] = ((uint32_t*)src)[0];
	((uint32_t*)dst)[1] = ((uint32_t*)src)[1];
	((uint32_t*)dst)[2] = ((uint32_t*)src)[2];
}

void
wsv_str_cpy_14(char* dst, const char* src)
{
	((uint32_t*)dst)[0] = ((uint32_t*)src)[0];
	((uint32_t*)dst)[1] = ((uint32_t*)src)[1];
	((uint32_t*)dst)[2] = ((uint32_t*)src)[2];
	((uint16_t*)dst)[6] = ((uint16_t*)src)[6];
}

void
wsv_str_cpy_15(char* dst, const char* src)
{
	((uint32_t*)dst)[0] = ((uint32_t*)src)[0];
	((uint32_t*)dst)[1] = ((uint32_t*)src)[1];
	((uint32_t*)dst)[2] = ((uint32_t*)src)[2];
	((uint16_t*)dst)[6] = ((uint16_t*)src)[6];
	dst[14] = src[14];
}

void
wsv_str_cpy_16(char* dst, const char* src)
{
	((uint32_t*)dst)[0] = ((uint32_t*)src)[0];
	((uint32_t*)dst)[1] = ((uint32_t*)src)[1];
	((uint32_t*)dst)[2] = ((uint32_t*)src)[2];
	((uint32_t*)dst)[3] = ((uint32_t*)src)[3];
}

void
wsv_str_cpy_18(char* dst, const char* src)
{
	((uint32_t*)dst)[0] = ((uint32_t*)src)[0];
	((uint32_t*)dst)[1] = ((uint32_t*)src)[1];
	((uint32_t*)dst)[2] = ((uint32_t*)src)[2];
	((uint32_t*)dst)[3] = ((uint32_t*)src)[3];
	((uint16_t*)dst)[8] = ((uint16_t*)src)[8];
}

void
wsv_str_cpy_20(char* dst, const char* src)
{
	((uint32_t*)dst)[0] = ((uint32_t*)src)[0];
	((uint32_t*)dst)[1] = ((uint32_t*)src)[1];
	((uint32_t*)dst)[2] = ((uint32_t*)src)[2];
	((uint32_t*)dst)[3] = ((uint32_t*)src)[3];
	((uint32_t*)dst)[4] = ((uint32_t*)src)[4];
}

void
wsv_str_cpy_24(char* dst, const char* src)
{
	((uint32_t*)dst)[0] = ((uint32_t*)src)[0];
	((uint32_t*)dst)[1] = ((uint32_t*)src)[1];
	((uint32_t*)dst)[2] = ((uint32_t*)src)[2];
	((uint32_t*)dst)[3] = ((uint32_t*)src)[3];
	((uint32_t*)dst)[4] = ((uint32_t*)src)[4];
	((uint32_t*)dst)[5] = ((uint32_t*)src)[5];
}

void
wsv_str_cpy_27(char* dst, const char* src)
{
	((uint32_t*)dst)[0] = ((uint32_t*)src)[0];
	((uint32_t*)dst)[1] = ((uint32_t*)src)[1];
	((uint32_t*)dst)[2] = ((uint32_t*)src)[2];
	((uint32_t*)dst)[3] = ((uint32_t*)src)[3];
	((uint32_t*)dst)[4] = ((uint32_t*)src)[4];
	((uint32_t*)dst)[5] = ((uint32_t*)src)[5];
	((uint16_t*)dst)[12] = ((uint16_t*)src)[12];
	dst[26] = src[26];
}

void
wsv_str_cpy_29(char* dst, const char* src)
{
	((uint32_t*)dst)[0] = ((uint32_t*)src)[0];
	((uint32_t*)dst)[1] = ((uint32_t*)src)[1];
	((uint32_t*)dst)[2] = ((uint32_t*)src)[2];
	((uint32_t*)dst)[3] = ((uint32_t*)src)[3];
	((uint32_t*)dst)[4] = ((uint32_t*)src)[4];
	((uint32_t*)dst)[5] = ((uint32_t*)src)[5];
	((uint32_t*)dst)[6] = ((uint32_t*)src)[6];
	dst[28] = src[28];
}

void
wsv_str_cpy_52(char* dst, const char* src)
{
	((uint32_t*)dst)[0] = ((uint32_t*)src)[0];
	((uint32_t*)dst)[1] = ((uint32_t*)src)[1];
	((uint32_t*)dst)[2] = ((uint32_t*)src)[2];
	((uint32_t*)dst)[3] = ((uint32_t*)src)[3];
	((uint32_t*)dst)[4] = ((uint32_t*)src)[4];
	((uint32_t*)dst)[5] = ((uint32_t*)src)[5];
	((uint32_t*)dst)[6] = ((uint32_t*)src)[6];
	((uint32_t*)dst)[7] = ((uint32_t*)src)[7];
	((uint32_t*)dst)[8] = ((uint32_t*)src)[8];
	((uint32_t*)dst)[9] = ((uint32_t*)src)[9];
	((uint32_t*)dst)[10] = ((uint32_t*)src)[10];
	((uint32_t*)dst)[11] = ((uint32_t*)src)[11];
	((uint32_t*)dst)[12] = ((uint32_t*)src)[12];
}
