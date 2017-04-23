/*============================================================================
 * Description: Debounce  
 * Author: Gaston Vallasciani
 * Date: 08/04/2017
 *===========================================================================*/

/*==================[inclusiones]============================================*/

#include "Debounce.h"   // <= su propio archivo de cabecera
#include "sapi.h"       // <= Biblioteca sAPI

/*==================[definiciones y macros]==================================*/

// Nuevo tipo de datos enumerado llamado estadoMEF
typedef enum{
   BUTTON_UP,      // 0
   BUTTON_FALLING, // 1
   BUTTON_RISING,  // 2
   BUTTON_DOWN     // 3
} estadoMEF_t;

/*==================[definiciones de datos internos]=========================*/

/*==================[definiciones de datos externos]=========================*/

/*==================[definiciones de datos globales]=========================*/

// Variable de estado (global)
estadoMEF_t estadoActual;

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

// Prototipos de funciones
static uint8_t buttonPressed( void );
static uint8_t buttonReleased( void );

/*==================[funcion principal]======================================*/

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/

static delay_t debounce_delay;
static bool_t estado;

// Función Inicializar MEF
void Ini_AntirreboteMEF( void ){
   estadoActual = BUTTON_UP;   
   delayConfig( &debounce_delay, 40 );
}

/*Funcion Act_AntirreboteMEF: 
 *Descripcion: Se utiliza para actualizar la máquina de estados finitos que controla el antirrebote.
 * int Act_AntirreboteMEF(int button) 
 *se le pasa como variable el pin al cual se encuentra conectado el pulsador y devuelve "1" que indica boton pulsado, "0" boton liberado y "2" sin accion*/ 

uint8_t Act_AntirreboteMEF(uint8_t button){
   uint8_t estado_pulsador = 2;
   switch(estadoActual) {
      
      case  BUTTON_UP:
         // Actualizar salida en el estado
         estado = gpioRead(button);
      
         // Chequear si debo cambiar de estado
         // Si se cumplio el tiempo cambiar de estado
         if( !(estado) ){
            // Cambiar a otro estado
            estadoActual = BUTTON_FALLING; 
         }
      break;
      
      case BUTTON_FALLING:
         // Actualizar salida en el estado
   
         // Chequear si debo cambiar de estado
         // Si se cumplio el tiempo cambiar de estado
         if( delayRead(&debounce_delay) ){
            // Cambiar a otro estado
            if (!(gpioRead(button)))
            {
               estadoActual = BUTTON_DOWN;
               estado_pulsador = buttonPressed();
            }
            else
            {estadoActual = BUTTON_UP;}
         }
      break;
      
      case BUTTON_DOWN:
         // Actualizar salida en el estado
         estado = gpioRead(button);
         // Chequear si debo cambiar de estado
         // Si se cumplio el tiempo cambiar de estado
         if( estado ){
            // Cambiar a otro estado
            estadoActual = BUTTON_RISING; 
         }
      break;
      
      case BUTTON_RISING:
         // Actualizar salida en el estado
         
         // Chequear si debo cambiar de estado
         // Si se cumplio el tiempo cambiar de estado
         if( delayRead(&debounce_delay) ){
            // Cambiar a otro estado
            if (gpioRead(button))
            {
               estadoActual = BUTTON_UP;
               estado_pulsador = buttonReleased();
            }
            else
            {estadoActual = BUTTON_DOWN;}
         }  
      break;
      
      default:
         //Si algo modificó la variable estadoActual
         // a un estado no válido llevo la MEF a un
         // lugar seguro, por ejemplo, la reinicio:
         Ini_AntirreboteMEF();
      break;
   }  
   return(estado_pulsador);
}

static uint8_t buttonPressed( void ){
   uint8_t k;
   //gpioWrite(LED1,1);
   //uartWriteString( UART_USB ,"Button Pressed\r\n");
   k=1;
   return(k);
}

static uint8_t buttonReleased( void ){
   uint8_t k;
   //gpioWrite(LED1,0);
   //uartWriteString( UART_USB ,"Button Realeased\r\n");
   k=0;
   return(k);
}

/*==================[fin del archivo]========================================*/
