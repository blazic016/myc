#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define GUARD_PATTERN 0xDEADBEEF
#define GUARD_SIZE    4
#define SIZE_FIELD    4
#define INDEX_FIELD   4
#define HEADER_SIZE   (SIZE_FIELD + INDEX_FIELD)

typedef enum {
    TKEL_NO_ERR,
    TKEL_TIMEOUT,
    TKEL_BAD_ARG,
    TKEL_NOT_DONE
} TKEL_err;

#define MAX_ALOKATORI 50000
static unsigned char* ALOKATOR_LISTA[MAX_ALOKATORI];
static int ALOKATOR_CNT = 0;

/* FUNCTIONS */


const char* helper_err_print(TKEL_err err)
{
    switch (err)
    {
        case TKEL_NO_ERR:
            return "TKEL_NO_ERR";
        case TKEL_TIMEOUT:
            return "TKEL_TIMEOUT";
        case TKEL_BAD_ARG:
            return "TKEL_BAD_ARG";
        case TKEL_NOT_DONE:
            return "TKEL_NOT_DONE";
        default:
            return "TKEL_UNKNOWN_ERROR";
    }
}


TKEL_err TKEL_Malloc(unsigned int memorySize, void **buffer) {

    if (memorySize == 0) {
        *buffer = 0;
        printf("WARNING: Null memory size requested\n");
        return TKEL_BAD_ARG;
    }

    unsigned int totalSize = HEADER_SIZE + GUARD_SIZE + memorySize + GUARD_SIZE;
    unsigned char *totalBuffer = (unsigned char *)malloc(totalSize);

    if (!totalBuffer) {
        printf("CRITICAL: Buffer not allocated\n");
        return TKEL_NOT_DONE;
    }

    /* HEADER */
    *(unsigned int *)(totalBuffer) = memorySize;                           // SIZE
    *(unsigned int *)(totalBuffer + SIZE_FIELD) = ALOKATOR_CNT;            // INDEX

    /* PREFIX GUARD */
    *(unsigned int *)(totalBuffer + HEADER_SIZE) = GUARD_PATTERN;

    /* SUFFIX GUARD */
    unsigned int suffixOffset = HEADER_SIZE + GUARD_SIZE + memorySize;
    *(unsigned int *)(totalBuffer + suffixOffset) = GUARD_PATTERN;

    /* PAYLOAD POINTER */
    *buffer = (void *)(totalBuffer + HEADER_SIZE + GUARD_SIZE);

    /* ADD TO LIST */
    if (ALOKATOR_CNT < MAX_ALOKATORI) {
        ALOKATOR_LISTA[ALOKATOR_CNT] = totalBuffer;
        ALOKATOR_CNT++;
    }
    else {
        printf("WARNING: Lista alokatora je puna!\n");
    }

    return TKEL_NO_ERR;
}

TKEL_err TKEL_Free(void *buffer) {

    if (!buffer) {
        printf("WARNING: No buffer to free\n");
        return TKEL_BAD_ARG;
    }

    unsigned char *userPtr = (unsigned char *)buffer;

    /* totalBuffer = userPtr - HEADER_SIZE - GUARD_SIZE */
    unsigned char *totalBuffer = userPtr - (GUARD_SIZE + HEADER_SIZE);

    unsigned int size  = *(unsigned int *)totalBuffer;
    unsigned int index = *(unsigned int *)(totalBuffer + SIZE_FIELD);

    if (index >= ALOKATOR_CNT) {
        printf("ERROR: Ne validan index u headeru (korupcija ili double-free)\n");
        return TKEL_NOT_DONE;
    }

    /* PREFIX CHECK */
    if (*(unsigned int *)(totalBuffer + HEADER_SIZE) != GUARD_PATTERN)
        printf("ERROR: PREFIX guard corrupted (buffer underrun)\n");

    /* SUFFIX CHECK */
    unsigned int suffixOffset = HEADER_SIZE + GUARD_SIZE + size;
    if (*(unsigned int *)(totalBuffer + suffixOffset) != GUARD_PATTERN)
        printf("ERROR: SUFFIX guard corrupted (buffer overflow)\n");

    /* FREE MEM */
    free(totalBuffer);

    /* REMOVE FROM LIST IN O(1) */
    if (index < ALOKATOR_CNT - 1) {
        ALOKATOR_LISTA[index] = ALOKATOR_LISTA[ALOKATOR_CNT - 1];
        unsigned char *moved = ALOKATOR_LISTA[index];
        *(unsigned int *)(moved + SIZE_FIELD) = index;
    }

    ALOKATOR_CNT--;

    return TKEL_NO_ERR;
}



void TKEL_CheckAll() {
    printf("[TKEL_CheckAll] Trenutno alocirano %d\n", ALOKATOR_CNT);

    unsigned long totalAllocated = 0;   // Nije jos implemetirano

    int n = 0;
    for ( n = 0; n < ALOKATOR_CNT; n++) {

        unsigned char* totalBuffer = ALOKATOR_LISTA[n];
        unsigned int size = *(unsigned int*)totalBuffer;

        /* PREFIX */
        if (*(unsigned int*)(totalBuffer + HEADER_SIZE) != GUARD_PATTERN)
            printf("[TKEL_CheckAll] ERROR: PREFIX guard corrupted at %p\n", totalBuffer);

        /* SUFFIX */
        unsigned int suffixStart = HEADER_SIZE + GUARD_SIZE + size;
        if (*(unsigned int*)(totalBuffer + suffixStart) != GUARD_PATTERN)
            printf("[TKEL_CheckAll] ERROR: SUFFIX guard corrupted at %p\n", totalBuffer);
    }
}

int main(void)
{
    TKEL_err err;
    void *p = NULL;

    printf("===== TEST 1: normalna alokacija =====\n");
      err = TKEL_Malloc(128, &p);
      if (err == TKEL_NO_ERR)
          printf("OK: memorija alocirana na %p\n", p);
      else
          printf("FAIL: error %s\n", helper_err_print(err));

      err = TKEL_Free(p);
      printf("TKEL_Free returned: %s\n\n", helper_err_print(err));



    printf("===== TEST 2: Alociranje sa velciinom 0 =====\n");
      p = NULL;
      err = TKEL_Malloc(0, &p);
      printf("Result error: %s, p = %p\n\n", helper_err_print(err), p);



    printf("===== TEST 3: Free with NULL pointer =====\n");
      p = NULL;
      err = TKEL_Free(p);
      printf("Result: %s\n\n", helper_err_print(err));



    printf("===== TEST 4: Multiple allocators =====\n");
      void *a=NULL, *b=NULL, *c=NULL;
      TKEL_Malloc(64, &a);
      TKEL_Malloc(256, &b);
      TKEL_Malloc(1, &c);
      printf("Allocated: a=%p, b=%p, c=%p\n", a, b, c);
      TKEL_Free(b);
      TKEL_Free(a);
      TKEL_Free(c);
      printf("All freed\n\n");



    printf("===== TEST 5: baffer overflow , POSTAMBOLA =====\n");
      unsigned char *myArray1;
      TKEL_Malloc(5, (void **)&myArray1);
      int myCounter;
      for (myCounter = 0; myCounter < 8; myCounter++ ) {
          myArray1[myCounter] = 'A';
      }

      printf("Ilija : Freeing my buffer\n");
      TKEL_Free((void *)myArray1);
      printf("\n");


    printf("===== TEST 6: baffer underrun , PREAMBOLA =====\n");
      unsigned char *myArray2;
      TKEL_Malloc(5, (void **)&myArray2);
      myArray2[-1] = 'A';
      myArray2[-2] = 'A';
      myArray2[-3] = 'A';
      printf("Freeing buffer...\n");
      TKEL_Free((void *)myArray2);
      printf("\n");




      printf("==== TEST 7: without free ====\n");
      {
          unsigned char* p;
          TKEL_Malloc(8, (void**)&p);

          printf("Simuliraj korupciju za suffix guard...\n");
          unsigned char* total = p - GUARD_SIZE - HEADER_SIZE;
          unsigned int size = *(unsigned int*)total;
          int suffixStart = HEADER_SIZE + GUARD_SIZE + size;

          *(unsigned int*)(total + suffixStart) = 0x12345678;

          TKEL_CheckAll();

          TKEL_Free(p);
          printf("\n");
      }




    printf("==== TEST 8: Random korupcija ====\n");
    {
        unsigned char* a;
        unsigned char* b;
        unsigned char* c;

        TKEL_Malloc(20, (void**)&a);
        TKEL_Malloc(30, (void**)&b);
        TKEL_Malloc(40, (void**)&c);

        printf("Allocated pointers: a=%p  b=%p  c=%p\n", a, b, c);
        printf("Corrupting prefix guard of b...\n");
        unsigned char* total_b = b - GUARD_SIZE - HEADER_SIZE;
        *(unsigned int*)(total_b + HEADER_SIZE) = 0xBADCAFE;

        TKEL_CheckAll();

        TKEL_Free(a);
        TKEL_Free(b);
        TKEL_Free(c);
        printf("\n");
    }

    printf("\n==== ALL TESTS DONE ====\n");


    return 0;
}