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
    std::vector<int> clients_sockest;
    std::string name;

};

void ft_error(int ind, const std::string &str);
int countWords(const std::string& str);
void remove_spaces(std::string& str);
std::vector<std::string> split_str(std::string str, char sep);
// int checkArg(const std::string str, int clientSocket);
void errorUser(const std::string& msg, int clientSocket);
int checkArg(const std::vector<std::string> &arg, int clientSocket);
void sendUser(const std::string& msg, int clientSocket);
int searchByUsername(const std::string& target, const Client* clients, int numClients);
int searchBychannelname(const std::string &channel_name, const Channel* channels, int num_channels);
std::string addRandomNumber(const std::string& input);
int searchByNickName(const std::string& target, const Client* clients, int numClients);
