#include <errno.h>

#include "wsv_wrapper.h"
#include "wsv_retval.h"
#include "wsv_mime_type.h"
#include "wsv_string.h"
#include "wsv_http_message.h"
#include "wsv_location.h"
#include "wsv_http_method.h"
#include "wsv_client.h"


static int
wsv_http_method_autoindex_payload(int fd, struct wsv_http_message_s* m)
{
	ssize_t			bytes_written;

	bytes_written = wsv_write(fd, (void*)m->buf_begin, m->content_length);

	wsv_close(fd); /* insert error check */
	if (bytes_written < 0 || m->content_length != (size_t)bytes_written)
	{
		if (m->ressource_fd >= 0)
			wsv_close(m->ressource_fd); /* insert error check */
		return (WSV_ERROR_500);
	}
	else
		return (WSV_SUCCESS_200);
}

static int
wsv_directory_listing(const char* directory, struct wsv_http_message_s* m)
{
	int					pipefd[2];
	int					n;
	const char*			entry_name;
	DIR*				dirp;
	struct dirent*		dir_entry;

	dirp = wsv_opendir(directory);
	if (dirp == 0)
		return (WSV_ERROR_404);

	wsv_str_cpy_29(m->buf_begin, "<html>\n<head><title>Index of ");
	n = 29;

	entry_name = directory;
	while ((m->buf_begin[n] = *entry_name) != '\0')
	{
		++n;
		++entry_name;
	}
	wsv_str_cpy_52(m->buf_begin + n,
				   "</title></head>\n<body bgcolor=\"white\">\n<h1>Index of ");
	n += 52;

	entry_name = directory;
	while ((m->buf_begin[n] = *entry_name) != '\0')
	{
		++n;
		++entry_name;
	}

	wsv_str_cpy_16(m->buf_begin + n, "</h1>\n<hr><pre>\n");
	n += 16;

	errno = 0;
	while ((dir_entry = wsv_readdir(dirp)) != 0)
	{
		wsv_str_cpy_9(m->buf_begin + n, "<a href=\"");
		n += 9;

		entry_name = dir_entry->d_name;
		while ((m->buf_begin[n] = *entry_name) != '\0')
		{
			++n;
			++entry_name;
		}
		if (dir_entry->d_type == DT_DIR) /* d_type -not portable- */
			m->buf_begin[n++] = '/';

		wsv_str_cpy_2(m->buf_begin + n, "\">");
		n += 2;

		entry_name = dir_entry->d_name;
		while ((m->buf_begin[n] = *entry_name) != '\0')
		{
			++n;
			++entry_name;
		}
		if (dir_entry->d_type == DT_DIR) /* d_type -not portable- */
			m->buf_begin[n++] = '/';

		wsv_str_cpy_5(m->buf_begin + n, "</a>\n");
		n += 5;

		if (n > (m->buf_size - 554))
		{
			wsv_closedir(dirp); /* insert error check */
			return (WSV_ERROR_413);
		}
	}

	wsv_str_cpy_27(m->buf_begin + n, "</pre><hr>\n</body>\n</html>\n");
	n += 27;

	wsv_closedir(dirp); /* insert error check */
	if (errno == 0)
	{
		if (wsv_pipe(pipefd) < 0)
			return (WSV_ERROR_500);

		wsv_client_static_content_write_intialize(pipefd[1], (void*)m);
		m->write_static_content_handler = &wsv_http_method_autoindex_payload;

		m->ressource_fd = pipefd[0];
		m->content_type = "text/html";
		m->content_length = (size_t)n;
		return (WSV_RETRY);
	}
	else
		return (WSV_ERROR_500);
}

int
wsv_http_method_get(struct wsv_http_message_s* m, const void* location)
{
	const struct wsv_location_s*	lc;
	const char*						target_ressource;
	int								fd;
	struct stat						statbuf;

	lc = (const struct wsv_location_s*)location;

	// wsv_http_method_target_ressource_2(&m->target_ressource,
	// 	m->absolute_path, lc->pattern, lc->root);
	wsv_http_method_target_ressource_3(&m->target_ressource,
		m->absolute_path, lc->pattern, lc->root, lc->index);
	target_ressource = m->target_ressource.c_str();

	size_t pos = m->target_ressource.find('.');
	if (pos == std::string::npos)
		m->abs_path_extension = (char*)"";
	else
		m->abs_path_extension = (char*)(m->target_ressource.c_str() + pos);

	if (stat(target_ressource, &statbuf) < 0)
			return (WSV_ERROR_404);

	if (S_ISDIR(statbuf.st_mode))
	{
		if (*(m->target_ressource.rbegin()) != '/')
			m->target_ressource.append("/");
		if (lc->index != 0)
		{
			m->target_ressource.append(lc->index);
			if (stat(m->target_ressource.c_str(), &statbuf) >= 0)
			{
				target_ressource = m->target_ressource.c_str();
				goto regular_file;
			}
		}
		goto directory;
	}

regular_file:
	fd = wsv_open(target_ressource,
			      O_RDONLY | O_NOFOLLOW | O_NONBLOCK);
	if (fd < 0)
		return (WSV_ERROR_404);

	m->ressource_fd = fd;
	wsv_mime_type(m->abs_path_extension, &m->content_type);
	m->content_length = (size_t)statbuf.st_size;
	m->last_modified = statbuf.st_mtime;
	m->content_location = m->absolute_path;
	return (WSV_SUCCESS_200);

directory:
	if (lc->autoindex == 0)
		return (WSV_ERROR_404);
	else
		return (wsv_directory_listing(target_ressource, m));
}
