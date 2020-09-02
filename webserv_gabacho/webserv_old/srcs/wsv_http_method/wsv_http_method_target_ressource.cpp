#include <string>
#include <string.h>

#include "wsv_wrapper.h"

char*
wsv_http_method_target_ressource(char* absolute_path,
								 char* abs_path_root,
								 char** abs_path_extension,
								 const char* location_root)
{
	char*			target_ressource;
	char*			it;
	char*			extension;

	++absolute_path;

	if (location_root != 0)
	{
		target_ressource = abs_path_root;
		it = abs_path_root;
		extension = 0;

		while ((*it = *location_root) != '\0')
		{
			++it;
			++location_root;
		}

		while ((*it = *absolute_path) != '\0')
		{
			if (*it == '.')
				extension = it;
			++it;
			++absolute_path;
		}

		if (extension == 0)
			*abs_path_extension = it;
		else
			*abs_path_extension = extension;

		return (target_ressource);
	}
	else
		return (absolute_path);
}


// m->absolute_path = /directory/youpi.bla
// lc->pattern = /directory
// lc->root = ./YoupiBanane
void
wsv_http_method_target_ressource_2(std::string *target_ressource,
	char* absolute_path, const char* pattern, const char *root)
{
	*target_ressource = absolute_path;
	size_t start = target_ressource->find(pattern);
	if(start != std::string::npos){
		target_ressource->replace(start, strlen(pattern), root);
	}
}

void
wsv_http_method_target_ressource_3(std::string *target_ressource,
	char* absolute_path, const char* pattern, const char *root,
	const char *index)
{
	struct stat						statbuf;

	*target_ressource = absolute_path;
	size_t start = target_ressource->find(pattern);
	if(start != std::string::npos){
		target_ressource->replace(start, strlen(pattern), root);
	}
	if(strcmp(root, target_ressource->c_str()) == 0 &&
		*(target_ressource->end() - 1) != '/')
	{
		if (wsv_stat((*target_ressource).c_str(), &statbuf) >= 0 &&
			S_ISDIR(statbuf.st_mode))
			*target_ressource += '/';
	}
	if(*(target_ressource->end() - 1) == '/')
	{
		if (index != 0)
			*target_ressource += index;
	}
}
