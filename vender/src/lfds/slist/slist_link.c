#include "slist_internal.h"





/****************************************************************************/
void slist_internal_link_element_to_head( struct slist_state *ss, struct slist_element *volatile se )
{
  ALIGN(ALIGN_SINGLE_POINTER) struct slist_element
    *se_next;

  assert( ss != NULL );
  assert( se != NULL );

  se_next = ss->head;

  do
  {
    se->next = se_next;
  }
  while( se->next != (se_next = (struct slist_element *) abstraction_cas((volatile atom_t *) &ss->head, (atom_t) se, (atom_t) se->next)) );

  return;
}





/****************************************************************************/
void slist_internal_link_element_after_element( struct slist_element *volatile slist_in_list_element, struct slist_element *volatile se )
{
  ALIGN(ALIGN_SINGLE_POINTER) struct slist_element
    *se_prev,
    *se_next;

  assert( slist_in_list_element != NULL );
  assert( se != NULL );

  se_prev = (struct slist_element *) slist_in_list_element;

  se_next = se_prev->next;

  do
  {
    se->next = se_next;
  }
  while( se->next != (se_next = (struct slist_element *) abstraction_cas((volatile atom_t *) &se_prev->next, (atom_t) se, (atom_t) se->next)) );

  return;
}

