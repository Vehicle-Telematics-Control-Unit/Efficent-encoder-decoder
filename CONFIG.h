#define TTYUSB_DEVICE "/dev/ttyUSB0"
#define THREAD_TERMINAL_OUTPUT_DEVICE "/dev/pts/17"
// #define payload_size 13
#define VERBOSE_INFO true

#define VERBOSE_BUFFER true

#if VERBOSE_BUFFER == true
    #define VERBOSE_RECIEVED_MESSAGES true
    #define VERBOSE_RECIEVED_MAC true
#endif


#define DSRC_READ_PRINT false
#define PAYLOAD_USB_LIMITER 4
#define PAYLOAD_ESP_LIMITER 255