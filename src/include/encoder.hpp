#ifndef encoder
#define encoder

#define RPI
#define MAC_ADDR_SIZE 12

#define VERBOSE_RECIEVED_MESSAGES_DECODE_PRINT(PAYLOAD)  \
	cout << "[BUFFER] [REC_PAYLOAD_DECODE] [BEGIN] :\n"; \
	(*((PAYLOAD *)&buffer[12])).print();                 \
	cout << "[BUFFER] [REC_PAYLOAD_DECODE] [DONE]\n\n"; \


void on_payload_recieved(char buffer[], int buffer_size);

void color_term(int x, int y);
void color_term_reset();

#endif