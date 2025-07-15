# C* (C aster) Safe Memory API Map

This document maps native C memory functions to their C* safe equivalents. Developers transitioning to memory-safe practices should use the provided wrappers to ensure safety and consistency.

---

## 🧠 Memory Allocation
| Native Function | C* Safe Equivalent | Notes |
|-----------------|--------------------|-------|
| `malloc(size)`  | `safe_malloc(size)` | Allocates memory and tracks the block in the registry. |
| `calloc(n, sz)` | `safe_calloc(n, sz)` | Allocates and zero-initializes memory, tracks block. |
| `realloc(ptr, new_size)` | `safe_realloc(ptr, new_size)` | Reallocates tracked memory safely. |

---

## 🧼 Memory Deallocation
| Native Function | C* Safe Equivalent | Notes |
|-----------------|--------------------|-------|
| `free(ptr)`     | `safe_free(ptr)`     | Removes the block from the registry before deallocating. |

---

## 🪞 String Duplication
| Native Function | C* Safe Equivalent | Notes |
|-----------------|--------------------|-------|
| `strdup(str)`   | `safe_strdup(str)`   | Allocates and tracks duplicated string. |

---

## 🔁 Memory Operations (Same behavior, optionally logged)
| Native Function | C* Optional Replacement | Notes |
|-----------------|--------------------------|-------|
| `memcpy(dst, src, sz)` | `memcpy` (no wrapper by default) | Use directly unless tracking is needed. |
| `memset(ptr, value, sz)` | `memset` | Use directly. |

> ℹ️ For `memcpy`/`memset` operations involving tracked memory, wrappers could be added to validate bounds or enhance diagnostics.

---

## ⛓ RTOS Compatibility (ESP32 FreeRTOS)
| Feature | C* Handling |
|---------|--------------|
| Mutex for registry | Uses `SemaphoreHandle_t` if `CONFIG_FREERTOS_USE` is defined |
| Thread-safe memory ops | Enabled when using FreeRTOS |

---

## 🗺 Adoption Tips
- **Discipline is key**: Avoid using native functions directly in memory-safe modules.
- **Mixing allowed**: Third-party/legacy modules can coexist as long as they don’t bypass or corrupt tracked memory.
- **Registry costs**: A minor performance cost (~small map) is incurred for safety.

> For bug reports or feature suggestions, please see the [README](README.md).

---

C* — Copyright (C) Rashid S. Vali — LGPL-3.0

