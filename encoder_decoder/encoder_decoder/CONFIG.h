// #define TTYUSB_DEVICE "/dev/ttyUSB0"
// #define THREAD_TERMINAL_OUTPUT_DEVICE "/dev/pts/17"
// #define payload_size 13
#define VERBOSE_INFO true

#define VERBOSE_BUFFER true

#if VERBOSE_BUFFER == true
    #define VERBOSE_RECIEVED_MESSAGES true
    #define VERBOSE_RECIEVED_MESSAGES_DECODE true
    #define VERBOSE_RECIEVED_MAC true
#endif

#define VERBOSE_SENT true

#define DSRC_READ_PRINT true

#define PAYLOAD_USB_LIMITER 255
#define PAYLOAD_ESP_LIMITER 255

#define INFO_COLOR color_term(34, 49)
#define REC_COLOR color_term(32, 49)
#define SEND_COLOR color_term(35, 49)
#define RESET_COLOR color_term_reset()