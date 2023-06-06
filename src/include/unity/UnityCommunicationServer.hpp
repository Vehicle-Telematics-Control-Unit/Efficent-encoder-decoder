#ifndef UnityCommunicationServer
#define UnityCommunicationServer

#include <iostream>
#include "payloads.hpp"

#define SOCKET_PATH "sock"

/**
 * @brief sends a location message of a certain vehicle to Unity
 * 
 * @param macAddr mac address of the vehicle
 * @param lat latitude
 * @param lon longitude 
 */
void unity_visualize_location(std::string macAddr, float lat, float lon);
void unity_visualize_heading(std::string macAddr, uint16_t heading);

int unity_start_socket(full_payload & my_vehicle);

#endif