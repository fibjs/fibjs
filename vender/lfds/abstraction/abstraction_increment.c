#include "abstraction_internal.h"





/****************************************************************************/
#if (defined _WIN64 && defined _MSC_VER)

  /* TRD : 64 bit Windows (user-mode or kernel) on any CPU with the Microsoft C compiler

           _WIN64    indicates 64 bit Windows
           _MSC_VER  indicates Microsoft C compiler
  */

  INLINE atom_t abstraction_increment( atom_t *value )
  {
    __int64
      rv;

    assert( value != NULL );

    rv = _InterlockedIncrement64( (__int64 *) value );

    return( (atom_t) rv );
  }

#endif





/****************************************************************************/
#if (!defined _WIN64 && defined _WIN32 && defined _MSC_VER)

  /* TRD : 32 bit Windows (user-mode or kernel) on any CPU with the Microsoft C compiler

           (!defined _WIN64 && defined _WIN32)  indicates 32 bit Windows
           _MSC_VER                             indicates Microsoft C compiler
  */

  INLINE atom_t abstraction_increment( atom_t *value )
  {
    long int
      rv;

    assert( value != NULL );

    rv = _InterlockedIncrement( (long int *) value );

    return( (atom_t) rv );
  }

#endif





/****************************************************************************/
#if (!defined __arm__ && __GNUC__ >= 4 && __GNUC_MINOR__ >= 1 && __GNUC_PATCHLEVEL__ >= 0)

  /* TRD : any OS on any CPU with GCC 4.1.0 or better

           GCC 4.1.0 introduced the __sync_*() atomic intrinsics

           __GNUC__ / __GNUC_MINOR__ / __GNUC_PATCHLEVEL__  indicates GCC and which version
  */

  INLINE atom_t abstraction_increment( atom_t *value )
  {
    atom_t
      rv;

    assert( value != NULL );

    // TRD : no need for casting here, GCC has a __sync_add_and_fetch() for all native types

    rv = __sync_add_and_fetch( value, 1 );

    return( rv );
  }

#endif





/****************************************************************************/
#if (defined __arm__ && __GNUC__ >= 4)

  /* TRD : any OS on any CPU with GCC 4.1.0 or better

           GCC 4.1.0 introduced the __sync_*() atomic intrinsics

           __arm__   indicates ARM
           __GNUC__  indicates GCC
  */

  INLINE atom_t abstraction_increment( atom_t *value )
  {
    atom_t
      stored_flag = 0,
      new_value = 0;

    assert( value != NULL );

    __asm__ __volatile__
    (
      "  mov    %[stored_flag], #1;"                        // move 1 into stored_flag
      "  mcr    p15, 0, %[zero], c7, c10, 5;"               // memory barrier (ARM v6 compatible)
      "atomic_add:;"
      "  ldrex  %[new_value], [%[value]];  "                // load *value into new_value
      "  add    %[new_value], #1;"                          // add 1 to new_value
      "  strex  %[stored_flag], %[new_value], [%[value]];"  // try to store new_value into *value (on success, strex puts 0 into stored_flag)
      "  teq    %[stored_flag], #0;"                        // check if stored_flag is 0
      "  bne    atomic_add;"                                // if not 0, retry (someone else touched *value after we loaded but before we stored)
      "  mcr    p15, 0, %[zero], c7, c10, 5;"               // memory barrier (ARM v6 compatible)

      // output
      : "+m" (*value), [new_value] "+&r" (new_value), [stored_flag] "+&r" (stored_flag)

      // input
      : [value] "r" (value), [zero] "r" (0)

      // clobbered
      : "cc", "memory"                                      // memory is clobbered because we issue a memory barrier
    );

    return( new_value );
  }

#endif

