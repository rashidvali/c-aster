#ifndef C_ASTER_API_H
#define C_ASTER_API_H

int *alnInt(int value);
float *alnFloat(float value);
char *alnChar(char value);
char *alnStr(const char *value);
int *alnIntArr(size_t count);
float *alnFloatArr(size_t count);
char *alnCharArr(size_t count);
void dispose(void* ptr);

#endif