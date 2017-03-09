#ifndef USART_H
#define USART_H

void init_usart();

void switch_usart_to_tx();
void switch_usart_to_rx();

void usart_send_char(char c);
void usart_send_str(char *s);

#endif
