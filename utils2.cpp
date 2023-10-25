#include "lib.hpp"

void ft_nick(std::vector<std::string> args,int j,Client *clients,int i,Channel *channels,glob *stru)
{
     check_nickname(args[j + 1]);
    if (args[j + 1].find (' ') != std::string::npos)
            sendUser(args[j + 1],clients[i].socket);
    else if (clients[i].status == 0)
    {
        clients[i].tmp_nick = args[j + 1];
        clients[i].status = 1;
    }
    else if (clients[i].status == 3 && searchByNickName(args[j + 1], clients, stru->num_clients) != -1 &&  args[j + 1] != clients[i].nickname)
        sendUser("433 " + clients[i].nickname + " " +  args[j + 1] + " :Nickname is already in use\r\n", clients[i].socket);
    else if (clients[i].status == 3 && searchByNickName(args[j + 1], clients, stru->num_clients) == -1)
    {                          
            std::string old_one = clients[i].nickname;
            clients[i].nickname = args[j + 1];
            std::vector<int> infrm_users = inform_users(clients[i].socket, channels, stru->nm_channels);
            sendUser(":" + old_one + " NICK " + clients[i].nickname, clients[i].socket);
            for (int k = 0; k < stru->nm_channels; k++)
                chng_nick_admin(old_one, channels, k, clients[i].nickname);
            for (size_t k = 0; k < infrm_users.size(); k++)
            {
                if (clients[i].socket != infrm_users[k])    
                sendUser(":" + old_one + "!~" + clients[i].username + "@" + clients[i].username  + " NICK " + clients[i].nickname, infrm_users[k]);
            }
    }
    }

void ft_Auth(Client *clients,int i, glob *stru)
{
    std::cout << "Authentification Succes !!" << std::endl;
     std::string tmp = clients[i].tmp_nick;
    while (searchByNickName(clients[i].tmp_nick, clients, stru->num_clients) != -1)
        clients[i].tmp_nick += "_";
    clients[i].nickname = clients[i].tmp_nick;
    std::string response = ":ma_server_ma_Walo 001 " + clients[i].nickname + " :Welcome to the IRC server\r\n";
    sendUser(":" + tmp + " NICK :" + clients[i].nickname + "\r\n", clients[i].socket);
    sendUser(response.c_str(), clients[i].socket);
    sendUser("002 :Your Host Is ma_server_ma_Walo\r\n", clients[i].socket);
    sendUser("003 :This server was created gha db\r\n", clients[i].socket);
    sendUser("004 :", clients[i].socket);
    clients[i].status = 3;
}

void err_Auth(Client *clients,int i,glob *stru)
{
    std::cout << "Authentification Failed !" << std::endl;
    sendUser("464 " + clients[i].tmp_nick + " :Passowrd Incorrect : Are You A Hidden Thief !!\r\n", clients[i].socket);
    clients[i].username = "";
    clients[i].nickname = "";
    clients[i].socket = -1;
    clients[i].status = -1;
    stru->num_clients--;
}
void ft_msg(std::string &message, Client *clients, int i, std::vector<std::string> args, int j,glob *stru,Channel *channels)
{
    std::string mssg;
    size_t pos = message.find(':');
    if (pos != std::string::npos)
        mssg = message.substr(pos);
    size_t posi = mssg.find("PRIVMSG");
    mssg = mssg.substr(0,posi);                       
    std::string privmsgCommand = clients[i].nickname + " : " + mssg;
    int ind = searchByNickName(args[j + 1], clients, stru->num_clients);
    if (args[j + 1][0] == '#')
    {
        int ind2 = searchBychannelname(args[j + 1], channels, stru->nm_channels);
        if (ind2 == -1)
            sendUser("403 " + clients[i].nickname + " " + args[j + 2], clients[i].socket);
        else if(srch_clnt_chan(clients[i].socket,channels,ind2) == -1)
            sendUser("404 " + clients[i].nickname +  " " + args[j + 1] + " :Cannot send to channel",clients[i].socket);
        else
        {
        for (std::size_t k = 0; k < channels[ind2].clients_sockets.size(); k++)
        {
            if (channels[ind2].clients_sockets[k] != clients[i].socket)
                sendUser(":" + clients[i].nickname + " PRIVMSG " + channels[ind2].name + " :" + mssg, channels[ind2].clients_sockets[k]);
        }
        }
    }
    else if (ind != -1)
    {
        mssg = mssg.substr(0,mssg.find_last_of(" "));
        sendUser(":" + clients[i].nickname +  "!~" + clients[i].username + "@" + clients[i].username + " PRIVMSG " + args[j + 1] + " " +  mssg, clients[ind].socket);
    }
    else
        sendUser("401 " + clients[i].nickname + " " +  args[j + 1] + " :No such nick/channel",clients[i].socket);
}

void ft_invite(std::vector<std::string> args, Channel *channels, glob *stru, Client *clients, int i, int j)
{
     int ind_chan = searchBychannelname(args[j + 2], channels, stru->nm_channels);
     if (ind_chan == -1)
         sendUser("403 " + clients[i].nickname + " " + args[j + 2], clients[i].socket);
     else if (searchByNickName(args[j + 1], clients, stru->num_clients) == -1)
          sendUser("401 " + clients[i].nickname + " " +  args[j + 1] + " :No such nick/channel",clients[i].socket);
     else if (srch_is_operator(clients[i].nickname, clients[i].socket, channels, ind_chan) == -1)
         sendUser("482 " + clients[i].nickname + " " +  channels[ind_chan].name +  " :You're not a channel operator",clients[i].socket);
     else
     {
         sendUser(":" + clients[i].nickname + ". INVITE " + args[j + 1] + " :" + args[j + 2], clients[i].socket);
         sendUser("341 " + clients[i].nickname + " " + args[j + 1] + " " + args[j + 2], clients[i].socket);
         sendUser(":" + clients[i].nickname + " INVITE " + args[j + 1] + " :" + args[j + 2], clients[searchByNickName(args[j + 1], clients, stru->num_clients)].socket);
         channels[ind_chan].invited.push_back(args[j + 1]);
     }
}

void ft_mode(Channel *channels,std::vector<std::string> args,glob *stru, Client *clients,int i, int j)
{ 
    int ind_chan = searchBychannelname(args[j + 1], channels, stru->nm_channels);
    if (args.size() == 2 && ind_chan != -1)
    {
        std::string mode = get_modes(channels, ind_chan);
        if (channels[ind_chan].mode_l == true)
            mode += " " + channels[ind_chan].limit;
        if (channels[ind_chan].mode_k == true)
            mode += " " + channels[ind_chan].password;
        sendUser("324 " + clients[i].nickname + " " + args[j + 1] + mode, clients[i].socket);
    }
    else if (args[j + 1] == clients[i].nickname);
    else if (ind_chan == -1)
        sendUser("403 " + clients[i].nickname + " " + args[j + 1], clients[i].socket);
    else if (srch_is_operator(clients[i].nickname, clients[i].socket, channels, ind_chan) == -1)
        sendUser("482 " + clients[i].nickname + " " +  args[j + 1] +  " :You're not a channel operator",clients[i].socket);
    else if (args.size() > 2 && ind_chan != -1 && srch_is_operator(clients[i].nickname, clients[i].socket, channels, ind_chan) != -1)
    {
        if ((args[j + 2][0] == '+' || args[j + 2][0] == '-'))
        {
            std::string tmp = "";
            tmp += args[j + 2][0];
            for (size_t p = 1; p < args[j + 2].size(); p++)
            {
                if (args[j + 2][p] != 'i' && args[j + 2][p] != 't' && args[j + 2][p] != 'k' && args[j + 2][p] != 'l' && args[j + 2][p] != 'o')
                    sendUser("472 " + clients[i].nickname + " " + args[j + 2][p] + " :Unknown mode", clients[i].socket);
                else if ((args[j + 2][0] == '+' || args[j + 2][0] == '-') && args[j + 2][p] == 'l')
                {
                    size_t flag = 3;
                    std::string tmp_arg = args[j + 2].substr(1,p-1);
                    if (tmp_arg.find('k') != std::string::npos && tmp_arg.find('o') != std::string::npos)
                        flag += 2;
                    else if (tmp_arg.find('k') != std::string::npos || tmp_arg.find('o') != std::string::npos)
                        flag += 1;
                    if (args.size() > flag && args[j + 2][0] == '+')
                    {
                        std::istringstream limiter(args[j + flag]);
                        limiter >> channels[ind_chan].lmt;
                        channels[ind_chan].limit = args[j + flag];
                        channels[ind_chan].mode_l = true;
                        tmp += args[j + 2][p];
                    }
                    else if (args[j + 2][0] == '-')
                    {
                        channels[ind_chan].lmt = 100000;
                        channels[ind_chan].mode_l = false;
                        tmp += args[j + 2][p];
                    }
                    else if (args.size() <= flag && args[j + 2][0] == '+')
                        sendUser("461 " + clients[i].nickname + " +" + args[j + 2][p] + " :Not enough parameters", clients[i].socket);
                }
                else if ((args[j + 2][0] == '+' || args[j + 2][0] == '-') && args[j + 2][p] == 'k')
                {
                    size_t flag = 3;
                    std::string tmp_arg = args[j + 2].substr(1,p-1);
                    if (tmp_arg.find('l') != std::string::npos && tmp_arg.find('o') != std::string::npos)
                        flag += 2;
                    else if (tmp_arg.find('l') != std::string::npos || tmp_arg.find('o') != std::string::npos)
                        flag += 1;
                        
                    if (args.size() > flag && args[j + 2][0] == '+' && channels[ind_chan].mode_k == true)
                        sendUser("696 " + clients[i].nickname + " "  + args[j + 1] + " :Channel key already set",clients[i].socket);
                    else if (args.size() > flag && args[j + 2][0] == '+')
                    {
                        tmp += args[j + 2][p];
                        channels[ind_chan].mode_k = true;
                        channels[ind_chan].password = args[j + flag];
                    }
                    else if (args[j + 2][0] == '-')
                    {
                            channels[ind_chan].mode_k = false;
                            tmp += args[j + 2][p];
                    }
                    else if (args.size() <= flag && args[j + 2][0] == '+')
                        sendUser("461 " + clients[i].nickname + " +" + args[j + 2][p] + " :Not enough parameters", clients[i].socket);
                }
                else if ((args[j + 2][0] == '+' || args[j + 2][0] == '-') && args[j + 2][p] == 't')
                {
                    if (args[j + 2][0] == '-')
                        channels[ind_chan].mode_t = false;
                    else
                        channels[ind_chan].mode_t = true;
                    tmp += args[j + 2][p];
                }
                else if ((args[j + 2][0] == '+' || args[j + 2][0] == '-') && args[j + 2][p] == 'i')
                {
                    if (args[j + 2][0] == '-')
                        channels[ind_chan].mode_i = false;
                    else
                        channels[ind_chan].mode_i = true;
                    tmp += args[j + 2][p];
                }
                else if ((args[j + 2][0] == '+'  || args[j + 2][0] == '-' ) && args[j + 2][p] == 'o')
                {
                    size_t flag = 3;
                    std::string tmp_arg = args[j + 2].substr(1,p-1);
                    if (tmp_arg.find('k') != std::string::npos && tmp_arg.find('l') != std::string::npos)
                        flag += 2;
                    else if (tmp_arg.find('l') != std::string::npos || tmp_arg.find('k') != std::string::npos)
                        flag += 1;
                    if (args.size() > flag && searchByNickName(args[j + flag],clients,stru->num_clients) == -1)
                        sendUser("401 " + clients[i].nickname + " " +  args[j + flag] + " :No such nick/channel",clients[i].socket);
                    else if (args.size() > flag && srch_clnt_chan(clients[searchByNickName(args[j + flag], clients,stru->num_clients)].socket, channels,ind_chan) == -1)
                        sendUser("441 " + clients[i].nickname + " " + args[j + flag] + " " + args[j + 1] + " :They aren't on that channel",clients[i].socket);
                    else if ((args.size() <= flag && (args[j + 2][0] == '+')) || (args.size() <= 3 && (args[j + 2][0] == '-')) )
                        sendUser("461 " + clients[i].nickname + " +" + args[j + 2][p] + " :Not enough parameters", clients[i].socket);
                    else
                    {
                      if (args[j + 2][0] == '-' )
                      {
                       channels[ind_chan].tmp_oprt = args[j + 3];
                        remove_admin(args[j + 3],channels,ind_chan);
                      }
                      else
                      {
                       channels[ind_chan].admins_users.push_back(args[j + flag]);
                       channels[ind_chan].tmp_oprt = args[j + flag];
                      }
                      tmp += args[j + 2][p];
                    }
                }
                if (p == (args[j + 2].size() - 1)&& args.size() > 3 && (args[j + 2][0] == '+' || args[j + 2][0] == '-')
                    && ((tmp.find('k') != std::string::npos || tmp.find('o') != std::string::npos)
                    ||  (tmp.find('l') != std::string::npos && args[j + 2][0] == '+')))
                {
                    size_t l = tmp.find('l');
                    size_t k = tmp.find('k');
                    size_t o = tmp.find('o');
                    for ( int i = 0; i < 3; i++)
                    {
                        if (l < k && l < o  && tmp.find('l') != std::string::npos)
                        {
                            if(args[j + 2][0] == '+')
                            {
                                tmp += " ";
                                tmp += channels[ind_chan].limit;
                            }
                            l = std::string::npos;
                        }
                        else if (k < l && k < o && tmp.find('k') != std::string::npos)
                        {
                            tmp += " ";
                            tmp += channels[ind_chan].password;                                              
                            k = std::string::npos;
                        }
                        else if ( o < l && o < k && tmp.find('o') != std::string::npos)
                        {
                            tmp += " ";
                            if (args[j + 2][0] == '+')
                             tmp += channels[ind_chan].tmp_oprt;
                            else 
                            tmp += args[j + 3];
                            o = std::string::npos;
                        }
                    }
                }
            }
        if (tmp != "+" && tmp != "-")
            for (size_t k = 0; k < channels[ind_chan].clients_sockets.size(); k++)
            {
                sendUser(":" + clients[i].nickname + "!~" + clients[i].username + "@localhost MODE " + args[j + 1] + " " + tmp , channels[ind_chan].clients_sockets[k]);
            }           
    }
    else
        sendUser("472 " + clients[i].nickname + " " + args[j + 2] + " :Unknown mode", clients[i].socket);
    }
}

void ft_topic (std::vector <std::string> &args, Channel *channels, int j, glob *stru,Client *clients, int i)
{
    int ind_chan = searchBychannelname(args[j + 1], channels, MAX_CHANNELS);
    if (args.size() == 1)
        sendUser("461 " + clients[i].nickname + "TOPIC :Not enough parameters", clients[i].socket);
    else if ((args.size() > 2 && ind_chan != -1) && (channels[ind_chan].mode_t == false || (srch_is_operator(clients[i].nickname, clients[i].socket, channels, ind_chan) != -1 && channels[ind_chan].mode_t == true)))
    {
        std::string topic = extract_message(args, 2);
        channels[searchBychannelname(args[j + 1], channels, MAX_CHANNELS)].topic = topic;
        sendUser("332 " + clients[i].nickname + " " + args[j + 1] + " " + topic, clients[i].socket);
        for (int k = 0; k < stru->num_clients; k++)
        {
            for (std::size_t l = 0; l < channels[searchBychannelname(args[j + 1], channels, MAX_CHANNELS)].clients_sockets.size(); l++)
            {
                if (clients[k].socket == channels[searchBychannelname(args[j + 1], channels, MAX_CHANNELS)].clients_sockets[l])
                    sendUser(":" + clients[i].nickname + "!~" + clients[i].username + "@localhost  TOPIC " + channels[searchBychannelname(args[j + 1], channels, MAX_CHANNELS)].name + " " + topic, clients[k].socket);
            }
        }
    }
    else if ((channels[ind_chan].mode_t == true && srch_is_operator(clients[i].nickname, clients[i].socket, channels, ind_chan) == -1))
        sendUser("482 " + clients[i].nickname + " " +   channels[ind_chan].name +  " :You're not a channel operator",clients[i].socket);
    else if (args.size() >= 2 && searchBychannelname(args[j + 1], channels, MAX_CHANNELS) == -1)
        sendUser("403 " + clients[i].nickname + " " + args[j + 1], clients[i].socket);
}

void ft_join(std::vector<std::string> &args,int j,Channel *channels,glob *stru,Client *clients,int i)
{
     std::vector<std::string> chaines = multi_chaines(args[j + 1]);
    for (size_t len = 0;len < chaines.size();len++)
    {
        int ind_chan = searchBychannelname(chaines[len], channels, stru->nm_channels);
        if (ind_chan == -1 ||  channels[ind_chan].clients_sockets.size() == 0)
        {
            if (ind_chan == -1)
            {
                create_channel(channels, chaines[len],stru->nm_channels);
                ind_chan = stru->nm_channels;
                stru->nm_channels++;
            }
            else
                create_channel(channels, chaines[len],ind_chan);
            channels[ind_chan].admins_users.push_back(clients[i].nickname);
            sendUser("324 " + clients[i].nickname + " " + chaines[len] + " +", clients[i].socket);
        }
        if (channels[ind_chan].clients_sockets.size() >= channels[ind_chan].lmt)
            sendUser("471 " + clients[i].nickname + " " + chaines[len] + " :Cannot join channel (+l)", clients[i].socket);
        else if (channels[ind_chan].mode_i == true && check_is_invited(clients[i].nickname, channels, ind_chan))
            sendUser("473 " + clients[i].nickname + " " + chaines[len] + " :Cannot join channel (+i)", clients[i].socket);
        else if ((channels[ind_chan].mode_k == true && (args.size() < 3 || channels[ind_chan].password != args[j + 2])))
            sendUser("475 " + clients[i].nickname + " " + chaines[len] + " :Cannot join channel (+k)", clients[i].socket);
        else if ((channels[ind_chan].mode_k == false || (channels[ind_chan].mode_k == true && channels[ind_chan].password == args[j + 2])) && channels[ind_chan].clients_sockets.size() < channels[ind_chan].lmt)
        {
            channels[ind_chan].clients_sockets.push_back(clients[i].socket);
            if (channels[ind_chan].topic != "")
                sendUser("332 " + clients[i].nickname + " " + chaines[len] + " " + channels[ind_chan].topic, clients[i].socket);
            std::string test = " :";
            for (int k = 0; k < stru->num_clients; k++)
            {
                for (std::size_t l = 0; l < channels[ind_chan].clients_sockets.size(); l++)
                {
                    if (clients[k].socket == channels[ind_chan].clients_sockets[l])
                    {
                        if (srch_admin_users(clients[k].nickname, channels,ind_chan) != -1)
                            test+= "@";
                        test += clients[k].nickname + " ";
                    }
                }
            }
            sendUser("353 " + clients[i].nickname + " = " + chaines[len] + test, clients[i].socket);
            if (channels[ind_chan].clients_sockets.size() > 1)
            {
                for (std::size_t l = 0; l < channels[ind_chan].clients_sockets.size(); l++)
                {
                    sendUser(":" + clients[i].nickname + "!~" + clients[i].username + "@localhost JOIN " + args[j+1],channels[ind_chan].clients_sockets[l]);
                }
            }
            sendUser("366 " + clients[i].nickname + " " + chaines[len] + " :End of /NAMES list", clients[i].socket);
        }
    }
}

void ft_quit(glob *stru, Client *clients,int i, Channel *channels)
{
    for (int j = 0; j < stru->nm_channels; j++)
    {
        int flag = srch_clnt_chan(clients[i].socket,channels,j);                          
        if(flag != -1)
        {
            if ((srch_is_operator(clients[i].nickname,clients[i].socket, channels,j)) != -1)    
                remove_admin(clients[i].nickname,channels,j);
            removeClient(channels[j].clients_sockets, clients[i].socket);
            for (size_t k = 0; k < channels[j].clients_sockets.size(); k++)
                sendUser(":" + clients[i].nickname + "!~" + clients[i].username + "@localhost QUIT " + channels[j].name, channels[j].clients_sockets[k]);
        }   
    }
}

void ft_part(std::vector<std::string> &args, Client *clients, int i, glob *stru, Channel *channels, int j)
{
    int ch_ind = searchBychannelname(args[j + 1], channels, stru->nm_channels);
    if (ch_ind == -1)
        sendUser("403 " + clients[i].nickname + " " + args[j + 1], clients[i].socket);
    else if (srch_clnt_chan(clients[i].socket, channels, ch_ind) == -1)
        sendUser("442 " + clients[i].nickname + " " + args[j + 1] + " :You're not on that channel", clients[i].socket);
    else
    {
        for (size_t k = 0; k < channels[ch_ind].clients_sockets.size(); k++)
                sendUser(":" + clients[i].nickname + "!~" + clients[i].username + "@localhost PART " + args[j + 1],channels[ch_ind].clients_sockets[k]);
        if ((srch_is_operator(clients[i].nickname, clients[i].socket, channels, ch_ind)) != -1)
            remove_admin(clients[i].nickname, channels, ch_ind);
        expired_invite(clients[i].nickname,channels,ch_ind);
        removeClient(channels[ch_ind].clients_sockets, clients[i].socket);
        if (channels[ch_ind].clients_sockets.size() == 0)
           channels[ch_ind].index = -1;
    }
}