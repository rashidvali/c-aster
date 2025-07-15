/*
 * C* (C aster) - Memory-safe micro library for C
 * Source: safe_log.c
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

 // safe_log.c

#ifdef __has_include
#  if __has_include("c_ast_config.h")
#    include "c_ast_config.h"
#  endif
#endif

// Lightweight logging abstraction for C*

#include "safe_log.h"

#include <stdio.h>
#include <stdarg.h>

#if C_ASTR_CONFIG_FREERTOS_USE
#include "esp_log.h"
#endif

void SAFE_LOGI(const char* tag, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
#if C_ASTR_CONFIG_FREERTOS_USE
    // Wrap the format string to append a newline
    char fmt_with_newline[256];
    snprintf(fmt_with_newline, sizeof(fmt_with_newline), "%s\n", fmt);
    esp_log_level_set(tag, ESP_LOG_INFO);  // optional
    esp_log_writev(ESP_LOG_INFO, tag, fmt_with_newline, args);
#else
    fprintf(stdout, "[INFO][%s] ", tag);
    vfprintf(stdout, fmt, args);
    fprintf(stdout, "\n");
#endif
    va_end(args);
}

void SAFE_LOGE(const char* tag, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
#if C_ASTR_CONFIG_FREERTOS_USE
    // Wrap the format string to append a newline
    char fmt_with_newline[256];
    snprintf(fmt_with_newline, sizeof(fmt_with_newline), "%s\n", fmt);
    esp_log_level_set(tag, ESP_LOG_ERROR);  // optional
    esp_log_writev(ESP_LOG_ERROR, tag, fmt_with_newline, args);
#else
    fprintf(stderr, "[ERROR][%s] ", tag);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
#endif
    va_end(args);
}
