#include <config.h>
#include <types.h>
#include <stdlib.h>

/*Notice:   Limitation for malloc and free */
/* Before using malloc to allocate new buffer,  the old malloced buffer must be freed */
/* Normal case:      malloc(A)-->free(A) -->malloc(B)-->free(B) */
/* Abnormal case:  malloc(A)-->malloc(B)-->free(A) -->free(B) */

/*In default SDKv1.50, malloc/free have been used in the process of  "4Way handshake" and "uart2wifi"*/
/*Thus ,as the limitation above-mentioned,   do not use malloc/free in these processes*/
void *malloc(size_t size)
{
    extern uint32 __OS_Heap_Start;
    void *ptr;

    /* Notice: We have no boundary check!! */
    ptr = (void *)__OS_Heap_Start;
    __OS_Heap_Start += size;
    return ptr;
}

void free(void *ptr)
{
    extern unsigned long _BSS_END;
    extern uint32 __OS_Heap_Start;

    /* restore to _BSS_END!! */
    __OS_Heap_Start = (uint32)&_BSS_END;
    return;
}

int32 atoi(const int8 *s)
{
    uint8 bchar='A', index, upbound=9;
    uint32 neg=0, value=0, base=10;

    if ( *s=='0' ) {
        switch (*++s) {
            case 'x':
                bchar = 'a';
            case 'X':
                base = 16;
                upbound = 15;
                break;
            case 'b':
            case 'B':
                base = 2;
                upbound = 1;
                break;
            default:
                return 0;
        }
        s++;
    } else if ( *s=='-' ) {
        neg = 1;
        s++;
    }

    while ( *s ) {
        index = *s - '0';
        if ( base==16 && (*s>=bchar) && (*s<=(bchar+5)) ) {
            index = 10 + *s - bchar;
        }
        if ( index > upbound ) {
            break;
        }
        value = value * base + index;
        s++;
    }

    if ( neg==1 )
        return -value;
    return (int32)value;
}




