#include "lib.hpp"

void ft_error(int ind, const std::string &str)
{
    
    std::cerr << "Error: " << str << std::endl;
     if (ind)
        return;
     exit(0);
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
//    std::cout << "x7aal mn klma" << wordCount << std::endl;
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

void errorUser(const std::string& msg, int clientSocket)
{
	std::string msgError = "Error : " + msg + "\n";
	send(clientSocket, msgError.c_str(), msgError.length(), 0);
	return ;
}

void sendUser(const std::string& msg, int clientSocket)
{
	std::string msgError = msg + "\n";
	send(clientSocket, msgError.c_str(), msgError.length(), 0);
	return ;
}

int searchByUsername(const std::string& target, const Client* clients, int numClients)
{
	std::string target2 = target;
	for (int i = 0; i < numClients; i++)
	{
		if (strcmp(clients[i].username.c_str(), target2.c_str()) == 0)
			return i;
	}
	return -1;
}

int searchByNickName(const std::string& target, const Client* clients, int numClients)
{
	std::string target2 = target;
	for (int i = 0; i < numClients; i++)
	{
		if (strcmp(clients[i].nickname.c_str(), target2.c_str()) == 0)
			return i;
	}
	return -1;
}
int searchBychannelname(const std::string &channel_name, const Channel* channels, int num_channels)
{
    std::string target2 = channel_name;
	for (int i = 0; i < num_channels; i++)
	{
		if (strcmp(channels[i].name.c_str(), target2.c_str()) == 0)
			return i;
	}
	return -1;
}

std::string addRandomNumber(const std::string& input) {
    std::srand(static_cast<unsigned int>(std::time(NULL)));
    
    int randomNumber = std::rand() % 1000 + 1;

    std::ostringstream oss;
    oss << randomNumber;
    std::string randomNumStr = oss.str();

    return input + randomNumStr;
}

int checkArg(const std::vector<std::string> &arg, int clientSocket)
{
	if (arg[0] == "KICK")
	{
		if (arg.size() < 3 || arg.size() > 4)
			errorUser("/KICK <#Channel> <user> <:Message>/Optional", clientSocket);
		else
			return 1;
	}
	else if (arg[0] == "INVITE ")
	{
		if (arg.size() != 3)
			errorUser("/INVITE <#Channel> <user>", clientSocket);
		else
			return 1;
	}
	else if (arg[0] == "TOPIC")
	{
		if (arg.size() < 2)
			errorUser("/TOPIC <#Channel> <Message>/Optional", clientSocket);
		else
			return 1;
	}
	else if (arg[0] == "MODE")
	{
		if (arg.size() < 3)
			errorUser("/MODE <#Channel> <arg> <Options>", clientSocket);
		else
			return 1;
	}
	else if (arg[0] == "JOIN")
	{
		if (arg.size() < 2)
			errorUser("/JOIN <#Channel>", clientSocket);
		else
			return 1;
	}
	else if (arg[0] == "PRIVMSG")
	{
		if (arg.size() < 3)
			errorUser("/PRIVMSG <#Channel/USER> <Message>", clientSocket);
		else
			return 1;
	}
	else if (arg[0] == "PART")
	{
		if (arg.size() != 2)
			errorUser("/PART <#Channel>", clientSocket);
		else
			return 1;
	}
	else if (arg[0] == "WHOIS")
	{
		if (arg.size() < 2)
			errorUser("/WHOIS <user>", clientSocket);
		else
			return 1;
	}
	else if (arg[0] == "NICK")
	{
		if (arg.size() != 2)
			errorUser("/NICK <new_nickname>", clientSocket);
		else
			return 1;
	}
	else if (arg[0] == "USER")
	{
		if (arg.size() < 2)
			errorUser("/USER <username>", clientSocket);
		else
			return 1;
	}
	else if (arg[0] == "PASS")
	{
		if (arg.size() != 2)
			errorUser("/PASS <password>", clientSocket);
		else
			return 1;
	}
	else if (arg[0] == "LIST" || arg[0] == "LIST\0")
	{
		if (arg.size() != 1)
			errorUser("/LIST", clientSocket);
		else
			return 1;
	}
	else if (arg[0] == "EXIT" || arg[0] == "EXIT\0")
		return 1;
	else if (arg[0] == "QUIT" || arg[0] == "QUIT\0")
		return 1;
	else
		errorUser("INVALID COMMAND!!", clientSocket);
	return -1;
}
