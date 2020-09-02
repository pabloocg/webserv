#include <stdint.h>

#include "wsv_wrapper.h"
#include "wsv_retval.h"
#include "wsv_http_special_characters.h"
#include "wsv_client.h"
#include "wsv_mime_type.h"

int
wsv_http_request_body_chunked_parse(struct wsv_http_message_s* m)
{
	enum states
	{
		chunk_size_read = 0,
		chunk_size_CRLF = 1,
		chunk_data_write = 2,
		chunk_data_CR = 3,
		chunk_data_LF = 4
	};
	char*			it;

	for (it = m->body_begin; it != m->body_end; ++it)
	{
		switch ((enum states)m->state_payload)
		{
		case chunk_size_read:
			if (*it >= '0' && *it <= '9')
				m->payload_size = (m->payload_size << 4) + (*it - '0');
			else if (*it >= 'a' && *it <= 'z')
				m->payload_size = (m->payload_size << 4) + (*it - 'a' + 10);
			else if (*it >= 'A' && *it <= 'Z')
				m->payload_size = (m->payload_size << 4) + (*it - 'A' + 10);
			else
			{
				switch (*it)
				{
				case CR:
					m->state_payload = chunk_size_CRLF;
					break;
				case LF:
					if (m->payload_size == 0)
						goto done;
					m->state_payload = chunk_data_write;
					break;
				default:
					goto error_400;
				}
				break;
			}
			if (m->payload_size > SIZE_MAX)
				goto error_400;
			break;

		case chunk_size_CRLF:
			if (*it == LF)
			{
				if (m->payload_size == 0)
					goto done;
				m->state_payload = chunk_data_write;
			}
			else
				goto error_400;
			break;

		case chunk_data_write:
			m->body_begin = it;
			wsv_client_static_content_write_intialize(m->ressource_fd,
													  (void*)m);
			return (WSV_RETRY);

		case chunk_data_CR:
			switch (*it)
			{
				case CR:
					m->state_payload = chunk_data_LF;
					break;
				case LF:
					m->state_payload = chunk_size_read;
					break;
				default:
					goto error_400;
			}
			break;

		case chunk_data_LF:
			if (*it == LF)
				m->state_payload = chunk_size_read;
			else
				goto error_400;
		}
	}

	wsv_client_receive_initialize(m->ressource_fd, (void*)m);
	return (WSV_RETRY);

error_400:
	wsv_close(m->ressource_fd); /* insert error check */
	return (WSV_ERROR_400);

done:
	wsv_close(m->ressource_fd); /* insert error check */
	m->ressource_fd = -1;
	wsv_mime_type(m->abs_path_extension, &m->content_type);
	m->content_length = 0;
	return (m->status_code);
}
