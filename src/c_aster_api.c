#include <string.h>
#include <stdint.h>
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

char *alnChar(char value)
{
    char *ptr = (char *)c_ast_allocate(sizeof(char), _Alignof(char));

    if (ptr == NULL)
        return NULL;

    *ptr = value;

    return ptr;
}

char *alnStr(const char *value)
{
    if (value == NULL)
        return NULL;

    size_t size = strlen(value) + 1;

    char *ptr = (char *)c_ast_allocate(size, _Alignof(char));

    if (ptr == NULL)
        return NULL;

    memcpy(ptr, value, size);

    return ptr;
}

int *alnIntArr(size_t count)
{
	if (count == 0)
    	return NULL;

	if (count > SIZE_MAX / sizeof(int))
    	return NULL;

    int *ptr = (int *)c_ast_allocate(
        count * sizeof(int),
        _Alignof(int)
    );

    if (ptr == NULL)
        return NULL;

    memset(ptr, 0, count * sizeof(int));

    return ptr;
}

float *alnFloatArr(size_t count)
{
    if (count == 0 || count > SIZE_MAX / sizeof(float))
        return NULL;

    size_t size = count * sizeof(float);

    float *ptr = (float *)c_ast_allocate(size, _Alignof(float));

    if (ptr == NULL)
        return NULL;

    memset(ptr, 0, size);

    return ptr;
}
