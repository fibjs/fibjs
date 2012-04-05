#include "queue_internal.h"





/****************************************************************************/
void queue_delete( struct queue_state *qs, void (*user_data_delete_function)(void *user_data, void *user_state), void *user_state )
{
  void
    *user_data;

  assert( qs != NULL );
  // TRD : user_data_delete_function can be NULL
  // TRD : user_state can be NULL

  while( queue_dequeue(qs, &user_data) )
    if( user_data_delete_function != NULL )
      user_data_delete_function( user_data, user_state );

  /* TRD : fully dequeuing will leave us
           with a single dummy element
           which both qs->enqueue and qs->dequeue point at
           we push this back onto the freelist
           before we delete the freelist
  */

  freelist_push( qs->fs, qs->enqueue[QUEUE_POINTER]->fe );

  freelist_delete( qs->fs, queue_internal_freelist_delete_function, NULL );

  abstraction_aligned_free( qs );

  return;
}





/****************************************************************************/
#pragma warning( disable : 4100 )

void queue_internal_freelist_delete_function( void *user_data, void *user_state )
{
  assert( user_data != NULL );
  assert( user_state == NULL );

  abstraction_aligned_free( user_data );

  return;
}

#pragma warning( default : 4100 )

