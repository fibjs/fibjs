#include "ringbuffer_internal.h"





/****************************************************************************/
#pragma warning( disable : 4100 )

void ringbuffer_query( struct ringbuffer_state *rs, enum ringbuffer_query_type query_type, void *query_input, void *query_output )
{
  assert( rs != NULL );
  // TRD : query_type can be any value in its range
  // TRD : query_input can be NULL
  assert( query_output != NULL );

  switch( query_type )
  {
    case RINGBUFFER_QUERY_VALIDATE:
      // TRD : query_input can be NULL

      ringbuffer_internal_validate( rs, (struct validation_info *) query_input, (enum data_structure_validity *) query_output, ((enum data_structure_validity *) query_output)+2 );
    break;
  }

  return;
}

#pragma warning( default : 4100 )





/****************************************************************************/
void ringbuffer_internal_validate( struct ringbuffer_state *rs, struct validation_info *vi, enum data_structure_validity *queue_validity, enum data_structure_validity *freelist_validity )
{
  assert( rs != NULL );
  // TRD : vi can be NULL
  assert( queue_validity != NULL );
  assert( freelist_validity != NULL );

  queue_query( rs->qs, QUEUE_QUERY_VALIDATE, vi, queue_validity );

  if( vi != NULL )
  {
    struct validation_info
      freelist_vi;

    atom_t
      total_elements;

    freelist_query( rs->fs, FREELIST_QUERY_ELEMENT_COUNT, NULL, (void *) &total_elements );
    freelist_vi.min_elements = total_elements - vi->max_elements;
    freelist_vi.max_elements = total_elements - vi->min_elements;
    freelist_query( rs->fs, FREELIST_QUERY_VALIDATE, (void *) &freelist_vi, (void *) freelist_validity );
  }

  if( vi == NULL )
    freelist_query( rs->fs, FREELIST_QUERY_VALIDATE, NULL, (void *) freelist_validity );

  return;
}

