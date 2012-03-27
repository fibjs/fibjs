/***** the library wide include file *****/
#include "../liblfds_internal.h"

/***** pragmas *****/

/***** defines *****/
#define QUEUE_STATE_UNKNOWN               -1
#define QUEUE_STATE_EMPTY                  0
#define QUEUE_STATE_ENQUEUE_OUT_OF_PLACE   1
#define QUEUE_STATE_ATTEMPT_DEQUEUE        2

#define QUEUE_POINTER 0
#define QUEUE_COUNTER 1
#define QUEUE_PAC_SIZE 2

/***** structures *****/
#pragma pack( push, ALIGN_DOUBLE_POINTER )

struct queue_state
{
  struct queue_element
    *volatile enqueue[QUEUE_PAC_SIZE],
    *volatile dequeue[QUEUE_PAC_SIZE];

  atom_t
    aba_counter;

  struct freelist_state
    *fs;
};

struct queue_element
{
  // TRD : next in a queue requires volatile as it is target of CAS
  struct queue_element
    *volatile next[QUEUE_PAC_SIZE];

  struct freelist_element
    *fe;

  void
    *user_data;
};

#pragma pack( pop )

/***** externs *****/

/***** private prototypes *****/
int queue_internal_freelist_init_function( void **user_data, void *user_state );
void queue_internal_freelist_delete_function( void *user_data, void *user_state );

void queue_internal_new_element_from_freelist( struct queue_state *qs, struct queue_element *qe[QUEUE_PAC_SIZE], void *user_data );
void queue_internal_guaranteed_new_element_from_freelist( struct queue_state *qs, struct queue_element * qe[QUEUE_PAC_SIZE], void *user_data );
void queue_internal_init_element( struct queue_state *qs, struct queue_element *qe[QUEUE_PAC_SIZE], struct freelist_element *fe, void *user_data );

void queue_internal_queue( struct queue_state *qs, struct queue_element *qe[QUEUE_PAC_SIZE] );

void queue_internal_validate( struct queue_state *qs, struct validation_info *vi, enum data_structure_validity *queue_validity, enum data_structure_validity *freelist_validity );

