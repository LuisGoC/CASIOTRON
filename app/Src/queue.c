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

    if (hqueue->Full == (uint8_t)NOT_OK) 
    { 
        /*cppcheck supression because the queue "buffer" type is void 
          and needs to be incremented by "size" queue's variable  */
        (void)memcpy(hqueue->Buffer + hqueue->Head, data, hqueue->Size);  /* cppcheck-suppress misra-c2012-18.4*/
        hqueue->Head = hqueue->Head + hqueue->Size;
        if (hqueue->Head == (hqueue->Size * hqueue->Elements)) 
        {
            hqueue->Head = 0;
        }
        if (hqueue->Tail == hqueue->Head)
        {
            hqueue->Full = OK;
        }
        if (hqueue->Empty == (uint8_t)OK)
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
    uint8_t err;
    if (hqueue->Empty == (uint8_t)NOT_OK) 
    {
        /*cppcheck supression because the queue "buffer" type is void 
          and needs to be incremented by "size" queue's variable  */
        (void)memcpy(data, hqueue->Buffer + hqueue->Tail, hqueue->Size); /* cppcheck-suppress misra-c2012-18.4*/
        hqueue->Tail = hqueue->Tail + hqueue->Size;
        if (hqueue->Tail == (hqueue->Size * hqueue->Elements)) 
        {
            hqueue->Tail = 0UL;
        }
        if(hqueue->Full == (uint8_t)OK)
        {
            hqueue->Full = NOT_OK;
        }
        if (hqueue->Tail == hqueue->Head)
        {
            hqueue->Empty = OK;
        }
        err = OK;
    }
    else
    {
        err = NOT_OK;
    }
    return err;
}

uint8_t HIL_QUEUE_IsEmpty(QUEUE_HandleTypeDef *hqueue)
{
    uint8_t val;

    if (hqueue->Empty == (uint8_t)OK)
    {
        val = OK;
    }
    else
    {
        val = NOT_OK;
    }

    return val;
}