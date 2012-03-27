#include "stack_internal.h"





/****************************************************************************/
int stack_new( struct stack_state **ss, atom_t number_elements )
{
  int
    rv = 0;

  assert( ss != NULL );
  // TRD : number_elements can be any value in its range

  *ss = (struct stack_state *) abstraction_aligned_malloc( sizeof(struct stack_state), ALIGN_DOUBLE_POINTER );

  if( *ss != NULL )
  {
    // TRD : the size of the freelist is the size of the stack
    freelist_new( &(*ss)->fs, number_elements, stack_internal_freelist_init_function, NULL );

    if( (*ss)->fs == NULL )
    {
      abstraction_aligned_free( *ss );
      *ss = NULL;
    }

    if( (*ss)->fs != NULL )
    {
      (*ss)->top[STACK_POINTER] = NULL;
      (*ss)->top[STACK_COUNTER] = 0;
      (*ss)->aba_counter = 0;
      rv = 1;
    }
  }

  return( rv );
}





/****************************************************************************/
#pragma warning( disable : 4100 )

int stack_internal_freelist_init_function( void **user_data, void *user_state )
{
  int
    rv = 0;

  assert( user_data != NULL );
  assert( user_state == NULL );

  *user_data = abstraction_aligned_malloc( sizeof(struct stack_element), ALIGN_DOUBLE_POINTER );

  if( *user_data != NULL )
    rv = 1;

  return( rv );
}

#pragma warning( default : 4100 )





/****************************************************************************/
void stack_internal_new_element_from_freelist( struct stack_state *ss, struct stack_element *se[STACK_PAC_SIZE], void *user_data )
{
  struct freelist_element
    *fe;

  assert( ss != NULL );
  assert( se != NULL );
  // TRD : user_data can be any value in its range

  freelist_pop( ss->fs, &fe );

  if( fe == NULL )
    se[STACK_POINTER] = NULL;

  if( fe != NULL )
    stack_internal_init_element( ss, se, fe, user_data );

  return;
}





/****************************************************************************/
void stack_internal_new_element( struct stack_state *ss, struct stack_element *se[STACK_PAC_SIZE], void *user_data )
{
  struct freelist_element
    *fe;

  assert( ss != NULL );
  assert( se != NULL );
  // TRD : user_data can be any value in its range

  freelist_guaranteed_pop( ss->fs, &fe );

  if( fe == NULL )
    se[STACK_POINTER] = NULL;

  if( fe != NULL )
    stack_internal_init_element( ss, se, fe, user_data );

  return;
}





/****************************************************************************/
void stack_internal_init_element( struct stack_state *ss, struct stack_element *se[STACK_PAC_SIZE], struct freelist_element *fe, void *user_data )
{
  assert( ss != NULL );
  assert( se != NULL );
  assert( fe != NULL );
  // TRD : user_data can be any value in its range

  freelist_get_user_data_from_element( fe, (void **) &se[STACK_POINTER] );

  se[STACK_COUNTER] = (struct stack_element *) abstraction_increment( (atom_t *) &ss->aba_counter );

  se[STACK_POINTER]->next[STACK_POINTER] = NULL;
  se[STACK_POINTER]->next[STACK_COUNTER] = 0;
  se[STACK_POINTER]->fe = fe;
  se[STACK_POINTER]->user_data = user_data;

  return;
}

