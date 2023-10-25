#include "lib.hpp"
Channel channels[MAX_CHANNELS];
Client clients[MAX_CLIENTS];

int check_port(std::string str)
{
    int port;
    std::istringstream nm(str);
    if (nm >> port && nm.eof() && (port < 1024 || port > 65535))
        ft_error(0, "Port Number Must Be An Integer Between 1024 And 65535.");
    else if (!nm.eof())
        ft_error(0, "The Port Must Be Integer Value !!");
    return port;
}

void connect_server_client(glob *stru)
{
    int max_fds_socket;
    char buffer[MAX_BUFFER_SIZE];
    stru->num_clients = 0;
    stru->nm_channels = 0;

    for (int i = 0; i < MAX_CLIENTS; ++i)
        clients[i].socket = -1;
    fd_set readfds;
    max_fds_socket = stru->serverSocket;

    while (true)
    {
        FD_ZERO(&readfds);
        FD_SET(stru->serverSocket, &readfds);

        // Add child sockets to set
        for (int i = 0; i < MAX_CLIENTS; ++i)
        {
            if (clients[i].socket != -1)
            {
                FD_SET(clients[i].socket, &readfds);
                max_fds_socket = std::max(max_fds_socket, clients[i].socket);
            }
        }

        // Wait for activity on one of the sockets
        int activity = select(max_fds_socket + 1, &readfds, NULL, NULL, NULL);
        if ((activity < 0))
            ft_error(0, "Select Failed");

        // If something happened on the server socket, then it's an incoming connection
        if (FD_ISSET(stru->serverSocket, &readfds))
        {
            struct sockaddr_in cl_addr;
            socklen_t cl_addr_len = sizeof(cl_addr);
            int clientSocket = accept(stru->serverSocket, (struct sockaddr *)&cl_addr, &cl_addr_len);
            if (clientSocket < 0)
                ft_error(0, "Accept Connection Failed");
            int ind = -1;
            for (int i = 0; i < MAX_CLIENTS; ++i)
            {
                if (clients[i].socket == -1)
                {
                    ind = i;
                    break;
                }
            }
            if (ind == -1)
            {
                close(clientSocket);
                std::cout << "Rejected new connection: Too many clients" << std::endl;
            }
            else
            {
                clients[ind].socket = clientSocket;
                std::cout << "New client Detected < " << ind << " >: With Ip : " <<  inet_ntoa(cl_addr.sin_addr) << std::endl;
                max_fds_socket = std::max(max_fds_socket, clientSocket);
                clients[ind].tmp_nick = "tmp_nick";
                clients[ind].status = -1;
                clients[ind].nickname = "";
                stru->num_clients++;
            }
        }

        // Check each client for activity

        for (size_t i = 0; i < MAX_CLIENTS; ++i)
        {
            int clientSocket = clients[i].socket;
            if (clientSocket != -1 && FD_ISSET(clientSocket, &readfds))
            {
                int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                if (bytesRead > 0)
                {
                    buffer[bytesRead] = '\0';
                    std::string message(buffer);
                    remove_spaces(message);
                    std::vector<std::string> args = split_str(message, ' ');

                    for (size_t j = 0; j < args.size() - 1; j++)
                    {
                        if (args[j] == "CAP")
                            sendUser("CAP * LS", clientSocket);
                        if (args[j] == "PASS" && args[j + 1] == stru->password)
                        {
                            sendUser("You Are Successfully Registered", clientSocket);
                            clients[i].status = 0;
                        }
                        if (clients[i].status == 1)
                           ft_Auth(clients,i,stru);
                        if (args.size() > 1 && args[j] == "NICK")
                            ft_nick(args,j,clients,i,channels,stru);
                        if (args[j] == "PASS" && args[j + 1] != stru->password)
                        {
                            err_Auth(clients,i,stru);
                            close(clientSocket);
                        }
                        if (args[j] == "USER")
                                clients[i].username = args[j + 1];
                        if (args[j] == "PING")
                            sendUser("PONG\r\n", clientSocket);
                        if (args[j] == "PRIVMSG")
                           ft_msg(message,clients,i,args,j,stru,channels);
                        if (args[j] == "LIST")
                            list_response(channels, clientSocket, stru->nm_channels, clients[i].nickname);
                        if (args[j] == "INVITE")
                           ft_invite(args,channels,stru,clients,i,j);
                        if (args[j] == "MODE")
                           ft_mode(channels,args,stru,clients,i,j);
                        if (args[j] == "TOPIC")
                            ft_topic(args,channels,j,stru,clients,i);
                        if (args[j] == "JOIN" && args[j + 1][0] == '#')
                            ft_join(args,j,channels,stru,clients,i);
                        if (args[j] == "QUIT")
                           ft_quit(stru,clients,i,channels);
                        if (args[j] == "PART")
                           ft_part(args,clients, i, stru, channels, j);
                        if (args[j] == "KICK")
                            ft_kick(args,j,channels,stru, clients, i);
                    }
                }
                else if (bytesRead == 0)
                {
                    std::cout << "Client " << i << " disconnected.\n";   
                    close(clientSocket);
                    clients[i].socket = -1;
                    clients[i].status = -1;
                    clients[i].nickname = "";
                    clients[i].username = "";
                    stru->num_clients--;
                }
                else
                {
                    std::cerr << "Error in recv from client " << i << ".\n";
                    close(clientSocket);
                    clients[i].socket = -1;
                }
            }
        }
    }
}

void setup_socket(int port, glob *stru)
{
    // Create A socket //
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    stru->serverSocket = serverSocket;

    if (serverSocket == -1)
        ft_error(0, "Failed to create socket !!");

    // Configure Server Adress //
    struct sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr)); // initialize struct
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
        ft_error(0, "Binding failed\n");

    if (listen(serverSocket, SOMAXCONN) == -1)
        ft_error(0, "Listen failed\n");

    std::cout << "Server listening.. On Port " << port << std::endl;

    stru->serverAddr = serverAddr;
    fcntl(serverSocket, F_SETFL, O_NONBLOCK);
}
int main(int ac, char **av)
{
    glob str;
    if (ac == 3)
    {
        int port = check_port(av[1]);
        str.password = av[2];
        setup_socket(port, &str);
        connect_server_client(&str);
    }
    else
        std::cerr << "Error : Number of Arguments Invalid !! " << std::endl;
}