#include "queue_internal.h"





/****************************************************************************/
#pragma warning( disable : 4100 )

void queue_query( struct queue_state *qs, enum queue_query_type query_type, void *query_input, void *query_output )
{
  assert( qs != NULL );
  // TRD : query_type can be any value in its range
  // TRD : query_input can be NULL
  assert( query_output != NULL );

  switch( query_type )
  {
    case QUEUE_QUERY_ELEMENT_COUNT:
      assert( query_input == NULL );

      freelist_query( qs->fs, FREELIST_QUERY_ELEMENT_COUNT, NULL, query_output );
    break;

    case QUEUE_QUERY_VALIDATE:
      // TRD : query_input can be NULL

      queue_internal_validate( qs, (struct validation_info *) query_input, (enum data_structure_validity *) query_output, ((enum data_structure_validity *) query_output)+1 );
    break;
  }

  return;
}

#pragma warning( default : 4100 )





/****************************************************************************/
void queue_internal_validate( struct queue_state *qs, struct validation_info *vi, enum data_structure_validity *queue_validity, enum data_structure_validity *freelist_validity )
{
  struct queue_element
    *qe,
    *qe_slow,
    *qe_fast;

  atom_t
    element_count = 0,
    total_elements;

  struct validation_info
    freelist_vi;

  assert( qs != NULL );
  // TRD : vi can be NULL
  assert( queue_validity != NULL );
  assert( freelist_validity != NULL );

  *queue_validity = VALIDITY_VALID;

  qe_slow = qe_fast = (struct queue_element *) qs->dequeue[QUEUE_POINTER];

  /* TRD : first, check for a loop
           we have two pointers
           both of which start at the dequeue end of the queue
           we enter a loop
           and on each iteration
           we advance one pointer by one element
           and the other by two

           we exit the loop when both pointers are NULL
           (have reached the end of the queue)

           or

           if we fast pointer 'sees' the slow pointer
           which means we have a loop
  */

  if( qe_slow != NULL )
    do
    {
      qe_slow = qe_slow->next[QUEUE_POINTER];

      if( qe_fast != NULL )
        qe_fast = qe_fast->next[QUEUE_POINTER];

      if( qe_fast != NULL )
        qe_fast = qe_fast->next[QUEUE_POINTER];
    }
    while( qe_slow != NULL and qe_fast != qe_slow );

  if( qe_fast != NULL and qe_slow != NULL and qe_fast == qe_slow )
    *queue_validity = VALIDITY_INVALID_LOOP;

  /* TRD : now check for expected number of elements
           vi can be NULL, in which case we do not check
           we know we don't have a loop from our earlier check
  */

  if( *queue_validity == VALIDITY_VALID and vi != NULL )
  {
    qe = (struct queue_element *) qs->dequeue[QUEUE_POINTER];

    while( qe != NULL )
    {
      element_count++;
      qe = (struct queue_element *) qe->next[QUEUE_POINTER];
    }

    /* TRD : remember there is a dummy element in the queue */
    element_count--;

    if( element_count < vi->min_elements )
      *queue_validity = VALIDITY_INVALID_MISSING_ELEMENTS;

    if( element_count > vi->max_elements )
      *queue_validity = VALIDITY_INVALID_ADDITIONAL_ELEMENTS;
  }

  /* TRD : now we validate the freelist

           we may be able to check for the expected number of
           elements in the freelist

           if the caller has given us an expected min and max
           number of elements in the queue, then the total number
           of elements in the freelist, minus that min and max,
           gives us the expected number of elements in the
           freelist
  */

  if( vi != NULL )
  {
    freelist_query( qs->fs, FREELIST_QUERY_ELEMENT_COUNT, NULL, (void *) &total_elements );

    /* TRD : remember there is a dummy element in the queue */
    total_elements--;

    freelist_vi.min_elements = total_elements - vi->max_elements;
    freelist_vi.max_elements = total_elements - vi->min_elements;

    freelist_query( qs->fs, FREELIST_QUERY_VALIDATE, (void *) &freelist_vi, (void *) freelist_validity );
  }

  if( vi == NULL )
    freelist_query( qs->fs, FREELIST_QUERY_VALIDATE, NULL, (void *) freelist_validity );

  return;
}

