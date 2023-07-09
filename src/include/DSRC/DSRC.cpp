#include "DSRC.hpp"
#include <thread>
#include "encoder.hpp"
#include <string>
#include "CONFIG.h"
#include <mutex>

using namespace std;

/*  [int] The return value of open() is a file descriptor, a small,
    nonnegative integer that is an index to an entry in the process's
    table of open file descriptors. */
int USB;
unsigned char read_buf[256];
int read_buf_size = 0;

extern string TTYUSB_DEVICE;
extern string THREAD_TERMINAL_OUTPUT_DEVICE;

mutex write_lock;

int init_dsrc()
{
#ifndef _WIN32
#if VERBOSE_INFO
    cout << "[INFO] [DSRC] starting up DSRC device." << endl;
#endif

    USB = open(TTYUSB_DEVICE.c_str(), O_RDWR);
    if (USB < 0)
    {
        std::cout << "[ERROR] [DSRC] serial port device " << TTYUSB_DEVICE << "NOT OPENING"
                  << endl
                  << errno << strerror(errno) << endl;
    }
    char cfg[200];
    sprintf(cfg, "stty -F %s cs8 115200 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts -hupcl\0", TTYUSB_DEVICE.c_str());
    cout << "[EXECUTING] " << cfg << endl;
    system(cfg);

    struct termios tty;

    memset(&tty, 0, sizeof tty);

    /* Error Handling */
    if (tcgetattr(USB, &tty) != 0)
    {
        std::cout << "[ERROR] [DSRC] " << errno << " from tcgetattr: " << strerror(errno) << std::endl;
    }

    // // /* Set Baud Rate */
    // cfsetspeed(&tty, (speed_t)B115200);

    // Set in/out baud rate
    cfsetispeed(&tty, (speed_t)B115200);
    cfsetospeed(&tty, (speed_t)B115200);

    if (tcsetattr(USB, TCSANOW, &tty) != 0)
    {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return -1;
    }

#if VERBOSE_INFO
    cout << "[INFO] [DSRC] done starting up DSRC device." << endl;
#endif

#endif
    return 0;
}

void dsrc_broadcast(uint8_t payload[], int size)
{
    SEND_COLOR;

    write_lock.lock();
    cout << "[INFO] [DSRC] [VAR] output data length: " << size << endl;
    // Write payload size first
    write(USB, (unsigned char *)&size, 1);
    // Write the payload
    write(USB, (unsigned char *)payload, size);
    write_lock.unlock();

#if VERBOSE_SENT
    cout << "[INFO] [DSRC] [BROADCAST] just broadcasted: " << payload << endl;
    cout << "[INFO] [DSRC] [BROADCAST] just broadcasted(int): ";
    // for (int i = 0; i < size + 1; i++)
    for (int i = 0; i < size; i++)
    {
        printf("%d ", (unsigned char)payload[i]);
    }
    cout << "\n";
#endif
    RESET_COLOR;
}

// #define __linux__

void dsrc_read()
{
    REC_COLOR;
    read(USB, &read_buf_size, 1);
    cout << "[INFO] [DSRC] [RECIEVE] REC BUFFER DATA LENGTH(INCLUDING MAC ADDRESS): " << read_buf_size << endl;
    memset(&read_buf, 0, read_buf_size + 2);

    int actual_read_size = 0;

    write_lock.lock();

    do
    {
        printf("[DEBUG] [DSRC] [INFO] actual_read_size = %d\n", actual_read_size);
        actual_read_size += read(USB, &read_buf[actual_read_size], read_buf_size - actual_read_size);
    } while (actual_read_size < read_buf_size);

    if (actual_read_size != read_buf_size)
    {
        printf("[ERROR] [DSRC] actual_read_size != read_buf_size\n");
        printf("%d != %d\n", actual_read_size, read_buf_size);
        exit(-1);
    }
    // write success message
    write_lock.unlock();

#if DSRC_READ_PRINT
    cout << "[INFO] [DSRC] [RECIEVE] REC BUFFER: ";
    for (int i = 0; i < actual_read_size; i++)
    {
        printf("%c", (uint8_t)read_buf[i]);
    }
    cout << "\n[INFO] [DSRC] [RECIEVE] REC BUFFER (HEX): "; // for repeating messaging
    for (int i = 0; i < actual_read_size; i++)
    {
        printf("\\x%x", (uint8_t)read_buf[i]);
    }
    cout << "\n[INFO] [DSRC] [RECIEVE] REC BUFFER(int): ";
    for (int i = 0; i < 12; i++)
    {
        printf("%d ", (uint8_t)read_buf[i]);
    }
    cout << ": ";
    for (int i = 12; i < actual_read_size; i++)
    {
        printf("%d ", (uint8_t)read_buf[i]);
    }
    cout << "\n";

#endif
    RESET_COLOR;
}

void DSRC_read_thread(void (*cb_function)(char buffer[], int buffer_size))
{
    while (1)
    {
        dsrc_read();
        if (read_buf_size == 0)
            continue;

#if DSRC_READ_PRINT
        cout << "\tMAC: ";
        for (int i = 0; i < 12; i++)
        {
            printf("%c", read_buf[i]);
        }
        cout << "\n\tPAYLOAD: ";
        for (int i = 12; i < read_buf_size; i++)
        {
            printf("%c", read_buf[i]);
        }
        cout << "\n";
#endif

        char *buffer_copy = new char[read_buf_size];
        memcpy((char *)buffer_copy, (char *)read_buf, (uint8_t)read_buf_size);
        std::thread thread_object(cb_function, std::ref(buffer_copy), read_buf_size);
        thread_object.detach();
    }
}