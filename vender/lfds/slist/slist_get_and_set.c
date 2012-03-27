#include "slist_internal.h"





/****************************************************************************/
int slist_get_user_data_from_element( struct slist_element *se, void **user_data )
{
  int
    rv = 1;

  assert( se != NULL );
  assert( user_data != NULL );

  *user_data = (void *) se->user_data_and_flags[SLIST_USER_DATA];

  if( (atom_t) se->user_data_and_flags[SLIST_FLAGS] & SLIST_FLAG_DELETED )
    rv = 0;

  return( rv );
}





/****************************************************************************/
int slist_set_user_data_in_element( struct slist_element *se, void *user_data )
{
  ALIGN(ALIGN_DOUBLE_POINTER) volatile void
    *user_data_and_flags[2],
    *new_user_data_and_flags[2];

  int
    rv = 1;

  assert( se != NULL );
  // TRD : user_data can be NULL

  user_data_and_flags[SLIST_USER_DATA] = se->user_data_and_flags[SLIST_USER_DATA];
  user_data_and_flags[SLIST_FLAGS] = se->user_data_and_flags[SLIST_FLAGS];

  new_user_data_and_flags[SLIST_USER_DATA] = user_data;

  do
  {
    new_user_data_and_flags[SLIST_FLAGS] = user_data_and_flags[SLIST_FLAGS];
  }
  while( !((atom_t) user_data_and_flags[SLIST_FLAGS] & SLIST_FLAG_DELETED) and 0 == abstraction_dcas((volatile atom_t *) se->user_data_and_flags, (atom_t *) new_user_data_and_flags, (atom_t *) user_data_and_flags) );

  if( (atom_t) user_data_and_flags[SLIST_FLAGS] & SLIST_FLAG_DELETED )
    rv = 0;

  return( rv );
}





/****************************************************************************/
struct slist_element *slist_get_head( struct slist_state *ss, struct slist_element **se )
{
  assert( ss != NULL );
  assert( se != NULL );

  *se = (struct slist_element *) ss->head;

  slist_internal_move_to_first_undeleted_element( se );

  return( *se );
}





/****************************************************************************/
struct slist_element *slist_get_next( struct slist_element *se, struct slist_element **next_se )
{
  assert( se != NULL );
  assert( next_se != NULL );

  *next_se = (struct slist_element *) se->next;

  slist_internal_move_to_first_undeleted_element( next_se );

  return( *next_se );
}





/****************************************************************************/
struct slist_element *slist_get_head_and_then_next( struct slist_state *ss, struct slist_element **se )
{
  assert( ss != NULL );
  assert( se != NULL );

  if( *se == NULL )
    slist_get_head( ss, se );
  else
    slist_get_next( *se, se );

  return( *se );
}





/****************************************************************************/
void slist_internal_move_to_first_undeleted_element( struct slist_element **se )
{
  assert( se != NULL );

  while( *se != NULL and (atom_t) (*se)->user_data_and_flags[SLIST_FLAGS] & SLIST_FLAG_DELETED )
    (*se) = (struct slist_element *) (*se)->next;

  return;
}

