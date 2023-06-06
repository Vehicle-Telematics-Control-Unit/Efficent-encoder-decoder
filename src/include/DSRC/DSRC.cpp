#include "DSRC.hpp"
#include <thread>
#include "encoder.hpp"
#include <string>
#include "CONFIG.h"

using namespace std;

/*  [int] The return value of open() is a file descriptor, a small,
    nonnegative integer that is an index to an entry in the process's
    table of open file descriptors. */
int USB;
unsigned char read_buf[256];
int read_buf_size = 0;

extern string TTYUSB_DEVICE;
extern string THREAD_TERMINAL_OUTPUT_DEVICE;

int init_dsrc()
{
#ifndef _WIN32
#if VERBOSE_INFO
    cout << "[INFO] starting up DSRC device." << endl;
#endif

    USB = open(TTYUSB_DEVICE.c_str(), O_RDWR);
    if (USB < 0)
    {
        std::cout << "[ERROR] serial port device " << TTYUSB_DEVICE << "NOT OPENING"
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
        std::cout << "[ERROR] " << errno << " from tcgetattr: " << strerror(errno) << std::endl;
    }

    // /* Set Baud Rate */
    cfsetspeed(&tty, (speed_t)B115200);

    // tty.c_cc[VTIME] = 0; // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    // tty.c_cc[VMIN] = 20;

    // tty.c_cflag &= ~(BRKINT);
    // tty.c_cflag &= ~CRTSCTS;
    // tty.c_cflag &= ~HUPCL;
    // tty.c_cflag |= CS8;

    // tty.c_iflag |= IGNBRK;
    // tty.c_iflag &= ~ICRNL;
    // tty.c_iflag &= ~IMAXBEL;
    // tty.c_iflag &= ~ISIG;
    // tty.c_iflag &= ~ICANON;
    // tty.c_iflag &= ~IEXTEN;
    // tty.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHOCTL | ECHOKE);
    // tty.c_iflag |= NOFLSH;
    // tty.c_iflag &= ~IXON;

    // tty.c_oflag &= ~OPOST;
    // tty.c_oflag &= ~ONLCR;

    ////////////////////////////////////////////////////////

    // tty.c_cflag |= CLOCAL | CREAD;
    // tty.c_cflag &= ~CSIZE;
    // tty.c_cflag |= CS8;      /* 8-bit characters */
    // tty.c_cflag &= ~PARENB;  /* no parity bit */
    // tty.c_cflag &= ~CSTOPB;  /* only need 1 stop bit */
    // tty.c_cflag &= ~CRTSCTS; /* no hardware flowcontrol */

    // tty.c_lflag |= ICANON | ISIG; /* canonical input */
    // tty.c_lflag &= ~(ECHO | ECHOE | ECHONL | IEXTEN);

    // tty.c_iflag &= ~IGNCR; /* preserve carriage return */
    // tty.c_iflag &= ~INPCK;
    // tty.c_iflag &= ~(INLCR | ICRNL | IUCLC | IMAXBEL);
    // tty.c_iflag &= ~(IXON | IXOFF | IXANY); /* no SW flowcontrol */

    // tty.c_oflag &= ~OPOST;

    // tty.c_cc[VEOL] = 0;
    // tty.c_cc[VEOL2] = 0;
    // tty.c_cc[VEOF] = 0x04;

    // Set in/out baud rate
    cfsetispeed(&tty, (speed_t)B115200);
    cfsetospeed(&tty, (speed_t)B115200);

    if (tcsetattr(USB, TCSANOW, &tty) != 0)
    {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return -1;
    }

#if VERBOSE_INFO
    cout << "[INFO] done starting up DSRC device." << endl;
#endif

#endif
    return 0;
}

void dsrc_broadcast(uint8_t payload[], int size)
{
SEND_COLOR;
#ifndef _WIN32
    cout << "[INFO] [VAR] output data length: " << size << endl;

    // Write payload size first
    write(USB, (unsigned char *)&size, 1);
    // Write the payload
    write(USB, (unsigned char *)payload, size);
#else
    strcpy(read_buf, "aabbccddeeff");
    memcpy(&read_buf[12], payload, size);
    read_buf_size = 12 + size;
#endif

#if VERBOSE_SENT
    cout << "[INFO] [BROADCAST] just broadcasted: " << payload << endl;
    cout << "[INFO] [BROADCAST] just broadcasted(int): ";
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
#ifndef _WIN32
    read(USB, &read_buf_size, 1);
    cout << "[INFO] [RECIEVE] REC BUFFER DATA LENGTH(INCLUDING MAC ADDRESS): " << read_buf_size << endl;
    memset(&read_buf, 0, read_buf_size + 2);
    // while (read_buf_size == 0)
    // sleep(0.1);
    int actual_read_size = 0;
    do{
        printf("[DEBUG] [INFO] actual_read_size = %d\n", actual_read_size);
        actual_read_size += read(USB, &read_buf[actual_read_size], read_buf_size - actual_read_size);
    } while (actual_read_size < read_buf_size);

    if(actual_read_size != read_buf_size){
        printf("[ERROR] actual_read_size != read_buf_size\n");
        printf("%d != %d\n", actual_read_size, read_buf_size);
        exit(-1);
    }

#if DSRC_READ_PRINT
    cout << "[INFO] [RECIEVE] REC BUFFER: ";
    for (int i = 0; i < actual_read_size; i++)
    {
        printf("%c", (uint8_t)read_buf[i]);
    }
    cout << "\n[INFO] [RECIEVE] REC BUFFER (HEX): "; // for repeating messaging
    for (int i = 0; i < actual_read_size; i++)
    {
        printf("\\x%x", (uint8_t)read_buf[i]);
    }
    cout << "\n[INFO] [RECIEVE] REC BUFFER(int): ";
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
#else
    std::this_thread::sleep_for(std::chrono::seconds(2));
    // char dump[] = "aabbccddeeff[dump_msg]";
    // memcpy(read_buf, dump, sizeof dump);
#endif
RESET_COLOR;
}

void DSRC_read_thread(void (*cb_function)(char buffer[], int buffer_size))
{
    while (1)
    {
        dsrc_read();
#if DSRC_READ_PRINT
        // cout << read_buf << endl;
        cout << "\tMAC: ";
        for (int i = 0; i < 12; i++)
        {
            printf("%c",read_buf[i]);
        }
        cout << "\n\tPAYLOAD: ";
        for (int i = 12; i < read_buf_size; i++)
        {
            printf("%c",read_buf[i]);
        }
        cout << "\n";

#endif
        char *buffer_copy = new char[read_buf_size];
        memcpy((char *)buffer_copy, (char *)read_buf, (uint8_t)read_buf_size);
        std::thread thread_object(cb_function, std::ref(buffer_copy), read_buf_size);
        thread_object.detach();

        // int buffer_size = read_buf_size;
        // char *buffer_copy = new char[buffer_size];
        // memcpy(buffer_copy, read_buf, buffer_size);
        // // buffer_copy[buffer_size] = '\0';
        // std::thread thread_object(cb_function, std::ref(buffer_copy), buffer_size);
        // // std::thread thread_object(cb_function, read_buf, buffer_size);
        // thread_object.detach();
    }
}