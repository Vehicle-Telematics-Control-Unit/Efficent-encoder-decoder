#include <stdio.h>
#include <stdint.h>
#include <ctime>
#include <chrono>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <cstring>

#ifdef __linux__
#include <sys/time.h>
#endif

#include <math.h>
#include <map>
#include "CONFIG.h"


enum MSG_ID {
	TIME_SYNC_MSG_ID = 0,        // not yet implemented
	LOCATION_MSG_ID = 1,
	HEADING_MSG_ID = 2,
	SPEED_MSG_ID = 3,
	BRAKES_MSG_ID = 4
};


/** @struct time_stamp
 * @brief timestamp union structure, to be used with every other data struct
 */
struct time_stamp
{
	uint8_t hh : 5;
	uint8_t mm : 6;
	uint8_t ss : 6;
	uint16_t ms : 10;       // max value (1024), suggested maximum (999)

	time_stamp() {
		hh = 0;
		mm = 0;
		ss = 0;
		ms = 0;
	}
};


struct location_payload
{
	uint8_t     id : 3;
	// location
	int8_t      lat : 8;	// range shall be (-85 to 85)
	int16_t     lon : 9;    // range shall be (-180 to 180)
	uint16_t    lat_frac;   // max value (65535), suggested maximum (9999)
	uint16_t    lon_frac;   // max value (65535), suggested maximum (9999)
	time_stamp _last_time_stamp;

	//uint8_t time_value[sizeof time_stamp];

	location_payload() {
		id = LOCATION_MSG_ID;
		lat = 0;
		lon = 0;
		lat_frac = 0;
		lon_frac = 0;
	}

};


//typedef union
//{
//	struct structure
//	{
//		uint8_t     id : 3;
//		uint8_t     id3 : 2;
//		uint32_t     id2 : 3;
//	}data;
//
//	uint8_t value[sizeof data];
//
//}test;

struct heading_payload
{
	uint8_t     id : 3;
	uint16_t    heading : 9;
	time_stamp _last_time_stamp;

	//uint8_t time_value[sizeof time_stamp];

	heading_payload() {
		id = HEADING_MSG_ID;
		heading = 0;
	}

};

struct speed_payload
{
	uint8_t  id : 3;
	uint8_t  speed;
	time_stamp _last_time_stamp;

	//uint8_t time_value[sizeof time_stamp];

	speed_payload() {
		id = SPEED_MSG_ID;
		speed = 0;
	}
};

struct brakes_payload
{
	uint8_t  id : 3;              // 3 bits
	uint8_t  brakes : 1;           // 1 bits
	//uint8_t time_value[sizeof time_stamp];;     // 27 bits
	time_stamp _last_time_stamp;

	brakes_payload() {
		id = BRAKES_MSG_ID;
		brakes = 0;
	}
};


struct full_payload
{
	location_payload    _location_payload;

	heading_payload     _heading_payload;

	speed_payload       _speed_payload;

	brakes_payload      _brakes_payload;

};
