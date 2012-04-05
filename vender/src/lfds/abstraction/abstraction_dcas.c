#include "abstraction_internal.h"





/****************************************************************************/
#if (defined _WIN64 && defined _MSC_VER)

  /* TRD : 64 bit Windows (user-mode or kernel) on any CPU with the Microsoft C compiler

           _WIN64    indicates 64 bit Windows
           _MSC_VER  indicates Microsoft C compiler
  */

  INLINE unsigned char abstraction_dcas( volatile atom_t *destination, atom_t *exchange, atom_t *compare )
  {
    unsigned char
      cas_result;

    assert( destination != NULL );
    assert( exchange != NULL );
    assert( compare != NULL );

    cas_result = _InterlockedCompareExchange128( (volatile __int64 *) destination, (__int64) *(exchange+1), (__int64) *exchange, (__int64 *) compare );

    return( cas_result );
  }

#endif





/****************************************************************************/
#if (!defined _WIN64 && defined _WIN32 && defined _MSC_VER)

  /* TRD : 32 bit Windows (user-mode or kernel) on any CPU with the Microsoft C compiler

           (!defined _WIN64 && defined _WIN32)  indicates 32 bit Windows
           _MSC_VER                             indicates Microsoft C compiler
  */

  INLINE unsigned char abstraction_dcas( volatile atom_t *destination, atom_t *exchange, atom_t *compare )
  {
    __int64
      original_compare;

    assert( destination != NULL );
    assert( exchange != NULL );
    assert( compare != NULL );

    *(__int64 *) &original_compare = *(__int64 *) compare;

    *(__int64 *) compare = InterlockedCompareExchange64( (volatile __int64 *) destination, *(__int64 *) exchange, *(__int64 *) compare );

    return( (unsigned char) (*(__int64 *) compare == *(__int64 *) &original_compare) );
  }

#endif





/****************************************************************************/
#if (defined __x86_64__ && __GNUC__ && !defined __pic__)

  /* TRD : any OS on x64 with GCC for statically linked code

           __x86_64__  indicates x64
           __GNUC__    indicates GCC
  */

  INLINE unsigned char abstraction_dcas( volatile atom_t *destination, atom_t *exchange, atom_t *compare )
  {
    unsigned char
      cas_result;

    assert( destination != NULL );
    assert( exchange != NULL );
    assert( compare != NULL );

    __asm__ __volatile__
    (
      "lock;"           // make cmpxchg16b atomic
      "cmpxchg16b %0;"  // cmpxchg16b sets ZF on success
      "setz       %3;"  // if ZF set, set cas_result to 1

      // output
      : "+m" (*(volatile atom_t (*)[2]) destination), "+a" (*compare), "+d" (*(compare+1)), "=q" (cas_result)

      // input
      : "b" (*exchange), "c" (*(exchange+1))

      // clobbered
      : "cc", "memory"
    );

    return( cas_result );
  }

#endif






/****************************************************************************/
#if (defined __i686__ && __GNUC__ && !defined __pic__)

  /* TRD : any OS on x86 with GCC for statically linked code

           __i686__  indicates x86
           __GNUC__  indicates GCC
  */

  INLINE unsigned char abstraction_dcas( volatile atom_t *destination, atom_t *exchange, atom_t *compare )
  {
    unsigned char
      cas_result;

    assert( destination != NULL );
    assert( exchange != NULL );
    assert( compare != NULL );

    __asm__ __volatile__
    (
      "lock;"          // make cmpxchg8b atomic
      "cmpxchg8b %0;"  // cmpxchg8b sets ZF on success
      "setz      %3;"  // if ZF set, set cas_result to 1

      // output
      : "+m" (*(volatile atom_t (*)[2]) destination), "+a" (*compare), "+d" (*(compare+1)), "=q" (cas_result)

      // input
      : "b" (*exchange), "c" (*(exchange+1))

      // clobbered
      : "cc", "memory"
    );

    return( cas_result );
  }

#endif





/****************************************************************************/
#if (defined __x86_64__ && __GNUC__ && defined __pic__)

  /* TRD : any OS on x64 with GCC for position independent code (e.g. a shared object)

           __x86_64__  indicates x64
           __GNUC__    indicates GCC
  */

  INLINE unsigned char abstraction_dcas( volatile atom_t *destination, atom_t *exchange, atom_t *compare )
  {
    unsigned char
      cas_result;

    assert( destination != NULL );
    assert( exchange != NULL );
    assert( compare != NULL );

    /* TRD : with a shared object, we cannot clobber RBX
             as such, we borrow RSI - we load half of the exchange value into it
             then swap it with RBX
             then do the compare-and-swap
             then swap the original value of RBX back from RSI
    */

    __asm__ __volatile__
    (
      "xchg %%rsi, %%rbx;"  // swap RBI and RBX 
      "lock;"               // make cmpxchg16b atomic
      "cmpxchg16b %0;"      // cmpxchg16b sets ZF on success
      "setz       %3;"      // if ZF set, set cas_result to 1
      "xchg %%rbx, %%rsi;"  // re-swap RBI and RBX

      // output
      : "+m" (*(volatile atom_t (*)[2]) destination), "+a" (*compare), "+d" (*(compare+1)), "=q" (cas_result)

      // input
      : "S" (*exchange), "c" (*(exchange+1))

      // clobbered
      : "cc", "memory"
    );

    return( cas_result );
  }

#endif






/****************************************************************************/
#if (defined __i686__ && __GNUC__ && defined __pic__)

  /* TRD : any OS on x86 with GCC for position independent code (e.g. a shared object)

           __i686__  indicates x86
           __GNUC__  indicates GCC
  */

  INLINE unsigned char abstraction_dcas( volatile atom_t *destination, atom_t *exchange, atom_t *compare )
  {
    unsigned char
      cas_result;

    assert( destination != NULL );
    assert( exchange != NULL );
    assert( compare != NULL );

    /* TRD : with a shared object, we cannot clobber EBX
             as such, we borrow ESI - we load half of the exchange value into it
             then swap it with EBX
             then do the compare-and-swap
             then swap the original value of EBX back from ESI
    */

    __asm__ __volatile__
    (
      "xchg %%esi, %%ebx;"  // swap EBI and EBX
      "lock;"               // make cmpxchg8b atomic
      "cmpxchg8b %0;"       // cmpxchg8b sets ZF on success
      "setz      %3;"       // if ZF set, set cas_result to 1
      "xchg %%ebx, %%esi;"  // re-swap EBI and EBX

      // output
      : "+m" (*(volatile atom_t (*)[2]) destination), "+a" (*compare), "+d" (*(compare+1)), "=q" (cas_result)

      // input
      : "S" (*exchange), "c" (*(exchange+1))

      // clobbered
      : "cc", "memory"
    );

    return( cas_result );
  }

#endif





/****************************************************************************/
#if (defined __arm__ && __GNUC__)

  /* TRD : any OS on any ARM with GCC

           Remember however we need to set into compare the original value of destination.

           __arm__   indicates ARM
           __GNUC__  indicates GCC
  */

  INLINE unsigned char abstraction_dcas( volatile atom_t *destination, atom_t *exchange, atom_t *compare )
  {
    atom_t
      *local_compare = compare,
      stored_flag = 1;

    register atom_t
      local_exchange_a __asm("r2"),
      local_exchange_b __asm("r3"),
      local_compare_a __asm("r4"),
      local_compare_b __asm("r5"),
      original_destination_a __asm("r6"),
      original_destination_b __asm("r7");

    assert( destination != NULL );
    assert( exchange != NULL );
    assert( compare != NULL );

    /* TRD : some notes

             the double word ldr and str instructions require contigous registers
             where the first register is an even number

             honouring this requirement requires us to specifically specify
             the registers to use (which is why we're using register __asm("rN")
             in the declerations above

             the arguments to the function occupy registers r0, r1 and r2

             we can use up to and including r8, but r9 can have a frame pointer in it

             so we make a copy of compare (freeing up r2, so we can use it for a double
             word load) but use destination (r0) and exchange (r1) directly

             note LDRD and STRD became available in armv6k

             apologies for the trickery with the mcr register variable - the code runs
             out of registers on armv6k
    */

    __asm__ __volatile__
    (
      "  mov     %[stored_flag], #1;"                                                                // put 1 into stored_flag
      "  mov     %[local_exchange_a], #0;"                                                           // borrow local_exchange_a for mcr, to save a register
      "  mcr     p15, 0, %[local_exchange_a], c7, c10, 5;"                                           // memory barrier (ARM v6 compatible)
      "  ldrd    %[local_exchange_a], %[local_exchange_b], [%[exchange]];"                           // load exchange into local_exchange_a and local_exchange_b (which are r2 and r3, respectively)
      "  ldrd    %[local_compare_a], %[local_compare_b], [%[local_compare]];"                        // load compare into local_compare_a and local_compare_b (which are r4 and r5, respectively)
      "atomic_dcas:;"
      "  ldrexd  %[original_destination_a], %[original_destination_b], [%[destination]];"            // load destination into original_destination_a and original_destination_b (which are r6 and r7, respectively)
      "  teq     %[original_destination_a], %[local_compare_a];"                                     // compare the first word of destination with the first word of compare
      "  teqeq   %[original_destination_b], %[local_compare_b];"                                     // if they're equal, compare the second word of destination with the second word of compare
      "  bne     exit;"                                                                              // if either word of destination does not match its respective word of compare, exit
      "  strexd  %[stored_flag], %[local_exchange_a], %[local_exchange_b], [%[destination]];"        // if both words were equal, try to store local_exchange_a and local_exchange_b into *destination (on success, strexed puts 0 into stored_flag)
      "  teq     %[stored_flag], #0;"                                                                // check if stored_flag is 0
      "  bne     atomic_dcas;"                                                                       // if not 0, retry (someone else touched *destination after we loaded but before we stored)
      "exit:;"
      "  strd    %[original_destination_a], %[original_destination_b], [%[local_compare]];"          // whether or not the CAS swapped, we always write the original value of destination into *compare
      "  mov     %[local_exchange_a], #0;"                                                           // borrow local_exchange_a for mcr, to save a register
      "  mcr     p15, 0, %[local_exchange_a], c7, c10, 5;"                                           // memory barrier (ARM v6 compatible)

      // output
      : "+m" (*(volatile atom_t (*)[2]) destination), "+m" (*(atom_t (*)[2]) local_compare),
        [stored_flag] "+&r" (stored_flag),
        [original_destination_a] "+&r" (original_destination_a), [original_destination_b] "+&r" (original_destination_b),
        [local_compare_a] "+&r" (local_compare_a), [local_compare_b] "+&r" (local_compare_b),
        [local_exchange_a] "+&r" (local_exchange_a), [local_exchange_b] "+&r" (local_exchange_b)

      // input
      : "m" (*(atom_t (*)[2]) exchange),
        [destination] "r" (destination),
        [local_compare] "r" (local_compare),
        [exchange] "r" (exchange)

      // clobbered
      : "cc", "memory"                                                                               // memory is clobbered because we issue a memory barrier
    );

    /* TRD : stored_flag is set to 0 on store, 1 on fail
             we need to return 1 on success, 0 on fail
    */

    return( (unsigned char) !stored_flag  );
  }

#endif


