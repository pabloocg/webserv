#include <sys/time.h>

#include "wsv_time.h"
#include "wsv_string.h"
#include "wsv_retval.h"

/* Unix Epoch : Thu, 1 Jan 1970 00:00:00 [GMT/UTC] */

#define DAY_SECONDS			86400
#define HOUR_SECONDS		3600
#define MINUTE_SECONDS		60

#define IS_LEAP_YEAR(Y)		(Y) % 4 == 0 && ((Y) % 100 != 0 || (Y) % 400 == 0)

#define NORMAL_YEAR_DAYS	365
#define LEAP_YEAR_DAYS		366

#define UNIX_EPOCH_YEAR		1970
#define UNIX_EPOCH_WEEK_DAY	4

void
wsv_time_date(time_t unix_time, struct wsv_time_s* date)
{
	/*
	** Normal year (from 0 not 1)
	** +) Jan : 0 -> 30
	** +) Feb : 31 -> 58
	** +) Mar : 59 -> 89
	** +) ...
	** +) Dec : 334 -> 364
	*/
	static const int				wsv_months_days[2][12] =
	{
		/* Normal years.  */
		{0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},
		/* Leap years.  */
		{0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335}
	};
	const int					*months_days;
	int							year;
	int							days;
	int							n;

	days = (int)(unix_time / DAY_SECONDS);
	unix_time = unix_time % DAY_SECONDS;

	/* ============ WEEK DAY ============ */
	date->tm_wday = (UNIX_EPOCH_WEEK_DAY + days) % 7;

	/* ============ HOUR ============ */
	date->tm_hour = (int)(unix_time / HOUR_SECONDS);
	unix_time = unix_time % HOUR_SECONDS;

	/* ============ MINUTE & SECOND ============ */
	date->tm_min = (int)(unix_time / MINUTE_SECONDS);
	date->tm_sec = (int)(unix_time % MINUTE_SECONDS);

	/* ============ YEAR & YEAR DAY ============ */
	year = UNIX_EPOCH_YEAR;
	for (;;)
	{
		n = IS_LEAP_YEAR(year) ? LEAP_YEAR_DAYS : NORMAL_YEAR_DAYS;

		if (days < n)
			break;
		else
			days -= n;

		++year;
	}
	date->tm_year = year;

	/* ============ MONTH & MONTH DAY ============ */
	months_days = wsv_months_days[IS_LEAP_YEAR(year)];
	n = 11;
	while (days < months_days[n])
		--n;
	date->tm_mon = n;
	date->tm_mday = (days - months_days[n]) + 1;
}

int
wsv_timeofday(struct wsv_time_s* date)
{
	struct timeval				tv;

	if (gettimeofday(&tv, 0) < 0)
		return (WSV_ERROR);
	else
	{
		wsv_time_date(tv.tv_sec, date);
		return (WSV_OK);
	}
}

/*
** +) example : "Sun, 06 Nov 1994 08:49:37 GMT"
*/
void
wsv_date_internet_message_format(struct wsv_time_s* date, char buf[29])
{
	static const char			day_name[7][6] =
	{
		"Sun, ", "Mon, ", "Tue, ", "Wed, ", "Thu, ", "Fri, ", "Sat, "
	};
	static const char			month[12][6] =
	{
		" Jan ", " Feb ", " Mar ", " Apr ", " May ", " Jun ",
		" Jul ", " Aug ", " Sep ", " Oct ", " Nov ", " Dec "
	};
	int							year;

	/* ============ day-name ============ */
	wsv_str_cpy_5(buf, day_name[date->tm_wday]);

	/* ============ date1 -> day ============ */
	buf[5] = (char)('0' + (date->tm_mday / 10));
	buf[6] = (char)('0' + (date->tm_mday % 10));

	/* ============ date1 -> month ============ */
	wsv_str_cpy_5(buf + 7, month[date->tm_mon]);

	/* ============ date1 -> year ============ */
	year = date->tm_year;
	buf[12] = (char)('0' + (year / 1000));

	year = year - (buf[12] - '0') * 1000;
	buf[13] = (char)('0' + (year / 100));

	year = year - (buf[13] - '0') * 100;
	buf[14] = (char)('0' + (year / 10));

	year = year - (buf[14] - '0') * 10;
	buf[15] = (char)('0' + year);

	/* ==== time-of-day (hour:minute:second) ==== */
	buf[16] = ' ';
	buf[17] = (char)('0' + date->tm_hour / 10);
	buf[18] = (char)('0' + date->tm_hour % 10);
	buf[19] = ':';
	buf[20] = (char)('0' + date->tm_min / 10);
	buf[21] = (char)('0' + date->tm_min % 10);
	buf[22] = ':';
	buf[23] = (char)('0' + date->tm_sec / 10);
	buf[24] = (char)('0' + date->tm_sec % 10);
	buf[25] = ' ';

	/* ============ GMT ============ */
	wsv_str_cpy_3(buf + 26, "GMT");
}
