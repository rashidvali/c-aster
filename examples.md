# C* Usage Examples

These examples demonstrate how to use C* (C aster) safe memory functions in place of native, unsafe operations.

---

## 1. Safe Allocation and Deallocation

```c
#include "safemem_embedded.h"

void example_allocation() {
    safemem_init();

    int* numbers = (int*) safe_malloc(5 * sizeof(int));
    if (numbers) {
        for (int i = 0; i < 5; i++) {
            set_mem_int(&numbers[i], i * 10);
        }

        for (int i = 0; i < 5; i++) {
            int value = get_mem_int(&numbers[i]);
            SAFE_LOGI("Value %d: %d", i, value);
        }

        safe_free(numbers);
    }
}
```

---

## 2. Safe String Duplication and Use

```c
#include "safemem_embedded.h"

void example_strdup() {
    safemem_init();

    const char* original = "hello world";
    char* copy = safe_strdup(original);

    if (copy) {
        SAFE_LOGI("Copied string: %s", copy);
        safe_free(copy);
    }
}
```

---

## 3. Buffer Copy with Bounds Checking

```c
void example_block_copy() {
    safemem_init();

    char* src = (char*) safe_malloc(16);
    char* dst = (char*) safe_malloc(16);

    if (src && dst) {
        strcpy(src, "safe copy");
        set_mem_block(dst, src, strlen(src) + 1);
        SAFE_LOGI("Copied buffer: %s", dst);
    }

    safe_free(src);
    safe_free(dst);
}
```

---

## 4. Memory Map Report

```c
void debug_memory() {
    safemem_report();  // Prints current allocated and freed blocks
}
```

---

## 5. Using C* in ESP32 (ESP-IDF)

```c
#include "safemem_embedded.h"
#include "safe_log.h"

void app_main() {
    safemem_init();

    char* msg = safe_strdup("ESP32 SafeMem Init");
    if (msg) {
        SAFE_LOGI("Message: %s", msg);
        safe_free(msg);
    }
}
```

---

For more usage patterns, see the `tests/` and `examples/` folders (coming soon).

