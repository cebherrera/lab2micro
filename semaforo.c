#include <avr/io.h>
#include <avr/interrupt.h>

// Variables Globales
volatile int counter = 0; 
volatile int boton = 0; 

// Estados de ME
typedef enum {
    estado0 = 0,
    estado1 = 1,
    estado2 = 2,
    estado3 = 3,
    estado4 = 4,
    estado5 = 5
} State;

// Estado de LED
typedef enum {
    Apagado = 0,
    Encendido = 1
} LEDState;

// Estados de Contador
typedef enum {
    s10 = 652,
    s3 = 196,
    s1 = 33
} CounterState;

// Funcion Principal
int main(void)
{
  sei(); // Se establece el bit I - Global Interrupt Enable

  // Configuracion de Registros
  TCCR0B |= (1 << CS00 ) | (1 << CS02 ) ; // CLK = CLK_IO[8 MHz]/1024 = 7812.5 HZ -> 1.28*10-4 s (Periodo del Reloj)

  TCCR0A |= (1 << WGM01 ) ; // Modo CTC 

  OCR0A  |= 0xF0; // Valor que se compara con TCNT0

  DDRB   |= (1 << DDB3 ) | (1 << DDB2 ) | (1 << DDB1 ) | (1 << DDB0 ); // Se establece el PIN B0, B1, B2 y B3 como Salida

  TIMSK  |= (1 << OCIE0A ) ; // Se habilita la interrupcion por comparacion con OCR0A    

  MCUCR  |= (1 << ISC01 ) | (1 << ISC11 ) ; //Se configura la interrupcion INT0 y INT1 por flanco decreciente

  GIMSK  |= (1 << INT1 ) | (1 << INT0 ) ; //Se habilita la interrupcion por INT0 y INT1

  // Estado Inicial
  int estado = 0; // Variable local
  PORTB |=  (1 << PORTB1 ); // B1 (LDPV) Encendido 
  PORTB &= ~(1 << PORTB0 ); // B0 (LDVD) Apagado
  PORTB &= ~(1 << PORTB2 ); // B2 (LDPP) Apagado 
  PORTB |=  (1 << PORTB3 ); // B3 (LDPD) Encendido

  // Logica Combinacional
  while (1) {
    switch (estado){
    case 0:
      switch (counter){
        case 326:
          // Pasaron los 10s
          estado = 1
          break;
        default:
          // No han pasado los 10s
          PORTB |=  (1 << PORTB1 ); // B1 (LDPV) Encendido 
          PORTB &= ~(1 << PORTB0 ); // B0 (LDVD) Apagado
          PORTB &= ~(1 << PORTB2 ); // B2 (LDPP) Apagado 
          PORTB |=  (1 << PORTB3 ); // B3 (LDPD) Encendido
          break;
      }
      break;
      
      case 1:
        switch (boton){
          case 1:
            /* code */
            break;
          
          default:
            break;
          }

    default:
      break;      
    }
  }
}

  // Logica Secuencial (Interrupciones)
  ISR ( TIMER0_COMPA_vect )
{
  counter = counter + 1; 
}

  ISR ( INT0_vect )
{
  boton = 1; 
}

  ISR ( INT1_vect )
{
  boton = 1; 
}
