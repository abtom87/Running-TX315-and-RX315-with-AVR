#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define FOSC            16000000UL
#define baud_rate       4800
#define baud            FOSC/16/baud_rate-1
#define SYNC            0xAA
#define RX_ADDR         0x21
#define LED_ON          0x11
#define LED_OFF         0x22

typedef enum state_t{LOW,HIGH}state;
typedef enum press_t{NOT_PRESSED, PRESSED}press_state;

void usart_init(unsigned int b_rate)
{
    UBRR0H=0x00;
    UBRR0L=0x00;
    UCSR0B=0x00;
    UCSR0C=0x00;
    UDR0  =0x00;
    
    UBRR0H  =(unsigned char)b_rate>>8;
    UBRR0L  =(unsigned char)b_rate;

    UCSR0B = ( (1<<RXEN0)  | (1<<TXEN0)    );
    UCSR0C = ( (1<<UCSZ00) | (1 << UCSZ01) );
}

void Transmit(unsigned char c)
{
   while( !(UCSR0A & (1<<UDRE0) ) );
     UDR0=c;
}

void send_payload(unsigned char Rx_addr, unsigned char data)
{
   Transmit(SYNC);
   _delay_ms(5);
   Transmit(Rx_addr);
   _delay_ms(5);
   Transmit(data);
}

press_state check_button_state()
{
 
  if( (PINC & (1<<PC0)) )
    return PRESSED;
  else
    return NOT_PRESSED;

}

int main (void)
{
   DDRC=0xFF;
   PORTC=0xFF;
   
   usart_init(baud);
   
   while(1)
   {

      if(check_button_state()==PRESSED)
      {
         send_payload(RX_ADDR,LED_ON);
         _delay_ms(10);
      }
      else
      {
         send_payload(RX_ADDR,LED_OFF);
         _delay_ms(10);
      }
      _delay_ms(10);
      
   }

   return 0;

}

