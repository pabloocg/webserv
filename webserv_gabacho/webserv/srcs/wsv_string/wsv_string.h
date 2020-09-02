#ifndef WSV_STRING_H
# define WSV_STRING_H

#include <stddef.h>

/*
** =============
** = str_cmp_n =
** =============
*/
int		wsv_str_cmp_2(const char* s1, const char* s2);
int		wsv_str_cmp_4(const char* s1, const char* s2);
int		wsv_str_cmp_5(const char* s1, const char* s2);
int		wsv_str_cmp_6(const char* s1, const char* s2);
int		wsv_str_cmp_7(const char* s1, const char* s2);
int		wsv_str_cmp_8(const char* s1, const char* s2);
int		wsv_str_cmp_9(const char* s1, const char* s2);
int		wsv_str_cmp_10(const char* s1, const char* s2);
int		wsv_str_cmp_11(const char* s1, const char* s2);
int		wsv_str_cmp_12(const char* s1, const char* s2);
int		wsv_str_cmp_16(const char* s1, const char* s2);
int		wsv_str_cmp_20(const char* s1, const char* s2);
int		wsv_str_cmp_27(const char* s1, const char* s2);

/*
** =============
** = str_cpy_n =
** =============
*/
void	wsv_str_cpy_2(char* dst, const char* src);
void	wsv_str_cpy_3(char* dst, const char* src);
void	wsv_str_cpy_4(char* dst, const char* src);
void	wsv_str_cpy_5(char* dst, const char* src);
void	wsv_str_cpy_6(char* dst, const char* src);
void	wsv_str_cpy_8(char* dst, const char* src);
void	wsv_str_cpy_9(char* dst, const char* src);
void	wsv_str_cpy_10(char* dst, const char* src);
void	wsv_str_cpy_12(char* dst, const char* src);
void	wsv_str_cpy_14(char* dst, const char* src);
void	wsv_str_cpy_15(char* dst, const char* src);
void	wsv_str_cpy_16(char* dst, const char* src);
void	wsv_str_cpy_18(char* dst, const char* src);
void	wsv_str_cpy_20(char* dst, const char* src);
void	wsv_str_cpy_24(char* dst, const char* src);
void	wsv_str_cpy_27(char* dst, const char* src);
void	wsv_str_cpy_29(char* dst, const char* src);
void	wsv_str_cpy_52(char* dst, const char* src);

/*
** ==================
** = wsv_string.cpp =
** ==================
*/
void	wsv_bzero(void *s, size_t n);
void*	wsv_memcpy(void *dst, const void *src, size_t n);
int		wsv_strcmp(const char *s1, const char *s2);

void	wsv_str_reverse(char* first, char* last);
void	wsv_str_trim_trailing_space(char* first, char* last);
char*	wsv_str_token(char* first, char* last, char** endptr, char delim);
void	wsv_str_extension_find_const(const char** dst, const char*src);
void	wsv_str_extension_find(char** dst, char *src);
char*	wsv_str_trim(char* str, char delim);
int		wsv_str_to_size(char* str, size_t* dst);

#endif
