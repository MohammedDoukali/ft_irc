#include "lib.hpp"

void ft_error(int ind, const std::string &str)
{
    
    std::cerr << "Error: " << str << std::endl;
     if (ind)
        return;
     exit(0);
}

int countWords(const std::string& str)
{
	std::istringstream iss(str);
	int count = 0;
	std::string word;
	
	while (iss >> word)
		count++;
	return count;
}

void remove_spaces(std::string& str) {
    std::string result;
    bool isSpace = false;
   int wordCount = 0;
    for (std::size_t i = 0; i < str.length(); ++i) {
        if (std::isspace(str[i])) {
            if (!isSpace)
            {
                result += ' ';
                 wordCount++;
                isSpace = true;
            }
        } 
        else {
            result += str[i];
            isSpace = false;
        }
    }
   std::cout << "x7aal mn klma" << wordCount << std::endl;
    str = result;
}

std::vector<std::string> split_str(std::string str, char sep)
{
	std::vector<std::string> row;
	std::string word;
	std::stringstream s(str);
	while (std::getline(s, word, sep)) {
		if (word.size())
			row.push_back(word);
	}
	return row;
}

int countWords(const std::string& str)
{
	std::istringstream iss(str);
	int count = 0;
	std::string word;
	
	while (iss >> word)
		count++;
	return count;
}