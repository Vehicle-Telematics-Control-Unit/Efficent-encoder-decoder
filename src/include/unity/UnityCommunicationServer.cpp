#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <thread>
#include <signal.h>
#include <payloads.hpp>
#include "UnityCommunicationServer.hpp"

#define SOCKET_PATH "/home/ahmed/Documents/Unity_Hub/Projects/TCU/socket/sock"

using namespace std;
static int serverSocket, clientSocket;

void read_thread(int clientSocket, full_payload &my_vehicle)
{
    printf("[info] unity read thread started!\n");
    // Read data from the client
    while (true)
    {
        char buffer[1024] = {0};
        int bytesRead = read(clientSocket, buffer, sizeof(buffer));
        if (strlen(buffer) == 0)
        {
            continue;
        }

        printf("[info] [unity] Received message from client: %s\n", buffer);


        double lat, lon;
        float heading;
        int speed;
        bool brakes;
        sscanf(buffer, "l:%lf,%lf&&h:%f&&s:%d&&b:%d", &lat, &lon, &heading, &speed, &brakes);

        my_vehicle._brakes_payload.brakes = brakes;
        my_vehicle._heading_payload.heading = heading;

        my_vehicle._location_payload.lat = lat;
        my_vehicle._location_payload.lon = lon;

        my_vehicle._speed_payload.speed = speed;

        // cout << my_vehicle._location_payload.lat << " " << my_vehicle._location_payload.lon << endl;
        // printf("%f %f\n", my_vehicle._location_payload.lat, my_vehicle._location_payload.lon);
    }
}

void unity_visualize_location(std::string macAddr, float lat, float lon)
{
    char packet[100] = {0};
    sprintf(packet, "%sl%f,%f", macAddr.c_str(), lat, lon);
    send(clientSocket, packet, strlen(packet), 0);
}

void unity_visualize_heading(std::string macAddr, uint16_t heading)
{
    // cout << "\n\nHEADING ALARM : " << heading << endl;
    char packet[100] = {0};
    sprintf(packet, "%sh%d", macAddr.c_str(), heading);
    // cout << "PACKET ALARM\n" << packet << endl;
    send(clientSocket, packet, strlen(packet), 0);
}

void signal_callback_handler(int signum)
{
    cout << "Caught signal " << signum << endl;
    if (signum == 2)
    {
        // Close the sockets
        close(clientSocket);
        close(serverSocket);

        // Remove the socket file
        unlink(SOCKET_PATH);
    }
    exit(signum);
}

int unity_start_socket(full_payload &my_vehicle)
{
    struct sockaddr_un serverAddress, clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    char buffer[1024] = {0};

    char os_buff[30];
    sprintf(os_buff, "rm %s", SOCKET_PATH);
    try
    {
        system(os_buff);
    }
    catch (const std::exception &e)
    {
        // do nth
    }

    signal(SIGINT, signal_callback_handler);

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

    sprintf(os_buff, "chmod 777 %s", SOCKET_PATH);
    try
    {
        system(os_buff);
    }
    catch (const std::exception &e)
    {
        // do nth
    }


    // Listen for incoming connections
    printf("[info] waiting for unity to connect!");
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

    std::thread thread_object(read_thread, clientSocket, std::ref(my_vehicle));
    thread_object.detach();

    return 0;
}
