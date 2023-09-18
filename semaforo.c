#include <avr/io.h>
#include <avr/interrupt.h>

// Variables Globales
volatile int counter = 0; 
volatile int boton = 0; 

// Estados de ME
typedef enum {
    espera10s = 0,
    revisaboton = 1,
    botonpresionado = 2,
    espera1s = 3,
    espera10s2 = 4,
    toggleLDDP = 5,
    espera1s2 = 6
} State;

// Estados de Contador
typedef enum {
    s10  = 652,
    s3   = 196,
    s1   = 66,
    s0_5 = 33
} CounterState;

// Estados de Boton
typedef enum {
    presionado    = 1,
    no_presionado = 0
} ButtonState;

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
  int estado = espera10s; // Variable local
  int countiter = 0; // Variable local
  PORTB |=  (1 << PORTB1 ); // B1 (LDPV) Encendido 
  PORTB |=  (1 << PORTB3 ); // B3 (LDPD) Encendido
  PORTB &= ~(1 << PORTB0 ); // B0 (LDVD) Apagado
  PORTB &= ~(1 << PORTB2 ); // B2 (LDPP) Apagado 

  // Logica Combinacional
  while (1) {
    switch (estado){
    case espera10s:
        switch (counter){
        case s10:
            // Pasaron los 10s
            estado = revisaboton;
            break;
        default:
            // No han pasado los 10s
            PORTB |=  (1 << PORTB1 ); // B1 (LDPV) Encendido 
            PORTB |=  (1 << PORTB3 ); // B3 (LDPD) Encendido
            PORTB &= ~(1 << PORTB0 ); // B0 (LDVD) Apagado
            PORTB &= ~(1 << PORTB2 ); // B2 (LDPP) Apagado 
            break;
          }
        break;
    case revisaboton:
        switch (boton){
        case presionado:
            PINB |=  (1 << PINB1 ); // B1 (LDPV) toggle
            PORTB |=  (1 << PORTB3 ); // B3 (LDPD) Encendido 
            counter = 0;
            boton = 0;
            estado = botonpresionado;
            break;
        case no_presionado:
            // No se ha presionado el boton
            break;
          }
        break;
    case botonpresionado:
        //boton = no_presionado;
        switch (counter){
        case s0_5:
            PINB |=  (1 << PINB1 ); // B1 (LDPV) toggle
            PORTB |=  (1 << PORTB3 ); // B3 (LDPD) Encendido
            counter = 0;
            countiter = countiter + 1;
            switch (countiter){
            case 6:
                PORTB |=  (1 << PORTB0 ); // B0 (LDVD) Encendido
                counter = 0;
                countiter = 0;
                estado = espera1s;
                break;
            default:
                // No han pasado los 3s
                break;
            }
            break;
        default:
            // No ha pasado 0.5s
            break;
          }
        break;
    case espera1s:
        switch (counter){
        case s1:
            PORTB |=   (1 << PORTB2 ); // B2 (LDPP) Enciende
            PORTB &=  ~(1 << PORTB3 ); // B3 (LDPD) Apaga
            counter = 0;
            estado = espera10s2;
            break;
        default:
            // NO ha pasado 1s
            break;
        }
        break;
    case espera10s2:
        switch (counter){
        case s10:
            estado = toggleLDDP;
            PINB |=  (1 << PINB2 ); // B2 (LDPP) toggle
            PORTB |=  (1 << PORTB0 ); // B0 (LDVD) Encendido
            counter = 0;
            break;
        default:
            break;
        }
        break;
    case toggleLDDP:
        switch (counter){
        case s0_5:
            PINB |=  (1 << PINB2 ); // B2 (LDPP) toggle
            PORTB |=  (1 << PORTB0 ); // B0 (LDVD) Encendido
            counter = 0;
            countiter = countiter + 1;
            switch (countiter){
            case 6:
                PORTB |=  (1 << PORTB3 ); // B3 (LDPD) Encendido
                counter = 0;
                countiter = 0;
                estado = espera1s2;
                break;
            default:
                // No han pasado los 3s
                break;
            }
            break;
        default:
            // No ha pasado 0.5s
            break;
          }
        break;
    case espera1s2:
        switch (counter){
        case s1:
          PORTB |=  (1 << PORTB1 ); // B1 (LDPV) Encendido
          PORTB &= ~(1 << PORTB0 ); // B0 (LDVD) Apagado
          counter = 0;
          estado = espera10s;
          break;
        
        default:
          // NO ha pasado 1s
          break;
        }
        break;
    default:
        // Caso en que ME entre en estado desconocido
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
  boton = presionado; 
}

  ISR ( INT1_vect )
{
  boton = presionado; 
}
