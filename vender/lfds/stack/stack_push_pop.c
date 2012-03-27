#include "stack_internal.h"





/****************************************************************************/
int stack_push( struct stack_state *ss, void *user_data )
{
  ALIGN(ALIGN_DOUBLE_POINTER) struct stack_element
    *se[STACK_PAC_SIZE];

  assert( ss != NULL );
  // TRD : user_data can be NULL

  stack_internal_new_element_from_freelist( ss, se, user_data );

  if( se[STACK_POINTER] == NULL )
    return( 0 );

  stack_internal_push( ss, se );

  return( 1 );
}





/****************************************************************************/
int stack_guaranteed_push( struct stack_state *ss, void *user_data )
{
  ALIGN(ALIGN_DOUBLE_POINTER) struct stack_element
    *se[STACK_PAC_SIZE];

  assert( ss != NULL );
  // TRD : user_data can be NULL

  /* TRD : this function allocated a new freelist element and uses that
           to push onto the stack, guaranteeing success (unless malloc()
           fails of course)
  */

  stack_internal_new_element( ss, se, user_data );

  // TRD : malloc failed
  if( se[STACK_POINTER] == NULL )
    return( 0 );

  stack_internal_push( ss, se );

  return( 1 );
}





/****************************************************************************/
void stack_internal_push( struct stack_state *ss, struct stack_element *se[STACK_PAC_SIZE] )
{
  ALIGN(ALIGN_DOUBLE_POINTER) struct stack_element
    *original_se_next[STACK_PAC_SIZE];

  assert( ss != NULL );
  assert( se != NULL );

  original_se_next[STACK_POINTER] = ss->top[STACK_POINTER];
  original_se_next[STACK_COUNTER] = ss->top[STACK_COUNTER];

  do
  {
    se[STACK_POINTER]->next[STACK_POINTER] = original_se_next[STACK_POINTER];
    se[STACK_POINTER]->next[STACK_COUNTER] = original_se_next[STACK_COUNTER];
  }
  while( 0 == abstraction_dcas((volatile atom_t *) ss->top, (atom_t *) se, (atom_t *) original_se_next) );

  return;
}





/****************************************************************************/
int stack_pop( struct stack_state *ss, void **user_data )
{
  ALIGN(ALIGN_DOUBLE_POINTER) struct stack_element
    *se[STACK_PAC_SIZE];

  assert( ss != NULL );
  assert( user_data != NULL );

  se[STACK_COUNTER] = ss->top[STACK_COUNTER];
  se[STACK_POINTER] = ss->top[STACK_POINTER];

  do
  {
    if( se[STACK_POINTER] == NULL )
      return( 0 );
  }
  while( 0 == abstraction_dcas((volatile atom_t *) ss->top, (atom_t *) se[STACK_POINTER]->next, (atom_t *) se) );

  *user_data = se[STACK_POINTER]->user_data;

  freelist_push( ss->fs, se[STACK_POINTER]->fe );

  return( 1 );
}

