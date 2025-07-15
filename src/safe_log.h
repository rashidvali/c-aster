/*
 * C* (C aster) - Memory-safe micro library for C
 * Header: safe_log.h
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

 // safe_log.h

 // Lightweight logging abstraction for C*

#pragma once

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

void SAFE_LOGI(const char* tag, const char* fmt, ...);
void SAFE_LOGE(const char* tag, const char* fmt, ...);
// You can add more levels if needed: safe_logw, safe_logd, safe_logv

#ifdef __cplusplus
}
#endif
