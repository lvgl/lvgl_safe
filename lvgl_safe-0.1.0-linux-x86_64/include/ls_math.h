/**
 * @file ls_math.h
 *
 */

#ifndef LS_MATH_H
#define LS_MATH_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>

/*********************
 *      DEFINES
 *********************/
#define LS_TRIGO_SIN_MAX 32768
#define LS_TRIGO_SHIFT 15 /**<  >> LS_TRIGO_SHIFT to normalize*/

/**********************
 *      TYPEDEFS
 **********************/


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Return with sinus of an angle
 * @param angle
 * @return sinus of 'angle'. sin(-90) = -32767, sin(90) = 32767
 */
int32_t ls_sin(int32_t angle);

int32_t ls_cos(int32_t angle);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
