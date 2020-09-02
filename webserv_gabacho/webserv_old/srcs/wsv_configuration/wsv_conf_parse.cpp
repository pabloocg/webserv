#include "wsv_configuration.h"
#include "wsv_retval.h"

int
wsv_conf_parse(struct wsv_conf_data_s* conf)
{
	enum states
	{
		key_begin = 0,
		key_end,
		value_begin,
		value_end,
		block,
	};
	char*			it;

	for (it = conf->buf_position; it != conf->buf_end; ++it)
	{
		switch ((enum states)conf->state)
		{
		case key_begin: /* ========== KEY_BEGIN ========== */
			conf->key_begin = 0;
			conf->key_end = 0;
			conf->value_begin = 0;
			conf->value_end = 0;
			switch (*it)
			{
			case '\t':
				break;
			case ' ':
				break;
			case '\n':
				break;
			case '}': /* == BLOCK_END == */
				--(conf->nblock);
				if (conf->nblock == -1)
					return (WSV_ERROR);
				goto done; /* -- BLOCK_END -- */
			default:
				conf->key_begin = it;
				conf->state = key_end;
			}
			break; /* ---------- KEY_BEGIN ---------- */

		case key_end: /* ========== KEY_END ========== */
			switch (*it)
			{
			case '\t':
				conf->key_end = it;
				conf->state = value_begin;
				break;
			case ' ':
				conf->key_end = it;
				conf->state = value_begin;
				break;
			case '{':
				conf->key_end = it;
				conf->state = block;
				break;
			case ';':
				conf->key_end = it;
				goto done;
			}
			break; /* ---------- KEY_END ---------- */

		case value_begin: /* ========== VALUE_BEGIN ========== */
			switch (*it)
			{
			case ' ':
				break;
			case '{':
				conf->state = block;
				break;
			case ';':
				goto done;
			default:
				conf->value_begin = it;
				conf->state = value_end;
			}
			break; /* ---------- VALUE_BEGIN ---------- */

		case value_end: /* ========== VALUE_END ========== */
			switch (*it)
			{
			case '{':
				conf->value_end = it;
				conf->state = block;
				break;
			case ';':
				conf->value_end = it;
				goto done;
			}
			break; /* ---------- VALUE_END ---------- */

		case block: /* ========== BLOCK_BEGIN ========== */
			++(conf->nblock);
			conf->state = 0;
			conf->buf_position = it;
			return (WSV_RETRY);
			/* ---------- BLOCK_BEGIN ---------- */
		}
	}

	return (conf->state == 0 ? WSV_OK : WSV_ERROR);

done:
	conf->state = 0;
	conf->buf_position = it + 1;
	return (WSV_RETRY);
}
