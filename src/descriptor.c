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
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Acts as pure virtual function by simply asserting.
 * Must be overridden by derived classes.
 */
static bool valid(const struct cusbd_descriptor *me);

/**
 * @brief Acts as pure virtual function by simply asserting.
 * Must be overridden by derived classes.
 */
static size_t wTotalLength(const struct cusbd_descriptor *me);

/**
 * @brief Acts as pure virtual function by simply asserting.
 * Must be overridden by derived classes.
 */
static bool process_std_request(struct cusbd_descriptor *me, 
                                const struct cusbd_std_request *request,
                                enum cusbd_state state, 
                                void *buf, 
                                size_t len);

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
/*------------------------------------------------------------*/

static bool valid(const struct cusbd_descriptor *me)
{
    (void)me;
    ECU_RUNTIME_ASSERT( (false) );
    return false;
}

static size_t wTotalLength(const struct cusbd_descriptor *me)
{
    (void)me;
    ECU_RUNTIME_ASSERT( (false) );
}

static bool process_std_request(struct cusbd_descriptor *me, 
                                const struct cusbd_std_request *request,
                                enum cusbd_state state, 
                                void *buf, 
                                size_t len)
{
    (void)me;
    (void)request;
    (void)state;
    (void)buf;
    (void)len;
    ECU_RUNTIME_ASSERT( (false) );
    return false;
}

/*------------------------------------------------------------*/
/*------- CUSBD DESCRIPTOR BASE CLASS MEMBER FUNCTIONS -------*/
/*------------------------------------------------------------*/

void cusbd_descriptor_ctor(struct cusbd_descriptor *me, ecu_object_id bDescriptorType)
{
    static const struct cusbd_descriptor_vtable vtable = CUSBD_DESCRIPTOR_VTABLE_CTOR(
        &valid, &wTotalLength, &process_std_request
    );

    ECU_RUNTIME_ASSERT( (me) );
    ecu_ntnode_ctor(&me->ntnode, ECU_NTNODE_DESTROY_UNUSED, bDescriptorType);
    me->vptr = &vtable;
}

ecu_object_id cusbd_descriptor_type(const struct cusbd_descriptor *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return ecu_ntnode_get_id(&me->ntnode);
}

bool cusbd_descriptor_vvalid(const struct cusbd_descriptor *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (me->vptr) );
    ECU_RUNTIME_ASSERT( (me->vptr->valid) );
    return ((*me->vptr->valid)(me));
}

size_t cusbd_descriptor_vwTotalLength(const struct cusbd_descriptor *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (me->vptr) );
    ECU_RUNTIME_ASSERT( (me->vptr->wTotalLength) );
    return ((*me->vptr->wTotalLength)(me));
}

bool cusbd_descriptor_vprocess_std_request(struct cusbd_descriptor *me, 
                                           const struct cusbd_std_request *request,
                                           enum cusbd_state state, 
                                           void *buf, 
                                           size_t len)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (me->vptr) );
    ECU_RUNTIME_ASSERT( (me->vptr->process_std_request) );
    return ((*me->vptr->process_std_request)(me, request, state, buf, len));
}
