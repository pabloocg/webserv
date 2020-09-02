#include <stddef.h>
#include <stdint.h>

#include "wsv_retval.h"

void
wsv_bzero(void *s, size_t n)
{
	while (n--)
		((unsigned char*)s)[n] = 0;
}

void*
wsv_memcpy(void *dst, const void *src, size_t n)
{
	while (n--)
		((unsigned char*)dst)[n] = ((unsigned char*)src)[n];
	return (dst);
}

int
wsv_strcmp(const char *s1, const char *s2)
{
	while ((*s1 != '\0') && (*s1 == *s2))
	{
		++s1;
		++s2;
	}

	return (*s1 - *s2);
}

void
wsv_str_reverse(char* first, char* last)
{
	char	tmp;

	--last;
	while (first < last)
	{
		tmp = *first;
		*first = *last;
		*last = tmp;

		++first;
		--last;
	}
}

void
wsv_str_trim_trailing_space(char* first, char* last)
{
	--last;
	while (first != last && *last == ' ')
		--last;

	last[1] = '\0';
}

char*
wsv_str_token(char* first, char* last, char** endptr, char delim)
{
	char*			it;

	while (first != last && *first == delim)
		++first;

	it = first;
	while (it != last && *it != delim)
		++it;
	*it = '\0';

	*endptr = it + 1;

	return (first);
}

void
wsv_str_extension_find_const(const char** dst, const char *src)
{
	const char*		extension;

	extension = 0;
	while (*src != '\0')
	{
		if (*src == '.')
			extension = src;
		++src;
	}

	*dst = (extension == 0 ? src : extension);
}

void
wsv_str_extension_find(char** dst, char *src)
{
	char*			extension;

	extension = 0;
	while (*src != '\0')
	{
		if (*src == '.')
			extension = src;
		++src;
	}

	*dst = (extension == 0 ? src : extension);
}

char*
wsv_str_trim(char* str, char delim)
{
	char*			token;

	while (*str != '\0' && *str == delim)
		++str;

	token = str;

	while (*str != '\0' && *str != delim)
		++str;
	*str = '\0';

	return (token);
}

int
wsv_str_to_size(char* str, size_t* dst)
{
	size_t					size;

	while (*str != '\0' && *str == ' ')
		++str;
	if (*str == '\0')
		return (WSV_ERROR);

	size = 0;
	while (*str >= '0' && *str <= '9')
	{
		size = size * 10 + (*str - '0');
		if (size > SIZE_MAX)
			return (WSV_ERROR);
		++str;
	}
	*dst = size;

	while (*str != '\0' && *str == ' ')
		++str;
	if (*str == '\0')
		return (WSV_OK);
	else
		return (WSV_ERROR);
}
