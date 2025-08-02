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
#include "cusbd/endpoint.h"
#include "cusbd/string.h"

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

/*------------------------------------------------------------*/
/*---------- STATIC FUNCTION DEFINITIONS - INTERFACE ---------*/
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
/*----- STATIC FUNCTION DEFINITIONS - ALTERNATE INTERFACE ----*/
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

/*------------------------------------------------------------*/
/*---------------------- STATIC ASSERTS ----------------------*/
/*------------------------------------------------------------*/

ECU_STATIC_ASSERT( (sizeof(struct cusbd_interface_descriptor) == (size_t)9),
                    "Interface descriptor is 9 bytes." );

/*------------------------------------------------------------*/
/*-------------- CUSBD INTERFACE MEMBER FUNCTIONS ------------*/
/*------------------------------------------------------------*/

void cusbd_interface_ctor(struct cusbd_interface *me,
                          const struct cusbd_interface_descriptor *descriptor)
{
    ECU_RUNTIME_ASSERT( (me && descriptor) );
    ECU_RUNTIME_ASSERT( (interface_descriptor_valid(descriptor)) );

    ecu_ntnode_ctor(&me->ntnode, ECU_NTNODE_DESTROY_UNUSED, (ecu_object_id)CUSBD_INTERFACE_BDESCRIPTORTYPE);
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
    ecu_ntnode_push_child_back(&me->ntnode, &alternate_interface->ntnode);
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
    ecu_ntnode_push_child_back(&me->ntnode, &endpoint->ntnode);
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
    return (ecu_ntnode_valid(&me->ntnode) &&
            ecu_ntnode_id(&me->ntnode) == (ecu_object_id)CUSBD_INTERFACE_BDESCRIPTORTYPE &&
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

    ecu_ntnode_ctor(&me->ntnode, ECU_NTNODE_DESTROY_UNUSED, (ecu_object_id)CUSBD_INTERFACE_BDESCRIPTORTYPE);
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
    ecu_ntnode_push_child_back(&me->ntnode, &endpoint->ntnode);
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
    return (ecu_ntnode_valid(&me->ntnode) &&
            ecu_ntnode_id(&me->ntnode) == (ecu_object_id)CUSBD_INTERFACE_BDESCRIPTORTYPE &&
            alternate_interface_descriptor_valid(&me->descriptor) &&
            ecu_dlist_valid(&me->strings));
}
