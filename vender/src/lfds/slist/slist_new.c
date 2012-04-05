#include "slist_internal.h"





/****************************************************************************/
int slist_new( struct slist_state **ss, void (*user_data_delete_function)(void *user_data, void *user_state), void *user_state )
{
  int
    rv = 0;

  assert( ss != NULL );
  // TRD : user_data_delete_function can be NULL
  // TRD : user_state can be NULL

  *ss = (struct slist_state *) abstraction_aligned_malloc( sizeof(struct slist_state), ALIGN_SINGLE_POINTER );

  if( *ss != NULL )
  {
    slist_internal_init_slist( *ss, user_data_delete_function, user_state );
    rv = 1;
  }

  return( rv );
}





/****************************************************************************/
void slist_internal_init_slist( struct slist_state *ss, void (*user_data_delete_function)(void *user_data, void *user_state), void *user_state )
{
  assert( ss != NULL );
  // TRD : user_data_delete_function can be NULL
  // TRD : user_state can be NULL

  ss->head = NULL;
  ss->user_data_delete_function = user_data_delete_function;
  ss->user_state = user_state;

  return;
}





/****************************************************************************/
struct slist_element *slist_new_head( struct slist_state *ss, void *user_data )
{
  ALIGN(ALIGN_SINGLE_POINTER) struct slist_element
    *volatile se;

  assert( ss != NULL );
  // TRD : user_data can be NULL

  se = (struct slist_element *) abstraction_aligned_malloc( sizeof(struct slist_element), ALIGN_DOUBLE_POINTER );

  if( se != NULL )
  {
    se->user_data_and_flags[SLIST_USER_DATA] = user_data;
    se->user_data_and_flags[SLIST_FLAGS] = SLIST_NO_FLAGS;

    slist_internal_link_element_to_head( ss, se );
  }

  return( (struct slist_element *) se );
}





/****************************************************************************/
struct slist_element *slist_new_next( struct slist_element *se, void *user_data )
{
  ALIGN(ALIGN_SINGLE_POINTER) struct slist_element
    *volatile se_next;

  assert( se != NULL );
  // TRD : user_data can be NULL

  se_next = (struct slist_element *) abstraction_aligned_malloc( sizeof(struct slist_element), ALIGN_DOUBLE_POINTER );

  if( se_next != NULL )
  {
    se_next->user_data_and_flags[SLIST_USER_DATA] = user_data;
    se_next->user_data_and_flags[SLIST_FLAGS] = SLIST_NO_FLAGS;

    slist_internal_link_element_after_element( se, se_next );
  }

  return( (struct slist_element *) se_next );
}

