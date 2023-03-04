#include "payloads.h"
#include "DSRC.h"
#include <cstring>
#include "CONFIG.h"
#include <map>
#include <thread>

/**
 * @brief must be called in every encode function
 * @brief returns time using system calls
 */
static void encode_time(time_stamp &_time)
{
	char buffer[4];
	int millisec;
	struct tm *tm_info;
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
	time_stamp::structure *p = &(_time._time_);
	p->hh = tm_info->tm_hour;
	p->mm = tm_info->tm_min;
	p->ss = tm_info->tm_sec;
	p->ms = millisec;
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
	location_payload::structure *new_readings = &(new_location_payload.data);
	location_payload::structure *vehicle_location_PL = &(vehicle_payload._location_payload.data);

	if (my_vehicle)
		encode_time(vehicle_payload._last_location_time_stamp);
	else
		memcpy(vehicle_payload._last_location_time_stamp.value, (void *)new_location_payload.data.time_value, TIME_STAMP_VALUE_SIZE);

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
void vehicle_payload_heading_update(full_payload &vehicle_payload, heading_payload &new_heading_payload, bool my_vehicle)
{
	heading_payload::structure *new_readings = &(new_heading_payload.data);
	heading_payload::structure *vehicle_heading_PL = &(vehicle_payload._heading_payload.data);

	if (my_vehicle)
		encode_time(vehicle_payload._last_heading_time_stamp);
	else
		memcpy(vehicle_payload._last_heading_time_stamp.value, (void *)new_heading_payload.data.time_value, TIME_STAMP_VALUE_SIZE);

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
void vehicle_payload_speed_update(full_payload &vehicle_payload, speed_payload &new_speed_payload, bool my_vehicle)
{
	speed_payload::structure *new_readings = &(new_speed_payload.data);
	speed_payload::structure *vehicle_speed_PL = &(vehicle_payload._speed_payload.data);

	if (my_vehicle)
		encode_time(vehicle_payload._last_speed_time_stamp);
	else
		memcpy(vehicle_payload._last_speed_time_stamp.value, (void *)new_speed_payload.data.time_value, TIME_STAMP_VALUE_SIZE);

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
void vehicle_payload_brakes_update(full_payload &vehicle_payload, brakes_payload &new_brakes_payload, bool my_vehicle)
{
	brakes_payload::structure *new_readings = &(new_brakes_payload.data);
	brakes_payload::structure *vehicle_brakes_PL = &(vehicle_payload._brakes_payload.data);

	if (my_vehicle)
		encode_time(vehicle_payload._last_brakes_time_stamp);
	else
		memcpy(vehicle_payload._last_brakes_time_stamp.value, (void *)new_brakes_payload.data.time_value, TIME_STAMP_VALUE_SIZE);

	vehicle_brakes_PL->Brakes = new_readings->Brakes;
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
void payloads_initializer(full_payload &my_vehicle)
{
	my_vehicle._location_payload.data.id = LOCATION_MSG;
	my_vehicle._heading_payload.data.id = HEADING_MSG;
	my_vehicle._speed_payload.data.id = SPEED_MSG;
	my_vehicle._brakes_payload.data.id = BRAKES_MSG;
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
	char mac_address[12];
	strncpy(mac_address, mac_address, 12);

#if VERBOSE_RECIEVED_MESSAGES == true
	std::cout << "[INFO] [";
	printf("%.*s", 12, mac_address); // or fwrite(string, 1, length, stdout);
	std::cout << "] says: ";
	printf("%.*s.\n", buffer_size - 12, mac_address[12]);
#elif VERBOSE_RECIEVED_MAC == true
	std::cout << "[INFO] Recived a message from [";
	printf("%.*s", 12, mac_address); // or fwrite(string, 1, length, stdout);
	std::cout << "]\n";
#endif
	


	delete[] buffer;
}

int main()
{
	full_payload my_vehicle;
	payloads_initializer(my_vehicle);

	// TEST CASE:
	// I got location readings from gps sensors
	location_payload lp;
	lp.data.lat = 4;
	lp.data.lon = 5;
	lp.data.lat_frac = 902;
	lp.data.lon_frac = 1003;

	// put it in my payload
	vehicle_payload_location_update(my_vehicle, lp, true);

	// DSRC-out some random string
	if (init_dsrc() == 0)
		;
	else
	{
		std::cout << "[ERROR] init_dsrc() failed" << std::endl;
	}
	dsrc_send_payload((uint8_t)"[besm allah]");

	on_payload_recieved((char *)lp.value, 7);

	std::thread thread_object(DSRC_read_thread, std::ref(on_payload_recieved));

	unsigned char s[] = "[besm allah alrahman alrahim]";
	while (1)
	{
		dsrc_broadcast(s, sizeof(s));
		// dsrc_read();
		sleep(1);
	}
	return 0;
}