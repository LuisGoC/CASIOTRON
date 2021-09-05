#ifndef QUEUE_H_
#define QUEUE_H_

#include "app_bsp.h"

typedef struct{
    void *Buffer;
    uint32_t Elements;
    uint8_t Size;     //tamaño del tipo de elementos   
    uint32_t Head;
    uint32_t Tail;
    uint8_t	Empty;
    uint8_t	Full;
    //agregar más elementos si se requieren
}QUEUE_HandleTypeDef;


void HIL_QUEUE_Init( QUEUE_HandleTypeDef *hqueue );
uint8_t HIL_QUEUE_Write( QUEUE_HandleTypeDef *hqueue, void *data );	
uint8_t HIL_QUEUE_Read( QUEUE_HandleTypeDef *hqueue, void *data );
uint8_t HIL_QUEUE_IsEmpty( QUEUE_HandleTypeDef *hqueue );

#endif