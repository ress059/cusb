/**
 * @file
 * @brief See @ref interface.h.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-05-16
 * @copyright Copyright (c) 2025
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* Translation unit. */
#include "cusbd/interface.h"

/* STDLib. */
#include <string.h> /* memcpy. */

/* CUSB. */
#include "cusbd/visitor/visitor.h"

/* ECU. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_NAME("cusbd/interface.c")

/*------------------------------------------------------------*/
/*---------- STATIC FUNCTION DECLARATIONS - INTERFACE --------*/
/*------------------------------------------------------------*/

/**
 * @brief Returns true if the supplied interface descriptor contains
 * valid data and was properly constructed via @ref CUSBD_INTERFACE_DESCRIPTOR_CTOR(). 
 * False otherwise.
 * 
 * @param descriptor Descriptor to check.
 */
static bool interface_descriptor_valid(const struct cusbd_interface_descriptor *descriptor);

/**
 * @brief Override of @ref v_cusbd_descriptor_accept().
 */
static void o_interface_accept(struct cusbd_interface *me, struct cusbd_visitor *visitor);

/**
 * @brief Override of @ref v_cusbd_descriptor_caccept().
 */
static void o_interface_caccept(const struct cusbd_interface *me, struct cusbd_cvisitor *visitor);

/*------------------------------------------------------------*/
/*---- STATIC FUNCTION DECLARATIONS - ALTERNATE INTERFACE ----*/
/*------------------------------------------------------------*/

/**
 * @brief Returns true if the supplied interface descriptor contains
 * valid data and was properly constructed via @ref CUSBD_ALTERNATE_INTERFACE_DESCRIPTOR_CTOR(). 
 * False otherwise.
 * 
 * @param descriptor Descriptor to check.
 */
static bool alternate_interface_descriptor_valid(const struct cusbd_interface_descriptor *descriptor);

/**
 * @brief Override of @ref v_cusbd_descriptor_accept().
 */
static void o_alternate_interface_accept(struct cusbd_alternate_interface *me, 
                                         struct cusbd_visitor *visitor);

/**
 * @brief Override of @ref v_cusbd_descriptor_caccept().
 */
static void o_alternate_interface_caccept(const struct cusbd_alternate_interface *me, 
                                          struct cusbd_cvisitor *visitor);

/*------------------------------------------------------------*/
/*----------- STATIC FUNCTION DEFINITIONS - COMMON -----------*/
/*------------------------------------------------------------*/

static bool interface_descriptor_valid(const struct cusbd_interface_descriptor *descriptor)
{
    bool status = false;
    ECU_RUNTIME_ASSERT( (descriptor) );

#pragma message("TODO: Figure out bInterfaceClass, bInterfaceSubClass, and bInterfaceProtocol.")
    /* Do not assert bInterfaceNumber, bNumEndpoints, and iInterface since 
    these are automatically updated when device starts. */
    if (descriptor->bLength == sizeof(struct cusbd_interface_descriptor) &&
        descriptor->bDescriptorType == (uint8_t)CUSBD_INTERFACE_BDESCRIPTORTYPE &&
        descriptor->bAlternateSetting == 0)
    {
        status = true;
    }

    return status;
}

/*------------------------------------------------------------*/
/*---------- STATIC FUNCTION DEFINITIONS - INTERFACE ---------*/
/*------------------------------------------------------------*/

static bool alternate_interface_descriptor_valid(const struct cusbd_interface_descriptor *descriptor)
{
    bool status = false;
    ECU_RUNTIME_ASSERT( (descriptor) );

#pragma message("TODO: Figure out bInterfaceClass, bInterfaceSubClass, and bInterfaceProtocol.")
    /* Do not assert bInterfaceNumber, bNumEndpoints, and iInterface since 
    these are automatically updated when device starts. */
    if (descriptor->bLength == sizeof(struct cusbd_interface_descriptor) &&
        descriptor->bDescriptorType == (uint8_t)CUSBD_INTERFACE_BDESCRIPTORTYPE &&
        descriptor->bAlternateSetting > 0)
    {
        status = true;
    }

    return status;
}

static void o_interface_accept(struct cusbd_interface *me, struct cusbd_visitor *visitor)
{
    /* Do not assert valid() since that is centralized in the cusbd_descriptor_accept() function. */
    ECU_RUNTIME_ASSERT( (me && visitor) );
    v_cusbd_visitor_visit_interface(visitor, me);
}

static void o_interface_caccept(const struct cusbd_interface *me, struct cusbd_cvisitor *visitor)
{
    /* Do not assert valid() since that is centralized in the cusbd_descriptor_caccept() function. */
    ECU_RUNTIME_ASSERT( (me && visitor) );
    v_cusbd_cvisitor_visit_interface(visitor, me);
}

/*------------------------------------------------------------*/
/*----- STATIC FUNCTION DEFINITIONS - ALTERNATE INTERFACE ----*/
/*------------------------------------------------------------*/

static void o_alternate_interface_accept(struct cusbd_alternate_interface *me, 
                                         struct cusbd_visitor *visitor)
{
    /* Do not assert valid() since that is centralized in the cusbd_descriptor_accept() function. */
    ECU_RUNTIME_ASSERT( (me && visitor) );
    v_cusbd_visitor_visit_alternate_interface(visitor, me);
}

static void o_alternate_interface_caccept(const struct cusbd_alternate_interface *me, 
                                          struct cusbd_cvisitor *visitor)
{
    /* Do not assert valid() since that is centralized in the cusbd_descriptor_caccept() function. */
    ECU_RUNTIME_ASSERT( (me && visitor) );
    v_cusbd_cvisitor_visit_alternate_interface(visitor, me);
}

/*------------------------------------------------------------*/
/*---------------------- STATIC ASSERTS ----------------------*/
/*------------------------------------------------------------*/

ECU_STATIC_ASSERT( (sizeof(struct cusbd_interface_descriptor) == (size_t)9),
                    "Interface descriptor is 9 bytes." );

ECU_STATIC_ASSERT( (CUSBD_DESCRIPTOR_IS_BASEOF(base, struct cusbd_interface)),
                    "cusbd_interface must inherit cusbd_descriptor." );

ECU_STATIC_ASSERT( (CUSBD_DESCRIPTOR_IS_BASEOF(base, struct cusbd_alternate_interface)),
                    "cusbd_alternate_interface must inherit cusbd_descriptor." );

/*------------------------------------------------------------*/
/*-------------- CUSBD INTERFACE MEMBER FUNCTIONS ------------*/
/*------------------------------------------------------------*/

void cusbd_interface_ctor(struct cusbd_interface *me,
                          const struct cusbd_interface_descriptor *descriptor)
{
    ECU_RUNTIME_ASSERT( (me && descriptor) );
    ECU_RUNTIME_ASSERT( (interface_descriptor_valid(descriptor)) );

    static const struct cusbd_descriptor_vtable vtable = CUSBD_DESCRIPTOR_VTABLE_CTOR(
        &o_interface_accept,
        &o_interface_caccept,
        &cusbd_interface_valid
    );

    cusbd_descriptor_ctor(&me->base, CUSBD_INTERFACE_BDESCRIPTORTYPE);
    me->base.vptr = &vtable; /* MUST be AFTER cusbd_descriptor_ctor(). */
    memcpy(&me->descriptor, descriptor, sizeof(struct cusbd_interface_descriptor));
    me->alternate_setting = 0;
    ecu_dlist_ctor(&me->strings);
}

void cusbd_interface_add_alternate_interface(struct cusbd_interface *me,
                                             struct cusbd_alternate_interface *alternate_interface)
{
    /* ECU library asserts if node is already within a tree. Necessary descriptor 
    contents (wTotalLength, bInterfaceNumber, bAlternateSetting) are not updated 
    here since that complexity is centralized to the cusbd_start() function. */
    ECU_RUNTIME_ASSERT( (me && alternate_interface) );
    ECU_RUNTIME_ASSERT( (cusbd_interface_valid(me)) );
    ECU_RUNTIME_ASSERT( (cusbd_alternate_interface_valid(alternate_interface)) );
    ecu_ntnode_push_back(&me->base.ntnode, &alternate_interface->base.ntnode);
}

void cusbd_interface_add_endpoint(struct cusbd_interface *me,
                                  struct cusbd_endpoint *endpoint)
{
    /* ECU library asserts if node is already within a tree. Necessary descriptor 
    contents (wTotalLength, bNumEndpoints) are not updated 
    here since that complexity is centralized to the cusbd_start() function. */
    ECU_RUNTIME_ASSERT( (me && endpoint) );
    ECU_RUNTIME_ASSERT( (cusbd_interface_valid(me)) );
    ECU_RUNTIME_ASSERT( (cusbd_endpoint_valid(endpoint)) );
    ecu_ntnode_push_back(&me->base.ntnode, &endpoint->base.ntnode);
}

void cusbd_interface_add_string(struct cusbd_interface *me,
                                struct cusbd_string *string)
{
    /* ECU library asserts if node is already within a list. */
    ECU_RUNTIME_ASSERT( (me && string) );
    ECU_RUNTIME_ASSERT( (cusbd_interface_valid(me)) );
    ECU_RUNTIME_ASSERT( (cusbd_string_valid(string)) );
    ecu_dlist_push_back(&me->strings, &string->dnode);
}

bool cusbd_interface_valid(const struct cusbd_interface *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return (cusbd_descriptor_valid(&me->base) &&
            cusbd_descriptor_type(&me->base) == CUSBD_INTERFACE_BDESCRIPTORTYPE &&
            interface_descriptor_valid(&me->descriptor) &&
            ecu_dlist_valid(&me->strings));
}

/*------------------------------------------------------------*/
/*--------- CUSBD ALTERNATE INTERFACE MEMBER FUNCTIONS -------*/
/*------------------------------------------------------------*/

void cusbd_alternate_interface_ctor(struct cusbd_alternate_interface *me,
                                    const struct cusbd_interface_descriptor *descriptor)
{
    ECU_RUNTIME_ASSERT( (me && descriptor) );
    ECU_RUNTIME_ASSERT( (alternate_interface_descriptor_valid(descriptor)) );

    static const struct cusbd_descriptor_vtable vtable = CUSBD_DESCRIPTOR_VTABLE_CTOR(
        &o_alternate_interface_accept,
        &o_alternate_interface_caccept,
        &cusbd_alternate_interface_valid
    );

    cusbd_descriptor_ctor(&me->base, CUSBD_INTERFACE_BDESCRIPTORTYPE);
    me->base.vptr = &vtable; /* MUST be AFTER cusbd_descriptor_ctor(). */
    memcpy(&me->descriptor, descriptor, sizeof(struct cusbd_interface_descriptor));
    ecu_dlist_ctor(&me->strings);
}

void cusbd_alternate_interface_add_endpoint(struct cusbd_alternate_interface *me,
                                            struct cusbd_endpoint *endpoint)
{
    /* ECU library asserts if node is already within a tree. Necessary descriptor 
    contents (wTotalLength, bNumEndpoints) are not updated 
    here since that complexity is centralized to the cusbd_start() function. */
    ECU_RUNTIME_ASSERT( (me && endpoint) );
    ECU_RUNTIME_ASSERT( (cusbd_alternate_interface_valid(me)) );
    ECU_RUNTIME_ASSERT( (cusbd_endpoint_valid(endpoint)) );
    ecu_ntnode_push_back(&me->base.ntnode, &endpoint->base.ntnode);
}

void cusbd_alternate_interface_add_string(struct cusbd_alternate_interface *me,
                                          struct cusbd_string *string)
{
    /* ECU library asserts if node is already within a list. */
    ECU_RUNTIME_ASSERT( (me && string) );
    ECU_RUNTIME_ASSERT( (cusbd_alternate_interface_valid(me)) );
    ECU_RUNTIME_ASSERT( (cusbd_string_valid(string)) );
    ecu_dlist_push_back(&me->strings, &string->dnode);
}

bool cusbd_alternate_interface_valid(const struct cusbd_alternate_interface *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return (cusbd_descriptor_valid(&me->base) &&
            cusbd_descriptor_type(&me->base) == CUSBD_INTERFACE_BDESCRIPTORTYPE &&
            alternate_interface_descriptor_valid(&me->descriptor) &&
            ecu_dlist_valid(&me->strings));
}
