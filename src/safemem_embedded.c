/*
 * C* (C aster) - Memory-safe micro library for C
 * Source: safemem_embedded.c
 *
 * Author: Rashid S. Vali
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * This file is part of the C* library.
 *
 * C* is free software: you can redistribute it and/or modify it under the terms of the
 * GNU Lesser General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * C* is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */


  // safemem_embedded.c

#ifdef __has_include
#  if __has_include("c_ast_config.h")
#    include "c_ast_config.h"
#  endif
#endif

#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include "safemem_embedded.h"
#include "safe_log.h"       // ?? Must come before using SAFE_LOGE

#if C_ASTR_CONFIG_FREERTOS_USE
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
static SemaphoreHandle_t safemem_mutex;
#endif

#ifndef C_ASTR_CONFIG_ARENA_SIZE
#define C_ASTR_CONFIG_ARENA_SIZE 1024
#endif

#ifndef C_ASTR_CONFIG_MAX_BLOCKS
#define C_ASTR_CONFIG_MAX_BLOCKS 32
#endif



static uint8_t arena[C_ASTR_CONFIG_ARENA_SIZE];
static FreeBlock block_pool[C_ASTR_CONFIG_MAX_BLOCKS];
static FreeBlock* node_pool = NULL;
static FreeBlock* allocated_list = NULL;

static const char *TAG_MEM = "MEM";

// === Locking for RTOS ===
void safemem_lock() {
#if C_ASTR_CONFIG_FREERTOS_USE
    if (safemem_mutex) xSemaphoreTake(safemem_mutex, portMAX_DELAY);
#endif
}

void safemem_unlock() {
#if C_ASTR_CONFIG_FREERTOS_USE
    if (safemem_mutex) xSemaphoreGive(safemem_mutex);
#endif
}

// === Node Pool ===
static FreeBlock* alloc_node() {
    if (!node_pool) return NULL;
    FreeBlock* node = node_pool;
    node_pool = node_pool->next;
    return node;
}

static void free_node(FreeBlock* node) {
    node->next = node_pool;
    node_pool = node;
}

// === Init ===
void safemem_init() {
    // SAFE_LOGI("CONFIG", "C_ASTR_CONFIG_FREERTOS_USE = %d", C_ASTR_CONFIG_FREERTOS_USE);
#if C_ASTR_CONFIG_FREERTOS_USE
    // SAFE_LOGI("CONFIG", "1: C_ASTR_CONFIG_FREERTOS_USE = %d", C_ASTR_CONFIG_FREERTOS_USE);
    safemem_mutex = xSemaphoreCreateMutex();
    SAFE_LOGI(TAG_MEM, "Semaphore created for safemem\n");
#else
    // SAFE_LOGI("CONFIG", "0: C_ASTR_CONFIG_FREERTOS_USE = %d", C_ASTR_CONFIG_FREERTOS_USE);
    SAFE_LOGI(TAG_MEM, "No mutex support, using simple memory management\n");
#endif

    for (int i = 0; i < C_ASTR_CONFIG_MAX_BLOCKS - 1; ++i)
        block_pool[i].next = &block_pool[i + 1];

    block_pool[C_ASTR_CONFIG_MAX_BLOCKS - 1].next = NULL;

    node_pool = &block_pool[0];
	allocated_list = NULL;
}

// === Allocation ===
void* safe_malloc(size_t size) {
    if (size == 0 || size > C_ASTR_CONFIG_ARENA_SIZE)
        return NULL;

    safemem_lock();

    uint8_t* candidate = arena;
    FreeBlock* prev = NULL;
    FreeBlock* curr = allocated_list;

    /*
     * allocated_list is kept sorted by address.
     * Find the first gap large enough for this allocation.
     */
    while (curr) {
        uint8_t* curr_addr = (uint8_t*)curr->addr;

        if ((size_t)(curr_addr - candidate) >= size)
            break;

        candidate = curr_addr + curr->size;
        prev = curr;
        curr = curr->next;
    }

    /*
     * If no suitable gap was found between allocations,
     * candidate points immediately after the last allocation.
     */
    uint8_t* arena_end = arena + C_ASTR_CONFIG_ARENA_SIZE;

    if ((size_t)(arena_end - candidate) < size) {
        SAFE_LOGE(TAG_MEM,
                  "Allocation of %zu bytes failed: insufficient memory\n",
                  size);
        safemem_unlock();
        return NULL;
    }

    FreeBlock* block = alloc_node();

    if (!block) {
        SAFE_LOGE(TAG_MEM,
                  "Allocation of %zu bytes failed: no metadata node available\n",
                  size);
        safemem_unlock();
        return NULL;
    }

    block->addr = candidate;
    block->size = size;

    /*
     * Insert into allocated_list at the gap we found,
     * preserving address order.
     */
    block->next = curr;

    if (prev)
        prev->next = block;
    else
        allocated_list = block;

    safemem_unlock();
    return candidate;
}

// === Free ===
void safe_free(void* ptr) {
    if (!ptr)
        return;

    safemem_lock();

    FreeBlock* prev = NULL;
    FreeBlock* curr = allocated_list;

    /*
     * safe_free() only accepts the exact start address
     * of a currently registered allocation.
     */
    while (curr && curr->addr != ptr) {
        prev = curr;
        curr = curr->next;
    }

    if (!curr) {
        SAFE_LOGE(TAG_MEM,
                  "Free failed: pointer is not a valid allocation\n");
        safemem_unlock();
        return;
    }

    /*
     * Remove the allocation from the sorted allocation list.
     */
    if (prev)
        prev->next = curr->next;
    else
        allocated_list = curr->next;

    /*
     * Return its metadata node to the node pool.
     * The memory itself automatically becomes available
     * because it is now a gap between allocations.
     */
    free_node(curr);

    safemem_unlock();
}

// === String operations ===
char* safe_strdup(const char* src) {
    if (!src) return NULL;

    size_t len = strlen(src) + 1;  // Include null terminator
    char* copy = (char*)safe_malloc(len);
    if (copy) {
        strcpy(copy, src);
    }
    else {
        SAFE_LOGE(TAG_MEM, "safe_strdup", "Failed to allocate %zu bytes", len);
    }

    return copy;
}

// === Accessors ===
#define VALIDATE(ptr, size) \
    (((uintptr_t)(ptr) >= (uintptr_t)arena) && \
     ((uintptr_t)(ptr) + (size) <= (uintptr_t)(arena + C_ASTR_CONFIG_ARENA_SIZE)))

int set_mem_int(int* p, int v) {
    if (VALIDATE(p, sizeof(int))) { *p = v; return 1; }
    SAFE_LOGE(TAG_MEM, "Invalid int write at %p\n", (void*)p); return 0;
}

int get_mem_int(int* p, int* out) {
    if (VALIDATE(p, sizeof(int))) { *out = *p; return 1; }
    SAFE_LOGE(TAG_MEM, "Invalid int read at %p\n", (void*)p); return 0;
}

int set_mem_char(char* p, char v) {
    if (VALIDATE(p, sizeof(char))) { *p = v; return 1; }
    SAFE_LOGE(TAG_MEM, "Invalid char write at %p\n", (void*)p); return 0;
}

int get_mem_char(char* p, char* out) {
    if (VALIDATE(p, sizeof(char))) { *out = *p; return 1; }
    SAFE_LOGE(TAG_MEM, "Invalid char read at %p\n", (void*)p); return 0;
}

int set_mem_float(float* p, float v) {
    if (VALIDATE(p, sizeof(float))) { *p = v; return 1; }
    SAFE_LOGE(TAG_MEM, "Invalid float write at %p\n", (void*)p); return 0;
}

int get_mem_float(float* p, float* out) {
    if (VALIDATE(p, sizeof(float))) { *out = *p; return 1; }
    SAFE_LOGE(TAG_MEM, "Invalid float read at %p\n", (void*)p); return 0;
}

int set_mem_block(void* p, const void* data, size_t len) {
    if (VALIDATE(p, len)) {
        for (size_t i = 0; i < len; ++i)
            ((uint8_t*)p)[i] = ((const uint8_t*)data)[i];
        return 1;
    }
    SAFE_LOGE(TAG_MEM, "Invalid block write at %p\n", p); return 0;
}

int get_mem_block(void* p, void* out, size_t len) {
    if (VALIDATE(p, len)) {
        for (size_t i = 0; i < len; ++i)
            ((uint8_t*)out)[i] = ((uint8_t*)p)[i];
        return 1;
    }
    SAFE_LOGE(TAG_MEM, "Invalid block read at %p\n", p); return 0;
}

int safe_memset(void* p, int val, size_t len) {
    if (VALIDATE(p, len)) {
        for (size_t i = 0; i < len; ++i)
            ((uint8_t*)p)[i] = (uint8_t)val;
        return 1;
    }
    SAFE_LOGE(TAG_MEM, "Invalid memset at %p\n", p); return 0;
}

void safemem_report() {
    safemem_lock();
    SAFE_LOGI(TAG_MEM, "=== Free Blocks ===\n");
    FreeBlock* curr = free_list;
    while (curr) {
        SAFE_LOGI(TAG_MEM, "  Addr: %p, Size: %zu\n", curr->addr, curr->size);
        curr = curr->next;
    }
    SAFE_LOGI(TAG_MEM, "===================\n");
    safemem_unlock();
}