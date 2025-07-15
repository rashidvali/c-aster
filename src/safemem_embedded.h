/*
 * C* (C aster) - Memory-safe micro library for C
 * Header: safemem_embedded.h
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

 // safemem_embedded.h

// This header file defines the API for a simple memory safe management system`
// that can be used in embedded systems. It provides functions for
// memory allocation, deallocation, and safe access to memory locations.
// The implementation is designed to be lightweight and suitable for
// environments with limited resources.


#pragma once

#ifndef SAFEMEM_EMBEDDED_H
#define SAFEMEM_EMBEDDED_H

#include <stddef.h>
#include <stdint.h>

// === TYPES ===
typedef struct FreeBlock {
    void* addr;
    size_t size;
    struct FreeBlock* next;
} FreeBlock;

// === API ===
void safemem_init();
void safemem_lock();
void safemem_unlock();
void* safe_malloc(size_t size);
void safe_free(void* ptr, size_t size);
char* safe_strdup(const char* src);

// === Safe Accessors ===
int set_mem_int(int* p, int v);
int get_mem_int(int* p, int* out);
int set_mem_char(char* p, char v);
int get_mem_char(char* p, char* out);
int set_mem_float(float* p, float v);
int get_mem_float(float* p, float* out);
int set_mem_block(void* p, const void* data, size_t len);
int get_mem_block(void* p, void* out, size_t len);
int safe_memset(void* p, int val, size_t len);

// === Debug/Monitoring ===
void safemem_report();

#endif // SAFEMEM_EMBEDDED_H