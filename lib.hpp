#ifndef LIB_HPP
#define LIB_HPP
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fcntl.h>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cerrno>
#include <vector>
#include <map>
#include <arpa/inet.h>
#include <ctime>

const int MAX_CLIENTS = FD_SETSIZE - 1;
const int MAX_CHANNELS = 1023;
const int MAX_BUFFER_SIZE = 1024;


struct glob
{
    struct sockaddr_in serverAddr;
    int serverSocket;
    std::string password;
    int num_clients;
    int nm_channels;
};
 class Client {
   public:
        int socket;
        int status;
        std::string nickname;
        std::string username;
        std::string tmp_nick;

};

class Channel{
    public:
    std::vector<int> clients_sockets;
    mutable std::vector<std::string> admins_users;
    std::string name;
    std::string topic;
    std::string password;
    int index;
    size_t lmt;
    std::string limit;
    std::string tmp_oprt;
    bool mode_i;
    bool mode_t;
    bool mode_l;
    bool mode_k;
    mutable std::vector<std::string> invited;

};

// UTILS // 
void ft_error(int ind, const std::string &str);
void remove_spaces(std::string& str);
std::vector<std::string> split_str(std::string str, char sep);
void sendUser(const std::string& msg, int clientSocket);
int searchByUsername(const std::string& target, const Client* clients, int numClients);
int searchBychannelname(const std::string &channel_name, const Channel* channels, int num_channels);
int searchByNickName(const std::string& target, const Client* clients, int numClients);
void create_channel(Channel *channels, std::string name, int channel_index);
std::string extract_message(std::vector<std::string> args, int ind);
int srch_clnt_chan(const int clientSocket,const Channel* channels,int ind);
int srch_admin_users(std::string nickname,const Channel* channels,int ind);
int srch_is_operator (std::string nickname, const int clientSocket, const Channel* channels, int ind);
void list_response(const Channel* channels,int clientSocket,int num_chan,std::string nickname);
std::string get_modes(const Channel* channels,int ind);
int check_is_invited(std::string nickname, const Channel* channels,int ind_chan);
void remove_admin(std::string nickname, const Channel * channels,int ind);
void removeClient(std::vector<int>& clients_sockets, int clientSocket);
void chng_nick_admin(std::string nickname, const Channel * channels,int ind,std::string new_nick);
int srch_vctor_int(const int clientSocket,std::vector<int> users);
std::vector<int> inform_users(const int clientSocket,Channel *channels, int num_channel);
std::vector<std::string> multi_chaines(const std::string& input);
void expired_invite(std::string nickname, const Channel * channels,int ind);
void check_nickname(std::string& nickname);
// CMDS // 
void ft_Auth(Client *clients,int i, glob *stru);
void err_Auth(Client *clients,int i,glob *stru);
void ft_nick(std::vector<std::string> args,int j,Client *clients,int i,Channel *channels,glob *stru);
void ft_msg(std::string &message, Client *clients, int i, std::vector<std::string> args, int j,glob *stru,Channel *channels);
void ft_invite(std::vector<std::string> args, Channel *channels, glob *stru, Client *clients, int i, int j);
void ft_mode(Channel *channels,std::vector<std::string> args,glob *stru, Client *clients,int i, int j);
void ft_topic (std::vector <std::string> &args, Channel *channels, int j, glob *stru,Client *clients, int i);
void ft_join(std::vector<std::string> &args,int j,Channel *channels,glob *stru,Client *clients,int i);
void ft_quit(glob *stru, Client *clients,int i, Channel *channels);
void ft_part(std::vector<std::string> &args, Client *clients, int i, glob *stru, Channel *channels, int j);
void ft_kick(std::vector<std::string> &args, int j, Channel *channels, glob * stru,Client *clients, int i);

#endif