/*============================================================================
 * Licencia: BSD 3 Clauses
 * Autor: Gastón Vallasciani
 * Fecha: 19/04/2017
 *===========================================================================*/

/*==================[inclusiones]============================================*/

#include "programa.h"   // <= su propio archivo de cabecera
#include "sapi.h"       // <= Biblioteca sAPI
//#include "Debounce.h"
#include "teclado_matricial.h"

/*==================[definiciones y macros]==================================*/

#define DECENA 10*digitos[0]
#define UNIDAD digitos[1]

/*==================[definiciones de datos internos]=========================*/

static uint8_t j,k=0;
static uint16_t digitos[2],TEMPORIZADOR1,TEMPORIZADOR2,time_out=50;

typedef enum{
   EN_ESPERA_DE_DIGITO_1,      // 0
   EN_ESPERA_DE_DIGITO_2_O_LETRA, // 1
   EN_ESPERA_DE_LETRA,  // 2
   GUARDAR_PISO     // 3
} tecladoMEF_t;

/*==================[definiciones de datos externos]=========================*/

tecladoMEF_t estadoActualTeclado; // Variable de estado global
uint16_t pisos_pendientes[10];

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

uint16_t Act_TecladoMEF( void );
void Ini_TecladoMEF( void );

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
/*int main( void ){
   uint16_t piso = 0XFF;
   // ---------- CONFIGURACIONES ------------------------------
   boardConfig();   
   uartConfig( UART_USB, 9600 );
   
   configurarTecladoMatricial();
   Ini_AntirreboteMEF();
   Ini_TecladoMEF();
   
   // ---------- REPETIR POR SIEMPRE --------------------------
   while( TRUE )
   {      
      piso = Act_TecladoMEF();
      if (piso!=0xff)
      {
         uartWriteString( UART_USB, "piso = " );
         uartWriteByte( UART_USB, piso );
      }
     // key_pressedAscii();
   } 
   return 0;
}
*/

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/

void Ini_TecladoMEF( void ){
   
   estadoActualTeclado = EN_ESPERA_DE_DIGITO_1;
   
   for (j=0;j<10;j++)
      {
         pisos_pendientes[j] = 0xff;
      }
}

uint16_t Act_TecladoMEF( void ){
   uint16_t tecla2, tecla1, letra;
   uint8_t i;
   
 //  uartWriteByte( UART_USB, estadoActualTeclado );
 //  delay(100);
   switch(estadoActualTeclado) {      
      
      case  EN_ESPERA_DE_DIGITO_1:
      
            for (i=0;i<2;i++)
            {
               digitos[i] = 0xff;
            }
           
            tecla1 = key_pressedHex();
            
            if ((tecla1 == (0x01))||(tecla1 == (0x02))||(tecla1 == (0x03))||(tecla1 == (0x04))||(tecla1 == (0x05))||(tecla1 == (0x06))||(tecla1 == (0x07))||(tecla1 == (0x08))||(tecla1 == (0x09))||(tecla1 == (0x00))){
               estadoActualTeclado = EN_ESPERA_DE_DIGITO_2_O_LETRA;
               TEMPORIZADOR1 = 1;
               digitos[0] = tecla1;
//             uartWriteByte( UART_USB, digitos[0] );
            }
         
      break;
      
      case  EN_ESPERA_DE_DIGITO_2_O_LETRA:
         
            tecla2 = key_pressedHex();
            
            if ((tecla2 == (0x0b))||(TEMPORIZADOR1 == time_out)){
               estadoActualTeclado = EN_ESPERA_DE_DIGITO_1;
//               uartWriteString( UART_USB, "TIME-OUT 1" );
            }
            else if ((tecla2 == (0x01))||(tecla2 == (0x02))||(tecla2 == (0x03))||(tecla2 == (0x04))||(tecla2 == (0x05))||(tecla2 == (0x06))||(tecla2 == (0x07))||(tecla2 == (0x08))||(tecla2 == (0x09))||(tecla2 == (0x00))){
                  TEMPORIZADOR2 = 1;
                  digitos[1] = tecla2;
//                  uartWriteByte( UART_USB, digitos[1] );
                  estadoActualTeclado = EN_ESPERA_DE_LETRA;
               }
               
            TEMPORIZADOR1++;
      break;
     
    
      case  EN_ESPERA_DE_LETRA:
      
            letra = key_pressedHex();
            
            if (( letra == (0x0b) )||( TEMPORIZADOR2 == time_out )){
               estadoActualTeclado = EN_ESPERA_DE_DIGITO_1;
//             uartWriteString( UART_USB, "TIME-OUT 2" );
            }
            else if (letra == 0x0a){
                  estadoActualTeclado = GUARDAR_PISO;
//                uartWriteString( UART_USB, "PISO GUARDADO" );
               }
            
            TEMPORIZADOR2++;
         
      break;
      
      case  GUARDAR_PISO:
         
            pisos_pendientes[k] = 10*digitos[0] + digitos[1]; 
//          uartWriteByte( UART_USB, pisos_pendientes[k] );
      
            k++;
            if (k==10){
               k=0;
               }
            estadoActualTeclado = EN_ESPERA_DE_DIGITO_1;
               
            return(pisos_pendientes[k-1]);
               
      break;
            
      default:
            Ini_TecladoMEF();
      break;
      
   }
   return(0xff);  
}
/*==================[fin del archivo]========================================*/
