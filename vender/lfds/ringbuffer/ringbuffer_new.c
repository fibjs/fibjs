#include "ringbuffer_internal.h"





/****************************************************************************/
int ringbuffer_new( struct ringbuffer_state **rs, atom_t number_elements, int (*user_data_init_function)(void **user_data, void *user_state), void *user_state )
{
  int
    rv = 0;

  assert( rs != NULL );
  // TRD : number_elements can be any value in its range
  // TRD : user_data_init_function can be NULL
  // TRD : user_state can be NULL

  *rs = (struct ringbuffer_state *) abstraction_aligned_malloc( sizeof(struct ringbuffer_state), ALIGN_DOUBLE_POINTER );

  if( *rs != NULL )
  {
    freelist_new( &(*rs)->fs, number_elements, user_data_init_function, user_state );

    if( (*rs)->fs != NULL )
    {
      queue_new( &(*rs)->qs, number_elements );

      if( (*rs)->qs != NULL )
        rv = 1;

      if( (*rs)->qs == NULL )
      {
        abstraction_aligned_free( *rs );
        *rs = NULL;
      }
    }

    if( (*rs)->fs == NULL )
    {
      abstraction_aligned_free( *rs );
      *rs = NULL;
    }
  }

  return( rv );
}

