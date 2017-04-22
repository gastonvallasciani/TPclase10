/*============================================================================
 * Licencia: 
 * Autor: 
 * Fecha: 
 *===========================================================================*/

#ifndef _MEFASCENSOR_H_
#define _MEFASCENSOR_H_

/*==================[inclusiones]============================================*/

/*==================[c++]====================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[tipos de datos declarados por el usuario]===============*/

/*==================[declaraciones de datos externos]========================*/
uint16_t vCantidadPisos;
uint16_t vCantidadSubsuelos;
/*==================[declaraciones de funciones externas]====================*/
void mefAscensorInsertar(uint16_t piso);
void mefAscensorInit( void );
void mefAscensorActualizar ( void );
void mefManejoPuertasInit( void );
void mefManejoPuertasActualizar( void );

/*==================[c++]====================================================*/
#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* _MEFASCENSOR_H_ */