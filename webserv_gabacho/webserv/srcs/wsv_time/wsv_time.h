#ifndef WSV_TIME_H
# define WSV_TIME_H

# include <sys/types.h>

struct			wsv_time_s
{
	int			tm_sec;		/* Seconds (0-60) */
	int			tm_min;		/* Minutes (0-59) */
	int			tm_hour;	/* Hours (0-23) */
	int			tm_mday;	/* Day of the month (1-31) */
	int			tm_mon;		/* Month (0-11) */
	int			tm_year;	/* Year */
	int			tm_wday;	/* Day of the week (0-6, Sunday = 0) */
};

void	wsv_time_date(time_t unix_time, struct wsv_time_s* date);
int		wsv_timeofday(struct wsv_time_s* date);
void	wsv_date_internet_message_format(struct wsv_time_s* date, char buf[29]);

#endif
