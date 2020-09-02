#include <iostream>

char*
wsv_http_method_target_ressource(char* absolute_path,
								 char* abs_path_root,
								 char** abs_path_extension,
								 const char* location_pattern,
								 const char* location_root)
{
	char*			target_ressource;
	char*			it;
	char*			extension;

#ifdef WSV_LOG
	std::cout << "\n____ target ressource ____\n";
	std::cout << "absolute path: " << absolute_path << '\n';
	std::cout << "location root: " << location_root << '\n';
	std::cout << "location pattern: " << location_pattern << '\n';
#endif

	++absolute_path;
	++location_pattern;

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

		while (*absolute_path == *location_pattern && *absolute_path != '\0')
		{
			++absolute_path;
			++location_pattern;
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

#ifdef WSV_LOG
		std::cout << "target ressource: " << target_ressource;
		std::cout << "\n__________________________" << std::endl;
#endif

		return (target_ressource);
	}
	else
	{

#ifdef WSV_LOG
		std::cout << "target ressource: " << absolute_path;
		std::cout << "\n__________________________" << std::endl;
#endif

		return (absolute_path);
	}
}
