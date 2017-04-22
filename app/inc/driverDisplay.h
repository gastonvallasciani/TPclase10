/*============================================================================
 * Licencia: 
 * Autor: 
 * Fecha: 
 *===========================================================================*/

#ifndef _DRIVERDISPLAY_H_
#define _DRIVERDISPLAY_H_

/*==================[inclusiones]============================================*/

/*==================[c++]====================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/
#define MENOS	26
#define NULO	25
#define CHAR_S	5
#define CHAR_b	11
#define CHAR_P	22
/*==================[tipos de datos declarados por el usuario]===============*/

/*==================[declaraciones de datos externos]========================*/

/*==================[declaraciones de funciones externas]====================*/
void driverDisplayShf(uint16_t digito,uint16_t vDisplay[4]);
void driverDisplayLimpiar(uint16_t vDisplay[4] );
void driverDisplayMostrar(uint16_t vDisplay[4]);
void driverDisplayInit( void );
void driverDisplayActualizar( void );

/*==================[c++]====================================================*/
#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* _DRIVERDISPLAY_H_ */