/**
 * @file
 * @brief Object representing an interface descriptor.
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

/* ECU. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_NAME("cusbd/interface.c")

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Returns true if supplied descriptor was properly
 * constructed via @ref CUSBD_INTERFACE_DESCRIPTOR_CTOR().
 * False otherwise.
 * 
 * @param descriptor Descriptor to check.
 */
static bool interface_descriptor_valid(const struct cusbd_interface_descriptor *descriptor);

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
/*------------------------------------------------------------*/

static bool interface_descriptor_valid(const struct cusbd_interface_descriptor *descriptor)
{
    bool status = false;
    ECU_RUNTIME_ASSERT( (descriptor) );

#pragma message("TODO: Figure out bInterfaceClass, bInterfaceSubClass, and bInterfaceProtocol.")
    /* Do not assert bInterfaceNumber, bAlternateSetting, bNumEndpoints, and 
    iInterface since these are automatically updated when descriptors are added
    to the device. */
    if (descriptor->bLength == sizeof(struct cusbd_interface_descriptor) &&
        descriptor->bDescriptorType == (uint8_t)CUSBD_INTERFACE_DESCRIPTOR_TYPE)
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

    cusbd_descriptor_ctor(CUSBD_DESCRIPTOR_BASE_CAST(me), CUSBD_INTERFACE_DESCRIPTOR_TYPE);
    memcpy(&me->descriptor, descriptor, sizeof(struct cusbd_interface_descriptor));
    ecu_dlist_ctor(&me->strings);
}

void cusbd_interface_add_alternate_interface(struct cusbd_interface *me,
                                             struct cusbd_alternate_interface *alternate_interface)
{
    /* ECU library asserts if node is already within a tree. Descriptor
    valid not asserted since that is done when cusbd device starts. It
    loops over everything in the tree and asserts they are valid. */
    ECU_RUNTIME_ASSERT( (me && alternate_interface) );
    ecu_ntnode_push_back(&alternate_interface->base.ntnode, &me->base.ntnode);
}

void cusbd_interface_add_endpoint(struct cusbd_interface *me,
                                  struct cusbd_endpoint *endpoint)
{
    const struct cusbd_descriptor *base = (const struct cusbd_descriptor *)0;
    const struct cusbd_endpoint *e = (const struct cusbd_endpoint *)0;
    struct ecu_ntnode_child_citerator citerator;
    ECU_RUNTIME_ASSERT( (me && endpoint) );

    /* Verify no endpoints are duplicated. I.e. cannot have multiple 
    endpoint1 INs attached to the same interface descriptor. */
    ECU_NTNODE_CONST_CHILD_FOR_EACH(n, &citerator, &me->base.ntnode)
    {
        base = ECU_NTNODE_GET_CONST_ENTRY(n, struct cusbd_descriptor, ntnode);
        if (cusbd_descriptor_type(base) == CUSBD_ENDPOINT_DESCRIPTOR_TYPE)
        {
            e = (const struct cusb_endpoint *)base;
            ECU_RUNTIME_ASSERT( (e->descriptor.bEndpointAddress != endpoint->descriptor.bEndpointAddress) );
        }
    }

!!!!!!!!! TODO Think node that is being added should be 2nd parameter not 1st....!!!!!
    /* ECU library asserts if node is already within a tree. Descriptor
    valid not asserted since that is done when cusbd device starts. It
    loops over everything in the tree and asserts they are valid. */
    ecu_ntnode_push_front(&endpoint->base.ntnode, &me->base.ntnode);
}

void cusbd_interface_add_string(struct cusbd_interface *me,
                                struct cusbd_string *string)
{
    /* ECU library asserts if node is already within a list. String
    valid not asserted since that is done when cusbd device starts. It
    loops over everything in the tree and asserts they are valid. */
    ECU_RUNTIME_ASSERT( (me && string) );
    ecu_dlist_push_back(&me->strings, &string->dnode);
}

bool cusbd_interface_valid(const struct cusbd_interface *me)
{
    /* This is a wrapper function in case logic specific to the 
    cusbd_interface object has to be added in the future. */
    ECU_RUNTIME_ASSERT( (me) );
    return interface_descriptor_valid(&me->descriptor);
}

size_t cusbd_interface_size(const struct cusbd_interface *me)
{
    struct ecu_dlist_citerator iterator;
    const struct cusbd_alternate_interface *alternate_interface = (const struct cusbd_alternate_interface *)0;
    size_t bytes = sizeof(struct cusbd_interface_descriptor); /* This interface descriptor. */
    ECU_RUNTIME_ASSERT( (me) );

    /* Add size of all of this interface descriptor's endpoints. */
    bytes = ecu_dlist_size(&me->endpoints) * sizeof(struct cusbd_endpoint_descriptor);

    /* Add size of all alternate interfaces and all of their endpoints. */
    ECU_DLIST_CONST_FOR_EACH(i, &iterator, &me->alternate_interfaces)
    {
        alternate_interface = ECU_DNODE_GET_CONST_ENTRY(i, struct cusbd_alternate_interface, dnode);
        bytes += cusbd_alternate_interface_size(alternate_interface);
    }

    return bytes;
}

/*------------------------------------------------------------*/
/*--------- CUSBD ALTERNATE INTERFACE MEMBER FUNCTIONS -------*/
/*------------------------------------------------------------*/

void cusbd_alternate_interface_ctor(struct cusbd_alternate_interface *me,
                                    const struct cusbd_interface_descriptor *descriptor)
{
    ECU_RUNTIME_ASSERT( (me && descriptor) );
    ECU_RUNTIME_ASSERT( (interface_descriptor_valid(descriptor)) );

    ecu_dnode_ctor(&me->dnode, ECU_DNODE_DESTROY_UNUSED, CUSBD_INTERFACE_DESCRIPTOR_TYPE);
    memcpy(&me->descriptor, descriptor, sizeof(struct cusbd_interface_descriptor));
    ecu_dlist_ctor(&me->endpoints);
    ecu_dlist_ctor(&me->strings);
}

void cusbd_alternate_interface_add_endpoint(struct cusbd_alternate_interface *me,
                                            struct cusbd_endpoint *endpoint)
{
    const struct cusbd_endpoint *e = (const struct cusbd_endpoint *)0;
    struct ecu_dlist_citerator citerator;
    ECU_RUNTIME_ASSERT( (me && endpoint) );

    /* Verify no endpoints are duplicated. I.e. cannot have multiple 
    endpoint1 INs attached to the same interface descriptor. */
    ECU_DLIST_CONST_FOR_EACH(n, &citerator, &me->endpoints)
    {
        e = ECU_DNODE_GET_CONST_ENTRY(n, struct cusbd_endpoint, dnode);
        ECU_RUNTIME_ASSERT( (e->descriptor.bEndpointAddress != endpoint->descriptor.bEndpointAddress) );
    }

    /* ECU library asserts if node is already within a list. Descriptor
    valid not asserted since that is done when cusbd device starts. It
    loops over everything in the tree and asserts they are valid. */
    ecu_dlist_push_back(&me->endpoints, &endpoint->dnode);
}

void cusbd_alternate_interface_add_string(struct cusbd_alternate_interface *me,
                                          struct cusbd_string *string)
{
    /* ECU library asserts if node is already within a list. String
    valid not asserted since that is done when cusbd device starts. It
    loops over everything in the tree and asserts they are valid. */
    ECU_RUNTIME_ASSERT( (me && string) );
    ecu_dlist_push_back(&me->strings, &string->dnode);
}

bool cusbd_alternate_interface_valid(const struct cusbd_alternate_interface *me)
{
    /* This is a wrapper function in case logic specific to the 
    cusbd_alternate_interface object has to be added in the future. */
    ECU_RUNTIME_ASSERT( (me) );
    return interface_descriptor_valid(&me->descriptor);
}

size_t cusbd_alternate_interface_size(const struct cusbd_alternate_interface *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    size_t bytes = sizeof(struct cusbd_interface_descriptor); /* This interface descriptor. */
    bytes += ecu_dlist_size(&me->endpoints) * sizeof(struct cusbd_endpoint_descriptor);
    return bytes;
}
