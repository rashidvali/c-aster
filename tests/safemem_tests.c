#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "safemem_embedded.h"
#include "safemem_tests.h"

#include "c_aster_api.h"
#include "c_aster_api.h"


#ifdef __has_include
#  if __has_include("c_ast_config.h")
#    include "c_ast_config.h"
#  endif
#endif

#include "c_ast_defaults.h"

typedef struct {
    int x;
    float y;
} test_pair_t;

static void test_allocation_boundary(void)
{
    printf("\n--- Allocation boundary test ---\n");

    safemem_init();

    uint8_t *a = safe_malloc(10);
    uint8_t *b = safe_malloc(10);

    printf("a = %p\n", (void *)a);
    printf("b = %p\n", (void *)b);

    uint8_t data[15] = {0};

    int valid_result = set_mem_block(a, data, 10);
    printf("10-byte write into 10-byte allocation: %s\n",
           valid_result ? "PASS" : "FAIL");

    int overflow_result = set_mem_block(a, data, 15);
    printf("15-byte write crossing allocation boundary: %s\n",
           overflow_result ? "FAIL" : "PASS");

    dispose(a);
    dispose(b);
}

static void test_mem_freeing(void)
{
    printf("\n--- Memory freeing / gap reuse test ---\n");

    safemem_init();

    uint8_t *a = safe_malloc(10);
    uint8_t *b = safe_malloc(10);
    uint8_t *c = safe_malloc(10);

    printf("a = %p\n", (void *)a);
    printf("b = %p\n", (void *)b);
    printf("c = %p\n", (void *)c);

    dispose(b);

    uint8_t *d = safe_malloc(8);

    printf("d = %p\n", (void *)d);

    printf("Freed gap reused: %s\n",
           d == b ? "PASS" : "FAIL");

    dispose(a);
    dispose(c);
    dispose(d);
}

static void test_use_after_free(void)
{
    printf("\n--- Use-after-free test ---\n");

    safemem_init();

    uint8_t *p = safe_malloc(10);
    uint8_t data[5] = {0};

    int before_free = set_mem_block(p, data, sizeof(data));

    dispose(p);

    int after_free = set_mem_block(p, data, sizeof(data));

    printf("Access before free: %s\n",
           before_free ? "PASS" : "FAIL");

    printf("Access after free rejected: %s\n",
           after_free ? "FAIL" : "PASS");
}

static void test_invalid_free(void)
{
    printf("\n--- Invalid free test ---\n");

    safemem_init();

    uint8_t *p = safe_malloc(10);

    /*
     * An interior pointer is not an allocation start
     * and must not be accepted by dispose().
     */
    dispose(p + 1);

    uint8_t data[10] = {0};

    int still_valid = set_mem_block(p, data, sizeof(data));

    printf("Interior-pointer free rejected: %s\n",
           still_valid ? "PASS" : "FAIL");

    /*
     * Free the actual allocation, then try freeing
     * the same pointer again.
     */
    dispose(p);
    dispose(p);

    int still_freed = set_mem_block(p, data, sizeof(data));

    printf("Double free rejected: %s\n",
           still_freed ? "FAIL" : "PASS");
}

static void test_fragmentation(void)
{
    printf("\n--- Fragmentation / first-fit test ---\n");

    safemem_init();

    uint8_t *a = safe_malloc(10);
    uint8_t *b = safe_malloc(5);
    uint8_t *c = safe_malloc(10);
    uint8_t *d = safe_malloc(15);
    uint8_t *e = safe_malloc(10);

    printf("a = %p\n", (void *)a);
    printf("b = %p\n", (void *)b);
    printf("c = %p\n", (void *)c);
    printf("d = %p\n", (void *)d);
    printf("e = %p\n", (void *)e);

    /*
     * Create two nonadjacent gaps:
     *
     *   b: 5 bytes
     *   d: 15 bytes
     */
    dispose(b);
    dispose(d);

    /*
     * 8 bytes cannot fit into b's old 5-byte gap,
     * so first-fit must skip it and reuse d's gap.
     */
    uint8_t *f = safe_malloc(12);

    printf("f = %p\n", (void *)f);

    printf("Too-small first gap skipped: %s\n",
           f != b ? "PASS" : "FAIL");

    printf("First suitable gap reused: %s\n",
           f == d ? "PASS" : "FAIL");

    dispose(a);
    dispose(c);
    dispose(e);
    dispose(f);
}

static void test_metadata_exhaustion(void)
{
    printf("\n--- Metadata exhaustion test ---\n");

    safemem_init();

    void *blocks[C_ASTR_CONFIG_MAX_BLOCKS] = {0};

    int allocated = 0;

    for (int i = 0; i < C_ASTR_CONFIG_MAX_BLOCKS; ++i) {
        blocks[i] = safe_malloc(1);

        if (blocks[i])
            allocated++;
        else
            break;
    }

    void *extra = safe_malloc(1);

    printf("Allocated %d metadata-backed blocks: %s\n",
           allocated,
           allocated == C_ASTR_CONFIG_MAX_BLOCKS ? "PASS" : "FAIL");

    printf("Allocation beyond metadata capacity rejected: %s\n",
           extra == NULL ? "PASS" : "FAIL");

    for (int i = 0; i < allocated; ++i)
        dispose(blocks[i]);
}

static void test_allocation_alignment(void)
{
    printf("\n--- Allocation alignment test ---\n");

    safemem_init();

	char *a = alnChar('A');
	int *b = alnInt(123);
	float *c = alnFloat(3.14f);
	char *d = alnStr("C* - C-aster");

    printf("a = %p\n", a);
    printf("b = %p\n", b);
    printf("c = %p\n", c);

    printf("char allocation aligned: %s\n",
       ((uintptr_t)a % _Alignof(char)) == 0 ? "PASS" : "FAIL"); //--
	
	printf("char allocation initialized: %s\n",
       (a != NULL && *a == 'A') ? "PASS" : "FAIL");

	printf("int allocation aligned: %s\n", 
       ((uintptr_t)b % _Alignof(int)) == 0 ? "PASS" : "FAIL"); //--

	printf("int allocation initialized: %s\n",
       (b != NULL && *b == 123) ? "PASS" : "FAIL");

	printf("float allocation aligned: %s\n",
       ((uintptr_t)c % _Alignof(float)) == 0 ? "PASS" : "FAIL"); //--

	printf("float allocation initialized: %s\n",
       (c != NULL && *c == 3.14f) ? "PASS" : "FAIL");

	printf("string allocation initialized: %s\n",
       (d != NULL && strcmp(d, "C* - C-aster") == 0) ? "PASS" : "FAIL"); //--

    dispose(a);
    dispose(b);
    dispose(c);
	dispose(d);

	// =================
	int *int_arr = alnIntArr(4);

	printf("int array aligned: %s\n",
       (int_arr != NULL &&
        ((uintptr_t)int_arr % _Alignof(int)) == 0)
           ? "PASS" : "FAIL");

	bool int_arr_zero = int_arr != NULL;

	if (int_arr_zero) {
		for (size_t i = 0; i < 4; ++i) {
			if (int_arr[i] != 0) {
				int_arr_zero = false;
				break;
			}
		}
	}

	printf("int array zero-initialized: %s\n",
		int_arr_zero ? "PASS" : "FAIL");

	dispose(int_arr);		

	// ==================
	float *float_arr = alnFloatArr(4);
	
	printf("float array aligned: %s\n",
       (float_arr != NULL &&
        ((uintptr_t)float_arr % _Alignof(float)) == 0)
           ? "PASS" : "FAIL");

	bool float_arr_zero = float_arr != NULL;

	if (float_arr_zero) {
		for (size_t i = 0; i < 4; ++i) {
			if (float_arr[i] != 0.0f) {
				float_arr_zero = false;
				break;
			}
		}
	}

	printf("float array zero-initialized: %s\n",
       float_arr_zero ? "PASS" : "FAIL");	

	dispose(float_arr);	

	//=====================
	char *char_arr = alnCharArr(4);

	printf("char array aligned: %s\n",
       (char_arr != NULL &&
        ((uintptr_t)char_arr % _Alignof(char)) == 0)
           ? "PASS" : "FAIL");

	bool char_arr_zero = char_arr != NULL;

	if (char_arr_zero) {
		for (size_t i = 0; i < 4; ++i) {
			if (char_arr[i] != 0) {
				char_arr_zero = false;
				break;
			}
		}
	}

	printf("char array zero-initialized: %s\n",
		char_arr_zero ? "PASS" : "FAIL");

	dispose(char_arr);

	//=====================
	test_pair_t *pair_arr = alnTypeArr(test_pair_t, 3);

	printf("typed array aligned: %s\n",
       (pair_arr != NULL &&
        ((uintptr_t)pair_arr % _Alignof(test_pair_t)) == 0)
           ? "PASS" : "FAIL");

	bool pair_arr_zero = pair_arr != NULL;

	if (pair_arr_zero) {
		for (size_t i = 0; i < 3; ++i) {
			if (pair_arr[i].x != 0 || pair_arr[i].y != 0.0f) {
				pair_arr_zero = false;
				break;
			}
		}
	}

	printf("typed array zero-initialized: %s\n",
       pair_arr_zero ? "PASS" : "FAIL");

	dispose(pair_arr);

	//=======================
	test_pair_t *pair = alnType(test_pair_t);

	printf("typed allocation aligned: %s\n",
       (pair != NULL &&
        ((uintptr_t)pair % _Alignof(test_pair_t)) == 0)
           ? "PASS" : "FAIL");

	dispose(pair);
}

static void test_aligned_fragmentation(void)
{
    printf("\n--- Aligned fragmentation test ---\n");

    safemem_init();

    uint8_t *a = safe_malloc(1);
    uint8_t *b = safe_malloc(8);
    uint8_t *c = safe_malloc(1);
    uint8_t *d = safe_malloc(8);

    printf("a = %p\n", (void *)a);
    printf("b = %p\n", (void *)b);
    printf("c = %p\n", (void *)c);
    printf("d = %p\n", (void *)d);

    dispose(b);

    uint8_t *e = safe_malloc(8);

    printf("e = %p\n", (void *)e);

    printf("Freed aligned gap reused: %s\n",
           e == b ? "PASS" : "FAIL");

    printf("Reused allocation aligned: %s\n",
           ((uintptr_t)e % _Alignof(max_align_t)) == 0 ? "PASS" : "FAIL");

    dispose(a);
    dispose(c);
    dispose(d);
    dispose(e);
}

void safemem_run_tests(void)
{
    printf("\n=== C* safemem tests === (7)\n");

    // test_allocation_boundary();  // 1
    // test_mem_freeing();          // 2
    // test_use_after_free();       // 3
    // test_invalid_free();         // 4
    // test_fragmentation();        // 5
    // test_metadata_exhaustion();     // 6
	test_allocation_alignment(); // 7
	// test_aligned_fragmentation(); // 8
}