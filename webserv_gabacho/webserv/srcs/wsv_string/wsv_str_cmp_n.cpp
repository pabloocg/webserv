#include <stdint.h>

int
wsv_str_cmp_2(const char* s1, const char* s2)
{
	return (*(uint16_t*)s1 == *(uint16_t*)s2);
}

int
wsv_str_cmp_4(const char* s1, const char* s2)
{
	return (*(uint32_t*)s1 == *(uint32_t*)s2);
}

int
wsv_str_cmp_5(const char* s1, const char* s2)
{
	return (*(uint32_t*)s1 == *(uint32_t*)s2 && s1[4] == s2[4]);
}

int
wsv_str_cmp_6(const char* s1, const char* s2)
{
	return (*(uint32_t*)s1 == *(uint32_t*)s2 && \
			((uint16_t*)s1)[2] == ((uint16_t*)s2)[2]);
}

int
wsv_str_cmp_7(const char* s1, const char* s2)
{
	return (*(uint32_t*)s1 == *(uint32_t*)s2 && \
			((uint16_t*)s1)[2] == ((uint16_t*)s2)[2] && \
			s1[6] == s2[6]);
}

int
wsv_str_cmp_8(const char* s1, const char* s2)
{
	return (((uint32_t*)s1)[0] == ((uint32_t*)s2)[0] && \
			((uint32_t*)s1)[1] == ((uint32_t*)s2)[1]);
}

int
wsv_str_cmp_9(const char* s1, const char* s2)
{
	return (((uint32_t*)s1)[0] == ((uint32_t*)s2)[0] && \
			((uint32_t*)s1)[1] == ((uint32_t*)s2)[1] && \
			s1[8] == s2[8]);
}

int
wsv_str_cmp_10(const char* s1, const char* s2)
{
	return (((uint32_t*)s1)[0] == ((uint32_t*)s2)[0] && \
			((uint32_t*)s1)[1] == ((uint32_t*)s2)[1] && \
			((uint16_t*)s1)[4] == ((uint16_t*)s2)[4]);
}

int
wsv_str_cmp_11(const char* s1, const char* s2)
{
	return (((uint32_t*)s1)[0] == ((uint32_t*)s2)[0] && \
			((uint32_t*)s1)[1] == ((uint32_t*)s2)[1] && \
			((uint16_t*)s1)[4] == ((uint16_t*)s2)[4] &&
			s1[10] == s2[10]);
}

int
wsv_str_cmp_12(const char* s1, const char* s2)
{
	return (((uint32_t*)s1)[0] == ((uint32_t*)s2)[0] && \
			((uint32_t*)s1)[1] == ((uint32_t*)s2)[1] && \
			((uint32_t*)s1)[2] == ((uint32_t*)s2)[2]);
}

int
wsv_str_cmp_16(const char* s1, const char* s2)
{
	return (((uint32_t*)s1)[0] == ((uint32_t*)s2)[0] && \
			((uint32_t*)s1)[1] == ((uint32_t*)s2)[1] && \
			((uint32_t*)s1)[2] == ((uint32_t*)s2)[2] && \
			((uint32_t*)s1)[3] == ((uint32_t*)s2)[3]);
}

int
wsv_str_cmp_20(const char* s1, const char* s2)
{
	return (((uint32_t*)s1)[0] == ((uint32_t*)s2)[0] && \
			((uint32_t*)s1)[1] == ((uint32_t*)s2)[1] && \
			((uint32_t*)s1)[2] == ((uint32_t*)s2)[2] && \
			((uint32_t*)s1)[3] == ((uint32_t*)s2)[3] && \
			((uint32_t*)s1)[4] == ((uint32_t*)s2)[4]);
}

int
wsv_str_cmp_27(const char* s1, const char* s2)
{
	return (((uint32_t*)s1)[0] == ((uint32_t*)s2)[0] && \
			((uint32_t*)s1)[1] == ((uint32_t*)s2)[1] && \
			((uint32_t*)s1)[2] == ((uint32_t*)s2)[2] && \
			((uint32_t*)s1)[3] == ((uint32_t*)s2)[3] && \
			((uint32_t*)s1)[4] == ((uint32_t*)s2)[4] && \
			((uint32_t*)s1)[5] == ((uint32_t*)s2)[5] && \
			((uint16_t*)s1)[12] == ((uint16_t*)s2)[12] && \
			s1[26] == s2[26]);
}
