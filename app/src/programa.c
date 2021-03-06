/*============================================================================
 * Licencia: 
 * Autor: 
 * Fecha: 
 *===========================================================================*/


/*==================[inclusiones]============================================*/

#include "stdint.h"
#include "programa.h"   // <= su propio archivo de cabecera
#include "sapi.h"       // <= Biblioteca sAPI
#include "mefAscensor.h"
#include "driverTeclado.h"
#include "driverDisplay.h"
#include "cola_circular.h"

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/

/*==================[definiciones de datos externos]=========================*/
int8_t buffer[10];
cola_t pedido;

/*==================[declaraciones de funciones internas]====================*/
static void colaVacia( void )
{
}
static void colaLlena( void )
{
}
/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main( void ){

   // ---------- CONFIGURACIONES ------------------------------
   // Inicializar y configurar la plataforma
   boardConfig();
   uartConfig(UART_USB,115200);
   configCola(&pedido,buffer,sizeof(buffer)/sizeof(int8_t),colaVacia,colaLlena);
   //inicializar driver de teclado
   //inicializar driver de display
   //inicializar mefAscensor
   //inicializar manejo de puertas
   uartWriteString(UART_USB,"Bienvenidos \n\r");


   // ---------- REPETIR POR SIEMPRE --------------------------
   while( TRUE )
   {  
     //Actualizar driverDisplay
     //Actualizar driverTeclado
     //Actualizar mefAscensor
     //Actualizar mefManejo de Puertas
   } 

   // NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta 
   // directamenteno sobre un microcontroladore y no es llamado/ por ningun
   // Sistema Operativo, como en el caso de un programa para PC.
   return 0;
}

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/
