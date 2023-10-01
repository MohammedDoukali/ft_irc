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

        std::string nickname;
        std::string username;
};

class Channel{
    public:
    std::vector<int> clients_sockest;

};

void ft_error(int ind, const std::string &str);
int countWords(const std::string& str);
void remove_spaces(std::string& str);
std::vector<std::string> split_str(std::string str, char sep);
int countWords(const std::string& str);