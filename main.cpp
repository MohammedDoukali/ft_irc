#include "lib.hpp"
// 4 octobre //
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
    int chan_indx = 0;

    stru->num_clients = 0;
    stru->nm_channels = 0;

    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        clients[i].socket = -1;
        // clients[i].status = -1;
        // clients[i].nickname = "MIKE";
        // clients[i].username = "";
    }
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
                std::cout << "New client connected: " << inet_ntoa(cl_addr.sin_addr) << std::endl;
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
                    std::cout << "Message from client " << i << ": " << message << "\n";
                    std::vector<std::string> args = split_str(message, ' ');

                    for (int j = 0; j < args.size() - 1; j++)
                    {
                        if (args[j] == "CAP")
                            sendUser("CAP * LS", clientSocket);
                        if (args[j] == "PASS" && args[j + 1] == stru->password)
                        {
                            sendUser("You Are Successfully Registered", clientSocket);
                            clients[i].status = 0;
                        }
                        if (clients[i].status == 1)
                        {
                            std::string tmp = clients[i].tmp_nick;
                            while (searchByNickName(clients[i].tmp_nick, clients, stru->num_clients) != -1)
                                clients[i].tmp_nick += "_";

                            clients[i].nickname = clients[i].tmp_nick;
                            std::string response = ":ma_server_ma_Walo 001 " + clients[i].nickname + " :Welcome to the IRC server\r\n";
                            // std::cout << response ;
                            sendUser(":" + tmp + " NICK :" + clients[i].nickname + "\r\n", clientSocket);
                            sendUser(response.c_str(), clientSocket);
                            sendUser("002 :Your Host Is ma_server_ma_Walo\r\n", clientSocket);
                            sendUser("003 :This server was created gha db\r\n", clientSocket);
                            sendUser("004 :", clientSocket);
                            clients[i].status = 3;
                        }
                        if (args.size() > 1 && args[j] == "NICK")
                        {
                            if (clients[i].status == 0)
                            {
                                clients[i].tmp_nick = args[j + 1];
                                clients[i].status = 1;
                            }
                            else if (clients[i].status == 3 && searchByNickName(args[j + 1], clients, stru->num_clients) != -1)
                                sendUser(":ma_server_ma_Walo 433 client_d " + args[j + 1] + " :Nickname is already in use\r\n", clientSocket);

                            else if (clients[i].status == 3 && searchByNickName(args[j + 1], clients, stru->num_clients) == -1)
                            {
                                std::string old_one = clients[i].nickname;
                                clients[i].nickname = args[j + 1];
                                sendUser(":" + old_one + " NICK :" + args[j + 1] + "\r\n", clientSocket);
                                std::cout << "donc dkhel hna:" << clients[i].nickname << " status:" << clients[i].status << std::endl;
                            }
                        }
                        if (args[j] == "PASS" && args[j + 1] != stru->password)
                        {
                            sendUser(":ma_server_ma_Walo 464 simo :Error : Passowrd Incorrect : Are You A Hidden Thief !!\r\n", clientSocket);
                            clients[i].username = "";
                            clients[i].nickname = "";
                            clients[i].socket = -1;
                            clients[i].status = -1;
                            stru->num_clients--;
                            // sendUser("QUIT",clientSocket);
                            close(clientSocket);
                            // break;
                        }
                        if (args[j] == "USER")
                        {
                            if (searchByUsername(args[j + 1], clients, stru->num_clients) != -1)
                                args[j + 1] = addRandomNumber(args[j + 1]);

                            if (searchByUsername(args[j + 1], clients, MAX_CLIENTS) == -1)
                            {
                                clients[i].username = args[j + 1];
                                sendUser("You're Username is : " + clients[i].username, clientSocket);
                            }
                        }
                        if (args[j] == "PING")
                            sendUser("PONG\r\n", clientSocket);
                        if (args[j] == "PRIVMSG")
                        {
                            std::cout << "sit tl3ab" << std::endl;
                            std::string mssg;
                            size_t pos = message.find(':');
                            if (pos != std::string::npos)
                                mssg = message.substr(pos + 1);
                            std::string privmsgCommand = clients[i].nickname + " : " + mssg;
                            int ind = searchByNickName(args[j + 1], clients, stru->num_clients);
                            if (args[j + 1][0] == '#')
                            {
                                int ind2 = searchBychannelname(args[j + 1], channels, MAX_CHANNELS);
                                for (std::size_t k = 0; k < channels[ind2].clients_sockets.size(); k++)
                                {
                                    std::cout << " chan " << channels[ind2].clients_sockets.size() << std::endl;
                                    if (channels[ind2].clients_sockets[k] != clientSocket)
                                        // sendUser2(privmsgCommand, channels[ind2].clients_sockets[k],channels[ind2].name);
                                        sendUser(":" + clients[i].nickname + " PRIVMSG " + channels[ind2].name + " :" + mssg, channels[ind2].clients_sockets[k]);
                                }
                            }
                            else if (ind != -1)
                                sendUser(":" + clients[i].nickname + " PRIVMSG " + clients[i].nickname + " :" + mssg, clients[ind].socket);
                            else
                                sendUser("411 User: " + args[j + 1] + " Not Found !\r\n", clientSocket);
                        }
                        if (args[j] == "LIST")
                            list_response(channels, clientSocket, stru->nm_channels, clients[i].nickname);
                        if (args[j] == "INVITE")
                        {
                            int ind_chan = searchBychannelname(args[j + 2], channels, stru->nm_channels);
                            if (ind_chan == -1)
                                sendUser("403 " + clients[i].nickname + " " + args[j + 2], clientSocket);
                            else if (searchByNickName(args[j + 1], clients, stru->num_clients) == -1)
                                sendUser("411 User: " + args[j + 1] + " Not Found !\r\n", clientSocket);
                            else if (srch_is_operator(clients[i].nickname, clientSocket, channels, ind_chan) == -1)
                                sendUser(":! NOTICE " + args[j + 2] + " :" + channels[ind_chan].name + " :You're not channel operator ", clientSocket);
                            else
                            {
                                sendUser(":" + clients[i].nickname + ". INVITE " + args[j + 1] + " :" + args[j + 2], clientSocket);
                                sendUser("341 " + clients[i].nickname + " " + args[j + 1] + " " + args[j + 2], clientSocket);
                                sendUser(":" + clients[i].nickname + " INVITE " + args[j + 1] + " :" + args[j + 2], clients[searchByNickName(args[j + 1], clients, stru->num_clients)].socket);
                                channels[ind_chan].invited.push_back(args[j + 1]);
                                // sendUser(clients[i].nickname + " invitees you to " + args[j + 2],clients[searchByNickName(args[j + 1],clients,stru->num_clients)].socket);
                            }
                        }
                        if (args[j] == "MODE")
                        {
                            int ind_chan = searchBychannelname(args[j + 1], channels, stru->nm_channels);
                            if (args.size() == 2 && ind_chan != -1)
                            {
                                std::string mode = get_modes(channels, ind_chan);
                                if (channels[ind_chan].mode_l == true)
                                    mode += " " + channels[ind_chan].limit;
                                if (channels[ind_chan].mode_k == true)
                                    mode += " " + channels[ind_chan].password;
                                sendUser("324 " + clients[i].nickname + " " + args[j + 1] + mode, clientSocket);
                            }
                            if (ind_chan == -1)
                                sendUser("403 " + clients[i].nickname + " " + args[j + 1], clientSocket);
                            else if (srch_is_operator(clients[i].nickname, clients[i].socket, channels, ind_chan) == -1)
                            {
                                std::cout << "aykoon dkheel hna" << std::endl;
                                // sendUser("481 " + clients[i].nickname + " :Permission Denied- You're not an IRC operator",clientSocket);
                                // sendUser(":" + clients[i].nickname + " 482 " +  args[j + 1] +  " :" +  args[j + 1] + " :You're not a channel operator",clientSocket);
                                sendUser(":! NOTICE " + args[j + 1] + " :" + channels[ind_chan].name + " :You're not channel operator ", clientSocket);
                            }
                            else if (args.size() > 2 && ind_chan != -1 && srch_is_operator(clients[i].nickname, clients[i].socket, channels, ind_chan) != -1)
                            {
                                if ((args[j + 2][0] == '+' || args[j + 2][0] == '-'))
                                {
                                    std::string tmp = "";
                                    tmp += args[j + 2][0];
                                    for (size_t p = 1; p < args[j + 2].size(); p++)
                                    {
                                        if (args[j + 2][p] != 'i' && args[j + 2][p] != 't' && args[j + 2][p] != 'k' && args[j + 2][p] != 'l' && args[j + 2][p] != 'o')
                                            sendUser("472 " + clients[i].nickname + " " + args[j + 2][p] + " :Unknown mode", clientSocket);
                                        // else if (tmp.find(args[j + 2][p]) == std::string::npos)
                                        //     tmp += args[j + 2][p];
                                        else if ((args[j + 2][0] == '+' || args[j + 2][0] == '-') && args[j + 2][p] == 'l')
                                        {
                                            if (args.size() == 4 && args[j + 2][0] == '+')
                                            {
                                                std::istringstream limiter(args[j + 3]);
                                                limiter >> channels[ind_chan].lmt;
                                                channels[ind_chan].limit = args[j + 3];
                                                channels[ind_chan].mode_l = true;
                                                tmp += args[j + 2][p];
                                            }
                                            else if (args[j + 2][0] == '-')
                                            {
                                                channels[ind_chan].lmt = 100000;
                                                channels[ind_chan].mode_l = false;
                                                tmp += args[j + 2][p];
                                            }
                                            else if (args.size() == 3 && args[j + 2][0] == '+')
                                                sendUser("461 " + clients[i].nickname + " +" + args[j + 2][p] + " :Not enough parameters", clientSocket);
                                        }
                                        else if ((args[j + 2][0] == '+' || args[j + 2][0] == '-') && args[j + 2][p] == 'k')
                                        {
                                            if (args.size() == 4 && args[j + 2][0] == '+' && channels[ind_chan].mode_k == true)
                                                sendUser("696 " + clients[i].nickname + " "  + args[j + 1] + " :Channel key already set",clientSocket);
                                            else if (args.size() == 4 && args[j + 2][0] == '+')
                                            {
                                                tmp += args[j + 2][p];
                                                channels[ind_chan].mode_k = true;
                                                channels[ind_chan].password = args[j + 3];
                                            }
                                            else if (args[j + 2][0] == '-')
                                            {
                                                    std::cout << "yakma dkhlt hna" << std::endl;
                                                    channels[ind_chan].mode_k = false;
                                                    tmp += args[j + 2][p];
                                            }
                                            else if (args.size() == 3 && args[j + 2][0] == '+')
                                                sendUser("461 " + clients[i].nickname + " +" + args[j + 2][p] + " :Not enough parameters", clientSocket);
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
                                   
                                            if (args.size() == 4 && searchByNickName(args[j + 3],clients,stru->num_clients) == -1)
                                                sendUser("401 " + clients[i].nickname + " " +  args[j + 3] + " :No such nick/channel",clientSocket);
                                            else if (args.size() == 4 && srch_clnt_chan(clients[searchByNickName(args[j + 3], clients,stru->num_clients)].socket, channels,ind_chan) == -1)
                                                sendUser("441 " + clients[i].nickname + " " + args[j + 3] + " " + args[j + 1] + " :They aren't on that channel",clientSocket);
                                            else if (args.size() == 3 && (args[j + 2][0] == '+' || args[j + 2][0] == '-'))
                                                sendUser("461 " + clients[i].nickname + " +" + args[j + 2][p] + " :Not enough parameters", clientSocket);
                                            else
                                            {
                                               channels[ind_chan].admins_users.push_back(args[j + 3]);
                                               tmp += args[j + 2][p];
                                            }
                                        }
                                        if (p == (args[j + 2].size() - 1)&& args.size() > 3 && (args[j + 2][0] == '+' || args[j + 2][0] == '-')
                                            && ((tmp.find('k') != std::string::npos || tmp.find('o') != std::string::npos)
                                            ||  (tmp.find('l') != std::string::npos && args[j + 2][0] == '+'))
                                        )
                                        {
                                            tmp += " ";
                                            tmp += args[j + 3];
                                            std::cout << args[j + 3] << std::endl;
                                            
                                        }
                                    }
                                    if (tmp != "+" && tmp != "-")

                                        for (size_t k = 0; k < channels[ind_chan].clients_sockets.size(); k++)
                                        {
                                            // if ((tmp.find('l') == std::string::npos) && (tmp.find('o') == std::string::npos) && (tmp.find('k') == std::string::npos))
                                            sendUser(":<" + clients[i].nickname + "> MODE " + args[j + 1] + " " + tmp , channels[ind_chan].clients_sockets[k]);
                                        // else
                                        //     sendUser(":<" + clients[i].nickname + "> MODE " + args[j + 1] + " " + tmp + " " + args[j + 3], channels[ind_chan].clients_sockets[k]);
                                            // sendUser("324 " + clients[i].nickname + " " + args[j + 1] + " " + tmp, channels[ind_chan].clients_sockets[k]);
                                        }
                             
                                }
                                else
                                {
                                    // sendUser(":ma_server_ma_Walo 324 zwita " + args[j + 1] + " " + args[j + 2],clientSocket);
                                    sendUser("472 " + clients[i].nickname + " " + args[j + 2] + " :Unknown mode", clientSocket);
                                }
                            }
                        }
                        if (args[j] == "WHO")
                        {
                            std::cout << " dkheel who" << std::endl;
                            sendUser("352 zwita #chan ~simo-dk simo-dk ma_server_ma_Walo zwita O :1 mohammed", clientSocket);
                            sendUser("315 zwita #chan :End of /WHO list", clientSocket);
                        }
                        if (args[j] == "TOPIC")
                        {
                            std::cout << args.size() << std::endl;
                            int ind_chan = searchBychannelname(args[j + 1], channels, MAX_CHANNELS);
                            if ((args.size() > 2 && ind_chan != -1) && (channels[ind_chan].mode_t == false || (srch_is_operator(clients[i].nickname, clientSocket, channels, ind_chan) != -1 && channels[ind_chan].mode_t == true)))
                            {
                                std::string topic = extract_message(args, 2);
                                channels[searchBychannelname(args[j + 1], channels, MAX_CHANNELS)].topic = topic;
                                sendUser("332 " + clients[i].nickname + " " + args[j + 1] + " " + topic, clientSocket);
                                //    sendUser(":ma_server_ma_Walo sets mode: +t #chan New Topic " + topic, clientSocket);
                                for (int k = 0; k < stru->num_clients; k++)
                                {
                                    for (std::size_t l = 0; l < channels[searchBychannelname(args[j + 1], channels, MAX_CHANNELS)].clients_sockets.size(); l++)
                                    {
                                        if (clients[k].socket == channels[searchBychannelname(args[j + 1], channels, MAX_CHANNELS)].clients_sockets[l])
                                            sendUser(":<" + clients[i].nickname + "> TOPIC " + channels[searchBychannelname(args[j + 1], channels, MAX_CHANNELS)].name + " " + topic, clients[k].socket);
                                    }
                                }
                            }
                            else if ((channels[ind_chan].mode_t == true && srch_is_operator(clients[i].nickname, clientSocket, channels, ind_chan) == -1))
                                sendUser(":! NOTICE " + args[j + 1] + " :" + channels[ind_chan].name + " :You're not channel operator ", clientSocket);
                            else if (args.size() >= 2 && searchBychannelname(args[j + 1], channels, MAX_CHANNELS) == -1)
                                sendUser("403 " + clients[i].nickname + " " + args[j + 1], clientSocket);
                            // else if (args.size() >= 2 && searchBychannelname(args[j + 1], channels, MAX_CHANNELS) != -1 && srch_clnt_chan(clientSocket,channels) == -1)
                            // sendUser(":ma_server_ma_Walo 442 " + clients[i].nickname + " " + args[j + 1] + " :You're not on that channel",clientSocket);

                            else if (args.size() == 1)
                            {
                                sendUser("*** TOPIC Not enough parameters", clientSocket);
                                std::cout << "Dsdsddsd" << std::endl;
                            }
                        }

                        if (args[j] == "JOIN" && args[j + 1][0] == '#')
                        {
                            int ind_chan = searchBychannelname(args[j + 1], channels, stru->nm_channels);
                            std::cout << "hadi nm channel : " << stru->nm_channels << std::endl;
                            if (ind_chan == -1)
                            {
                                create_channel(clientSocket, channels, clients, args[j + 1], i, stru->nm_channels);
                                ind_chan = stru->nm_channels;
                                stru->nm_channels++;
                            }
                            if (channels[ind_chan].lmt != -1 && channels[ind_chan].clients_sockets.size() >= channels[ind_chan].lmt)
                            {
                                std::cout << "limit mn dakhel : " << channels[ind_chan].lmt << " size:" << channels[ind_chan].clients_sockets.size() << std::endl;
                                sendUser("471 " + clients[i].nickname + " " + args[j + 1] + " :Cannot join channel (+i)", clientSocket);
                            }
                            else if (channels[ind_chan].mode_i == true && check_is_invited(clients[i].nickname, channels, ind_chan))
                            {
                                sendUser("473 " + clients[i].nickname + " " + args[j + 1] + " :Cannot join channel (+l)", clientSocket);
                            }
                            else if ((channels[ind_chan].mode_k == true && (args.size() < 3 || channels[ind_chan].password != args[j + 2])))
                                sendUser("475 " + clients[i].nickname + " " + args[j + 1] + " :Cannot join channel (+k)", clientSocket);
                            else if ((channels[ind_chan].mode_k == false || (channels[ind_chan].mode_k == true && channels[ind_chan].password == args[j + 2])) && (channels[ind_chan].lmt == -1 || (channels[ind_chan].lmt > -1 && channels[ind_chan].clients_sockets.size() < channels[ind_chan].lmt)))
                            {
                                std::cout << "wax dkhlti hna-----------------" << std::endl;
                                channels[ind_chan].clients_sockets.push_back(clients[i].socket);
                                if (channels[ind_chan].topic != "")
                                    sendUser("332 " + clients[i].nickname + " " + args[j + 1] + " " + channels[ind_chan].topic, clientSocket);
                                //   sendUser(":ma_server_ma_Walo JOIN " + args[i + 1], clientSocket);

                                for (int k = 0; k < stru->num_clients; k++)
                                {
                                    for (std::size_t l = 0; l < channels[ind_chan].clients_sockets.size(); l++)
                                    {
                                        if (clients[k].socket == channels[ind_chan].clients_sockets[l])
                                        {
                                            std::string tmp = " :";
                                            if (srch_admin_users(clients[k].nickname, channels) != -1)
                                                tmp += "@";
                                            sendUser("353 " + clients[k].nickname + " = " + args[j + 1] + tmp + clients[k].nickname, clientSocket); // status of user in channel
                                        }
                                    }
                                }
                                for (std::size_t l = 0; l < channels[ind_chan].clients_sockets.size(); l++)
                                {
                                    std::cout << " chan " << channels[ind_chan].clients_sockets.size() << std::endl;
                                    sendUser2(clients[i].nickname + " Has Joigned " + channels[ind_chan].name, channels[ind_chan].clients_sockets[l], channels[ind_chan].name);
                                    // sendUser(":ma_server_ma_Walo JOIN " + args[i + 1],channels[ind_chan].clients_sockets[l]);
                                }
                                sendUser(":ma_server_ma_Walo 366 " + clients[i].nickname + " " + args[j + 1] + " :End of JOIN", clientSocket);
                                // sendUser(":ma_server_ma_waloo 353 "  + clients[i].nickname + " = " + args[j + 1] + " :@username1 username2 username3",clientSocket);
                            }
                        }
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