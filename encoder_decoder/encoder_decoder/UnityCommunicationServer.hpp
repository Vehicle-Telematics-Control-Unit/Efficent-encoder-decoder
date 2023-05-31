#ifndef UnityCommunicationServer
#define UnityCommunicationServer

#include <iostream>

#define SOCKET_PATH "sock"

/**
 * @brief sends a location message of a certain vehicle to Unity
 * 
 * @param macAddr mac address of the vehicle
 * @param lat latitude
 * @param lon longitude 
 */
void unity_visualize_location(std::string macAddr, float lat, float lon){};

#endif