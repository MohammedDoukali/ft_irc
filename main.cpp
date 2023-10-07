#include "lib.hpp"
// 4 octobre // 
Channel channels[MAX_CHANNELS];
Client clients[MAX_CLIENTS];

int  check_port(std::string str)
{
    int port;
    std::istringstream nm(str);
    if (nm >> port && nm.eof() && (port < 1024 || port > 65535))
        ft_error(0, "Port Number Must Be An Integer Between 1024 And 65535.");
    else if (!nm.eof())
        ft_error(0,"The Port Must Be Integer Value !!");
    return port;
}

void connect_server_client(glob *stru)
{
    int max_fds_socket;
    char buffer[MAX_BUFFER_SIZE];
    int chan_indx = 0;

    for(int i = 0;i < MAX_CLIENTS;++i)
    {
        clients[i].socket = -1;
     //   clients[i].status = -1;
        // clients[i].nickname = "MIKE";
        // clients[i].username = "";
    }
    fd_set readfds;
    max_fds_socket = stru->serverSocket;

    while (true) {
    FD_ZERO(&readfds);
    FD_SET(stru->serverSocket, &readfds);

    // Add child sockets to set
    for (int i = 0; i < MAX_CLIENTS; ++i) {
         if (clients[i].socket != -1)
           {
            FD_SET(clients[i].socket, &readfds);
			max_fds_socket = std::max(max_fds_socket, clients[i].socket);
           }
    }
    
    // Wait for activity on one of the sockets
    int activity = select(max_fds_socket + 1, &readfds, NULL, NULL, NULL);
    if ((activity < 0))
        ft_error(0,"Select Failed");

    // If something happened on the server socket, then it's an incoming connection
    if (FD_ISSET(stru->serverSocket, &readfds))
    {
        struct sockaddr_in cl_addr;
        socklen_t cl_addr_len = sizeof(cl_addr);
		int clientSocket = accept(stru->serverSocket, (struct sockaddr*)&cl_addr, &cl_addr_len);
        if (clientSocket  < 0)
            ft_error(0,"Accept Connection Failed");
        
        int ind = -1;
		for (int i = 0; i < MAX_CLIENTS; ++i) {
			if (clients[i].socket == -1) {
				ind = i;
				break;
			}
		}

        if (ind == -1){
		    close(clientSocket);
		    std::cout << "Rejected new connection: Too many clients" << std::endl;
        }
        else
        {
			clients[ind].socket = clientSocket;
			std::cout << "New client connected: " << inet_ntoa(cl_addr.sin_addr) << std::endl;
            max_fds_socket = std::max(max_fds_socket, clientSocket);
            clients[ind].tmp_nick = "tmp_nick";
            clients[ind].nickname = "";
            clients[ind].status = -1;
        }
    
    }
        
   //Check each client for activity
    
    for (size_t i = 0; i < MAX_CLIENTS; ++i) {
    int clientSocket = clients[i].socket;
    if (FD_ISSET(clientSocket, &readfds)) {
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);  
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::string message(buffer); 
            remove_spaces(message);
            std::cout << "Message from client " << i << ": " << message << "\n";
            std::vector<std::string> args = split_str(message, ' ');
            
            for (int i = 0; i < args.size() - 1;i++)
            {
                if (args[i] == "CAP")
                    sendUser("CAP * LS",clientSocket);
                  if (args[i] == "PASS" && args[i + 1] == stru->password)
                {
                    sendUser("You Are Successfully Registered",clientSocket);
                    clients[i].status = 0;
                    std::cout << clients[i].nickname << clients[i].status <<  std::endl;
                                        // clients[i].nickname = "nick0"; 
                    // clients[i].username = "user0";   
                }
                 if (args.size() > 1 && args[i] == "NICK")
                {
                    // if (clients[i].status <= 0)
                    // {
                    //    clients[i].tmp_nick = args[i + 1];
                    //    clients[i].status = 1;
                    // }
                    if (searchByNickName(args[i + 1], clients, MAX_CLIENTS) != -1)
                    {
                    sendUser(":ma_server_ma_waloo 433 client_dzeeb " + args[i + 1] + " :Nickname is already in use\r\n", clientSocket);
                    std::cout << "makhssox ydkheel hna:" << clients[i].status << std::endl;
                    }
                    else if(searchByNickName(args[i + 1], clients, MAX_CLIENTS) == -1)
                    {
                    std::string old_one = clients[i].nickname;
					clients[i].nickname = args[i + 1];
                    sendUser(":" + old_one + " NICK :" + args[i + 1] + "\r\n", clientSocket);
                    std::cout << "donc dkhel hna:" << clients[i].nickname << " status:" << clients[i].status << std::endl;
                    }
                }
                 if (clients[i].nickname != "" && clients[i].status == 0)
                {
                  std::string response =  ":ma_server_ma_Walo 001 "+ clients[i].nickname + " :Welcome to the IRC server\r\n";
                  std::cout << response ;
                  sendUser(response.c_str(), clientSocket); 
                }
               
                //  if (clients[i].status == 1)
                // {
                                     				    
                //     // clients[i].nickname = clients[i].tmp_nick;
                //     // clients[i].tmp_nick = "walo";
                //     // std::cout << "tmp: " << clients[i].nickname << std::endl;
                //  if (searchByNickName(clients[i].tmp_nick, clients, MAX_CLIENTS) != -1 )
                //  {
                //     std::cout << "hnaaaaa" << std::endl;
                 
                //    // std::string tmp = clients[i].nickname;
                //     // clients[i].tmp_nick = addRandomNumber(clients[i].tmp_nick);
                //      sendUser(":ma_server_ma_waloo 433 client_dzeeb " + clients[i].tmp_nick + " :Nickname is already in use\r\n", clientSocket);

                //  }
                //  //   std::cout << clients[i].nickname << std::endl;
                    
                //     clients[i].nickname = clients[i].tmp_nick;
                //     std::string response =  ":ma_server_ma_Walo 001 "+ clients[i].nickname + " :Welcome to the IRC server\r\n";
                //     std::cout << response ;
                //     sendUser(":zwita NICK :" + clients[i].nickname + "\r\n", clientSocket);
                //    // sendUser(response.c_str(), clientSocket); 
                //     clients[i].status = 2;

                //     // sendUser(":ma_server_ma_Walo 001 " + clients[i].nickname + " :Nickname Set !! \r\n",clientSocket);
                //     // sendUser(":" +  clients[i].tmp_nick + " NICK :" + clients[i].nickname + "\r\n", clientSocket);
                // }
                
                if (args[i] == "PASS" && args[i + 1] != stru->password)
                {
                    errorUser("Passowrd Incorrect : Are You A Hidden Thief !!\r\n",clientSocket);
                  //  sendUser("QUIT",clientSocket);
					clients[i].username = "";
					clients[i].nickname = "";
					clients[i].socket = -1;
                    clients[i].status = -1;
			 	    close(clientSocket);
                    //break;
                }
                if (args[i] == "USER")
                {
                if (searchByUsername(args[i + 1], clients, MAX_CLIENTS) != -1)
                    // args[i + 1] = addRandomNumber(args[i + 1]);
                    
                if(searchByUsername(args[i + 1], clients, MAX_CLIENTS) == -1)
                {
					clients[i].username = args[i+1];
                    sendUser("You're New Username is : " + clients[i].username, clientSocket);
                }
                }
                if (args[i] == "PING")
                    sendUser("PONG\r\n",clientSocket);
            }
            
        } else if (bytesRead == 0) {
            std::cout << "Client " << i << " disconnected.\n";
            close(clientSocket);
            clients[i].socket = -1;
            clients[i].status = -1;
            clients[i].nickname = "";
            clients[i].username = "";
        } else {
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
        ft_error(0,"Failed to create socket !!");

        // Configure Server Adress //
    struct sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr)); // initialize struct 
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
        ft_error(0,"Binding failed\n");

    if (listen(serverSocket, SOMAXCONN) == -1)
        ft_error(0,"Listen failed\n");

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
       setup_socket(port,&str);
       connect_server_client(&str);
    }   
    else
        std::cerr << "Error : Number of Arguments Invalid !! " << std::endl;
}