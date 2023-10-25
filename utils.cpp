#include "lib.hpp"

void ft_error(int ind, const std::string &str)
{
    
    std::cerr << "Error: " << str << std::endl;
     if (ind)
        return;
     exit(0);
}

void remove_spaces(std::string& str)
{
	std::string result;
	bool isSpace = false;
	int wordCount = 0;
    for (std::size_t i = 0; i < str.length(); ++i) {
    	if (std::isspace(str[i]))
		{
    		if (!isSpace)
    		{
				result += ' ';
				wordCount++;
    		    isSpace = true;
    		}
        } 
		else
		{
            result += str[i];
            isSpace = false;
        }
    }
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

void sendUser(const std::string& msg, int clientSocket)
{
	std::string msgError = msg + "\r\n";
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

void create_channel(Channel *channels, std::string name,int channel_index)
{
	channels[channel_index].index = 1;
	channels[channel_index].name = name;
	channels[channel_index].lmt = 100000;
	channels[channel_index].topic = "";
	channels[channel_index].limit = "";
	channels[channel_index].password = "";
	channels[channel_index].tmp_oprt = "";
	channels[channel_index].mode_i = false;
	channels[channel_index].mode_t = false;
	channels[channel_index].mode_k = false;
	channels[channel_index].mode_l = false;
}

int srch_clnt_chan(const int clientSocket,const Channel* channels,int ind)
{
	for (size_t i = 0; i < channels[ind].clients_sockets.size(); i++)
	{
		if (channels[ind].clients_sockets[i] == clientSocket)
		return (i);
	}
	return (-1);
}

std::string get_modes(const Channel* channels,int ind)
{
	std::string mode = " +";
		if (channels[ind].mode_i == true)
			mode += "i";
		if (channels[ind].mode_t == true)
			mode += "t";
		if (channels[ind].mode_l == true)
			mode += "l";
		if (channels[ind].mode_k == true)
			mode += "k";
	return (mode);
}

int srch_is_operator(std::string nickname,const int clientSocket,const Channel* channels,int ind)
{
	if (srch_clnt_chan(clientSocket,channels,ind) != -1)
	{
		for (size_t i = 0; i < channels[ind].admins_users.size(); i++)
		{
			if (nickname == channels[ind].admins_users[i])
			return (0);
		}
	}
	return (-1);
}

int check_is_invited(std::string nickname, const Channel* channels,int ind_chan)
{
	for(size_t i = 0; i < channels[ind_chan].invited.size();i++)
	{
		if (nickname == channels[ind_chan].invited[i])
			return (0);
	}
	return (-1);
}

void list_response(const Channel* channels,int clientSocket,int num_chan,std::string nickname)
{
	for (int i = 0 ;i < num_chan; i++)
	{
		std::ostringstream oss;
   		oss << channels[i].clients_sockets.size();
    	std::string nm_users = oss.str();
		if (channels[i].clients_sockets.size() != 0)
		sendUser("322 " + nickname + " " + channels[i].name + " " + nm_users + " " + channels[i].topic,clientSocket);
	}
}

int srch_admin_users(std::string nickname,const Channel* channels,int ind)
{
	for (size_t i = 0; i < channels[ind].admins_users.size();i++)
	{
		if (nickname == channels[ind].admins_users[i])
			return i;
	}
	return (-1);
}

std::string extract_message(std::vector<std::string> args, int ind) {
    std::string mssg = "";
    for (int indd = ind; indd < static_cast<int>(args.size()); indd++) {
        mssg += args[indd];
        mssg += " ";
    }
    return mssg;
}

void remove_admin(std::string nickname, const Channel * channels,int ind)
{
	  for (std::vector<std::string>::iterator it = channels[ind].admins_users.begin(); it != channels[ind].admins_users.end(); /* No increment here */) {
        if (*it == nickname) {
            it = channels[ind].admins_users.erase(it);  
        } else {
            ++it; 
        }
    }
}

void expired_invite(std::string nickname, const Channel * channels,int ind)
{
	  for (std::vector<std::string>::iterator it = channels[ind].invited.begin(); it != channels[ind].invited.end(); /* No increment here */) {
        if (*it == nickname)
            it = channels[ind].invited.erase(it);  
		else
            ++it; 
    }
}

void chng_nick_admin(std::string nickname, const Channel * channels,int ind,std::string new_nick)
{
      for (size_t l = 0 ;l < channels[ind].admins_users.size();l++)
	  {
		if (channels[ind].admins_users[l] == nickname)
			channels[ind].admins_users[l] = new_nick;
	  }
	  for (size_t l = 0; l < channels[ind].invited.size();l++)
	  {
		if (channels[ind].invited[l] == nickname)
			channels[ind].invited[l] = new_nick;
	  }
}
void removeClient(std::vector<int>& clients_sockets, int clientSocket)
{
    for (std::vector<int>::iterator it = clients_sockets.begin(); it != clients_sockets.end(); /* No increment here */) {
        if (*it == clientSocket) {
            it = clients_sockets.erase(it);  
        } 
		else
            ++it; 
    }
}

int srch_vctor_int(const int clientSocket,std::vector<int> users)
{
    for (size_t i = 0; i < users.size(); i++)
    {
        if (clientSocket == users[i])
            return (i);
    }
    return (-1);
}

std::vector<int> inform_users(const int clientSocket,Channel *channels, int num_channel)
{
    std::vector<int> all_users;
    for (int i = 0; i < num_channel; i++)
    {
        if (srch_clnt_chan(clientSocket, channels, i) != -1)
        {
            for (size_t k = 0; k < channels[i].clients_sockets.size(); k++)
            {
                if (srch_vctor_int(channels[i].clients_sockets[k],all_users) == -1)
                    all_users.push_back(channels[i].clients_sockets[k]);
            }
        }
    }
    return(all_users);
}

std::vector<std::string> multi_chaines(const std::string& input)
{
    std::vector<std::string> result;
    std::istringstream ss(input);
    std::string token;
    while (std::getline(ss, token, ','))
	    result.push_back(token);
    return result;
}

void check_nickname(std::string& nickname)
{
	size_t pos = nickname.find_first_of(",*?!@$:.#&");
	size_t pos_2 = nickname.find_first_of("0123456789");
	if (pos == 0 || pos_2 == 0)
	{
		std::string tmp = nickname;
		nickname = "432 " + tmp + " " + tmp +  " :Erroneous Nickname";
	}
	else if (pos != 0 && pos !=std::string::npos)
		nickname = nickname.substr(0,pos);
}

void ft_kick(std::vector<std::string> &args, int j, Channel *channels, glob * stru,Client *clients, int i)
{
    int ch_ind = searchBychannelname(args[j + 1], channels, stru->nm_channels);
    int cl_ind = searchByNickName(args[j + 2], clients, stru->num_clients);
    if (ch_ind == -1)
        sendUser("403 " + clients[i].nickname + " " + args[j + 1], clients[i].socket);
    else if (cl_ind == -1)
        sendUser("401 " + clients[i].nickname + " " +  args[j + 2] + " :No such nick/channel",clients[i].socket);
    else if(srch_clnt_chan(clients[i].socket,channels,ch_ind) == -1)
        sendUser("442 " + clients[i].nickname +  " " + args[j + 1] + " :You're not on that channel",clients[i].socket);
    else if(srch_clnt_chan(clients[cl_ind].socket,channels,ch_ind) == -1)
        sendUser("441 " + clients[i].nickname + " " + args[j + 2] + " " + args[j + 1] + " :They aren't on that channel",clients[i].socket);
    else if ((srch_is_operator(clients[i].nickname,clients[cl_ind].socket, channels,ch_ind) != -1))
    {
       for (size_t k = 0; k < channels[ch_ind].clients_sockets.size();k++)
           sendUser(":" + clients[i].nickname +  "!~" + clients[i].username + "@" + clients[i].username + " KICK " + args[j + 1] + " " + clients[cl_ind].nickname ,channels[ch_ind].clients_sockets[k]);
       if ((srch_is_operator(clients[i].nickname,clients[cl_ind].socket, channels,ch_ind)) != -1)
           remove_admin(clients[cl_ind].nickname,channels, ch_ind);
        expired_invite(clients[cl_ind].nickname,channels,ch_ind);
        removeClient(channels[ch_ind].clients_sockets, clients[cl_ind].socket);     
    }
    else if ((srch_is_operator(clients[i].nickname,clients[cl_ind].socket, channels,ch_ind) == -1))                                
        sendUser("482 " + clients[i].nickname + " " +   channels[ch_ind].name +  " :You're not a channel operator",clients[i].socket);
}
