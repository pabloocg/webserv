#include "wsv_configuration.h"
#include "wsv_wrapper.h"
#include "wsv_retval.h"

int
wsv_conf_read(const char* pathname,
			  struct wsv_configuration_s* conf,
			  struct wsv_conf_data_s* cd)
{
	int							fd;
	ssize_t						bytes_read;
	struct stat					statbuf;

	if (wsv_stat(pathname, &statbuf) < 0)
		return (WSV_ERROR);

	fd = wsv_open(pathname, O_RDONLY);
	if (fd < 0)
		return (WSV_ERROR);

	conf->buf = (char*)wsv_malloc((size_t)statbuf.st_size);
	if (conf->buf == 0)
		return (WSV_ERROR);

	bytes_read = wsv_read(fd, (void*)(conf->buf),
							  (size_t)statbuf.st_size);
	if (bytes_read <= 0)
		return (WSV_ERROR);

	cd->buf_begin = conf->buf;
	cd->buf_position = conf->buf;
	cd->buf_end = conf->buf + bytes_read;

	if (wsv_close(fd) < 0)
		return (WSV_ERROR);
	else
		return (WSV_OK);
}
