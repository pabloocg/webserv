#include "wsv_wrapper.h"
#include "wsv_http_message.h"
#include "wsv_configuration.h"
#include "wsv_internet_protocol.h"
#include "wsv_string.h"
#include "wsv_tcp.h"

void
wsv_http_message_initialize(struct wsv_http_message_s* m)
{
	m->buf_begin = m->buf;
	m->buf_end = m->buf;
	m->buf_position = m->buf;
	m->buf_size = BUF_SIZE;
	m->buf_count = BUF_SIZE;

	m->state_request = 0;
	m->state_request_line = 0;
	m->state_headers = 0;
	m->state_payload = 0;
	m->state_response = 0;

	m->method = 0;
	m->scheme = 0;
	m->host_begin = 0;
	m->host_end = 0;
	m->port_begin = 0;
	m->port_end = 0;
	m->absolute_path_begin = 0;
	m->absolute_path_end = 0;
	m->query_begin = 0;
	m->query_end = 0;
	m->http_version = 0;

	m->header_name_begin = 0;
	m->header_name_end = 0;
	m->header_value_begin = 0;
	m->header_value_end = 0;

	wsv_bzero((void*)m->headers,
			  sizeof(char) * HEADER_VALUE_SIZE * HEADERS_MAX);
	m->headers_flag = 0;

	m->body_begin = 0;
	m->body_end = 0;
	m->payload_size = 0;

	m->read_request_handler = &wsv_http_request_read;
	m->write_static_content_handler = 0;
	m->read_static_content_handler = &wsv_http_response_ressource_read;

	m->status_code = 0;
	m->ressource_fd = -1;

	m->content_type = 0;
	m->content_length = 0;
	m->last_modified = ((time_t) -1);;
	m->location = 0;
	m->content_location = 0;
	m->limit_except = 0;

	m->cgi_size = -1;

	m->max = 0;
}

void
wsv_http_message_terminate(struct wsv_http_message_s* m)
{
	if (m->ressource_fd >= 0)
	{
		wsv_tcp_multiplexing_clear(m->ressource_fd, m->fd_sets);
		wsv_close(m->ressource_fd); /* insert error check */
	}
	m->cgi_string.clear();
	m->cgi_headers.clear();
}
