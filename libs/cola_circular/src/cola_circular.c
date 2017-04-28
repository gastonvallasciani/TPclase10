/*============================================================================
 * Licencia: 
 * Autor: 
 * Fecha: 
 *===========================================================================*/


/*==================[inclusiones]============================================*/

#include "stdint.h"
#include "sapi.h"
#include "cola_circular.h"

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/
void configCola(cola_t *cola, int8_t *buffer, uint32_t tam, void (*cbColaVacia)( void ), void (*cbColaLlena)( void ))
{
  cola->elemento=buffer;
  cola->primero=0;
  cola->ultimo=0;
  cola->tamano=tam;
  cola->cbColaVacia=cbColaVacia;
  cola->cbColaLlena=cbColaLlena;
}

bool_t ponerEnCola(cola_t *cola, int8_t dato)
{
   uint32_t p=cola->primero;
   uint32_t u=cola->ultimo;
   uint32_t sz=cola->tamano;
   
   if ((p+1)%sz==u)
   {
      cola->cbColaLlena();
      return 0;
   }
   else
   {
     cola->elemento[cola->primero++]=dato;
     cola->primero%=sz;
     return 1;
   }
}
bool_t sacarDeCola(cola_t *cola, int8_t *dato)
{
   uint32_t p=cola->primero;
   uint32_t u=cola->ultimo;
   uint32_t sz=cola->tamano;
   
   if (p==u)
   {
      cola->cbColaVacia();
      return 0;
   }
   else
   {
     *dato=cola->elemento[cola->ultimo++];
     cola->ultimo%=sz;
   }
}
/*==================[funcion principal]======================================*/

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/
