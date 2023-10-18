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

void sendUser2(const std::string& msg, int clientSocket, std::string name)
{
	std::string msgError = "PRIVMSG " + name + " :" + msg + "\n";
	std::cout << msgError << std::endl;
	// std::cout <<"Send  --> [" << msg << "]" << std::endl;
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

void create_channel(const int clientSocket,Channel *channels,const Client *clients, std::string name, const int i, int channel_index)
{

		channels[channel_index].index = channel_index;
		channels[channel_index].name = name;
	//	channels[channel_index].clients_sockets.push_back(clients[i].socket);
	//	std::cout << "------------------------------------" << std::endl;
		channels[channel_index].admins_users.push_back(clients[i].nickname);
		channels[channel_index].topic = "";
		sendUser("Channel " + name + " Created",clientSocket);
		channels[channel_index].lmt = 100000;
		channels[channel_index].limit = "";
		channels[channel_index].mode_i = false;
		channels[channel_index].mode_t = false;
		channels[channel_index].mode_k = false;
		channels[channel_index].mode_l = false;
		channels[channel_index].mode_o = false;
}
int srch_clnt_chan(const int clientSocket,const Channel* channels,int ind)
{
	for (size_t i = 0; i < channels[ind].clients_sockets.size(); i++)
	{
		if (channels[ind].clients_sockets[i] == clientSocket)
		return (i);
	}
	return (-0);
}
std::string get_modes(const Channel* channels,int ind)
{
	std::string mode = " +";

		if (channels[ind].mode_i == true)
			mode += "i";
		if (channels[ind].mode_t == true)
			mode += "t";
		if (channels[ind].mode_k == true)
			mode += "k";
		if (channels[ind].mode_l == true)
			mode += "l";
		if (channels[ind].mode_o == true)
			mode += "o";
	return (mode);
}

// int check_valid_mode(std::string mode)
// {
// 	if ((mode[0] == '+' || mode[0] == '-')
// 	&& mode.find_first_not_of("itkol") == std::string::npos)
// 	{
// 		if ()
// 	}
	
// 	return -1;
// }

int srch_is_operator (std::string nickname,const int clientSocket,const Channel* channels,int ind)
{
	if (srch_clnt_chan(clientSocket,channels,ind) != -1)
	{
		for (size_t i = 0; i < channels[ind].admins_users.size(); i++)
		{
			if (nickname == channels[ind].admins_users[i])
			return (1);
		}
	}
	return (-1);
}

void list_response(const Channel* channels,int clientSocket,int num_chan,std::string nickname)
{
	for (size_t i = 0 ;i < num_chan; i++)
	{
		std::ostringstream oss;
   		oss << channels[i].clients_sockets.size();
    	std::string nm_users = oss.str();
		sendUser("322 " + nickname + " " + channels[i].name + " " + nm_users + " " + channels[i].topic,clientSocket);
	}
}
int srch_admin_users(std::string nickname,const Channel* channels)
{
	for (size_t i = 0; i < channels->admins_users.size();i++)
	{
		if (nickname == channels->admins_users[i])
			return i;
	}
	return (-1);
}
std::string extract_message(std::vector<std::string> args,int ind)
{
	std::string mssg = "";
	for (int indd = ind; indd  < args.size(); indd++)
	{
		mssg += args[indd];
		mssg += " ";
	}
	return(mssg);
}

std::string addRandomNumber(const std::string& input) {
    std::srand(static_cast<unsigned int>(std::time(NULL)));
    
    int randomNumber = std::rand() % 1000 + 1;

    std::ostringstream oss;
    oss << randomNumber;
    std::string randomNumStr = oss.str();

    return input + randomNumStr;
}
