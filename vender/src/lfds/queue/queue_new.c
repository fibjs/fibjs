#include "queue_internal.h"





/****************************************************************************/
int queue_new( struct queue_state **qs, atom_t number_elements )
{
  int
    rv = 0;

  struct queue_element
    *qe[QUEUE_PAC_SIZE];

  assert( qs != NULL );
  // TRD : number_elements can be any value in its range

  *qs = (struct queue_state *) abstraction_aligned_malloc( sizeof(struct queue_state), ALIGN_DOUBLE_POINTER );

  if( *qs != NULL )
  {
    // TRD : the size of the freelist is the size of the queue (+1 for the leading dummy element, which is hidden from the caller)
    freelist_new( &(*qs)->fs, number_elements+1, queue_internal_freelist_init_function, NULL );

    if( (*qs)->fs != NULL )
    {
      queue_internal_new_element_from_freelist( *qs, qe, NULL );
      (*qs)->enqueue[QUEUE_POINTER] = (*qs)->dequeue[QUEUE_POINTER] = qe[QUEUE_POINTER];
      (*qs)->aba_counter = 0;
      rv = 1;
    }

    if( (*qs)->fs == NULL )
    {
      abstraction_aligned_free( *qs );
      *qs = NULL;
    }
  }

  return( rv );
}





/****************************************************************************/
#pragma warning( disable : 4100 )

int queue_internal_freelist_init_function( void **user_data, void *user_state )
{
  int
    rv = 0;

  assert( user_data != NULL );
  assert( user_state == NULL );

  *user_data = abstraction_aligned_malloc( sizeof(struct queue_element), ALIGN_DOUBLE_POINTER );

  if( *user_data != NULL )
    rv = 1;

  return( rv );
}

#pragma warning( default : 4100 )





/****************************************************************************/
void queue_internal_new_element_from_freelist( struct queue_state *qs, struct queue_element *qe[QUEUE_PAC_SIZE], void *user_data )
{
  struct freelist_element
    *fe;

  assert( qs != NULL );
  assert( qe != NULL );
  // TRD : user_data can be any value in its range

  qe[QUEUE_POINTER] = NULL;

  freelist_pop( qs->fs, &fe );

  if( fe != NULL )
    queue_internal_init_element( qs, qe, fe, user_data );

  return;
}





/****************************************************************************/
void queue_internal_guaranteed_new_element_from_freelist( struct queue_state *qs, struct queue_element *qe[QUEUE_PAC_SIZE], void *user_data )
{
  struct freelist_element
    *fe;

  assert( qs != NULL );
  assert( qe != NULL );
  // TRD : user_data can be any value in its range

  qe[QUEUE_POINTER] = NULL;

  freelist_guaranteed_pop( qs->fs, &fe );

  if( fe != NULL )
    queue_internal_init_element( qs, qe, fe, user_data );

  return;
}





/****************************************************************************/
void queue_internal_init_element( struct queue_state *qs, struct queue_element *qe[QUEUE_PAC_SIZE], struct freelist_element *fe, void *user_data )
{
  assert( qs != NULL );
  assert( qe != NULL );
  assert( fe != NULL );
  // TRD : user_data can be any value in its range

  freelist_get_user_data_from_element( fe, (void **) &qe[QUEUE_POINTER] );
  qe[QUEUE_COUNTER] = (struct queue_element *) abstraction_increment( (atom_t *) &qs->aba_counter );

  qe[QUEUE_POINTER]->next[QUEUE_POINTER] = NULL;
  qe[QUEUE_POINTER]->next[QUEUE_COUNTER] = (struct queue_element *) abstraction_increment( (atom_t *) &qs->aba_counter );

  qe[QUEUE_POINTER]->fe = fe;
  qe[QUEUE_POINTER]->user_data = user_data;

  return;
}

