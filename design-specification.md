# C* (`c-aster`) Design Specification

**Status:** Working design specification  
**Purpose:** Authoritative reference for agreed C* architecture and API decisions during the current redesign.

This document records decisions that are already agreed, clearly separates unresolved items, and avoids introducing implementation choices that have not yet been approved.

## Status markers

- **AGREED** — settled design decision; implementation should follow it.
- **TBD** — intentionally unresolved; must not be silently assumed.
- **FUTURE** — explicitly deferred beyond the current implementation step/release scope.

---

## 1. Project scope

**AGREED**

C* (`c-aster`) is an embedded C memory-safety micro-library currently developed and tested on ESP32-S3 / ESP-IDF with FreeRTOS.

The library is designed around:

- statically allocated managed memory;
- statically allocated metadata;
- no dependency on a dynamic heap for its own allocator metadata structures;
- validated managed-memory access;
- predictable resource bounds;
- incremental implementation and board testing.

The current local library repository structure is:

```text
c-aster/
    src/
        safemem_embedded.h
        safemem_embedded.c
        c_aster_api.h
        c_aster_api.c
    tests/
        safemem_tests.h
        safemem_tests.c
```

The separate `c-aster-test` ESP-IDF project exists only to build and run the C* library/tests on hardware. Test cases belong in the C* repository itself.

---

## 2. Development rule

**AGREED**

Implementation changes must be made incrementally.

The required workflow is:

```text
one design/implementation change
        ↓
build
        ↓
flash ESP32-S3
        ↓
run relevant board test(s)
        ↓
confirm result
        ↓
only then proceed
```

Do not bundle unrelated architectural/API changes into one step unless explicitly agreed.

Previously agreed decisions are constraints. Alternative designs should not be introduced silently during implementation.

---

## 3. Arena and allocation architecture

**AGREED**

C* uses a statically allocated arena.

C* no longer maintains a list of free/available memory blocks.

Instead, it maintains an address-sorted list of metadata for live allocations:

```text
allocated_list
```

Free memory is inferred from gaps between live allocations.

Allocation uses first-fit gap searching.

Because first-fit must inspect address-ordered live allocations, allocation is expected to remain **O(n)** in the number of live allocations.

This O(n) allocation cost is accepted.

---

## 4. Metadata pool

**AGREED**

Allocation metadata comes from a fixed static pool:

```text
block_pool
```

`C_ASTR_CONFIG_MAX_BLOCKS` is the maximum number of simultaneous live allocations / metadata blocks.

A live allocation consumes one metadata block.

On allocation:

```text
free metadata entry
    → initialized
    → linked into allocated_list
```

On deallocation:

```text
metadata entry
    → removed from allocated_list
    → returned to the available metadata pool
```

The existing metadata architecture should be preserved rather than replaced unnecessarily.

At minimum, allocation metadata contains:

```text
starting address
allocation size
```

Existing linkage/state fields required by the current implementation remain valid.

---

## 5. Runtime type metadata

**AGREED**

`AllocationBlock` will **not** store a runtime C type identifier at this stage.

The library's primary responsibility is memory safety, not implementing a runtime type system.

For example, scalar access validates that:

- the pointer identifies a live allocation;
- the allocation is large enough for the requested operation.

C* does not currently need to distinguish semantically between different C types that happen to have compatible sizes.

**FUTURE**

Runtime type metadata may be reconsidered only if a concrete requirement justifies the per-block RAM and complexity cost.

---

## 6. Alignment-aware allocation

**AGREED**

The old behavior of aligning every allocation to `_Alignof(max_align_t)` is being replaced by caller-specified alignment.

The core primitive is:

```c
void *c_ast_allocate(size_t size, size_t alignment);
```

Typed allocation uses the actual type alignment wherever possible:

```c
_Alignof(type)
```

The existing first-fit allocator remains responsible for finding a gap large enough after alignment is applied.

The arena itself remains suitably aligned so typed allocations can be satisfied.

---

## 7. Public API separation

**AGREED**

Public user-facing C* API functions are separated from allocator internals.

```text
c_aster_api.h
c_aster_api.c
```

contain the public API layer.

```text
safemem_embedded.h
safemem_embedded.c
```

contain the allocator/core implementation.

Public convenience functions such as `alnInt()` call core allocation primitives but must not manipulate `allocated_list`, `block_pool`, or other allocator internals directly.

Core operations such as deallocation may be publicly declared while remaining implemented in the allocator/core file when they fundamentally manipulate allocator state.

---

## 8. Scalar allocation API

**AGREED**

The following public allocation functions are part of the API:

```c
int   *alnInt(int value);
float *alnFloat(float value);
char  *alnChar(char value);
char  *alnStr(const char *value);
```

Scalar typed allocation:

- allocates enough storage for the type;
- uses the actual type alignment;
- initializes the allocation with the supplied value.

`alnStr()`:

- rejects `NULL`;
- allocates enough bytes for the string plus terminating `\0`;
- copies the complete string including the terminator.

---

## 9. Generic typed allocation

**AGREED**

The generic typed allocation macro is:

```c
#define alnType(type)     ((type *)c_ast_allocate(sizeof(type), _Alignof(type)))
```

`alnType(type)` guarantees appropriate allocation size and alignment.

It does **not** imply zero initialization.

---

## 10. Array allocation

**AGREED**

The concrete array API includes:

```c
int   *alnIntArr(size_t count);
float *alnFloatArr(size_t count);
char  *alnCharArr(size_t count);
```

Arrays are zero-initialized.

All concrete array allocators use a shared core primitive:

```c
void *c_ast_allocate_array(
    size_t count,
    size_t element_size,
    size_t alignment);
```

The core array primitive is responsible for:

- rejecting zero count / zero element size as currently defined;
- checking multiplication overflow;
- computing total allocation size;
- allocating using the requested alignment;
- zero-initializing the complete allocation.

Concrete array functions are thin wrappers around this core primitive.

---

## 11. Generic typed arrays

**AGREED**

The generic typed array macro is:

```c
#define alnTypeArr(type, count)     ((type *)c_ast_allocate_array(         (count), sizeof(type), _Alignof(type)))
```

The resulting allocation:

- is aligned for `type`;
- contains `count` elements;
- is zero-initialized.

This has been board-tested with a user-defined struct type.

---

## 12. Deallocation

**AGREED**

The public deallocation API is:

```c
void dispose(void *ptr);
```

The previous public name `safe_free()` is being replaced by `dispose()`.

The implementation remains in the allocator/core because deallocation manipulates allocator metadata.

Deallocation requires the **exact allocation-start pointer**.

An interior pointer is not a valid deallocation target.

Double deallocation must be rejected safely.

---

## 13. Allocation index

**AGREED**

A second lookup structure will be added to accelerate metadata lookup for ordinary managed-memory access.

This structure is an internal **allocation index**.

It does **not** replace `allocated_list`.

The two structures have different purposes:

```text
allocated_list
    address-sorted
    used for first-fit gap search during allocation

allocation index
    keyed by allocation-start address
    used for fast metadata lookup
```

The allocation index must be:

- fully static;
- preallocated;
- bounded;
- free of dynamic allocation.

---

## 14. Allocation-index key and value

**AGREED**

The index key is the allocation-start address returned to the caller.

Conceptually:

```text
key   = allocation start address
value = index into block_pool
```

The index does not duplicate `AllocationBlock` metadata.

It stores a compact reference to the existing metadata entry in `block_pool`.

Example:

```text
allocation_index[slot] = 12
```

means that the live allocation metadata is:

```c
&block_pool[12]
```

---

## 15. Hash-index implementation

**AGREED**

The allocation index will use a statically allocated, fixed-size open-addressed hash table.

```c
#define C_ASTR_INDEX_SIZE \
    ((C_ASTR_CONFIG_MAX_BLOCKS * 2U) + 1U)

typedef uint16_t c_ast_block_index_t;

#define C_ASTR_INDEX_EMPTY      UINT16_MAX
#define C_ASTR_INDEX_TOMBSTONE  (UINT16_MAX - 1U)
```

The table size is derived from `C_ASTR_CONFIG_MAX_BLOCKS`, keeping the maximum live load factor below 50%. The implementation must enforce at compile time that configured metadata capacity does not collide with the two reserved index values.

Each occupied entry stores an index into the existing `block_pool`; allocation metadata is not duplicated.

The hash key is the exact allocation-start address. Hashing uses the pointer offset relative to the static arena and a lightweight multiplicative hash.

Collision resolution uses **linear probing**. Every probing loop is bounded by `C_ASTR_INDEX_SIZE`.

Deletion marks a slot `TOMBSTONE`; lookup continues through tombstones; insertion reuses the first suitable tombstone encountered in its probe sequence.

The implementation maintains:

```c
static size_t allocation_index_tombstones;
```

for diagnostics and measurement.

No dynamic allocation, resizing, or dynamically allocated collision structure is permitted.

**AGREED — accepted first-release compromise**

Tombstones are accepted for the first stable release. Their accumulation must remain visible and measurable.

**FUTURE — HIGH PRIORITY AFTER FIRST STABLE RELEASE**

Eliminate or substantially reduce tombstone accumulation as soon as practical after the first stable release. Candidate approaches may include backward-shift deletion, probe-cluster repair, or controlled static-table rebuilding/compaction. No replacement strategy is selected yet.

---

## 16. Scalar get/set lookup strategy

**AGREED**

Scalar get/set operations use the allocation-start pointer as the index key.

For example:

```c
int *i = alnInt(100);
```

The address stored in `i` is the key used to retrieve its `AllocationBlock`.

Conceptually:

```text
getInt(i, ...)
    ↓
hash lookup(i)
    ↓
not found → reject
found     → retrieve block_pool metadata
    ↓
validate requested access fits allocation
    ↓
perform access
```

If the key is absent, the pointer is not the start of a currently live managed allocation and access is rejected.

A successful lookup alone is not sufficient: the allocation size must also be valid for the requested type.

For example, `getInt()` requires the live allocation to contain at least `sizeof(int)` bytes.

---

## 17. Scalar validated-access API

**AGREED**

The public scalar access API direction is:

```c
bool setInt(int *ptr, int value);
bool getInt(const int *ptr, int *value);

bool setFloat(float *ptr, float value);
bool getFloat(const float *ptr, float *value);

bool setChar(char *ptr, char value);
bool getChar(const char *ptr, char *value);
```

These operations should not walk `allocated_list`.

Their metadata lookup should use the allocation index.

Target metadata lookup complexity is average **O(1)**.

---

## 18. Array element access

**AGREED**

Array element operations use the allocation-start pointer as the hash key.

The agreed access path is:

```text
1. look up the allocation-start pointer in the allocation index
2. retrieve the AllocationBlock from block_pool
3. use element index and data type/element size to calculate the element offset/address
4. verify the requested element lies completely inside the allocation
5. perform the read/write
```

The intended API direction is:

```c
bool setIntElm(int *ptr, size_t index, int value);
bool getIntElm(const int *ptr, size_t index, int *value);

bool setFloatElm(float *ptr, size_t index, float value);
bool getFloatElm(const float *ptr, size_t index, float *value);

bool setCharElm(char *ptr, size_t index, char value);
bool getCharElm(const char *ptr, size_t index, char *value);
```

With a valid allocation-start pointer and fixed-size element arithmetic, ordinary element access should not require walking `allocated_list`.

Target metadata lookup complexity is average **O(1)**.

**TBD**

Exact overflow-safe element-address arithmetic and helper-function layout will be finalized when array access is implemented.

---

## 19. Synchronization

**AGREED**

Allocation, deallocation, metadata lookup, validation, and managed-memory access must be synchronized.

The following state is logically protected together:

```text
allocated_list
block_pool / metadata availability state
allocation index
managed-memory access performed through C* validated operations
```

The design will initially use **one C* synchronization boundary / mutex**, not separate locks for each structure.

This avoids lock-ordering complexity and deadlock risks.

---

## 20. Atomic access rule

**AGREED**

Lookup, validation, and the actual managed-memory read/write must occur within the same synchronization boundary.

This is invalid:

```text
lock
lookup
validate
unlock

read/write
```

because another task could dispose the allocation between validation and access.

The required conceptual behavior is:

```text
LOCK
    lookup
    validate
    read/write
UNLOCK
```

The same rule applies to scalar and array-element access.

---

## 21. Allocation and deallocation consistency

**AGREED**

Allocation must update all relevant allocator/index state atomically from the perspective of other C* operations.

Conceptually:

```text
LOCK
    find first-fit gap
    obtain metadata block
    initialize metadata
    insert into allocated_list
    insert into allocation index
UNLOCK
```

A live allocation must not become externally visible in only one of the two metadata structures.

Deallocation similarly performs the relevant removal/recycling work under the same synchronization boundary.

---

## 22. Internal locked/unlocked helpers

**AGREED**

Internal helper functions should avoid recursive mutex acquisition.

Helpers that require the caller to already hold the C* lock should be clearly internal and follow an explicit convention such as:

```c
index_lookup_unlocked(...)
index_insert_unlocked(...)
index_remove_unlocked(...)
validate_range_unlocked(...)
```

Exact names may be adjusted, but the ownership rule remains:

> Internal metadata/index helpers do not acquire the mutex when their caller already owns the synchronization boundary.

---

## 23. Complexity targets

**AGREED**

Expected complexity:

```text
Operation                         Expected complexity

allocation / first-fit            O(n)
scalar metadata lookup            average O(1)
scalar get/set                    average O(1)
array metadata lookup             average O(1)
array element get/set             average O(1)
```

Hash-table operations are average O(1), not mathematically guaranteed constant time in the presence of collisions.

Allocation remains O(n) because the address-sorted list must be walked to locate a suitable free gap.

This tradeoff is accepted: allocation is less frequent than ordinary managed-memory access in the intended usage model.

---

## 24. Real-time applicability

**AGREED**

C* is suitable for **soft real-time** applications.

C* may also be suitable for **hard real-time** applications when used with appropriate application-level design constraints and timing analysis.

A particularly important strategy for hard real-time use is to perform allocations and deallocations outside timing-critical execution paths whenever possible. Managed objects can be allocated ahead of time during initialization or another non-critical phase, while timing-critical code primarily performs bounded indexed access to already allocated objects.

The design supports real-time predictability through:

- a statically allocated arena;
- a fixed `block_pool`;
- a fixed, preallocated allocation index;
- compile-time bounded allocation capacity;
- no dynamic resizing of allocator metadata/index structures;
- bounded data structures whose maximum sizes are known from configuration.

Allocation remains O(n), with:

```text
n <= C_ASTR_CONFIG_MAX_BLOCKS
```

Indexed metadata lookup targets average O(1). Because the allocation index is fixed in size, probing will also have a finite implementation-defined upper bound.

Synchronization can add blocking latency. A get/set operation may wait for another C* operation that currently owns the shared synchronization boundary, including an allocation or deallocation operation.

For hard real-time deployments, application design should therefore prefer:

```text
initialization / non-critical phase
    → allocate managed objects

timing-critical phase
    → operate primarily on preallocated objects
    → use bounded indexed get/set access
    → avoid allocation/deallocation where timing requires it
```

**TBD**

Exact worst-case execution time (WCET), maximum hash-probe count, and maximum synchronization blocking duration must be measured or analyzed after the allocation index and synchronized access paths are implemented.

C* does not claim universal hard real-time suitability independent of configuration, workload, and application architecture.

---

## 25. String access

**AGREED**

The intended string API is:

```c
const char *getStr(const char *ptr);
bool setStr(char *ptr, const char *value);
```

`setStr()` succeeds only when the new string has **exactly the same length** as the currently stored string.

A shorter replacement is rejected.

A longer replacement is rejected.

**TBD**

Exact synchronization/validation helper implementation for strings will be finalized when string access is implemented.

---

## 26. Struct access

**AGREED**

Struct access is intended to be macro-based.

Conceptual API:

```text
getStrc(Type, ptr, member)
setStrc(Type, ptr, member, value)
```

Nested structs are accessed one level at a time.

**TBD**

Exact macro definitions and validation semantics are not yet finalized.

---

## 27. Raw block storage

**AGREED**

`alnBlock(size_t size)` represents managed random-access byte storage.

Conceptual API direction includes:

```text
setBlock(block, offset, src, size)
getBlock(block, offset, ...)
```

**TBD**

Exact `getBlock()` semantics are not yet finalized.

**FUTURE**

Random-access block operations may be deferred to a later release if they would unnecessarily complicate the current release.

---

## 28. Diagnostics

**AGREED**

Diagnostics remain part of the public design, including:

```c
void safemem_report(void);
```

and logging macros such as:

```text
SAFE_LOGI(...)
SAFE_LOGE(...)
```

Exact diagnostics evolution is outside the current allocation-index work.

---

## 29. Current implementation/test status

**AGREED / VERIFIED ON ESP32-S3**

The following have been implemented and board-tested during the current redesign:

```text
c_ast_allocate(size, alignment)

alnChar()
alnInt()
alnFloat()
alnStr()

alnIntArr()
alnFloatArr()
alnCharArr()

alnType(type)
alnTypeArr(type, count)

c_ast_allocate_array(...)

dispose()
```

Verified behaviors include:

- actual type-specific alignment;
- scalar initialization;
- string initialization;
- zero-initialized arrays;
- generic typed-array alignment;
- generic typed-array zero initialization;
- generic typed allocation alignment.

The allocation index and new get/set implementation have **not** yet been implemented.

---

## 30. Next architectural step

**AGREED / NEXT**

Implement the allocation index incrementally before scalar `get...` / `set...` functions depend on it.

The first implementation step is limited to:

1. static allocation-index storage;
2. reserved `EMPTY` / `TOMBSTONE` representation;
3. index initialization;
4. tombstone-counter initialization.

Do not connect allocation or `dispose()` to the index in this first step.

After storage/initialization is board-tested, implement and independently test hash lookup/insertion/removal in subsequent steps.

Only after the allocation index is stable and synchronized with allocator metadata should scalar get/set begin relying on it.

---

## 31. Source of truth

**AGREED**

This document is the working architectural source of truth for the current C* redesign.

When implementation choices conflict with this document:

- do not silently choose an alternative;
- stop and explicitly revisit the relevant decision;
- update this specification when a design decision is intentionally changed.

The public API draft remains useful historical/input material, but this specification should be updated as decisions become settled.
