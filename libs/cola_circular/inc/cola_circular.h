#ifndef _COLA_CIRCULAR_H_
#define _COLA_CIRCULAR_H_
/*****************************************************************************
Definiciones de tipos
******************************************************************************/
typedef struct
{
int8_t *elemento;
int32_t primero;
int32_t ultimo;
int32_t tamano;
void (*cbColaVacia)( void );
void (*cbColaLlena)( void );
} cola_t;
/*****************************************************************************
Definiciones de funciones externas
******************************************************************************/
void configCola(cola_t *cola, int8_t *buffer, uint32_t tam, void (*cbColaVacia)( void ), void (*cbColaLlena)( void ));
bool_t ponerEnCola(cola_t *cola, int8_t dato);
bool_t sacarDeCola(cola_t *cola, int8_t *dato);

#endif