#include "lib.hpp"

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
        clients[i].password = false;
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
            std::ostringstream usrnm;
            std::ostringstream nicknm;
            usrnm  << "user" << ind;
            nicknm << "nick" << ind;
			std::cout << "New client connected: " << inet_ntoa(cl_addr.sin_addr) << std::endl;
    		clients[ind].username = usrnm.str();;
			clients[ind].nickname = nicknm.str();
            max_fds_socket = std::max(max_fds_socket, clientSocket);
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
            
            if (clients[i].password == false)
            {
                if (args[0] == "NICK" && args.size() == 7)
                {
                    clients[i].username = args[1];
                    clients[i].nickname = args[3];
                }
                else if (args[0] == "CAP" && args.size() == 10)
                {
                    clients[i].username = args[6];
                    clients[i].nickname = args[4];
                }
                else if (args[0] != "PASS")
                {
                    errorUser("You Have To Enter password : by /PASS <password> ", clientSocket);   
                }
                else if (args[1] != stru->password)
				{
					errorUser("Wrong Password! disconecting", clientSocket);
					close(clientSocket);
					clients[i].username = "";
					clients[i].nickname = "";
					clients[i].socket = -1;
					//clients[i].password = true;
					continue ;
				}
                else if (args[0] == "PASS" && args[1] == stru->password)
                {
                    sendUser("You Successfully Identified", clientSocket);
                    clients[i].password = true;
                }
            }
            else if (checkArg(args, clientSocket) == -1)
					continue;
            else if (args[0] == "NICK")
            {
                clients[i].nickname = args[1];
                sendUser("You're New Nickname is : " + clients[i].nickname, clientSocket);
            }
            else if (args[0] == "USER")
			{
				if(searchByUsername(args[1], clients, MAX_CLIENTS) == -1)
                {
					clients[i].username = args[1];
                    sendUser("You're New Username is : " + clients[i].username, clientSocket);
                }
                else
					errorUser("UserName Already Exist", clients[i].socket);
            }
            
            	else if (args[0] == "JOIN")
				{
					if (args[1][0] != '#')
						errorUser("/JOIN <#channel>", clientSocket);
                    else if (searchBychannelname(args[1].substr(1), channels, MAX_CHANNELS) == -1)
                        sendUser ("Channel Not exist",clientSocket);
		
				}
            else if (args[0] == "QUIT")
			{
				close(clients[i].socket);
				clients[i].socket = -1;
				std::cout << "Client disconnected : "<< clients[i].username << std::endl;
			}
         // Send a response to the client
         // std::string response = "Hello, client! Your message was received.\n";
         // int bytesSent = send(clientSocket, response.c_str(), response.length(), 0);
         // if (bytesSent < 0)
             // std::cerr << "Error in sending response to client " << i << ".\n";
        } else if (bytesRead == 0) {
            std::cout << "Client " << i << " disconnected.\n";
            close(clientSocket);
            clients[i].socket = 0;
        } else {
            std::cerr << "Error in recv from client " << i << ".\n";
            close(clientSocket);
            clients[i].socket = 0;
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