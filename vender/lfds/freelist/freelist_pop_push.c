#include "freelist_internal.h"





/****************************************************************************/
struct freelist_element *freelist_pop( struct freelist_state *fs, struct freelist_element **fe )
{
  ALIGN(ALIGN_DOUBLE_POINTER) struct freelist_element
    *fe_local[FREELIST_PAC_SIZE];

  assert( fs != NULL );
  assert( fe != NULL );

  fe_local[FREELIST_COUNTER] = fs->top[FREELIST_COUNTER];
  fe_local[FREELIST_POINTER] = fs->top[FREELIST_POINTER];

  /* TRD : note that abstraction_dcas loads the original value of the destination (fs->top) into the compare (fe_local)
           (this happens of course after the CAS itself has occurred inside abstraction_dcas)
  */

  do
  {
    if( fe_local[FREELIST_POINTER] == NULL )
    {
      *fe = NULL;
      return( *fe );
    }
  }
  while( 0 == abstraction_dcas((volatile atom_t *) fs->top, (atom_t *) fe_local[FREELIST_POINTER]->next, (atom_t *) fe_local) );

  *fe = (struct freelist_element *) fe_local[FREELIST_POINTER];

  return( *fe );
}





/****************************************************************************/
struct freelist_element *freelist_guaranteed_pop( struct freelist_state *fs, struct freelist_element **fe )
{
  assert( fs != NULL );
  assert( fe != NULL );

  freelist_internal_new_element( fs, fe );

  return( *fe );
}





/****************************************************************************/
void freelist_push( struct freelist_state *fs, struct freelist_element *fe )
{
  ALIGN(ALIGN_DOUBLE_POINTER) struct freelist_element
    *fe_local[FREELIST_PAC_SIZE],
    *original_fe_next[FREELIST_PAC_SIZE];

  assert( fs != NULL );
  assert( fe != NULL );

  fe_local[FREELIST_POINTER] = fe;
  fe_local[FREELIST_COUNTER] = (struct freelist_element *) abstraction_increment( (atom_t *) &fs->aba_counter );

  original_fe_next[FREELIST_POINTER] = fs->top[FREELIST_POINTER];
  original_fe_next[FREELIST_COUNTER] = fs->top[FREELIST_COUNTER];

  /* TRD : note that abstraction_dcas loads the original value of the destination (fs->top) into the compare (original_fe_next)
           (this happens of course after the CAS itself has occurred inside abstraction_dcas)
           this then causes us in our loop, should we repeat it, to update fe_local->next to a more
           up-to-date version of the head of the freelist
  */

  do
  {
    fe_local[FREELIST_POINTER]->next[FREELIST_POINTER] = original_fe_next[FREELIST_POINTER];
    fe_local[FREELIST_POINTER]->next[FREELIST_COUNTER] = original_fe_next[FREELIST_COUNTER];
  }
  while( 0 == abstraction_dcas((volatile atom_t *) fs->top, (atom_t *) fe_local, (atom_t *) original_fe_next) );

  return;
}

