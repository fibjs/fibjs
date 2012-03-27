#include "stack_internal.h"





/****************************************************************************/
void stack_delete( struct stack_state *ss, void (*user_data_delete_function)(void *user_data, void *user_state), void *user_state )
{
  void
    *user_data;

  assert( ss != NULL );
  // TRD : user_data_delete_function can be NULL
  // TRD : user_state can be NULL

  while( stack_pop(ss, &user_data) )
    if( user_data_delete_function != NULL )
      user_data_delete_function( user_data, user_state );

  freelist_delete( ss->fs, stack_internal_freelist_delete_function, NULL );

  abstraction_aligned_free( ss );

  return;
}





/****************************************************************************/
void stack_clear( struct stack_state *ss, void (*user_data_clear_function)(void *user_data, void *user_state), void *user_state )
{
  void
    *user_data;

  assert( ss != NULL );
  // TRD : user_data_clear_function can be NULL
  // TRD : user_state can be NULL

  while( stack_pop(ss, &user_data) )
    if( user_data_clear_function != NULL )
      user_data_clear_function( user_data, user_state );

  return;
}





/****************************************************************************/
#pragma warning( disable : 4100 )

void stack_internal_freelist_delete_function( void *user_data, void *user_state )
{
  assert( user_data != NULL );
  assert( user_state == NULL );

  abstraction_aligned_free( user_data );

  return;
}

#pragma warning( default : 4100 )

