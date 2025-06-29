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

void cusbd_descriptor_ctor(struct cusbd_descriptor *me, uint8_t bDescriptorType)
{
    ECU_RUNTIME_ASSERT( (me) );
    ecu_ntnode_ctor(&me->ntnode, ECU_NTNODE_DESTROY_UNUSED, (ecu_object_id)bDescriptorType);
    /* Emulate pure virtual interface by setting vptr to NULL. */
    me->vptr = (const struct cusbd_descriptor_vtable *)0;
}

void cusbd_descriptor_accept(struct cusbd_descriptor *me, struct cusbd_visitor *visitor)
{
    ECU_RUNTIME_ASSERT( (me && visitor) );
    ECU_RUNTIME_ASSERT( (v_cusbd_descriptor_valid(me)) );
    struct ecu_ntnode_postorder_iterator iter;
    struct cusbd_descriptor *descriptor = (struct cusbd_descriptor *)0;

    /* Use a postorder iteration to allow safe removal of nodes. */
    ECU_NTNODE_POSTORDER_FOR_EACH(n, &iter, &me->ntnode)
    {
        descriptor = ECU_NTNODE_GET_ENTRY(n, struct cusbd_descriptor, ntnode);
        ECU_RUNTIME_ASSERT( (v_cusbd_descriptor_valid(descriptor)) );
        v_cusbd_descriptor_accept(descriptor, visitor);
    }
}

void cusbd_descriptor_caccept(const struct cusbd_descriptor *me, struct cusbd_cvisitor *visitor)
{
    ECU_RUNTIME_ASSERT( (me && visitor) );
    ECU_RUNTIME_ASSERT( (v_cusbd_descriptor_valid(me)) );
    struct ecu_ntnode_postorder_citerator citer;
    const struct cusbd_descriptor *descriptor = (const struct cusbd_descriptor *)0;

    /* Use a postorder iteration to remain consistence with cusbd_descriptor_accept(). */
    ECU_NTNODE_CONST_POSTORDER_FOR_EACH(n, &citer, &me->ntnode)
    {
        descriptor = ECU_NTNODE_GET_CONST_ENTRY(n, struct cusbd_descriptor, ntnode);
        ECU_RUNTIME_ASSERT( (v_cusbd_descriptor_valid(descriptor)) );
        v_cusbd_descriptor_caccept(descriptor, visitor);
    }
}

uint8_t cusbd_descriptor_type(const struct cusbd_descriptor *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (cusbd_descriptor_valid(me)) );
    uint8_t t = (uint8_t)ecu_ntnode_id(&me->ntnode);
    return t;
}

bool cusbd_descriptor_valid(const struct cusbd_descriptor *me)
{
    /* Do not assert vtable to allow this to be called directly on cusbd_descriptor. */
    ECU_RUNTIME_ASSERT( (me) );
    return (ecu_ntnode_valid(&me->ntnode));
}

void v_cusbd_descriptor_accept(struct cusbd_descriptor *me, struct cusbd_visitor *visitor)
{
    /* Do not assert valid() since that is centralized in the cusbd_descriptor_accept() function. */
    ECU_RUNTIME_ASSERT( (me && visitor) );
    ECU_RUNTIME_ASSERT( (me->vptr) );
    ECU_RUNTIME_ASSERT( (me->vptr->accept) );
    (*me->vptr->accept)(me, visitor);
}

void v_cusbd_descriptor_caccept(const struct cusbd_descriptor *me, struct cusbd_cvisitor *visitor)
{
    /* Do not assert valid() since that is centralized in the cusbd_descriptor_caccept() function. */
    ECU_RUNTIME_ASSERT( (me && visitor) );
    ECU_RUNTIME_ASSERT( (me->vptr) );
    ECU_RUNTIME_ASSERT( (me->vptr->caccept) );
    (*me->vptr->caccept)(me, visitor);
}

bool v_cusbd_descriptor_valid(const struct cusbd_descriptor *me)
{
    /* Do not assert valid() since this is a valid() virtual call... */
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (me->vptr) );
    ECU_RUNTIME_ASSERT( (me->vptr->valid) );
    bool status = (*me->vptr->valid)(me);
    return status;
}
