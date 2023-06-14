#include <payloads.hpp>
#include <DSRC.hpp>
#include <cstring>
#include "CONFIG.h"
#include <map>
#include <thread>
#include <string>
#include <UnityCommunicationServer.hpp>
#include <someipconf.hpp>
#include <ServiceManagerAdapter.hpp>
#include "encoder.hpp"
#include "asciArt.hpp"
#include "json.hpp"

using namespace std;

// #define RPI
// #define NO_VSOMEIP

string THREAD_TERMINAL_OUTPUT_DEVICE;
string TTYUSB_DEVICE;

full_payload my_vehicle;
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
#ifndef RPI 
		unity_visualize_location(rec_mac_address, surrounding_vehicles[rec_mac_address]->_location_payload.lat,
		surrounding_vehicles[rec_mac_address]->_location_payload.lon);
#endif

#ifdef VERBOSE_RECIEVED_MESSAGES_DECODE
		VERBOSE_RECIEVED_MESSAGES_DECODE_PRINT(location_payload);
#endif // VERBOSE_RECIEVED_MESSAGES_DECODE
		break;

	case HEADING_MSG_ID:
		vehicle_payload_heading_update(*(surrounding_vehicles[rec_mac_address]), *((heading_payload *)&buffer[MAC_ADDR_SIZE]), false);
#ifndef RPI 
		unity_visualize_heading(rec_mac_address, surrounding_vehicles[rec_mac_address]->_heading_payload.heading);
#endif

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

void on_heading_msg_recieved(const std::shared_ptr<vsomeip::message> &_response)
{
	std::cout<<"heading recieved!!!" << endl;
	std::stringstream its_message;
	its_message << "CLIENT: received a notification for event ["
				<< std::setw(4) << std::setfill('0') << std::hex
				<< _response->get_service() << "."
				<< std::setw(4) << std::setfill('0') << std::hex
				<< _response->get_instance() << "."
				<< std::setw(4) << std::setfill('0') << std::hex
				<< _response->get_method() << "] to Client/Session ["
				<< std::setw(4) << std::setfill('0') << std::hex
				<< _response->get_client() << "/"
				<< std::setw(4) << std::setfill('0') << std::hex
				<< _response->get_session()
				<< "] = ";

	std::shared_ptr<vsomeip::payload> its_payload = _response->get_payload();
	its_message << "(" << std::dec << its_payload->get_length() << ") ";
	for (uint32_t i = 0; i < its_payload->get_length(); ++i)
		its_message << its_payload->get_data()[i];

	cout << "heading = " << its_payload->get_data() << endl;

	try
	{
		// cout << "iam about to crash now in heading" << endl;
		my_vehicle._heading_payload.heading = (int)stoi((char *)its_payload->get_data());
		// encode_time(my_vehicle._heading_payload._last_time_stamp);
		dsrc_broadcast((uint8_t *)&(my_vehicle._heading_payload), sizeof(my_vehicle._heading_payload));
		my_vehicle._heading_payload.print();
	}
	catch (const std::exception &e)
	{
		std::cerr << "[ERROR] [on_heading_msg_recieved] data error!" << e.what() << '\n';
	}
}

void on_GPS_msg_recieved(const std::shared_ptr<vsomeip::message> &_response)
{
	std::stringstream its_message;
	its_message << "CLIENT: received a notification for event ["
				<< std::setw(4) << std::setfill('0') << std::hex
				<< _response->get_service() << "."
				<< std::setw(4) << std::setfill('0') << std::hex
				<< _response->get_instance() << "."
				<< std::setw(4) << std::setfill('0') << std::hex
				<< _response->get_method() << "] to Client/Session ["
				<< std::setw(4) << std::setfill('0') << std::hex
				<< _response->get_client() << "/"
				<< std::setw(4) << std::setfill('0') << std::hex
				<< _response->get_session()
				<< "] = ";

	std::shared_ptr<vsomeip::payload> its_payload = _response->get_payload();
	its_message << "(" << std::dec << its_payload->get_length() << ") ";
	for (uint32_t i = 0; i < its_payload->get_length(); ++i)
	{
		its_message << its_payload->get_data()[i];
	}
	std::cout << its_message.str() << std::endl;

	cout << "heading = " << its_payload->get_data() << endl;

	if (its_payload->get_length() == 0)
		return;

	// int heading;
	// sscanf((char *)its_payload->get_data(), "%d", &heading);

	try
	{
		std::string ss((char *)its_payload->get_data(), its_payload->get_length());
		using json = nlohmann::json;
		json gps_location = json::parse(ss);
		std::cout << gps_location.dump(4);

		int lat, lon;
		my_vehicle._location_payload.lat = std::stod(string(gps_location["lat"]).c_str());
		my_vehicle._location_payload.lon = std::stod(string(gps_location["lon"]).c_str());

		encode_time(my_vehicle._location_payload._last_time_stamp);
		dsrc_broadcast((uint8_t *)&(my_vehicle._location_payload), sizeof(my_vehicle._location_payload));
		my_vehicle._location_payload.print();
	}
	catch (const std::exception &e)
	{
		std::cerr << "[ERROR] [on_heading_msg_recieved] data error!" << e.what() << '\n';
	}
}

void on_speed_msg_recieved(const std::shared_ptr<vsomeip::message> &_response)
{

	std::stringstream its_message;
	its_message << "CLIENT: received a notification for event ["
				<< std::setw(4) << std::setfill('0') << std::hex
				<< _response->get_service() << "."
				<< std::setw(4) << std::setfill('0') << std::hex
				<< _response->get_instance() << "."
				<< std::setw(4) << std::setfill('0') << std::hex
				<< _response->get_method() << "] to Client/Session ["
				<< std::setw(4) << std::setfill('0') << std::hex
				<< _response->get_client() << "/"
				<< std::setw(4) << std::setfill('0') << std::hex
				<< _response->get_session()
				<< "] = ";

	std::shared_ptr<vsomeip::payload> its_payload = _response->get_payload();
	its_message << "(" << std::dec << its_payload->get_length() << ") ";
	for (uint32_t i = 0; i < its_payload->get_length(); ++i)
		its_message << its_payload->get_data()[i];

	cout << "speed = " << its_payload->get_data() << endl;

	try
	{
		my_vehicle._speed_payload.speed = (int)stoi((char *)its_payload->get_data());
		// encode_time(my_vehicle._speed_payload._last_time_stamp);
		dsrc_broadcast((uint8_t *)&(my_vehicle._speed_payload), sizeof(my_vehicle._speed_payload));
		my_vehicle._speed_payload.print();
	}
	catch (const std::exception &e)
	{
		std::cerr << "[ERROR] [on_speed_msg_recieved] data error!" << e.what() << '\n';
	}
}

/*
void on_brakes_msg_recieved(const std::shared_ptr<vsomeip::message> &_response)
{

	std::stringstream its_message;
	its_message << "CLIENT: received a notification for event ["
				<< std::setw(4) << std::setfill('0') << std::hex
				<< _response->get_service() << "."
				<< std::setw(4) << std::setfill('0') << std::hex
				<< _response->get_instance() << "."
				<< std::setw(4) << std::setfill('0') << std::hex
				<< _response->get_method() << "] to Client/Session ["
				<< std::setw(4) << std::setfill('0') << std::hex
				<< _response->get_client() << "/"
				<< std::setw(4) << std::setfill('0') << std::hex
				<< _response->get_session()
				<< "] = ";

	std::shared_ptr<vsomeip::payload> its_payload = _response->get_payload();
	its_message << "(" << std::dec << its_payload->get_length() << ") ";
	for (uint32_t i = 0; i < its_payload->get_length(); ++i)
		its_message << its_payload->get_data()[i];

	cout << "brakes = " << its_payload->get_data() << endl;

	my_vehicle._brakes_payload.brakes = stoi((char *)its_payload->get_data());
	dsrc_broadcast((uint8_t *)&(my_vehicle._brakes_payload), sizeof(my_vehicle._brakes_payload));
	my_vehicle._brakes_payload.print();
}
*/

/*
void on_location_msg_recieved(const std::shared_ptr<vsomeip::message> &_response)
{

	std::stringstream its_message;
	its_message << "CLIENT: received a notification for event ["
				<< std::setw(4) << std::setfill('0') << std::hex
				<< _response->get_service() << "."
				<< std::setw(4) << std::setfill('0') << std::hex
				<< _response->get_instance() << "."
				<< std::setw(4) << std::setfill('0') << std::hex
				<< _response->get_method() << "] to Client/Session ["
				<< std::setw(4) << std::setfill('0') << std::hex
				<< _response->get_client() << "/"
				<< std::setw(4) << std::setfill('0') << std::hex
				<< _response->get_session()
				<< "] = ";

	std::shared_ptr<vsomeip::payload> its_payload = _response->get_payload();
	its_message << "(" << std::dec << its_payload->get_length() << ") ";
	for (uint32_t i = 0; i < its_payload->get_length(); ++i)
		its_message << its_payload->get_data()[i];

	cout << "location = " << its_payload->get_data() << endl;

	int lat, lon;
	sscanf((char *)its_payload->get_data(), "%d.%d,%d.%d", &lat, &(my_vehicle._location_payload.lat_frac),
	&lon, &(my_vehicle._location_payload.lon_frac));
	my_vehicle._location_payload.lat = lat;
	my_vehicle._location_payload.lon = lon;

	dsrc_broadcast((uint8_t *)&(my_vehicle._location_payload), sizeof(my_vehicle._location_payload));
	my_vehicle._location_payload.print();
}
*/

int encoder_loop()
{
	printf("[info] encoder loop just started!");
	std::thread thread_object(DSRC_read_thread, std::ref(on_payload_recieved));

	payloads_initializer(my_vehicle);

	// unity_start_socket(my_vehicle);

	while (1)
	{
		// dsrc_broadcast((uint8_t *)&(my_vehicle._location_payload), sizeof(my_vehicle._location_payload));
		// dsrc_broadcast((uint8_t *)&(my_vehicle._heading_payload), sizeof(my_vehicle._heading_payload));
		// dsrc_broadcast((uint8_t *)&(my_vehicle._brakes_payload), sizeof(my_vehicle._brakes_payload));
		// dsrc_broadcast((uint8_t *)&(my_vehicle._speed_payload), sizeof(my_vehicle._speed_payload));
		// my_vehicle._heading_payload.print();
		// sleep(15);
	}
}

#ifdef RPI
int main(int argc, char *argv[])
{
	INFO_COLOR;
	TTYUSB_DEVICE = argv[1];
	THREAD_TERMINAL_OUTPUT_DEVICE = argv[2];


#ifndef NO_VSOMEIP

	std::shared_ptr<ServiceManagerAdapter> vsomeService_shared = std::make_shared<ServiceManagerAdapter>(SERVICE_ID, INSTANCE_ID, EVENTGROUP_ID, "encoder");
	if (!vsomeService_shared->init())
	{
		std::cerr << "Couldn't initialize vsomeip services" << std::endl;
		return -1;
	}

	std::vector<ServiceManagerAdapter::METHOD> methods;
	methods.push_back({SUB_SPEED_EVENT_ID, on_speed_msg_recieved});
	methods.push_back({SUB_HEADING_EVENT_ID, on_heading_msg_recieved});

	std::vector<ServiceManagerAdapter::METHOD> GPS_methods;
	methods.push_back({SUB_GPS_EVENT_ID, on_GPS_msg_recieved});
	// methods.push_back({SUB_BRAKES_EVENT_ID, on_brakes_msg_recieved});
	// methods.push_back({SUB_LOCATION_EVENT_ID, on_location_msg_recieved});

	vsomeService_shared->requestServicesANDRegisterMethods(REQUEST_SERVICE_ID, REQUEST_INSTANCE_ID, methods);
	vsomeService_shared->requestServicesANDRegisterMethods(REQUEST_GPS_SERVICE_ID, REQUEST_GPS_INSTANCE_ID, GPS_methods);
	std::thread subSpeed(std::move(std::thread([&]
											   { vsomeService_shared->subOnEvent(REQUEST_SERVICE_ID, REQUEST_INSTANCE_ID, SUB_SPEED_EVENT_ID); })));
	std::thread subHeading(std::move(std::thread([&]
												 { vsomeService_shared->subOnEvent(REQUEST_SERVICE_ID, REQUEST_INSTANCE_ID, SUB_HEADING_EVENT_ID); })));
	std::thread subLocation(std::move(std::thread([&]
												 { vsomeService_shared->subOnEvent(REQUEST_GPS_SERVICE_ID, REQUEST_GPS_INSTANCE_ID, SUB_GPS_EVENT_ID); })));

#endif

	cout << "[INFO] [VAR] TTYUSB_DEVICE:" << TTYUSB_DEVICE << '\n';
	cout << "[INFO] [VAR] THREAD_TERMINAL_OUTPUT_DEVICE:" << THREAD_TERMINAL_OUTPUT_DEVICE << '\n';
	
	// print_RPI(TTYUSB_DEVICE);

	std::thread print_RPI_active_thread(print_RPI_thread, TTYUSB_DEVICE);
	print_RPI_active_thread.detach();


	if (init_dsrc() == 0)
		;
	else
	{
		std::cout << "[ERROR] init_dsrc() failed" << std::endl;
		return -1;
	}

	color_term_reset();

	std::thread encoder_loop_thread(encoder_loop);
	encoder_loop_thread.detach();

#ifndef NO_VSOMEIP
	vsomeService_shared->start();
#else
	while(true);
#endif


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

	std::thread print_Unity_active_thread(print_Unity_thread, TTYUSB_DEVICE);
	print_Unity_active_thread.detach();


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
		sleep(1);
	}
	return 0;
}

#endif