/*
 * C* (C aster) - Memory-safe micro library for C
 * Header: c_ast_config.h
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

// c_ast_config.h

#pragma once

// 0 = Windows/Linux environment without FreeRTOS mutex support
// 1 = Embedded environment with FreeRTOS mutex support
#define C_ASTR_CONFIG_FREERTOS_USE 0 
// #define C_ASTR_CONFIG_FREERTOS_USE 1 

#define C_ASTR_CONFIG_ARENA_SIZE 2048
#define C_ASTR_CONFIG_MAX_BLOCKS 64
