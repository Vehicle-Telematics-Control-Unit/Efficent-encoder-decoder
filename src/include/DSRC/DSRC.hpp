#ifndef DSRC
#define DSRC

#include <stdio.h>
#include <iostream>
#include <fcntl.h>
// #include "payloads.h"
#include <stdint.h>
#include <stdlib.h>

#ifdef __linux__
#include <unistd.h>
#include <termios.h>
#endif

#include <cstring>
#include "CONFIG.h"

/**
 * @brief initializes USB connection related to the dsrc(ESP) module
 *
 * @return 0 if success, else -1
 */
int init_dsrc();

/**
 * @brief adds bytes msg terminals and writes output to dsrc device on TTYUSB_DEVICE 
 * 
 * @param payload message be send
 * @param size how many bytes in the message, usually sizeof(message)   
 */
void dsrc_broadcast(uint8_t payload[], int size);

/**
 * @brief the function that actually gets the readings from the TTYUSB device
 * however, this function is to be called from the DSRC_read_thread function
 * @return void 
 */
void dsrc_read();

/**
 * @brief a thread function to keep listening for new messages, on recieving a new
 * message it opens a new thread for the call back function cb_function and gives it 
 * a copy of the buffer of the newly recieved message (without the termination character '\0')
 * the cb_function is meant to decode that message (buffer) of the other vehicles 
 * 
 * @param cb_function the call back function to be called on new message recieved, given a
 * copy of the message
 */
void DSRC_read_thread(void (*cb_function)(char buffer[], int buffer_size));

#endif