#include "freelist_internal.h"





/****************************************************************************/
void freelist_query( struct freelist_state *fs, enum freelist_query_type query_type, void *query_input, void *query_output )
{
  assert( fs != NULL );
  // TRD : query type can be any value in its range
  // TRD : query_input can be NULL in some cases
  assert( query_output != NULL );

  switch( query_type )
  {
    case FREELIST_QUERY_ELEMENT_COUNT:
      assert( query_input == NULL );

      *(atom_t *) query_output = fs->element_count;
    break;

    case FREELIST_QUERY_VALIDATE:
      // TRD : query_input can be NULL

      freelist_internal_validate( fs, (struct validation_info *) query_input, (enum data_structure_validity *) query_output );
    break;
  }

  return;
}





/****************************************************************************/
void freelist_internal_validate( struct freelist_state *fs, struct validation_info *vi, enum data_structure_validity *freelist_validity )
{
  struct freelist_element
    *fe,
    *fe_slow,
    *fe_fast;

  atom_t
    element_count = 0;

  assert( fs != NULL );
  // TRD : vi can be NULL
  assert( freelist_validity != NULL );

  *freelist_validity = VALIDITY_VALID;

  fe_slow = fe_fast = (struct freelist_element *) fs->top[FREELIST_POINTER];

  /* TRD : first, check for a loop
           we have two pointers
           both of which start at the top of the freelist
           we enter a loop
           and on each iteration
           we advance one pointer by one element
           and the other by two

           we exit the loop when both pointers are NULL
           (have reached the end of the freelist)

           or

           if we fast pointer 'sees' the slow pointer
           which means we have a loop
  */

  if( fe_slow != NULL )
    do
    {
      fe_slow = fe_slow->next[FREELIST_POINTER];

      if( fe_fast != NULL )
        fe_fast = fe_fast->next[FREELIST_POINTER];

      if( fe_fast != NULL )
        fe_fast = fe_fast->next[FREELIST_POINTER];
    }
    while( fe_slow != NULL and fe_fast != fe_slow );

  if( fe_fast != NULL and fe_slow != NULL and fe_fast == fe_slow )
    *freelist_validity = VALIDITY_INVALID_LOOP;

  /* TRD : now check for expected number of elements
           vi can be NULL, in which case we do not check
           we know we don't have a loop from our earlier check
  */

  if( *freelist_validity == VALIDITY_VALID and vi != NULL )
  {
    fe = (struct freelist_element *) fs->top[FREELIST_POINTER];

    while( fe != NULL )
    {
      element_count++;
      fe = (struct freelist_element *) fe->next[FREELIST_POINTER];
    }

    if( element_count < vi->min_elements )
      *freelist_validity = VALIDITY_INVALID_MISSING_ELEMENTS;

    if( element_count > vi->max_elements )
      *freelist_validity = VALIDITY_INVALID_ADDITIONAL_ELEMENTS;
  }

  return;
}

