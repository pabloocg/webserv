#include "wsv_wrapper.h"
#include "wsv_retval.h"
#include "wsv_http_message.h"
#include "wsv_http_method.h"
#include "wsv_location.h"

int
wsv_http_method_delete(struct wsv_http_message_s* m, const void* location)
{
	const char*						target_ressource;
	struct stat						statbuf;
	const struct wsv_location_s*	lc;

	lc = (const struct wsv_location_s*)location;

	wsv_http_method_target_ressource_2(&m->target_ressource,
									   m->absolute_path,
									   lc->pattern,
									   lc->root);
	target_ressource = m->target_ressource.c_str();

	size_t pos = m->target_ressource.find('.');
	if (pos == std::string::npos)
		m->abs_path_extension = (char*)"";
	else
		m->abs_path_extension = (char*)(m->target_ressource.c_str() + pos);

	if (wsv_stat(target_ressource, &statbuf) < 0)
		return (WSV_ERROR_404);

	if (S_ISDIR(statbuf.st_mode))
		return (WSV_ERROR_404);

	if (wsv_unlink(target_ressource) < 0)
		return (WSV_ERROR_500);

	return (WSV_SUCCESS_204);
}
