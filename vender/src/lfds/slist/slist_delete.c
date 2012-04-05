#include "slist_internal.h"





/****************************************************************************/
void slist_delete( struct slist_state *ss )
{
  slist_delete_all_elements( ss );

  abstraction_aligned_free( ss );

  return;
}





/****************************************************************************/
void slist_delete_element( struct slist_state *ss, struct slist_element *se )
{
  ALIGN(ALIGN_DOUBLE_POINTER) void
    *volatile user_data_and_flags[2],
    *volatile new_user_data_and_flags[2];

  unsigned char
    cas_rv = 0;

  assert( ss != NULL );
  assert( se != NULL );

  user_data_and_flags[SLIST_USER_DATA] = se->user_data_and_flags[SLIST_USER_DATA];
  user_data_and_flags[SLIST_FLAGS] = se->user_data_and_flags[SLIST_FLAGS];

  do
  {
    new_user_data_and_flags[SLIST_USER_DATA] = user_data_and_flags[SLIST_USER_DATA];
    new_user_data_and_flags[SLIST_FLAGS] = (void *) ((atom_t) user_data_and_flags[SLIST_FLAGS] | SLIST_FLAG_DELETED);
  }
  while( !((atom_t) user_data_and_flags[SLIST_FLAGS] & SLIST_FLAG_DELETED) and 0 == (cas_rv = abstraction_dcas((volatile atom_t *) se->user_data_and_flags, (atom_t *) new_user_data_and_flags, (atom_t *) user_data_and_flags)) );

  if( cas_rv == 1 )
    if( ss->user_data_delete_function != NULL )
      ss->user_data_delete_function( (void *) user_data_and_flags[SLIST_USER_DATA], ss->user_state );

  return;
}





/****************************************************************************/
void slist_delete_all_elements( struct slist_state *ss )
{
  struct slist_element
    *volatile se,
    *volatile se_temp;

  se = ss->head;

  while( se != NULL )
  {
    // TRD : if a non-deleted element and there is a delete function, call the delete function
    if( ss->user_data_delete_function != NULL )
      ss->user_data_delete_function( (void *) se->user_data_and_flags[SLIST_USER_DATA], ss->user_state );

    se_temp = se;
    se = se->next;
    abstraction_aligned_free( (void *) se_temp );
  }

  slist_internal_init_slist( ss, ss->user_data_delete_function, ss->user_state );

  return;
}

