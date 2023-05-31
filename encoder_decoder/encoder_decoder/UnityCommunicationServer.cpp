#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <thread>

#define SOCKET_PATH "sock"

using namespace std;
static int serverSocket, clientSocket;


void read_thread(int clientSocket)
{
    // Read data from the client
    while (true)
    {
        char buffer[1024] = {0};
        int bytesRead = read(clientSocket, buffer, sizeof(buffer));
        if (strlen(buffer) == 0)
        {
            continue;
        }
        // printf("Received message from client: %s\n", buffer);
        float lat, lon, heading;
        int speed;
        bool brakes;
        sscanf(buffer, "l:%f,%f&&h:%f&&s:%d&&b:%d", lat, lon, heading, speed, brakes);
    }
}

void unity_visualize_location(std::string macAddr, float lat, float lon)
{
        char packet[200] = {0};
        sprintf("%sl%f,%f", macAddr.c_str(), lat, lon);
        send(clientSocket, packet, strlen(packet), 0);
}

void unity_visualize_heading(std::string macAddr, float heading)
{
        char packet[200] = {0};
        sprintf("%sh%f", macAddr.c_str(), heading);
        send(clientSocket, packet, strlen(packet), 0);
}

int main()
{
    struct sockaddr_un serverAddress, clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    char buffer[1024] = {0};

    // Create a socket
    if ((serverSocket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        perror("Failed to create socket");
        return 1;
    }

    // Set server details
    serverAddress.sun_family = AF_UNIX;
    strncpy(serverAddress.sun_path, SOCKET_PATH, sizeof(serverAddress.sun_path) - 1);

    // Bind the socket to a specific path
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        perror("Failed to bind");
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 1) == -1)
    {
        perror("Failed to listen");
        return 1;
    }

    // Accept a client connection
    if ((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength)) == -1)
    {
        perror("Failed to accept");
        return 1;
    }

    // Read data from the client
    // int bytesRead = read(clientSocket, buffer, sizeof(buffer));
    // printf("Received message from client: %s\n", buffer);

    std::thread thread_object(read_thread, clientSocket);

    while (true)
    {
        // Send a response to the client
        const char *response = {"000000000000l3,3"};
        // while (true)
        // {
        send(clientSocket, response, strlen(response), 0);
        sleep(2);
        // }

        const char *response2 = {"00000000000xl9,9"};
        send(clientSocket, response2, strlen(response2), 0);
        sleep(3);

        const char *response3 = {"00000000000xl-3,-2"};
        send(clientSocket, response3, strlen(response3), 0);
        sleep(3);

        const char *response4 = {"0000000000A0l-10,-5"};
        send(clientSocket, response4, strlen(response4), 0);
        sleep(3);

        const char *response5 = {"00000000000xl9,9"};
        send(clientSocket, response5, strlen(response5), 0);
        sleep(3);

        const char *response6 = {"basebasebasel9,9"};
        send(clientSocket, response6, strlen(response6), 0);
        sleep(3);

        const char *response7 = {"00000000000xh30"};
        send(clientSocket, response7, strlen(response7), 0);
        sleep(3);
    }
    // Close the sockets
    close(clientSocket);
    close(serverSocket);

    // Remove the socket file
    unlink(SOCKET_PATH);

    return 0;
}
