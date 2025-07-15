# Changelog

All notable changes to the **C\*** (C aster) project will be documented in this file.
This project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [v0.1.0] - 2025-06-13
### Added
- Initial release of the C\* micro-library.
- Safe memory wrappers:
  - `safe_malloc`, `safe_calloc`, `safe_realloc`, `safe_free`
  - `safe_strdup`, `safe_strndup`
- Embedded-specific synchronization with optional FreeRTOS support.
- Internal arena allocator with linked list registry for fragmentation management.
- ESP-IDF friendly logging via `SAFE_LOG()` macro.
- Complete integration with an ESP32 MQTT client as a working example.
- Support for standard C and embedded (RTOS or bare-metal) environments.
- Documentation:
  - `README.md`
  - `Safe API Map.md`
  - `CONTRIBUTING.md`
  - `SECURITY.md`
  - `LICENSE`

### Notes
- This release is suitable for evaluation and early adoption.
- Focus is on stability and ease of integration with existing C/ESP-IDF projects.
- Licensed under LGPL-3.0.

---

Past releases will be documented here as the project evolves.

