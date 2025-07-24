> **🚀 Public Release Announcement**  
> This is the initial public release of **C\*** (C aster) — a memory-safe micro-library for C.  
> We welcome your feedback, testing, and contributions as the project grows.  
> See [CONTRIBUTING.md](./contributing.md) to get involved.


# C\* (C aster) ![OlitDB Logo](assets/Aster_Flower_small.png)

**C\*** (pronounced "C aster") is a memory-safe micro library for C, with zero OS or compiler dependencies. Designed for simplicity, modularity, and rapid adoption, it empowers developers to fortify existing projects without full rewrites. It introduces a lightweight memory safety framework, allowing developers to write safer C code without giving up control or performance.

---

## Why C\*?

The C programming language is powerful, but inherently unsafe when it comes to dynamic memory management. **C\*** helps eliminate common memory-related issues such as:

- Buffer overflows
- Use-after-free
- Memory leaks
- Invalid memory access

### Key Motivations

- **Pure C, zero setup**: Drop it into any codebase — no special toolchains or build flags required.
- **Memory safety by design**: Rewritten modules are automatically protected from common C pitfalls.
- **Incremental refactoring**: Convert legacy projects one module at a time. Untouched code remains functional.
- **Interoperable**: Plays well with third-party libraries, legacy components, and multi-module structures.
- **Small footprint**: Less than 300 lines of logic — lean, elegant, and purpose-built.
- **Memory Safety**: Enforces bounds checking and controlled memory access.
- **Embedded Compatibility**: Designed to run on constrained systems like ESP-IDF (ESP32 microcontroller) with or without RTOS.
- **Performance Awareness**: Uses pre-allocated arenas and simple allocators to avoid heap fragmentation.
- **Safe alternatives to memory related functions**: Developers are expected to avoid unsafe native functions and use C\* safe alternatives instead. A mapping reference between unsafe and safe APIs is provided.
- **Runtime Registry**: C\* maintains a registry of available memory chunks, offering runtime validation and safety. This introduces a minor performance overhead, which is the tradeoff for improved reliability.
- **Conformance with Government Guidance**: Aligned with security recommendations from documents like [ONCD's Technical Report (2024)](https://bidenwhitehouse.archives.gov/wp-content/uploads/2024/02/Final-ONCD-Technical-Report.pdf), promoting safe software development practices.

---

## Strategic Compliance Alignment

The ONCD 2024 report from the Executive Office of the President advises against using C/C++ for new secure systems. However, it also acknowledges that replacing existing infrastructure is often impractical. **C\*** bridges this gap:

> "...some legacy software must continue to be maintained. Tooling, education, and coding standards can reduce harm and risk..."

By integrating C\*, developers can achieve the memory safety goals outlined by agencies like CISA, FBI, and ONCD — without discarding years of proven C infrastructure.

---

## Unsafe to Safe Function Mapping

C\* encourages developers to avoid unsafe native memory functions by providing secure alternatives. Here's a quick reference guide:

| **Unsafe Function**    | **C\* Alternative**               | **Notes**                                                     |
| ---------------------- | --------------------------------- | ------------------------------------------------------------- |
| `malloc(size)`         | `safe_malloc(size)`               | Allocates memory from a managed arena with bounds tracking.   |
| `free(ptr)`            | `safe_free(ptr)`                  | Safely frees memory and updates internal registry.            |
| `strdup(str)`          | `safe_strdup(str)`                | Copies string into registered memory.                         |
| `memcpy(dst, src)`     | `set_mem_block(dst, src, size)`   | Validates buffer boundaries during copy.                      |
| `*(int*)ptr = val`     | `set_mem_int(ptr, val)`           | Checked write to an int-sized memory location.                |
| `int val = *(int*)ptr` | `get_mem_int(ptr)`                | Checked read from a safe-allocated pointer.                   |
| `printf(...)`          | `SAFE_LOGI(...) / SAFE_LOGE(...)` | Logging macros for embedded-safe output (ESP-IDF compatible). |

> ✅ Use these wrappers consistently to maintain memory safety guarantees throughout your project.

---

## Features

- `safe_malloc`, `safe_free`: Custom allocators with internal registry.
- Typed safe setters/getters (e.g., `set_mem_int`, `get_mem_float`, `set_mem_block`, etc.).
- Optional support for FreeRTOS mutex protection.
- Arena-based allocation (no reliance on `malloc`/`free`).
- Safe logging macros: `SAFE_LOGI`, `SAFE_LOGE`.
- Built-in memory map reporting (`safemem_report`).

---

## Getting Started

### 1. Add C\* to Your Project
Add the following files to your project **source list or build configuration**:
- `c_ast_config.h`
- `safemem_embedded.h`
- `safemem_embedded.c`
- `safe_log.h`
- `safe_log.c`

Create and add to your directory ```c_ast_config.h``` file:

```c
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
// #define C_ASTR_CONFIG_FREERTOS_USE 0 
#define C_ASTR_CONFIG_FREERTOS_USE 1 

#define C_ASTR_CONFIG_ARENA_SIZE 2048
#define C_ASTR_CONFIG_MAX_BLOCKS 64
```

The micro-library requires minimal configuration using three self-explanatory macros.

>You can choose between an embedded environment with FreeRTOS or a Windows/Linux OS by setting `C_ASTR_CONFIG_FREERTOS_USE` to 1 or 0, respectively.

> You can change `C_ASTR_CONFIG_ARENA_SIZE` and `C_ASTR_CONFIG_MAX_BLOCKS` to suit your memory model.

### 2. Initialize C\* in your code

Import safemem_embedded.h and safe_log.h files:

```c
#include "safemem_embedded.h"
#include "safe_log.h"
```

Initialize:

```c
safemem_init();
```

### 3. Use Safe Functions

```c
int* p = (int*) safe_malloc(sizeof(int));
if (p) set_mem_int(p, 42);
```

---

## Examples

Include the ```c_ast_config.h``` file (available in the [`examples/`](./examples) folder) in your working directory

For basic usage examples see [examples.md](./examples.md). 

You can find basic tests in the [`examples/`](./examples) folder:

- `safemem_simple_test.c` – runs `test_memory_cycle()` and `simulate_workload()` to validate allocation and reuse logic.

\* The ```c_ast_config.h``` file sets the macro `C_ASTR_CONFIG_MAX_BLOCKS` to 64, which allows `test_memory_cycle()` 64 successful memory allocations. It will fail on the 65th allocation (i.e., when the allocation count reaches 64).

---

## Target Platforms

- **Standard C on Linux/Windows** (via Visual Studio or GCC/Clang)
- **Embedded C on ESP32** (ESP-IDF with/without FreeRTOS)

---

## Licensing

This project is licensed under the **LGPL (Lesser General Public License)**.

- You may use it in commercial or proprietary projects.
- Modifications to the library must be shared if distributed.

---

## Contributing

Contributions are welcome! You can:

- Suggest or implement safer replacements for more standard library functions
- Optimize memory pool behavior
- Add RTOS or hardware-specific integrations

Please open an issue or submit a pull request on GitHub.

### Bug Reporting

If you discover unexpected behavior or a potential memory safety issue, please open a [GitHub issue](https://github.com/rashidvali/c-aster/issues) with details, test case, and reproduction steps.

---

## Author

**C\*** was initiated by **Rashid S. Vali** as a response to the increasing demand for memory safety in low-level software. Contributions from the community are encouraged and appreciated.

---

## Roadmap

This project does not currently maintain a formal roadmap. Planned improvements may be tracked via issues or discussions.

---

## Disclaimer

Developers are expected to avoid unsafe native functions and use C\* safe alternatives instead. A mapping reference between unsafe and safe APIs is provided. C\* maintains a registry of available memory chunks, offering runtime validation and safety. This introduces a minor performance overhead, which is the tradeoff for improved reliability. While C\* greatly improves memory safety, it is not a full memory-safe language replacement. Use it as a building block toward safer systems within the power and constraints of C.
For Windows/Linux multithreaded environments, safe_malloc and safe_free are not thread-safe by default. You can wrap calls with your own mutex or fork the library to add POSIX/Win32 locking if needed.

---

## Stay Connected

Follow the project, suggest improvements, and become part of a safer future for C programming.

