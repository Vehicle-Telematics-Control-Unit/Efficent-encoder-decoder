#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <string>

using namespace std;

int clientSocket;

int Send_Recieve_To_From_Server(string message)
{
    char buffer[5];
    send(clientSocket, message.c_str(), message.length(), 0);
    std::cout << "Sent message: " << message << std::endl;

    // Receive response from server
    memset(buffer, 0, sizeof(buffer));
    recv(clientSocket, buffer, sizeof(buffer), 0);
    std::cout << "Received response: " << buffer << std::endl;
    return buffer[0] - '0';
}

int Socket_Init()
{
    struct sockaddr_in serverAddr;

    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0)
    {
        std::cerr << "Error creating socket\n";
        return 1;
    }

    // Set up server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8009);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server IP address

    // Connect to server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        std::cerr << "Error connecting to server\n";
        return 1;
    }
    return 0;
}

void close_socket(){

    // Close socket
    close(clientSocket);

}

int main(){
    Socket_Init();
    string s = "0,0,0,0,0,0";
    Send_Recieve_To_From_Server(s);
    
    return 0;
}
