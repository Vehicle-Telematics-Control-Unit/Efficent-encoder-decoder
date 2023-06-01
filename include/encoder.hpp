#ifndef encoder
#define encoder

void on_payload_recieved(char buffer[], int buffer_size);

void color_term(int x, int y);
void color_term_reset();

#endif