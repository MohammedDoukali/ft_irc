#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
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
    std::vector<std::string> admins_users;
    std::string name;
    std::string topic;
    int index;
    int lmt;
    std::string limit;
    bool mode_i;
    bool mode_t;
    bool mode_l;
    bool mode_k;
    bool mode_o;
    std::vector<std::string> invited;

};

void ft_error(int ind, const std::string &str);
int countWords(const std::string& str);
void remove_spaces(std::string& str);
std::vector<std::string> split_str(std::string str, char sep);
// int checkArg(const std::string str, int clientSocket);
void errorUser(const std::string& msg, int clientSocket);
int checkArg(const std::vector<std::string> &arg, int clientSocket);
void sendUser(const std::string& msg, int clientSocket);
void sendUser2(const std::string& msg, int clientSocket, std::string name);
int searchByUsername(const std::string& target, const Client* clients, int numClients);
int searchBychannelname(const std::string &channel_name, const Channel* channels, int num_channels);
void create_channel(const std::string &channel_name);
std::string addRandomNumber(const std::string& input);
int searchByNickName(const std::string& target, const Client* clients, int numClients);
void create_channel(const int clientSocket,Channel *channels,const Client *clients, std::string name, const int i, int channel_index);
std::string extract_message(std::vector<std::string> args, int ind);
int srch_clnt_chan(const int clientSocket,const Channel* channels,int ind);
int srch_admin_users(std::string nickname,const Channel* channels);
void list_response(const Channel* channels,int clientSocket,int num_chan,std::string nickname);
int srch_is_operator (std::string nickname, const int clientSocket, const Channel* channels, int ind);
std::string get_modes(const Channel* channels,int ind);
int check_is_invited(std::string nickname, const Channel* channels,int ind_chan);