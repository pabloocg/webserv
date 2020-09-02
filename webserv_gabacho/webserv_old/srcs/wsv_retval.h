#ifndef WSV_RETVAL_H
# define WSV_RETVAL_H

# define WSV_IS_HTTP_ERROR(n)	((n) >= 4)

# define WSV_ERROR_505			13
# define WSV_ERROR_501			12
# define WSV_ERROR_500			11
# define WSV_ERROR_414			10
# define WSV_ERROR_413			9
# define WSV_ERROR_411			8
# define WSV_ERROR_405			7
# define WSV_ERROR_404			6
# define WSV_ERROR_400			5

# define WSV_SUCCESS_204		4
# define WSV_SUCCESS_201		3
# define WSV_SUCCESS_200		2

# define WSV_OK					1
# define WSV_RETRY				0
# define WSV_ERROR				-1

# define WSV_CGI				-2

#endif
