#include <stdio.h>
#include <stdint.h>
#include <ctime>
#include <chrono>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <sys/time.h>
#include <math.h>
#include <map>
#include "CONFIG.h"


enum MSG_ID{
    TIME_SYNC       = 0,        // not yet implemented
    LOCATION_MSG    = 1,
    HEADING_MSG     = 2,
    SPEED_MSG       = 3,
    BRAKES_MSG      = 4
};

enum DATA_SIZES{
    TIME_STAMP_VALUE_SIZE = 4
};


/** @struct time_stamp
 * @brief timestamp union structure, to be used with every other data struct
 */
typedef union
{
	uint8_t value[4];           // total bits needed = 27
	struct structure
	{
		uint8_t hh  :  5;
		uint8_t mm  :  6;
		uint8_t ss  :  6;
		uint16_t ms : 10;       // max value (1024), suggested maximum (999)
	}_time_;

}time_stamp;

typedef union
{
	uint8_t value[7];           // total used bits 101, total available 104 bits (using 13 bytes)
	struct structure
	{
		uint8_t     id: 3;
		uint32_t    time_value: 27; // 27 bits

		// location
		int8_t      lat :  8;	// range shall be (-85 to 85)
		uint16_t    lat_frac;   // max value (65535), suggested maximum (9999)
		int16_t     lon :  9;    // range shall be (-180 to 180)
		uint16_t    lon_frac;   // max value (65535), suggested maximum (9999)

	}data;

}location_payload;

typedef union
{
	uint8_t value[6];
	struct structure
	{
		uint8_t     id:          3;
		uint32_t    time_value: 27;     // 27 bits

		uint16_t    heading :    9;
	}data;

} heading_payload;

typedef union
{
	uint8_t value[5];
	struct structure
	{
		uint8_t  id: 3;
		uint32_t time_value: 27;     // 27 bits

		uint8_t  speed;
	}data;

} speed_payload;

typedef union
{
	uint8_t value[5];
	struct structure
	{
		uint8_t  id: 3;              // 3 bits
		uint32_t time_value: 27;     // 27 bits

		uint8_t  Brakes:1;           // 1 bits
	}data;                          // 31 bits -> 4 bytes

} brakes_payload;


struct full_payload
{
    location_payload    _location_payload;
    time_stamp          _last_location_time_stamp;

    heading_payload     _heading_payload;
    time_stamp          _last_heading_time_stamp;

    speed_payload       _speed_payload;
    time_stamp          _last_speed_time_stamp;

    brakes_payload      _brakes_payload;
    time_stamp          _last_brakes_time_stamp;

};
