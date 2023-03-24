#include "DSRC.h"
#include <map>
#include <thread>
#include "encoder.h"

using namespace std;

/*  [int] The return value of open() is a file descriptor, a small,
    nonnegative integer that is an index to an entry in the process's
    table of open file descriptors. */
int USB;
map<int, uint8_t *> buffers_for_all_sizes;
char read_buf[256];

int init_dsrc()
{
#ifndef _WIN32
#if VERBOSE_INFO
    cout << "[info] starting up DSRC device sending" << endl;
#endif

    USB = open(TTYUSB_DEVICE, O_RDWR);
    if (USB < 0)
    {
        std::cout << "[ERROR] serial port device " << TTYUSB_DEVICE << "NOT OPENING"
                  << endl
                  << errno << strerror(errno) << endl;
    }

    struct termios tty;

    memset(&tty, 0, sizeof tty);

    /* Error Handling */
    if (tcgetattr(USB, &tty) != 0)
    {
        std::cout << "[ERROR] " << errno << " from tcgetattr: " << strerror(errno) << std::endl;
    }

    /* Set Baud Rate */
    cfsetspeed(&tty, (speed_t)B115200);

    // tty.c_cc[VTIME] = 0; // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    // tty.c_cc[VMIN] = 20;

    // tty.c_iflag |= IGNBRK;
    // tty.c_cflag &= ~(BRKINT);
    // tty.c_iflag &= ~ICRNL;
    // tty.c_iflag &= ~IMAXBEL;
    // tty.c_oflag &= ~OPOST;
    // tty.c_oflag &= ~ONLCR;
    // tty.c_iflag &= ~ISIG;
    // tty.c_iflag &= ~ICANON;
    // tty.c_iflag &= ~IEXTEN;
    // tty.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHOCTL | ECHOKE);
    // tty.c_iflag |= NOFLSH;
    // tty.c_iflag &= ~IXON;
    // tty.c_cflag &= ~CRTSCTS;
    // tty.c_cflag &= ~HUPCL;
    // tty.c_cflag |= CS8;

    tty.c_cflag |= CLOCAL | CREAD;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;      /* 8-bit characters */
    tty.c_cflag &= ~PARENB;  /* no parity bit */
    tty.c_cflag &= ~CSTOPB;  /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS; /* no hardware flowcontrol */

    tty.c_lflag |= ICANON | ISIG; /* canonical input */
    tty.c_lflag &= ~(ECHO | ECHOE | ECHONL | IEXTEN);

    tty.c_iflag &= ~IGNCR; /* preserve carriage return */
    tty.c_iflag &= ~INPCK;
    tty.c_iflag &= ~(INLCR | ICRNL | IUCLC | IMAXBEL);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); /* no SW flowcontrol */

    tty.c_oflag &= ~OPOST;

    tty.c_cc[VEOL] = 0;
    tty.c_cc[VEOL2] = 0;
    tty.c_cc[VEOF] = 0x04;

    // Set in/out baud rate
    cfsetispeed(&tty, (speed_t)B115200);
    cfsetospeed(&tty, (speed_t)B115200);

    if (tcsetattr(USB, TCSANOW, &tty) != 0)
    {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return -1;
    }

    // unsigned char msg[] = "[besm allah]\x04\n";

    // write(USB, msg, sizeof(msg) - 1);
    // // Allocate memory for read buffer, set size according to your needs
    // char read_buf[256];
    // while (1)
    // {
    //     memset(&read_buf, 0, 256);
    //     // Read bytes. The behaviour of read() (e.g. does it block?,
    //     // how long does it block for?) depends on the configuration
    //     // settings above, specifically VMIN and VTIME
    //     int n = read(USB, &read_buf, sizeof(read_buf));
    //     std::cout << read_buf << endl;
    //     // n is the number of bytes read. n may be 0 if no bytes were received, and can also be negative to signal an error.
    // }

#endif
    return 0;
}

void dsrc_broadcast(uint8_t payload[], int size)
{
#ifndef _WIN32
    size--;
    if (buffers_for_all_sizes[size] == 0)
    {
        buffers_for_all_sizes[size] = new uint8_t[size + 2];
        buffers_for_all_sizes[size][size] = PAYLOAD_USB_LIMITER;
        buffers_for_all_sizes[size][size + 1] = PAYLOAD_ESP_LIMITER;
    }
    memcpy((buffers_for_all_sizes[size]), payload, size);
    write(USB, (unsigned char *)buffers_for_all_sizes[size], size + 2);
#else 
    memcpy(read_buf, payload, size);
#endif
}

//#define __linux__

void dsrc_read()
{

#ifndef _WIN32
    memset(&read_buf, 0, 256);
    int n = read(USB, &read_buf, sizeof(read_buf));
    // cout << read_buf << endl;
#else
    std::this_thread::sleep_for(std::chrono::seconds(10));
    //char dump[] = "aabbccddeeff[dump_msg]";
    //memcpy(read_buf, dump, sizeof dump);
#endif
}

void DSRC_read_thread(void (*cb_function)(char buffer[], int buffer_size))
{
    while (1)
    {
        dsrc_read();
#if DSRC_READ_PRINT
        cout << read_buf << endl;
#endif  
        int buffer_size = strlen(read_buf);
        char *buffer_copy = new char[buffer_size + 1];
        strncpy(buffer_copy, read_buf, buffer_size);
        buffer_copy[buffer_size] = '\0';
        std::thread thread_object(cb_function, std::ref(buffer_copy), buffer_size);
        //std::thread thread_object(cb_function, read_buf, buffer_size);
        thread_object.detach();
    }
}