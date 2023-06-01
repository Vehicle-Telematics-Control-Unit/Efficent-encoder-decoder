#include <payloads.hpp>
#include <DSRC.hpp>
#include <cstring>
#include "CONFIG.h"
#include <map>
#include <thread>
#include <string>
#include <UnityCommunicationServer.hpp>

#ifdef _WIN32
#include <chrono>
#include <iostream>
#endif

using namespace std;

#define MAC_ADDR_SIZE 12

#define VERBOSE_RECIEVED_MESSAGES_DECODE_PRINT(PAYLOAD)  \
	cout << "[BUFFER] [REC_PAYLOAD_DECODE] [BEGIN] :\n"; \
	(*((PAYLOAD *)&buffer[12])).print();                 \
	cout << "[BUFFER] [REC_PAYLOAD_DECODE] [DONE]\n\n"; // \
// cout << "\n-- stored\n"; \
// (*surrounding_vehicles[rec_mac_address])._ ## PAYLOAD.print(); \

string THREAD_TERMINAL_OUTPUT_DEVICE;
string TTYUSB_DEVICE;

// mac_address, full_payload
std::map<string, full_payload *> surrounding_vehicles;

void color_term(int x, int y)
{
	// std::cout << "\033[" << x << ";" << y << "m" ;
}

void color_term_reset()
{
	// std::cout << "\033[0m" ;
	// std::cout << "\r\e[K" << std::flush;
	std::cout << "\n";
}

/**
 * @brief must be called in every encode function
 * @brief returns time using system calls
 */
static void encode_time(time_stamp &_time)
{
	char buffer[4];
	int millisec;
	struct tm *tm_info;
	time_stamp *p = &(_time);
	memset((void *)&_time, 0, sizeof(time_stamp));

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
	// p->hh = 3;
	// p->mm = 3;
	// p->ss = 3;
	// p->ms = 3;
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
void vehicle_payload_location_update(full_payload &vehicle_payload, location_payload &new_location_payload, bool my_vehicle)
{
	location_payload *new_readings = &(new_location_payload);
	location_payload *vehicle_location_PL = &(vehicle_payload._location_payload);

	memcpy((uint8_t *)vehicle_location_PL, (uint8_t *)new_readings, sizeof(location_payload));

	if (my_vehicle)
		encode_time(vehicle_payload._location_payload._last_time_stamp);
	else
		;

	// memcpy(&vehicle_payload._location_payload._last_time_stamp, &new_location_payload._last_time_stamp, sizeof time_stamp);

	// vehicle_location_PL->lat = new_readings->lat;
	// vehicle_location_PL->lat_frac = new_readings->lat_frac;
	// vehicle_location_PL->lon = new_readings->lon;
	// vehicle_location_PL->lon_frac = new_readings->lon_frac;
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
void vehicle_payload_heading_update(full_payload &vehicle_payload, heading_payload &new_heading_payload, bool my_vehicle)
{
	heading_payload *vehicle_heading_PL = &(vehicle_payload._heading_payload);
	heading_payload *new_readings = &(new_heading_payload);

	memcpy((uint8_t *)vehicle_heading_PL, (uint8_t *)new_readings, sizeof(heading_payload));

	if (my_vehicle)
		encode_time(vehicle_payload._location_payload._last_time_stamp);
	else
		;
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
void vehicle_payload_speed_update(full_payload &vehicle_payload, speed_payload &new_speed_payload, bool my_vehicle)
{
	speed_payload *new_readings = &(new_speed_payload);
	speed_payload *vehicle_speed_PL = &(vehicle_payload._speed_payload);

	memcpy((uint8_t *)vehicle_speed_PL, (uint8_t *)new_readings, sizeof(speed_payload));

	if (my_vehicle)
		encode_time(vehicle_payload._location_payload._last_time_stamp);
	else
		;
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
void vehicle_payload_brakes_update(full_payload &vehicle_payload, brakes_payload &new_brakes_payload, bool my_vehicle)
{
	brakes_payload *new_readings = &(new_brakes_payload);
	brakes_payload *vehicle_brakes_PL = &(vehicle_payload._brakes_payload);

	memcpy((uint8_t *)vehicle_brakes_PL, (uint8_t *)new_readings, sizeof(brakes_payload));

	if (my_vehicle)
		encode_time(vehicle_payload._location_payload._last_time_stamp);
	else
		;
}

/**
 * @brief A function to call on main to initialize ID's of the payloads of the current vehicle
 * and also create current vehicle payloads instances
 *
 */
void payloads_initializer(full_payload &my_vehicle)
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
	/*char rec_mac_address[13];
	strncpy(rec_mac_address, buffer, 12);
	rec_mac_address[MAC_ADDR_SIZE] = '\0';*/
	string rec_mac_address(buffer, 12);

	rec_mac_address.copy(buffer, 12);

#if VERBOSE_RECIEVED_MESSAGES == true
#ifdef __linux__
	int output_term = open(THREAD_TERMINAL_OUTPUT_DEVICE.c_str(), 1);
	write(output_term, "[INFO] [PAYLOAD_RECIEVED] [", 28);
	// printf("%.*s", 12, mac_address);
	write(output_term, buffer, 12);
	write(output_term, "] => [", 7);
	write(output_term, &buffer[12], buffer_size - 12);
	write(output_term, "].\n", 4);

	write(output_term, "[INFO] [PAYLOAD_RECIEVED(int)] [", 33);
	// printf("%.*s", 12, mac_address);
	write(output_term, buffer, 12);
	write(output_term, "] => [", 7);
	for (int i = 12; i < buffer_size; i++)
	{
		char s[10];
		sprintf(s, "%d ", (unsigned char)buffer[i]);
		write(output_term, s, strlen(s));
	}

	write(output_term, "].\n", 4);

#else
	cout << "[INFO] [";
	// printf("%.*s", 12, mac_address);
	cout << rec_mac_address;
	cout << "] says: ";
	cout << &buffer[12] << '\n';

#endif // _WIN32
#endif // VERBOSE_RECIEVED_MESSAGES

	uint8_t rec_payload_id = buffer[12];

	if (surrounding_vehicles.find(rec_mac_address) == surrounding_vehicles.end())
	{
		surrounding_vehicles[rec_mac_address] = new full_payload;
	}

	switch (rec_payload_id)
	{
	case LOCATION_MSG_ID:
		vehicle_payload_location_update(*(surrounding_vehicles[rec_mac_address]), *((location_payload *)&buffer[MAC_ADDR_SIZE]), false);
#ifdef VERBOSE_RECIEVED_MESSAGES_DECODE
		VERBOSE_RECIEVED_MESSAGES_DECODE_PRINT(location_payload);
#endif // VERBOSE_RECIEVED_MESSAGES_DECODE
		break;

	case HEADING_MSG_ID:
		vehicle_payload_heading_update(*(surrounding_vehicles[rec_mac_address]), *((heading_payload *)&buffer[MAC_ADDR_SIZE]), false);
#ifdef VERBOSE_RECIEVED_MESSAGES_DECODE
		VERBOSE_RECIEVED_MESSAGES_DECODE_PRINT(heading_payload);
#endif
		break;

	case SPEED_MSG_ID:
		vehicle_payload_speed_update(*(surrounding_vehicles[rec_mac_address]), *((speed_payload *)&buffer[MAC_ADDR_SIZE]), false);
#ifdef VERBOSE_RECIEVED_MESSAGES_DECODE
		VERBOSE_RECIEVED_MESSAGES_DECODE_PRINT(speed_payload);
#endif
		break;

	case BRAKES_MSG_ID:
		vehicle_payload_brakes_update(*(surrounding_vehicles[rec_mac_address]), *((brakes_payload *)&buffer[MAC_ADDR_SIZE]), false);
#ifdef VERBOSE_RECIEVED_MESSAGES_DECODE
		VERBOSE_RECIEVED_MESSAGES_DECODE_PRINT(brakes_payload);
#endif
		break;

	default:
		break;
	}

#if VERBOSE_RECIEVED_MESSAGES_DECODE
	cout << "";
#endif // VERBOSE_RECIEVED_MESSAGES_DECODE

	free(buffer);
}

#ifdef RPI
int main(int argc, char *argv[])
{
	INFO_COLOR;
	cout << "[DEBUG] [INFO] running main2\n";
	TTYUSB_DEVICE = argv[1];
	THREAD_TERMINAL_OUTPUT_DEVICE = argv[2];

	cout << "[INFO] [VAR] TTYUSB_DEVICE:" << TTYUSB_DEVICE << '\n';
	cout << "[INFO] [VAR] THREAD_TERMINAL_OUTPUT_DEVICE:" << THREAD_TERMINAL_OUTPUT_DEVICE << '\n';

	if (init_dsrc() == 0)
		;
	else
	{
		std::cout << "[ERROR] init_dsrc() failed" << std::endl;
		return 0;
	}

	RESET_COLOR;

	std::thread thread_object(DSRC_read_thread, std::ref(on_payload_recieved));

	// dsrc_send_payload((uint8_t)"[besm allah]");

	location_payload lp;
	lp.lat = 2;
	lp.lat_frac = 2;
	lp.lon = 2;
	lp.lon_frac = 2;
	encode_time(lp._last_time_stamp);

	heading_payload hp;
	hp.heading = 50;
	encode_time(hp._last_time_stamp);

	speed_payload sp;
	sp.speed = 12;
	encode_time(sp._last_time_stamp);

	brakes_payload bp;
	bp.brakes = 1;
	encode_time(bp._last_time_stamp);

	while (1)
	{
		encode_time(hp._last_time_stamp);
		dsrc_broadcast((uint8_t *)&hp, sizeof(hp));
		// dsrc_broadcast((uint8_t *)argv[3], strlen(argv[3]));
		SEND_COLOR;
		hp.print();
		RESET_COLOR;

#ifndef _WIN32
		sleep(4);
#else
		std::this_thread::sleep_for(std::chrono::seconds(1));
#endif
	}
	return 0;
}

#else

int main(int argc, char *argv[])
{
	INFO_COLOR;
	TTYUSB_DEVICE = argv[1];
	THREAD_TERMINAL_OUTPUT_DEVICE = argv[2];



	cout << "[INFO] [VAR] TTYUSB_DEVICE:" << TTYUSB_DEVICE << '\n';
	cout << "[INFO] [VAR] THREAD_TERMINAL_OUTPUT_DEVICE:" << THREAD_TERMINAL_OUTPUT_DEVICE << '\n';

	if (init_dsrc() == 0)
		;
	else
	{
		std::cout << "[ERROR] init_dsrc() failed" << std::endl;
		return -1;
	}

	color_term_reset();

	std::thread thread_object(DSRC_read_thread, std::ref(on_payload_recieved));

	full_payload my_vehicle;
	payloads_initializer(my_vehicle);
	unity_start_socket(my_vehicle);

	while (1)
	{

		dsrc_broadcast((uint8_t *)&(my_vehicle._location_payload), sizeof(my_vehicle._location_payload));
		dsrc_broadcast((uint8_t *)&(my_vehicle._heading_payload), sizeof(my_vehicle._heading_payload));
		dsrc_broadcast((uint8_t *)&(my_vehicle._brakes_payload), sizeof(my_vehicle._brakes_payload));
		dsrc_broadcast((uint8_t *)&(my_vehicle._speed_payload), sizeof(my_vehicle._speed_payload));
		my_vehicle._location_payload.print();

#ifndef _WIN32
		sleep(1);
#else
		std::this_thread::sleep_for(std::chrono::seconds(1));
#endif
	}
	return 0;
}

#endif