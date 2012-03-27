#include "ringbuffer_internal.h"





/****************************************************************************/
void ringbuffer_delete( struct ringbuffer_state *rs, void (*user_data_delete_function)(void *user_data, void *user_state), void *user_state )
{
  assert( rs != NULL );
  // TRD : user_data_delete_function can be NULL
  // TRD : user_state can be NULL

  queue_delete( rs->qs, NULL, NULL );

  freelist_delete( rs->fs, user_data_delete_function, user_state );

  abstraction_aligned_free( rs );

  return;
}

