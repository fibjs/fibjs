#include "ringbuffer_internal.h"





/****************************************************************************/
struct freelist_element *ringbuffer_get_read_element( struct ringbuffer_state *rs, struct freelist_element **fe )
{
  assert( rs != NULL );
  assert( fe != NULL );

  queue_dequeue( rs->qs, (void **) fe );

  return( *fe );
}





/****************************************************************************/
struct freelist_element *ringbuffer_get_write_element( struct ringbuffer_state *rs, struct freelist_element **fe, int *overwrite_flag )
{
  assert( rs != NULL );
  assert( fe != NULL );
  // TRD : overwrite_flag can be NULL

  /* TRD : we try to obtain an element from the freelist
           if we can, we populate it and add it to the queue

           if we cannot, then the ringbuffer is full
           so instead we grab the current read element and
           use that instead

           dequeue may fail since the queue may be emptied
           during our dequeue attempt

           so what we actually do here is a loop, attempting
           the freelist and if it fails then a dequeue, until
           we obtain an element

           once we have an element, we queue it

           you may be wondering why this operation is in a loop
           remember - these operations are lock-free; anything
           can happen in between

           so for example the pop could fail because the freelist
           is empty; but by the time we go to get an element from
           the queue, the whole queue has been emptied back into
           the freelist!

           if overwrite_flag is provided, we set it to 0 if we
           obtained a new element from the freelist, 1 if we
           stole an element from the queue
  */

  do
  {
    if( overwrite_flag != NULL )
      *overwrite_flag = 0;

    freelist_pop( rs->fs, fe );

    if( *fe == NULL )
    {
      ringbuffer_get_read_element( rs, fe );

      if( overwrite_flag != NULL and *fe != NULL )
        *overwrite_flag = 1;
    }
  }
  while( *fe == NULL );

  return( *fe );
}





/****************************************************************************/
void ringbuffer_put_read_element( struct ringbuffer_state *rs, struct freelist_element *fe )
{
  assert( rs != NULL );
  assert( fe != NULL );

  freelist_push( rs->fs, fe );

  return;
}





/****************************************************************************/
void ringbuffer_put_write_element( struct ringbuffer_state *rs, struct freelist_element *fe )
{
  assert( rs != NULL );
  assert( fe != NULL );

  queue_enqueue( rs->qs, fe );

  return;
}

