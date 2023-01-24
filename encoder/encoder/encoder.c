#include <stdio.h>
#include <stdint.h>
//#include <stdlib.h>

#define payload_size 13

typedef union {
	uint8_t value[payload_size]; // total used bits 101, total available 104 bits (using 13 bytes)
	struct structure{

		// time
		uint8_t hh : 5;
		uint8_t mm : 6;
		uint8_t ss : 6;
		uint16_t ns; // max value (65535), suggested maximum (9999)
		
		// location
		int8_t lat : 8; // range shall be (-85 to 85)
		uint16_t lat_frac; // max value (65535), suggested maximum (9999)

		int16_t lon : 9; // range shall be (-180 to 180)
		uint16_t lon_frac; // max value (65535), suggested maximum (9999)

		//heading
		uint16_t heading : 9;

		//speed
		uint8_t speed;

		
		/* BOOLEANS */

		// brakes
		uint8_t brakes : 1;
		uint8_t gps_updated : 1;

	} data ;
} payload;

/**
 * @brief puts payload data into a small decodeable format to be decoded on recieving side.
 * @brief not essential since you can directly acces payload value.
 * @param p pointer to the payload to have its data encoded
 * @param buffer buffer to put output encode into
 */
void encode(payload *p, uint8_t* buffer) {
	memcpy(buffer, p->value, payload_size);
}

/**
 * @brief decodes payload recieved from other senders
 * @param p pointer to a temp payload to read decoded values
 * @param buffer buffer to put value into
 */
void decode(payload *p, uint8_t* buffer) {
	memcpy(p->value, buffer, payload_size);
}

void display_payload_data(payload *p) {
	struct structure* ptr = &p->data;
}

void main() {
	payload p;
	struct structure* ptr = &p.data;

	ptr->hh = 23;
	ptr->mm = 59;
	ptr->ss = 59;
	ptr->ns = 9999;
	ptr->brakes = 1;
	ptr->gps_updated = 1;
	ptr->lat = -85;
	ptr->lat_frac = 9999;
	ptr->lon = -180;
	ptr->lon_frac = 9999;
	ptr->heading = 140;
	ptr->speed = 90;


	//printf("%d %d %d %d\n", ptr->lat, ptr->mm, ptr->ss, ptr->ns);

	uint8_t output[13];

	for (int i = 0; i < payload_size; i++) {
		printf("%d", p.value[i]);
		output[i] = p.value[i];
	}
	// output = 215251251204153917120415397620515

	// test -> assign this output to data after changing some values
	// check if same data values
	ptr->lat_frac = 0;
	ptr->mm = 0;
	ptr->ss = 0;

	memcpy(p.value, output, payload_size);
	// check if values is the same old values

}