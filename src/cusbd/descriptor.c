/**
 * @file
 * @brief See @ref descriptor.h.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-09-24
 * @copyright Copyright (c) 2025
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* Translation unit. */
#include "cusbd/descriptor.h"

/* ECU. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_FILE("cusbd/descriptor.c")

/*------------------------------------------------------------*/
/*---------------- DESCRIPTOR MEMBER FUNCTIONS ---------------*/
/*------------------------------------------------------------*/

void cusbd_descriptor_ctor(struct cusbd_descriptor *me, 
                           enum cusbd_descriptor_id id,
                           void *data,
                           uint8_t bLength)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (id > CUSBD_DESCRIPTOR_ID_RESREVED && id < CUSBD_DESCRIPTOR_ID_COUNT) );
    ECU_ASSERT( (data) );
    ECU_ASSERT( (bLength > 0) );

    me->vptr = (struct cusbd_descriptor_vtable *)0;
    ecu_ntnode_ctor(&me->ntnode, ECU_NTNODE_DESTROY_UNUSED, id);
    me->data = data;
    me->bLength = bLength;
}

uint8_t cusbd_descriptor_b_length(const struct cusbd_descriptor *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (cusbd_descriptor_valid(me)) );
    return (me->bLength);
}

void *cusbd_descriptor_data(struct cusbd_descriptor *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (cusbd_descriptor_valid(me)) );
    return (me->data);
}

const void *cusbd_descriptor_cdata(const struct cusbd_descriptor *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (cusbd_descriptor_valid(me)) );
    return (me->data);
}

enum cusbd_descriptor_id cusbd_descriptor_id(const struct cusbd_descriptor *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (cusbd_descriptor_valid(me)) );
    ecu_object_id_t id = ecu_ntnode_id(&me->ntnode);
    return ((enum cusbd_descriptor_id)id);
}

struct ecu_ntnode *cusbd_descriptor_ntnode(struct cusbd_descriptor *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (cusbd_descriptor_valid(me)) );
    struct ecu_ntnode *ntnode = &me->ntnode;
    return ntnode;
}

const struct ecu_ntnode *cusbd_descriptor_cntnode(const struct cusbd_descriptor *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (cusbd_descriptor_valid(me)) );
    const struct ecu_ntnode *cntnode = &me->ntnode;
    return cntnode;
}

bool cusbd_descriptor_valid(const struct cusbd_descriptor *me)
{
    ECU_ASSERT( (me) );
    bool status = false;
    ecu_object_id_t id = ecu_ntnode_id(&me->ntnode);

    if ((ecu_ntnode_valid(&me->ntnode)) &&
        (id > CUSBD_DESCRIPTOR_ID_RESREVED && id < CUSBD_DESCRIPTOR_ID_COUNT) &&
        (me->data) &&
        (me->bLength))
    {
        status = true;
    }

    return status;
}

size_t cusbd_descriptor_v_process_setup_packet_in(const struct cusbd_descriptor *me, 
                                                  const struct cusbd_setup_packet *packet, 
                                                  enum cusbd_descriptor_state state,
                                                  void *buffer, 
                                                  size_t buffer_len)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (me->vptr) );
    ECU_ASSERT( (me->vptr->process_setup_packet_in) );
    size_t num_bytes_written = (*me->vptr->process_setup_packet_in)(me, packet, state, buffer, buffer_len);
    return num_bytes_written;
}

bool cusbd_descriptor_v_process_setup_packet_out(struct cusbd_descriptor *me, 
                                                 const struct cusbd_setup_packet *packet,
                                                 enum cusbd_descriptor_state state)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (me->vptr) );
    ECU_ASSERT( (me->vptr->process_setup_packet_out) );
    bool status = (*me->vptr->process_setup_packet_out)(me, packet, state);
    return status;
}

size_t cusbd_descriptor_v_string_index_set(struct cusbd_descriptor *me, size_t index)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (me->vptr) );
    ECU_ASSERT( (me->vptr->string_index_set) );
    size_t new_index = (*me->vptr->string_index_set)(me, index);
    return new_index;
}

size_t cusbd_descriptor_v_w_total_length(const struct cusbd_descriptor *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (me->vptr) );
    ECU_ASSERT( (me->vptr->w_total_length) );
    size_t len = (*me->vptr->w_total_length)(me);
    return len;
}
