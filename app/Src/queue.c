#include "app_bsp.h"
#include "queue.h"

void HIL_QUEUE_Init(QUEUE_HandleTypeDef *hqueue)
{
    hqueue->Head = 0;
    hqueue->Tail = 0;
    hqueue->Empty = OK;
    hqueue->Full = NOT_OK;
}

uint8_t HIL_QUEUE_Write(QUEUE_HandleTypeDef *hqueue, void *data)
{
    uint8_t err; 

    if (hqueue->Full == NOT_OK) 
    { 
        memcpy(hqueue->Buffer + hqueue->Head, data, hqueue->Size);  
        hqueue->Head = hqueue->Head + hqueue->Size;
        if (hqueue->Head == (hqueue->Size * hqueue->Elements)) 
        {
            hqueue->Head = 0;
        }
        if (hqueue->Tail == hqueue->Head)
        {
            hqueue->Full = OK;
        }
        if (hqueue->Empty == OK)
        {
            hqueue->Empty = NOT_OK;
        }
        err = OK;
    }
    else
    {
        err = NOT_OK;
    }

    return err;
}

uint8_t HIL_QUEUE_Read(QUEUE_HandleTypeDef *hqueue, void *data)
{
    if (hqueue->Empty == NOT_OK) 
    {
        memcpy(data, hqueue->Buffer + hqueue->Tail, hqueue->Size);
        hqueue->Tail = hqueue->Tail + hqueue->Size;
        if (hqueue->Tail == (hqueue->Size * hqueue->Elements)) 
        {
            hqueue->Tail = 0;
        }
        if(hqueue->Full == OK)
        {
            hqueue->Full = NOT_OK;
        }
        if (hqueue->Tail == hqueue->Head)
        {
            hqueue->Empty = OK;
        }
        return OK;
    }
    else
    {
        return NOT_OK;
    }
}

uint8_t HIL_QUEUE_IsEmpty(QUEUE_HandleTypeDef *hqueue)
{
    if (hqueue->Empty == (uint8_t)OK)
    {
        return OK;
    }
    return NOT_OK;
}