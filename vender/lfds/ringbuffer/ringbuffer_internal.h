/***** the library wide include file *****/
#include "../liblfds_internal.h"

/***** defines *****/

/***** structures *****/
#pragma pack( push, ALIGN_DOUBLE_POINTER )

struct ringbuffer_state
{
  struct queue_state
    *qs;

  struct freelist_state
    *fs;
};

#pragma pack( pop )

/***** externs *****/

/***** private prototypes *****/
void ringbuffer_internal_validate( struct ringbuffer_state *rs, struct validation_info *vi, enum data_structure_validity *queue_validity, enum data_structure_validity *freelist_validity );

