// safemem_full_test.c
// License: LGPL-3.0
// Author: Rashid S. Vali
// Full memory test suite for C* (C aster)

//  safemem_simple_test.c

#include <stdio.h>
#include <string.h>
#include "safemem_embedded.h"
#include "safe_log.h"

#define ITERATIONS 500
#define STRING_LEN 64

void test_memory_cycle() {
    safemem_init();

    for (int i = 0; i < 100; i++) {
        char* text = (char*)safe_malloc(32);
        if (text == NULL) {
            SAFE_LOGE("ERROR", "Allocation failed at iteration #%d", i);
            continue;
        }

        snprintf(text, 32, "Message number %d", i);
        SAFE_LOGI("TEST", "Allocated string: %s", text);

        char* copy = safe_strdup(text);
        SAFE_LOGI("TEST", "Duplicated string: %s", copy);

        SAFE_LOGI("TEST", "Iteration: #%d", i);

        safe_free(text, strlen(text) + 1);
        safe_free(copy, strlen(copy) + 1);
    }

    SAFE_LOGI("TEST", "Memory test completed.");
}

void simulate_workload() {
    safemem_init();

    for (int i = 0; i < ITERATIONS; i++) {
        // Allocate int
        int* num = (int*)safe_malloc(sizeof(int));
        if (num == NULL) {
            SAFE_LOGE("TEST", "Failed to allocate int at %d", i);
            continue;
        }
        *num = i;

        // Allocate and copy string
        char buffer[STRING_LEN];
        snprintf(buffer, STRING_LEN, "Iteration: %d, Value: %d", i, *num);

        char* copied = safe_strdup(buffer);
        if (copied == NULL) {
            SAFE_LOGE("TEST", "Failed to duplicate string at %d", i);
            safe_free(num, sizeof(int));
            continue;
        }

        // Simulate math
        int square = (*num) * (*num);
        if (square % 10 == 0) {
            SAFE_LOGI("TEST", "[%d] %s | Square: %d", i, copied, square);
        }

        // Free memory
        safe_free(copied, strlen(copied) + 1);
        safe_free(num, sizeof(int));
    }

    SAFE_LOGI("TEST", "All memory cycles completed.");
}