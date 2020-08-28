#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <regex>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>

#define ROOT_DIR "dir"

namespace http
{
	inline int file_exists(std::string file)
	{
		struct stat st;

		return (stat(file.c_str(), &st));
	}

	inline long int file_size(std::string file)
	{
		struct stat st;
		
		if (!stat(file.c_str(), &st))
			return (st.st_size);
		else
			return -1;
	}

	inline std::vector<std::string> special_split(std::string str, char delimiter)
	{
		std::vector<std::string> internal;
		std::stringstream ss;

		for (size_t i = 0; i < str.length(); i++)
		{
			if (str[i] == delimiter)
			{
				internal.push_back(ss.str());
				ss.str("");
				ss.clear();
			}
			else if (str[i] == '{')
			{
				while (str[i] != '}')
					ss << str[i++];
				ss << str[i];
				internal.push_back(ss.str());
				ss.str("");
				ss.clear();
			}
			else
				ss << str[i];
		}
		return (internal);
	}

	inline std::vector<std::string> split(std::string str, char delimiter)
	{
		std::vector<std::string> internal;
		std::stringstream ss(str);
		std::string tok;

		while (std::getline(ss, tok, delimiter))
		{
			if (tok.size() > 0)
				if (!(tok.size() == 1 && tok[0] == '\r'))
					internal.push_back(tok);
		}
		return (internal);
	}

	static std::string ltrim(std::string str)
	{
		return (std::regex_replace(str, std::regex("^\\s+"), std::string("")));
	}

	static std::string rtrim(std::string str)
	{
		return (std::regex_replace(str, std::regex("\\s+$"), std::string("")));
	}

	inline std::string trim(std::string str)
	{
		return (rtrim(ltrim(str)));
	}

	inline std::string trim2(std::string s, std::string trimChars)
	{
		std::stringstream buff;
		size_t i = 0;
		while (i < s.length())
		{
			if (trimChars.find(s[i]) == std::string::npos)
				buff << s[i];
			i++;
		}
		return (buff.str());
	}

	inline std::vector<std::string> charptrptrToVector(char **env)
	{
		std::vector<std::string> vec;
		int i = -1;
		while (env[++i])
		{
			std::string word = "";
			for (int j = 0; env[i][j]; j++)
			{
				word += env[i][j];
			}
			vec.push_back(word);
		}
		return (vec);
	}

	inline char **vecToCharptrptr(std::vector<std::string> env)
	{
		char **ret;
		if (!(ret = (char **)malloc(sizeof(char *) * (env.size() + 1))))
		{
			perror("malloc");
		}
		ret[env.size()] = NULL;
		for (int i = 0; i < (int)env.size(); i++)
		{
			ret[i] = strdup(env[i].c_str());
		}
		return (ret);
	}

	inline std::string get_actual_date(void)
	{
		struct timeval tv;
		char buf[30];
		if (gettimeofday(&tv, NULL) != 0)
		{
			perror("gettimeofday");
		}
		ssize_t written = -1;
		struct tm *gm = gmtime(&tv.tv_sec);
		if (gm)
		{
			written = (ssize_t)strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", gm);
			if (!(written > 0))
			{
				perror("strftime");
			}
		}
		return (buf);
	}

} // namespace http

#endif
