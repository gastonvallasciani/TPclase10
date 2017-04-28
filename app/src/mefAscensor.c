/* Copyright 2017, Eric Pernia.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
/*
 * Date: 2017-04-14 
 */
/*==================[inlcusiones]============================================*/

#include "mefAscensor.h"   // <= su propio archivo de cabecera
#include "mefIngresoPiso.h"
#include "mefPuertas.h"
#include "mefModoConfiguracion.h"
#include "driverDisplay.h"
#include "driverTeclado.h"
#include "sapi.h"       // <= Biblioteca sAPI
#include "cola_circular.h" // Biblioteca Cola Circular

/*==================[definiciones y macros]==================================*/

#define INDICADOR_MOVIENDOSE   LEDB // ascensor en movimiento.
#define INDICADOR_DETENIDO     LED3 // ascensor detenido o en planta baja.

#define TIMEOUT_PARADO  5 // Si se encuentra 10 segundos detenido en un piso 
                          // vuelve a planta baja

typedef enum{
   EN_PLANTA_BAJA,
   MODO_CONFIGURACION,
   BAJANDO,
   SUBIENDO,
   PARADO,
   YENDO_A_PLANTA_BAJA
} estadoMefAscensor_t;

/*==================[definiciones de datos internos]=========================*/
static uint32_t tpoEntrePisos=0;
static uint16_t vDisplay[4];

/*==================[definiciones de datos externos]=========================*/


extern uint32_t velocidadEntrePisos; // En segundos, de 1 en adelante
extern uint32_t cantidadDePisos;     // De 1 a 20
extern uint32_t cantidadDeSubsuelos; // De 0 a 5
extern cola_t pedidos;
extern eConfiguracion;
extern eFinConfiguracion;

estadoMefAscensor_t estadoMefAscensor;

uint8_t temporizador = 0;
delay_t base1seg;

int8_t pisoActual = 0;
int8_t pisoDestino = 0;

/*==================[declaraciones de funciones internas]====================*/

// Funcion de test: Devuelve TRUE si llego al piso correspondiente
static bool_t llegoAlPiso( int8_t piso );

// Moverse hacia abajo
static bool_t bajarUnPiso( void );

// Moverse hacia arriba
static bool_t subirUnPiso( void );

// Chequea si hay que entrar en modo configuraci'on
static bool_t modoConfiguracion( void );

// Chequea si hay que subir
static bool_t hayPeticionDeSubirPendiente( int8_t pisoActual );

// Chequea si hay que bajar
static bool_t hayPeticionDeBajarPendiente( int8_t pisoActual );

/*==================[definiciones de funciones internas]=====================*/

// Funcion de test: Devuelve TRUE si llego al piso correspondiente
static bool_t llegoAlPiso( int8_t piso ){   
   return ( pisoActual == piso );
}

// Moverse hacia abajo
static bool_t bajarUnPiso( void ){
   if (delayRead(&base1seg))
      tpoEntrePisos--;
   if(tpoEntrePisos==0)
   {
     pisoActual--;
     return TRUE;
   }
   return FALSE;
}

// Moverse hacia arriba
static bool_t subirUnPiso( void ){
   if (delayRead(&base1seg))
      tpoEntrePisos--;
   if(tpoEntrePisos==0)
   {
     pisoActual++;
     return TRUE;
   }
   return FALSE;
}

// Chequea si hay que entrar en modo configuraci'on
static bool_t modoConfiguracion( void )
{
     if(pisoDestino=99)
     {
       pisoDestino=0;
       return TRUE;
     }
     return FALSE;
}

// Chequea si hay que subir
static bool_t hayPeticionDeSubirPendiente( int8_t pisoActual )
{
     if(pisoDestino>pisoActual)
       return TRUE;
     return FALSE;
}

// Chequea si hay que bajar
static bool_t hayPeticionDeBajarPendiente( int8_t pisoActual )
{
     if(pisoDestino<pisoActual)
       return TRUE;
     return FALSE;
}

/*==================[definiciones de funciones externas]=====================*/

uint8_t mensaje[10];
/**
 * C++ version 0.4 char* style "itoa":
 * Written by Luk�s Chmela
 * Released under GPLv3.

 */
char* itoa(int value, char* result, int base) {
   // check that the base if valid
   if (base < 2 || base > 36) { *result = '\0'; return result; }

   char* ptr = result, *ptr1 = result, tmp_char;
   int tmp_value;

   do {
      tmp_value = value;
      value /= base;
      *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
   } while ( value );

   // Apply negative sign
   if (tmp_value < 0) *ptr++ = '-';
   *ptr-- = '\0';
   while(ptr1 < ptr) {
      tmp_char = *ptr;
      *ptr--= *ptr1;
      *ptr1++ = tmp_char;
   }
   return result;
}

/************************************************
Agrega el piso en los 2 d'igitos menos significa-
tivos de vDisplay
*************************************************/
void mostrarPiso( void )
{
        if (pisoActual<0)
        {
         if(pisoActual<-cantidadDeSubsuelos)
         {
           uartWriteString(UART_USB,"valor invalido del subsuelo corriente\n\n");
           ascensorInicializarMEF();
         }
         vDisplay[3]=(-pisoActual);
         vDisplay[2]=MENOS;
        }
        else if (pisoActual==0)
        {
          vDisplay[3]=CHAR_b;
          vDisplay[2]=CHAR_P;
        }
        else
        {
         if(pisoActual>cantidadDePisos)
         {
           uartWriteString(UART_USB,"valor invalido del piso corriente\n\n");
           ascensorInicializarMEF();
         }
         vDisplay[3]=pisoActual%10;
         if (!(vDisplay[2]=pisoActual/10))
           vDisplay[2]=NULO;
        }
        driverDisplayMostrar(vDisplay);
} 

void mostrarEstado( void ){
   
   switch( estadoMefAscensor ){
      
      case EN_PLANTA_BAJA:
         uartWriteString( UART_USB, "EN_PLANTA_BAJA     ");
      break;
      
      case MODO_CONFIGURACION:
         uartWriteString( UART_USB, "MODO_CONFIGURACION " );
      break;
      
      case BAJANDO:
         uartWriteString( UART_USB, "BAJANDO            " );
      break;
      
      case SUBIENDO:
         uartWriteString( UART_USB, "SUBIENDO           " );
      break;
      
      case PARADO:
         uartWriteString( UART_USB, "PARADO             " );
      break;
      
      case YENDO_A_PLANTA_BAJA:
         uartWriteString( UART_USB, "YENDO_A_PLANTA_BAJA" );
      break;
      
      default:
         uartWriteString( UART_USB, "Estado invalido    " );
      break;
   }

   itoa( pisoActual, mensaje, 10);
   uartWriteString( UART_USB, " | piso actual: " );
   uartWriteString( UART_USB, mensaje );

   itoa( pisoDestino, mensaje, 10);
   uartWriteString( UART_USB, " | piso destino: " );
   uartWriteString( UART_USB, mensaje );
   uartWriteString( UART_USB, "\r\n" ); 
} 


// Inicializar la MEF de ascensor
void ascensorInicializarMEF( void ){
   estadoMefAscensor = EN_PLANTA_BAJA;
}

// Actualizar la MEF de ascensor
void ascensorActualizarMEF( void ){
   
   static bool_t flagEnPlantaBaja = FALSE;
   static bool_t flagModoConfiguracion = FALSE;
   static bool_t flagBajando = FALSE;
   static bool_t flagSubiendo = FALSE;
   static bool_t flagParado = FALSE;
   static bool_t flagYendoAPlantaBaja = FALSE;

   switch( estadoMefAscensor ){
      
      case EN_PLANTA_BAJA:
         // Al ingresar al estado se ejecuta 1 vez:
         if( flagEnPlantaBaja == FALSE ){
            flagEnPlantaBaja = TRUE;  
            mostrarPiso();
            mostrarEstado();
            secuenciaDeAperturaDePuertas();
         }
         // Salida en el estado:
            // No hace nada.
         // Condicion/es de transicion de estado:
         if (sacarDeCola(&pedidos,&pisoDestino))
         {
           if( modoConfiguracion() ){
              // Se ingresa al modo configuracion
              estadoMefAscensor = MODO_CONFIGURACION;
           }
           if( hayPeticionDeSubirPendiente( pisoActual ) ){ 
              // Existe peticion pendiente de subir
              estadoMefAscensor = SUBIENDO;
              flagEnPlantaBaja = FALSE;
           }
           if( hayPeticionDeBajarPendiente( pisoActual ) ){
              // Existe peticion pendiente de bajar
              estadoMefAscensor = BAJANDO;
              flagEnPlantaBaja = FALSE;
           }
         }
         // Ejecutar si hubo cambio de estado
         if( estadoMefAscensor != EN_PLANTA_BAJA ){            
            secuenciaDeCerradoDePuertas();
            flagEnPlantaBaja = FALSE;
         }
      break;
      
      case MODO_CONFIGURACION:
         // Al ingresar al estado se ejecuta 1 vez:
         if( flagModoConfiguracion == FALSE ){
            flagModoConfiguracion = TRUE;  
            mostrarEstado();
            secuenciaDeConfiguracion();
         }
         // Salida en el estado:
            // No hace nada.
         // Condicion/es de transicion de estado:
         if( seCompletoLaConfiguracion() ){ 
            // Se completo la configuracion
            estadoMefAscensor = EN_PLANTA_BAJA;
         }
         // Ejecutar si hubo cambio de estado
         if( estadoMefAscensor != MODO_CONFIGURACION ){
            flagModoConfiguracion = FALSE;
         }
      break;
      
      case BAJANDO:
         // Al ingresar al estado se ejecuta 1 vez:
         if( flagBajando == FALSE ){
            flagBajando = TRUE;
            vDisplay[0]=CHAR_b;
            mostrarEstado();
            mostrarPiso();
            tpoEntrePisos=velocidadEntrePisos;
         }
         // Salida en el estado:
         if(bajarUnPiso())
         {
            mostrarPiso();
         }; // Moverse hacia abajo.
         // Condicion/es de transicion de estado:
         if( llegoAlPiso( pisoDestino ) ){ 
            // Llego al piso
            estadoMefAscensor = PARADO;
         }
         // Ejecutar si hubo cambio de estado
         if( estadoMefAscensor != BAJANDO ){
            flagBajando = FALSE;
         }
      break;
      
      case SUBIENDO:
         // Al ingresar al estado se ejecuta 1 vez:
         if( flagSubiendo == FALSE ){
            flagSubiendo = TRUE;
            vDisplay[0]=CHAR_S;
            mostrarEstado();
            mostrarPiso();
            tpoEntrePisos=velocidadEntrePisos;
         }
         // Salida en el estado:
         if (subirPiso())
           mostrarPiso(); // Moverse hacia arriba.
         // Condicion/es de transicion de estado:
         if( llegoAlPiso( pisoDestino ) ){ 
            // Llego al piso
            estadoMefAscensor = PARADO;
         }
         // Ejecutar si hubo cambio de estado
         if( estadoMefAscensor != SUBIENDO ){
            flagSubiendo = FALSE;
         }
      break;
      
      case PARADO:
         // Al ingresar al estado se ejecuta 1 vez:
         if( flagParado == FALSE ){
            flagParado = TRUE;
            mostrarPiso();
            mostrarEstado();
            secuenciaDeAperturaDePuertas();
            temporizador = 0;
            delayConfig( &base1seg, 1000 );
         }
         // Salida en el estado:
         if( delayRead( &base1seg ) ){           
            temporizador++;
         }
         if (puertasCerradas())
         {        
           if(sacarDeCola(&pedidos,&pisoDestino))
           {
             // Condicion/es de transicion de estado:
             if( hayPeticionDeSubirPendiente( pisoActual ) ){ 
                // Existe peticion pendiente de subir
           
                estadoMefAscensor = SUBIENDO;
                break;
             }
             if( hayPeticionDeBajarPendiente( pisoActual ) ){
                // Existe peticion pendiente de bajar
                estadoMefAscensor = BAJANDO;
                break;
             }
           }
           if( temporizador >= TIMEOUT_PARADO ){
              // Se va a planta baja
              uartWriteString( UART_USB, "Se cumplio el time-out de PARADO\r\n" ); 
              estadoMefAscensor = YENDO_A_PLANTA_BAJA;
              break;
           }
         }
         // Ejecutar si hubo cambio de estado
         if (estadoMefAscensor!=PARADO)
           flagParado = FALSE;
      break;
      
      case YENDO_A_PLANTA_BAJA:
         // Al ingresar al estado se ejecuta 1 vez:
         if( flagYendoAPlantaBaja == FALSE ){
            flagYendoAPlantaBaja = TRUE;
            mostrarEstado();
            pisoDestino = 0;
            cancelarPeticionPendiente(0);
         }
         // Salida en el estado:
         mostrarPiso();
         if( pisoActual > 0 ){
            bajarUnPiso(); // Moverse hacia abajo.
         }
         if( pisoActual < 0 ){
            subirUnPiso(); // Moverse hacia arriba.
         }
         // Condicion/es de transicion de estado:
         if( llegoAlPiso( 0 ) ){ 
            // Llego al piso
            estadoMefAscensor = EN_PLANTA_BAJA;
         }
         // Ejecutar si hubo cambio de estado
         if( estadoMefAscensor != YENDO_A_PLANTA_BAJA ){
            flagYendoAPlantaBaja = FALSE;
         }
      break;
      
      default:
         ascensorInicializarMEF();
      break;
   }
}

/*==================[fin del archivo]========================================*/
