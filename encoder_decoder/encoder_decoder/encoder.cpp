#include "payloads.h"
#include "DSRC.h"
#include <cstring>
#include "CONFIG.h"
#include <map>
#include <thread>

#ifdef _WIN32
#include <chrono>
#include <iostream>
using namespace std;
#endif

#define MAC_ADDR_SIZE 12

std::map<char*, full_payload*> surrounding_vehicles;

/**
 * @brief must be called in every encode function
 * @brief returns time using system calls
 */
static void encode_time(time_stamp& _time)
{
	char buffer[4];
	int millisec;
	struct tm* tm_info;
	time_stamp* p = &(_time);
	memset(&_time, 0, sizeof _time);

#ifndef _WIN32
	struct timeval tv;
	gettimeofday(&tv, NULL);

	millisec = lrint(tv.tv_usec / 1000.0); // Round to nearest millisec
	if (millisec >= 1000)
	{ // Allow for rounding up to nearest second
		millisec -= 1000;
		tv.tv_sec++;
	}

	tm_info = localtime(&tv.tv_sec);

	// strftime(buffer, 26, "%2H%2M%3S", tm_info);
	// printf("%s.%03d\n", buffer, millisec);
	p->hh = tm_info->tm_hour;
	p->mm = tm_info->tm_min;
	p->ss = tm_info->tm_sec;
	p->ms = millisec;
#else
	p->hh = 0;
	p->mm = 0;
	p->ss = 0;
	p->ms = 0;
#endif
}

/**
 * @brief Function responsible to take a newly recieved location payload, and puts it into
 * it's vehicle's full_payload.
 * if it is my vehicle, this function takes the time from the TCU system and puts it
 * in the location last time stamp, else it takes the time from the location payload
 * and updates the full_payload _last_location_time_stamp with it
 *
 * @param vehicle_payload The full payload of the vehicle that owns this new location
 * @param new_location_payload  The new location either coming from DSRC(other vehicle) or from
 * current vehicle's GPS readings
 * @param my_vehicle if true, take current time from the TCU, else take time from the location payload
 */
void vehicle_payload_location_update(full_payload& vehicle_payload, location_payload& new_location_payload, bool my_vehicle)
{
	location_payload* new_readings = &(new_location_payload);
	location_payload* vehicle_location_PL = &(vehicle_payload._location_payload);

	if (my_vehicle)
		encode_time(vehicle_payload._location_payload._last_time_stamp);
	else
		memcpy(&vehicle_payload._location_payload._last_time_stamp, &new_location_payload._last_time_stamp, sizeof time_stamp);


	//*((uint8_t*)&vehicle_payload._location_payload.time_value) = 0;
	//*((uint8_t *)&vehicle_payload._location_payload.time_value + sizeof uint8_t) = 0;
	//*((uint8_t *)&vehicle_payload._location_payload.time_value + 2 * sizeof uint8_t) = 0;
	//*((uint8_t *)&vehicle_payload._location_payload.time_value + 3 * sizeof uint8_t) = 0;

	//uint32_t v = 0; // used only for testing memcpy
	//memcpy((uint8_t *)&vehicle_payload._location_payload.time_value, (uint8_t *)&v, 4);

	memcpy((uint8_t*)&vehicle_payload._location_payload._last_time_stamp, (uint8_t*)&vehicle_payload._location_payload._last_time_stamp, sizeof time_stamp);

	vehicle_location_PL->lat = new_readings->lat;
	vehicle_location_PL->lat_frac = new_readings->lat_frac;
	vehicle_location_PL->lon = new_readings->lon;
	vehicle_location_PL->lon_frac = new_readings->lon_frac;
}

/**
 * @brief Function responsible to take a newly recieved heading payload, and puts it into
 * it's vehicle's full_payload.
 * if it is my vehicle, this function takes the time from the TCU system and puts it
 * in the heading last time stamp, else it takes the time from the heading payload
 * and updates the full_payload _last_heading_time_stamp with it
 *
 * @param vehicle_payload The full payload of the vehicle that owns this new heading
 * @param new_heading_payload  The new heading either coming from DSRC(other vehicle) or from
 * current vehicle's IMU readings
 * @param my_vehicle if true, take current time from the TCU, else take time from the heading payload
 */
void vehicle_payload_heading_update(full_payload& vehicle_payload, heading_payload& new_heading_payload, bool my_vehicle)
{
	heading_payload* new_readings = &(new_heading_payload);
	heading_payload* vehicle_heading_PL = &(vehicle_payload._heading_payload);

	if (my_vehicle)
		encode_time(vehicle_payload._heading_payload._last_time_stamp);
	else
		memcpy(&vehicle_payload._heading_payload._last_time_stamp, &new_heading_payload._last_time_stamp, sizeof time_stamp);

	vehicle_heading_PL->heading = new_readings->heading;
}

/**
 * @brief Function responsible to take a newly recieved speed payload, and puts it into
 * it's vehicle's full_payload.
 * if it is my vehicle, this function takes the time from the TCU system and puts it
 * in the speed last time stamp, else it takes the time from the speed payload
 * and updates the full_payload _last_speed_time_stamp with it
 *
 * @param vehicle_payload The full payload of the vehicle that owns this new speed
 * @param new_speed_payload  The new speed either coming from DSRC(other vehicle) or from
 * current vehicle's IMU readings
 * @param my_vehicle if true, take current time from the TCU, else take time from the speed payload
 */
void vehicle_payload_speed_update(full_payload& vehicle_payload, speed_payload& new_speed_payload, bool my_vehicle)
{
	speed_payload* new_readings = &(new_speed_payload);
	speed_payload* vehicle_speed_PL = &(vehicle_payload._speed_payload);

	if (my_vehicle)
		encode_time(vehicle_payload._speed_payload._last_time_stamp);
	else
		memcpy(&vehicle_payload._speed_payload._last_time_stamp, &new_speed_payload._last_time_stamp, sizeof time_stamp);

	vehicle_speed_PL->speed = new_readings->speed;
}

/**
 * @brief Function responsible to take a newly recieved brakes payload, and puts it into
 * it's vehicle's full_payload.
 * if it is my vehicle, this function takes the time from the TCU system and puts it
 * in the brakes last time stamp, else it takes the time from the brakes payload
 * and updates the full_payload _last_brakes_time_stamp with it
 *
 * @param vehicle_payload The full payload of the vehicle that owns this new brakes
 * @param new_brakes_payload  The new brakes either coming from DSRC(other vehicle) or from
 * current vehicle's brakes readings
 * @param my_vehicle if true, take current time from the TCU, else take time from the brakes payload
 */
void vehicle_payload_brakes_update(full_payload& vehicle_payload, brakes_payload& new_brakes_payload, bool my_vehicle)
{
	brakes_payload* new_readings = &(new_brakes_payload);
	brakes_payload* vehicle_brakes_PL = &(vehicle_payload._brakes_payload);

	if (my_vehicle)
		encode_time(vehicle_payload._brakes_payload._last_time_stamp);
	else
		memcpy(&vehicle_payload._brakes_payload._last_time_stamp, &new_brakes_payload._last_time_stamp, sizeof time_stamp);

	vehicle_brakes_PL->brakes = new_readings->brakes;
}

/**
 * @brief broadcast a payload to surrounding vehicles
 *
 * @param value uint8_t arrays containing the payload
 */
void dsrc_send_payload(uint8_t value)
{
}

void dsrc_recieved_payload(uint8_t value)
{
}

/**
 * @brief A function to call on main to initialize ID's of the payloads of the current vehicle
 * and also create current vehicle payloads instances
 *
 */
void payloads_initializer(full_payload& my_vehicle)
{
	my_vehicle._location_payload.id = LOCATION_MSG_ID;
	my_vehicle._heading_payload.id = HEADING_MSG_ID;
	my_vehicle._speed_payload.id = SPEED_MSG_ID;
	my_vehicle._brakes_payload.id = BRAKES_MSG_ID;
}


/**
 * @brief a call back function that's called on every new message recieved
 * this function is called in a new thread, given a copy of the recieved messages buffer
 * so this function must deallocate that copy buffer before termination
 * @param buffer the message recieved
 * @param buffer_size size of message recieved, don't include termination character
 */
void on_payload_recieved(char buffer[], int buffer_size)
{
	// grab the mac address from the buffer (first 12 bytes)
	char rec_mac_address[12];
	strncpy(rec_mac_address, buffer, 12);

#if VERBOSE_RECIEVED_MESSAGES == true
#ifndef _WIN32
	int output_term = open(THREAD_TERMINAL_OUTPUT_DEVICE, 1);
	write(output_term, "[INFO] [", 9);
	// printf("%.*s", 12, mac_address);
	write(output_term, (unsigned char*)mac_address, 12);
	write(output_term, "] says: ", 9);
	write(output_term, &buffer[12], buffer_size - 12);
	write(output_term, ".\n", 2);
#else
	cout << "[INFO] [";
	// printf("%.*s", 12, mac_address);
	cout.write(rec_mac_address, MAC_ADDR_SIZE);
	cout << "] says: ";
	cout << &buffer[12] << '\n';
#endif
#endif

	int rec_payload_id = buffer[12] & 0b11100000;

	if (surrounding_vehicles.find(rec_mac_address) == surrounding_vehicles.end()) {

		surrounding_vehicles[rec_mac_address] = new full_payload();
	}


	switch (rec_payload_id)
	{
	case LOCATION_MSG_ID:
		vehicle_payload_location_update(*(surrounding_vehicles[rec_mac_address]), *((location_payload*)&buffer[12]), false);
		break;
	case HEADING_MSG_ID:
		vehicle_payload_heading_update(*(surrounding_vehicles[rec_mac_address]), *((heading_payload*)&buffer[12]), false);
		break;
	case SPEED_MSG_ID:
		vehicle_payload_speed_update(*(surrounding_vehicles[rec_mac_address]), *((speed_payload*)&buffer[12]), false);
		break;
	case BRAKES_MSG_ID:
		vehicle_payload_brakes_update(*(surrounding_vehicles[rec_mac_address]), *((brakes_payload*)&buffer[12]), false);
		break;
	default:
		break;
	}

	delete[] buffer;


}

int main()
{
	full_payload my_vehicle;
	payloads_initializer(my_vehicle);

	// TEST CASE:
	// I got location readings from gps sensors
	location_payload lp;
	lp.lat = 1;
	lp.lon = 1;
	lp.lat_frac = 1;
	lp.lon_frac = 1;
	lp._last_time_stamp.hh = 5;
	lp._last_time_stamp.mm = 5;
	lp._last_time_stamp.ms = 5;
	lp._last_time_stamp.ss = 5;

	// put it in my payload
	vehicle_payload_location_update(my_vehicle, lp, true);

	// make a new vehicle which is a reciever
	full_payload rec_vehicle;
	payloads_initializer(rec_vehicle);
	location_payload r_lp;

	// as if dsrc sent my_vehicle location to rec_vehicle
	vehicle_payload_location_update(rec_vehicle, lp, false);

	// DSRC-out some random string
	if (init_dsrc() == 0)
		;
	else
	{
		std::cout << "[ERROR] init_dsrc() failed" << std::endl;
	}
	// dsrc_send_payload((uint8_t)"[besm allah]");

	// on_payload_recieved((char *)lp.value, 7);

	std::thread thread_object(DSRC_read_thread, std::ref(on_payload_recieved));

	unsigned char s[sizeof lp];
	memcpy(s, (void*)&lp, sizeof lp);
	while (1)
	{
		dsrc_broadcast(s, sizeof(lp));
#ifndef _WIN32
		sleep(1);
#else
		std::this_thread::sleep_for(std::chrono::seconds(1));
#endif
	}
	return 0;
}