#include "queue_internal.h"





/****************************************************************************/
int queue_enqueue( struct queue_state *qs, void *user_data )
{
  ALIGN(ALIGN_DOUBLE_POINTER) struct queue_element
    *qe[QUEUE_PAC_SIZE];

  assert( qs != NULL );
  // TRD : user_data can be NULL

  queue_internal_new_element_from_freelist( qs, qe, user_data );

  if( qe[QUEUE_POINTER] == NULL )
    return( 0 );

  queue_internal_queue( qs, qe );

  return( 1 );
}





/****************************************************************************/
int queue_guaranteed_enqueue( struct queue_state *qs, void *user_data )
{
  ALIGN(ALIGN_DOUBLE_POINTER) struct queue_element
    *qe[QUEUE_PAC_SIZE];

  assert( qs != NULL );
  // TRD : user_data can be NULL

  queue_internal_guaranteed_new_element_from_freelist( qs, qe, user_data );

  if( qe[QUEUE_POINTER] == NULL )
    return( 0 );

  queue_internal_queue( qs, qe );

  return( 1 );
}





/****************************************************************************/
void queue_internal_queue( struct queue_state *qs, struct queue_element *qe[QUEUE_PAC_SIZE] )
{
  ALIGN(ALIGN_DOUBLE_POINTER) struct queue_element
    *enqueue[QUEUE_PAC_SIZE],
    *next[QUEUE_PAC_SIZE];

  unsigned char
    cas_result = 0;

  assert( qs != NULL );
  assert( qe != NULL );

  do
  {
    enqueue[QUEUE_POINTER] = qs->enqueue[QUEUE_POINTER];
    enqueue[QUEUE_COUNTER] = qs->enqueue[QUEUE_COUNTER];

    next[QUEUE_POINTER] = enqueue[QUEUE_POINTER]->next[QUEUE_POINTER];
    next[QUEUE_COUNTER] = enqueue[QUEUE_POINTER]->next[QUEUE_COUNTER];

    /* TRD : this if() ensures that the next we read, just above,
             really is from qs->enqueue (which we copied into enqueue)
    */

    if( qs->enqueue[QUEUE_POINTER] == enqueue[QUEUE_POINTER] and qs->enqueue[QUEUE_COUNTER] == enqueue[QUEUE_COUNTER] )
    {
      if( next[QUEUE_POINTER] == NULL )
      {
        qe[QUEUE_COUNTER] = next[QUEUE_COUNTER] + 1;
        cas_result = abstraction_dcas( (volatile atom_t *) enqueue[QUEUE_POINTER]->next, (atom_t *) qe, (atom_t *) next );
      }
      else
      {
        next[QUEUE_COUNTER] = enqueue[QUEUE_COUNTER] + 1;
        abstraction_dcas( (volatile atom_t *) qs->enqueue, (atom_t *) next, (atom_t *) enqueue );
      }
    }
  }
  while( cas_result == 0 );

  qe[QUEUE_COUNTER] = enqueue[QUEUE_COUNTER] + 1;
  abstraction_dcas( (volatile atom_t *) qs->enqueue, (atom_t *) qe, (atom_t *) enqueue );

  return;
}





/****************************************************************************/
int queue_dequeue( struct queue_state *qs, void **user_data )
{
  ALIGN(ALIGN_DOUBLE_POINTER) struct queue_element
    *enqueue[QUEUE_PAC_SIZE],
    *dequeue[QUEUE_PAC_SIZE],
    *next[QUEUE_PAC_SIZE];

  unsigned char
    cas_result = 0;

  int
    rv = 1,
    state = QUEUE_STATE_UNKNOWN,
    finished_flag = LOWERED;

  assert( qs != NULL );
  assert( user_data != NULL );

  do
  {
    dequeue[QUEUE_POINTER] = qs->dequeue[QUEUE_POINTER];
    dequeue[QUEUE_COUNTER] = qs->dequeue[QUEUE_COUNTER];

    enqueue[QUEUE_POINTER] = qs->enqueue[QUEUE_POINTER];
    enqueue[QUEUE_COUNTER] = qs->enqueue[QUEUE_COUNTER];

    next[QUEUE_POINTER] = dequeue[QUEUE_POINTER]->next[QUEUE_POINTER];
    next[QUEUE_COUNTER] = dequeue[QUEUE_POINTER]->next[QUEUE_COUNTER];

    /* TRD : confirm that dequeue didn't move between reading it
             and reading its next pointer
    */

    if( dequeue[QUEUE_POINTER] == qs->dequeue[QUEUE_POINTER] and dequeue[QUEUE_COUNTER] == qs->dequeue[QUEUE_COUNTER] )
    {
      if( enqueue[QUEUE_POINTER] == dequeue[QUEUE_POINTER] and next[QUEUE_POINTER] == NULL )
        state = QUEUE_STATE_EMPTY;

      if( enqueue[QUEUE_POINTER] == dequeue[QUEUE_POINTER] and next[QUEUE_POINTER] != NULL )
        state = QUEUE_STATE_ENQUEUE_OUT_OF_PLACE;

      if( enqueue[QUEUE_POINTER] != dequeue[QUEUE_POINTER] )
        state = QUEUE_STATE_ATTEMPT_DEQUEUE;

      switch( state )
      {
        case QUEUE_STATE_EMPTY:
          *user_data = NULL;
          rv = 0;
          finished_flag = RAISED;
        break;

        case QUEUE_STATE_ENQUEUE_OUT_OF_PLACE:
          next[QUEUE_COUNTER] = enqueue[QUEUE_COUNTER] + 1;
          abstraction_dcas( (volatile atom_t *) qs->enqueue, (atom_t *) next, (atom_t *) enqueue );
        break;

        case QUEUE_STATE_ATTEMPT_DEQUEUE:
          *user_data = next[QUEUE_POINTER]->user_data;

          next[QUEUE_COUNTER] = dequeue[QUEUE_COUNTER] + 1;
          cas_result = abstraction_dcas( (volatile atom_t *) qs->dequeue, (atom_t *) next, (atom_t *) dequeue );

          if( cas_result == 1 )
            finished_flag = RAISED;
        break;
      }
    }
  }
  while( finished_flag == LOWERED );

  if( cas_result == 1 )
    freelist_push( qs->fs, dequeue[QUEUE_POINTER]->fe );

  return( rv );
}

