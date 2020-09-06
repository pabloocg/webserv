#include "Request.hpp"
#include "../utils/base64.hpp"

bool http::Request::needs_auth(http::Routes route)
{
	if (route.needAuth())
	{
		this->_realm = route.getAuthMessage();
		this->_auth_file_path = route.getPassAuthFile();
		return (true);
	}
	else if (!route.needExplicitAuth() && this->_server.needAuth())
	{
		this->_realm = this->_server.getAuthMessage();
		this->_auth_file_path = this->_server.getPassAuthFile();
		return (true);
	}
	return (false);
}

bool http::Request::validate_password(std::string auth)
{
	std::ifstream htpasswd;
	std::string buf;
	std::string decoded_str;
	std::string user;
	std::string password;

	if (auth.back() == '\r')
		auth = auth.substr(0, auth.size() - 1);
	std::vector<std::uint8_t> decoded = base64::decode(auth);
	for (int i = 0; i < (int)decoded.size(); i++)
		decoded_str += (char)decoded[i];
	int pos = decoded_str.find(':');
	user = decoded_str.substr(0, pos);
	password = decoded_str.substr(pos + 1, (decoded_str.size() - pos + 1));
	htpasswd.open(this->_auth_file_path);
	if (htpasswd.is_open())
	{
		while (std::getline(htpasswd, buf))
		{
			if (user == buf.substr(0, buf.find(':')))
			{
				std::string encrypted_passwd = buf.substr(buf.find(':') + 1, buf.size() - buf.find(':') + 1);
				decoded.clear();
				for (int i = 0; i < (int)password.size(); i++)
					decoded.push_back((unsigned char)password[i]);
				std::string encoded = base64::encode(decoded);
				if (encoded == encrypted_passwd)
					return (true);
			}
		}
	}
	return (false);
}
