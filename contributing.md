# Contributing to C\* (C aster)

We welcome contributions to the C\* project — whether you're fixing bugs, improving documentation, or proposing new memory-safe APIs.

Please read this guide before submitting contributions.

---

## 🛠 How to Contribute

1. **Fork the repository**

   - Use GitHub’s Fork button to create your copy.

2. **Create a feature branch**

   ```sh
   git checkout -b feature/my-contribution
   ```

3. *Make your changes using C\* safe memory functions*

   - Instead of `malloc`, `free`, `strdup`, etc., use `safe_malloc`, `safe_free`, `safe_strdup`, etc.
   - Refer to [`Safe Api Map`](Safe%20Api%20Map.md) for mappings.

4. **Write or run tests (if applicable)**

   - If your changes affect behavior, include minimal tests or verify integration with an ESP32 project.

5. **Commit with clarity**

   ```sh
   git commit -m "Fix: Replace unsafe malloc with safe_malloc in MQTT module"
   ```

6. **Push your changes and submit a Pull Request (PR)**

   - Title your PR clearly and describe your changes.
   - Link related issues or documentation.

---

## 📐 Code Style

- Follow clear naming (`safe_` prefix for wrappers).
- Prefer small, composable functions.
- Avoid deep nesting.
- Document any platform-specific logic (e.g., FreeRTOS).

---

## 🐛 Bug Reports & Feature Requests

- Open a GitHub Issue with clear reproduction steps or a feature rationale.
- For memory bugs, include:
  - The function(s) involved.
  - The memory size or input data.
  - Whether you're using `CONFIG_FREERTOS_USE`.

We track all contributions under the LGPL license and preserve authorship credits via commit history.

---

## 🤝 Contributor Credit

All contributions are assumed to be under LGPL-3.0, and contributors will be acknowledged in release notes unless requested otherwise.

---

With appreciation,\
**Rashid S. Vali**\
Initial author and maintainer

---

Thank you for helping improve **C\*** — a safer way to write C.

