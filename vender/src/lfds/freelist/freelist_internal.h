/***** the library wide include file *****/
#include "../liblfds_internal.h"

/***** defines *****/
#define FREELIST_POINTER 0
#define FREELIST_COUNTER 1
#define FREELIST_PAC_SIZE 2

/***** structures *****/
#pragma pack( push, ALIGN_DOUBLE_POINTER )

struct freelist_state
{
  struct freelist_element
    *volatile top[FREELIST_PAC_SIZE];

  int
    (*user_data_init_function)( void **user_data, void *user_state );

  void
    *user_state;

  atom_t
    aba_counter,
    element_count;
};

struct freelist_element
{
  struct freelist_element
    *next[FREELIST_PAC_SIZE];

  void
    *user_data;
};

#pragma pack( pop )

/***** private prototypes *****/
atom_t freelist_internal_new_element( struct freelist_state *fs, struct freelist_element **fe );
void freelist_internal_validate( struct freelist_state *fs, struct validation_info *vi, enum data_structure_validity *freelist_validity );

