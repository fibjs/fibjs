#include "abstraction_internal.h"





/****************************************************************************/
#if (defined _WIN32 && defined _MSC_VER)

  /* TRD : 64 bit and 32 bit Windows (user-mode or kernel) on any CPU with the Microsoft C compiler

           _WIN32    indicates 64-bit or 32-bit Windows
           _MSC_VER  indicates Microsoft C compiler
  */

  INLINE atom_t abstraction_cas( volatile atom_t *destination, atom_t exchange, atom_t compare )
  {
    assert( destination != NULL );
    // TRD : exchange can be any value in its range
    // TRD : compare can be any value in its range

    return( (atom_t) InterlockedCompareExchangePointer((void * volatile *) destination, (void *) exchange, (void *) compare) );
  }

#endif





/****************************************************************************/
#if (!defined __arm__ && __GNUC__ >= 4 && __GNUC_MINOR__ >= 1 && __GNUC_PATCHLEVEL__ >= 0)

  /* TRD : any OS on any CPU except ARM with GCC 4.1.0 or better

           GCC 4.1.0 introduced the __sync_*() atomic intrinsics

           __GNUC__ / __GNUC_MINOR__ / __GNUC_PATCHLEVEL__  indicates GCC and which version
  */

  INLINE atom_t abstraction_cas( volatile atom_t *destination, atom_t exchange, atom_t compare )
  {
    assert( destination != NULL );
    // TRD : exchange can be any value in its range
    // TRD : compare can be any value in its range

    // TRD : note the different argument order for the GCC instrinsic to the MSVC instrinsic

    return( (atom_t) __sync_val_compare_and_swap(destination, compare, exchange) );
  }

#endif





/****************************************************************************/
#if (defined __arm__ && __GNUC__)

  /* TRD : any OS on any ARM with GCC

           Remember however we need to set into compare the original value of destination.

           __arm__   indicates ARM
           __GNUC__  indicates GCC
  */

  INLINE atom_t abstraction_cas( volatile atom_t *destination, atom_t exchange, atom_t compare )
  {
    atom_t
      stored_flag,
      original_destination;

    assert( destination != NULL );
    // TRD : exchange can be any value in its range
    // TRD : compare can be any value in its range

    /* TRD : this is a standard, plain CAS, vulnerable to ABA */

    __asm__ __volatile__
    (
      "  mov    %[stored_flag], #1;"                             // put 1 into stored_flag
      "  mcr    p15, 0, %[zero], c7, c10, 5;"                    // memory barrier (ARM v6 compatible)
      "atomic_cas:;"
      "  ldrex  %[original_destination], [%[destination]];"      // load *destination into original_destination
      "  teq    %[original_destination], %[compare];"            // compare original_destination with compare
      "  bne    exit;"                                           // if not equal, exit
      "  strex  %[stored_flag], %[exchange], [%[destination]];"  // if equal, try to store exchange into *destination (on success, strex puts 0 into stored_flag)
      "  teq    %[stored_flag], #0;"                             // check if stored_flag is 0
      "  bne    atomic_cas;"                                     // if not 0, retry (someone else touched *destination after we loaded but before we stored)
      "  mcr    p15, 0, %[zero], c7, c10, 5;"                    // memory barrier (ARM v6 compatible)
      "exit:;"

      // output
      : "+m" (*destination), [original_destination] "=&r" (original_destination), [stored_flag] "=&r" (stored_flag)

      // input
      : [destination] "r" (destination), [compare] "r" (compare), [exchange] "r" (exchange), [zero] "r" (0)

      // clobbered
      : "cc", "memory"                                           // memory is clobbered because we issue a memory barrier
    );

    return( original_destination );
  }

#endif

