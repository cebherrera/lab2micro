#include <avr/io.h>
#include <avr/interrupt.h>

volatile int boton = 0; // Variable global, debe ser volatile

int main(void)
{
  sei(); // Se establece el bit I - Global Interrupt Enable

  TCCR0B |= (1 << CS00 ); TCCR0B |= (1 << CS02 ) ; // CLK = CLK_IO[1 MHz]/1024 = 976.5 HZ -> 1.02*10-3 s

  TCCR0A |= (1 << WGM01 ) ; // Modo CTC (OCOA?)

  //TCCR0A |= (1 << COM0A0 ) ; // Toggle OCOA on Compare Match (PIN B2)

  OCR0A = 0xF0; // Valor que se compara con TCNT0

  //DDRB = 0x04; // Se establece el PIN B2 como Salida

  DDRB = 0x08; // Se establece el PIN B3 como Salida

  TIMSK |= (1 << OCIE0A ) ; // TIFR |= (1 << OCF0A  ) ; // Se habilita la interrupcion por comparacion con OCR0A    

  int estado_LED = 0;

  // Logica Combinacional
  while (1) {
    switch (estado_LED){
    case 1:
        switch(boton){
          case 1:
            PORTB = 0x00; // PIN B3 como Sink (Se apaga LED)
            estado_LED = 0;
            boton = 0;
            break;
          case 0:
            // Led queda encendido
            break;
        }
        break;
    case 0:
        switch(boton){
          case 1:
            PORTB = 0x08; // PIN B3 como Source (Se enciende LED)
            estado_LED = 1;
            boton = 0;
            break;
          case 0:
            // Led queda apagado
            break;
        }
      break;
    }
  }
}

  // Logica Secuencial (Interrupciones)
  ISR ( TIMER0_COMPA_vect )
{
  boton = 1; 
}
