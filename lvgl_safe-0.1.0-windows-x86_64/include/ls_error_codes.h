/**
 * @file ls_error_codes.h
 *
 */

#ifndef LS_ERROR_CODES_H
#define LS_ERROR_CODES_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "stdio.h"

/*********************
 *      DEFINES
 *********************/

#define LS_RETURN_ERROR(error_code) \
			do { \
	          if(error_code) printf("ERROR %d: %s: %d\n", error_code,  __FILE__, __LINE__); \
			  return error_code; \
            } while(0)

#define LS_RETURN_OK  return LS_ERROR_CODE_OK


/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
	LS_ERROR_CODE_OK = 0,
	LS_ERROR_CODE_INTERNAL_ERROR = 1,
	LS_ERROR_CODE_INVALID_ARGUMENT = 2,
	LS_ERROR_CODE_INVALID_CONFIGURATION = 3,
	LS_ERROR_CODE_MEMORY_CORRUPTION = 4,
	LS_ERROR_CODE_RENDER_FAILED = 5,
}ls_error_code_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LS_ERROR_CODES_H*/
