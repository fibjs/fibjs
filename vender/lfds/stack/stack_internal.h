/***** the library wide include file *****/
#include "../liblfds_internal.h"

/***** pragmas *****/

/***** defines *****/
#define STACK_POINTER 0
#define STACK_COUNTER 1
#define STACK_PAC_SIZE 2

/***** structures *****/
#pragma pack( push, ALIGN_DOUBLE_POINTER )

struct stack_state
{
  // TRD : must come first for alignment
  struct stack_element
    *volatile top[STACK_PAC_SIZE];

  atom_t
    aba_counter;

  struct freelist_state
    *fs;
};

struct stack_element
{
  struct stack_element
    *next[STACK_PAC_SIZE];

  struct freelist_element
    *fe;

  void
    *user_data;
};

#pragma pack( pop )

/***** private prototypes *****/
int stack_internal_freelist_init_function( void **user_data, void *user_state );
void stack_internal_freelist_delete_function( void *user_data, void *user_state );

void stack_internal_new_element_from_freelist( struct stack_state *ss, struct stack_element *se[STACK_PAC_SIZE], void *user_data );
void stack_internal_new_element( struct stack_state *ss, struct stack_element *se[STACK_PAC_SIZE], void *user_data );
void stack_internal_init_element( struct stack_state *ss, struct stack_element *se[STACK_PAC_SIZE], struct freelist_element *fe, void *user_data );

void stack_internal_push( struct stack_state *ss, struct stack_element *se[STACK_PAC_SIZE] );

