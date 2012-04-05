/***** the library wide include file *****/
#include "../liblfds_internal.h"

/***** defines *****/
#define SLIST_USER_DATA  0
#define SLIST_FLAGS      1

#define SLIST_NO_FLAGS      0x0
#define SLIST_FLAG_DELETED  0x1

/***** structures *****/
#pragma pack( push, ALIGN_SINGLE_POINTER )

struct slist_state
{
  struct slist_element
    *volatile head;

  void
    (*user_data_delete_function)( void *user_data, void *user_state ),
    *user_state;
};

#pragma pack( pop )

#pragma pack( push, ALIGN_DOUBLE_POINTER )

/* TRD : this pragma pack doesn't seem to work under Windows
         if the structure members are the correct way round
         (next first), then user_data_and_flags ends up on
         a single pointer boundary and DCAS crashes

         accordingly, I've moved user_data_and_flags first
*/

struct slist_element
{
  void
    *volatile user_data_and_flags[2];

  // TRD : requires volatile as is target of CAS
  struct slist_element
    *volatile next;
};

#pragma pack( pop )

/***** private prototypes *****/
void slist_internal_init_slist( struct slist_state *ss, void (*user_data_delete_function)(void *user_data, void *user_state), void *user_state );

void slist_internal_link_element_to_head( struct slist_state *slist_state, struct slist_element *volatile se );
void slist_internal_link_element_after_element( struct slist_element *volatile slist_in_list_element, struct slist_element *volatile se );

void slist_internal_move_to_first_undeleted_element( struct slist_element **se );

