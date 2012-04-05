#include "abstraction_internal.h"





/****************************************************************************/
#if (defined _WIN32)

  /* TRD : any Windows (user-mode) on any CPU with the Microsoft C compiler

           _WIN32             indicates 64-bit or 32-bit Windows
           _MSC_VER           indicates Microsoft C compiler
           !WIN_KERNEL_BUILD  indicates Windows user-mode
  */

  void *abstraction_aligned_malloc( size_t size, size_t align_in_bytes )
  {
    void
      *rv;

    rv = _aligned_malloc( size, align_in_bytes );

    return( rv );
  }

#else

  /* TRD : any OS on any CPU with any compiler with POSIX 6.00 or better

           _XOPEN_SOURCE  is actually set by the user, not by the compiler
                          it is the way the user signals to the compiler what
                          level of POSIX should be available
                          (it assumes of course the compiler has support for the given level of POSIX requested)
  */

  void *abstraction_aligned_malloc( size_t size, size_t align_in_bytes )
  {
    int
      rv;

    void
      *memory;

    rv = posix_memalign( &memory, align_in_bytes, size );

    // TRD : posix_memalign returns 0 on success, docs do not say *memory == NULL on fail
    if( rv != 0 )
      memory = NULL;

    return( memory );
  }

#endif


