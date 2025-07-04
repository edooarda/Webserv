#include "../include/ConfigParser.hpp"

bool ConfigParser::is_host_valid(std::string value)
{
	if (value == "localhost")
		return true;
	else
	{
		int i = 0;
		while(value[i])
		{
			if (value[i] == '.' && value[0] != '.')
				i++;
			else if (isdigit(value[i]))
				i++;
			else
				return false;
		}
		if (value[i - 1] == '.')
			return false;
	}
	return true;
}

bool ConfigParser::is_digit_valid(std::string value)
{
	for (size_t i = 0; i < value.size(); ++i)
	{
		if (!isdigit(value[i]))
		{
			return false;
		}
	}
	return true;
}

void ConfigParser::trim_spaces(std::string &str)
{
	size_t start = str.find_first_not_of(" \t");
	size_t end = str.find_last_not_of(" \t");

	if (start == std::string::npos) {
		str.clear();
		return ;
	}
	str = str.substr(start, end - start + 1);
}

void ConfigParser::remove_comments(std::string &str)
{
	int i = 0;
	int start = 0;

	while(str[i] != '\0')
	{
		if (str[i] == '#' && start == 0){
			start = i;
			str.erase(start, str.length());
			return ;
		}
		i++;
	}
}

bool ConfigParser::is_value_empty(std::string key, std::string value)
{
    if (value.back() == ';')
        value.pop_back();
    trim_spaces(value);
	if (value.empty())
	{
		std::cerr << key << " cannot be set as empty" << std::endl;
		return true;
	}
	return false;
}

std::vector<std::string> ConfigParser::split_by_whitespace(const std::string& str) {
	std::vector<std::string> tokens;
	size_t i = 0;

	while (i < str.length()) {
		while (i < str.length() && (str[i] == ' ' || str[i] == '\t') )
			i++;

		size_t start = i;

		while (i < str.length() && !(str[i] == ' ' || str[i] == '\t'))
			i++;

		if (start < i)
			tokens.push_back(str.substr(start, i - start));
	}
	return tokens;
}

bool ConfigParser::is_semicolon_present(std::string& value)
{
	size_t pos = value.find(';');

	if (pos == std::string::npos){
		std::cout << "Invalid format, end of string need to have ; "<< std::endl;
		return false;
	}
	else
	{
		size_t end = value.size() - 1;

		while (end > 0)
		{
			if (value[end] == ';')
			{
				value.erase(end, 1);
			}
			end--;
		}
	}
	return true;
}