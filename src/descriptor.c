/**
 * @file
 * @brief See @ref descriptor.h.
 * 
 * @author Ian Ress 
 * @version 0.1
 * @date 2025-05-28
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

ECU_ASSERT_DEFINE_NAME("cusbd/descriptor.c")

/*------------------------------------------------------------*/
/*------- CUSBD DESCRIPTOR BASE CLASS MEMBER FUNCTIONS -------*/
/*------------------------------------------------------------*/

void cusbd_descriptor_ctor(struct cusbd_descriptor *me, ecu_object_id bDescriptorType)
{
    ECU_RUNTIME_ASSERT( (me) );
    ecu_ntnode_ctor(&me->ntnode, ECU_NTNODE_DESTROY_UNUSED, bDescriptorType);
    /* Emulate pure virtual interface by setting vptr to NULL. */
    me->vptr = (const struct cusbd_descriptor_vtable *)0;
}

ecu_object_id cusbd_descriptor_type(const struct cusbd_descriptor *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return ecu_ntnode_get_id(&me->ntnode);
}

bool v_cusbd_descriptor_valid(const struct cusbd_descriptor *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (me->vptr) );
    ECU_RUNTIME_ASSERT( (me->vptr->valid) );
    return ((*me->vptr->valid)(me));
}

uint16_t v_cusbd_descriptor_wTotalLength(const struct cusbd_descriptor *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (me->vptr) );
    ECU_RUNTIME_ASSERT( (me->vptr->wTotalLength) );
    return ((*me->vptr->wTotalLength)(me));
}

enum cusbd_request_status v_cusbd_descriptor_default_state_in(struct cusbd_descriptor *me,
                                                              const struct cusbd_request *request,
                                                              void *buf,
                                                              size_t len)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (me->vptr) );
    ECU_RUNTIME_ASSERT( (me->vptr->default_state_in) );
    return ((*me->vptr->default_state_in)(me, request, buf, len));
}

enum cusbd_request_status v_cusbd_descriptor_default_state_out(struct cusbd_descriptor *me,
                                                               const struct cusbd_request *request)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (me->vptr) );
    ECU_RUNTIME_ASSERT( (me->vptr->default_state_out) );
    return ((*me->vptr->default_state_out)(me, request));
}

enum cusbd_request_status v_cusbd_descriptor_address_state_in(struct cusbd_descriptor *me,
                                                              const struct cusbd_request *request,
                                                              void *buf,
                                                              size_t len)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (me->vptr) );
    ECU_RUNTIME_ASSERT( (me->vptr->address_state_in) );
    return ((*me->vptr->address_state_in)(me, request, buf, len));
}

enum cusbd_request_status v_cusbd_descriptor_address_state_out(struct cusbd_descriptor *me,
                                                               const struct cusbd_request *request)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (me->vptr) );
    ECU_RUNTIME_ASSERT( (me->vptr->address_state_out) );
    return ((*me->vptr->address_state_out)(me, request));
}

enum cusbd_request_status v_cusbd_descriptor_configured_state_in(struct cusbd_descriptor *me,
                                                                 const struct cusbd_request *request,
                                                                 void *buf,
                                                                 size_t len)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (me->vptr) );
    ECU_RUNTIME_ASSERT( (me->vptr->configured_state_in) );
    return ((*me->vptr->configured_state_in)(me, request, buf, len));
}

enum cusbd_request_status v_cusbd_descriptor_configured_state_out(struct cusbd_descriptor *me,
                                                                  const struct cusbd_request *request)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (me->vptr) );
    ECU_RUNTIME_ASSERT( (me->vptr->configured_state_out) );
    return ((*me->vptr->configured_state_out)(me, request));
}
