#include <stdio.h>
#include <stdint.h>

#include "safemem_embedded.h"
#include "safemem_tests.h"

void safemem_run_tests(void)
{
    printf("\n=== C* safemem tests ===\n");

    safemem_init();

    uint8_t *a = safe_malloc(10);
    uint8_t *b = safe_malloc(10);

    printf("a = %p\n", (void *)a);
    printf("b = %p\n", (void *)b);

    uint8_t data[15] = {0};

    int valid_result = set_mem_block(a, data, 10);
    printf("10-byte write into 10-byte allocation: %s\n",
           valid_result ? "PASS" : "FAIL");

    int overflow_result = set_mem_block(a, data, 15);
    printf("15-byte write crossing allocation boundary: %s\n",
           overflow_result ? "FAIL" : "PASS");

    safe_free(a);
    safe_free(b);
}