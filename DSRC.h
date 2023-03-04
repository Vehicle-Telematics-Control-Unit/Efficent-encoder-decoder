#include <stdio.h>
#include <iostream>
#include <fcntl.h>
// #include "payloads.h"

#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
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


char * dsrc_read();

void DSRC_read_thread();