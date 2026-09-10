#include "c_aster_api.h"
#include "safemem_embedded.h"

int *alnInt(int value)
{
    int *ptr = (int *)c_ast_allocate(sizeof(int), _Alignof(int));

    if (ptr == NULL)
        return NULL;

    *ptr = value;

    return ptr;
}

float *alnFloat(float value)
{
    float *ptr = (float *)c_ast_allocate(sizeof(float), _Alignof(float));

    if (ptr == NULL)
        return NULL;

    *ptr = value;

    return ptr;
}

