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
#include "cusb/interface.h"

/* STDLib. */
#include <string.h> /* memcpy. */

/* ECU. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_NAME("cusb/interface.c")

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Returns true if supplied descriptor was properly
 * constructed via @ref CUSB_INTERFACE_DESCRIPTOR_CTOR().
 * False otherwise.
 * 
 * @param descriptor Descriptor to check.
 */
static bool interface_descriptor_valid(const struct cusb_interface_descriptor *descriptor);

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
/*------------------------------------------------------------*/

static bool interface_descriptor_valid(const struct cusb_interface_descriptor *descriptor)
{
    bool status = false;
    ECU_RUNTIME_ASSERT( (descriptor) );

#pragma message("TODO: Figure out bInterfaceClass, bInterfaceSubClass, and bInterfaceProtocol.")
    /* Do not assert bInterfaceNumber, bAlternateSetting, bNumEndpoints, and 
    iInterface since these are automatically updated when descriptors are added
    to the device. */
    if (descriptor->bLength == sizeof(struct cusb_interface_descriptor) &&
        descriptor->bDescriptorType == (uint8_t)CUSB_INTERFACE_DESCRIPTOR_TYPE)
    {
        status = true;
    }

    return status;
}

/*------------------------------------------------------------*/
/*---------------------- STATIC ASSERTS ----------------------*/
/*------------------------------------------------------------*/

ECU_STATIC_ASSERT( (sizeof(struct cusb_interface_descriptor) == (size_t)9),
                    "Interface descriptor is 9 bytes." );

/*------------------------------------------------------------*/
/*--------------- CUSB INTERFACE MEMBER FUNCTIONS ------------*/
/*------------------------------------------------------------*/

void cusb_interface_ctor(struct cusb_interface *me,
                         const struct cusb_interface_descriptor *descriptor)
{
    ECU_RUNTIME_ASSERT( (me && descriptor) );
    ECU_RUNTIME_ASSERT( (interface_descriptor_valid(descriptor)) );

    ecu_dnode_ctor(&me->dnode, ECU_DNODE_DESTROY_UNUSED, CUSB_INTERFACE_DESCRIPTOR_TYPE);
    memcpy(&me->descriptor, descriptor, sizeof(struct cusb_interface_descriptor));
    ecu_dlist_ctor(&me->alternate_interfaces);
    ecu_dlist_ctor(&me->endpoints);
    ecu_dlist_ctor(&me->strings);
}

void cusb_interface_add_alternate_interface(struct cusb_interface *me,
                                            struct cusb_alternate_interface *alternate_interface)
{
    /* ECU library asserts if node is already within a list. Descriptor
    valid not asserted since that is done when cusb device starts. It
    loops over everything in the tree and asserts they are valid. */
    ECU_RUNTIME_ASSERT( (me && alternate_interface) );
    ecu_dlist_push_back(&me->alternate_interfaces, &alternate_interface->dnode);
}

void cusb_interface_add_endpoint(struct cusb_interface *me,
                                 struct cusb_endpoint *endpoint)
{
    const struct cusb_endpoint *e = (const struct cusb_endpoint *)0;
    struct ecu_dlist_const_iterator citerator;
    ECU_RUNTIME_ASSERT( (me && endpoint) );

    /* Verify no endpoints are duplicated. I.e. cannot have multiple 
    endpoint1 INs attached to the same interface descriptor. */
    ECU_DLIST_CONST_FOR_EACH(n, &citerator, &me->endpoints)
    {
        e = ECU_DNODE_GET_CONST_ENTRY(n, struct cusb_endpoint, dnode);
        ECU_RUNTIME_ASSERT( (e->descriptor.bEndpointAddress != endpoint->descriptor.bEndpointAddress) );
    }

    /* ECU library asserts if node is already within a list. Descriptor
    valid not asserted since that is done when cusb device starts. It
    loops over everything in the tree and asserts they are valid. */
    ecu_dlist_push_back(&me->endpoints, &endpoint->dnode);
}

void cusb_interface_add_string(struct cusb_interface *me,
                               struct cusb_string *string)
{
    /* ECU library asserts if node is already within a list. String
    valid not asserted since that is done when cusb device starts. It
    loops over everything in the tree and asserts they are valid. */
    ECU_RUNTIME_ASSERT( (me && string) );
    ecu_dlist_push_back(&me->strings, &string->dnode);
}

bool cusb_interface_valid(const struct cusb_interface *me)
{
    /* This is a wrapper function in case logic specific to the 
    cusb_interface object has to be added in the future. */
    ECU_RUNTIME_ASSERT( (me) );
    return interface_descriptor_valid(&me->descriptor);
}

size_t cusb_interface_size(const struct cusb_interface *me)
{
    struct ecu_dlist_const_iterator iterator;
    const struct cusb_alternate_interface *alternate_interface = (const struct cusb_alternate_interface *)0;
    size_t bytes = sizeof(struct cusb_interface_descriptor); /* This interface descriptor. */
    ECU_RUNTIME_ASSERT( (me) );

    /* Add size of all of this interface descriptor's endpoints. */
    bytes = ecu_dlist_size(&me->endpoints) * sizeof(struct cusb_endpoint_descriptor);

    /* Add size of all alternate interfaces and all of their endpoints. */
    ECU_DLIST_CONST_FOR_EACH(i, &iterator, &me->alternate_interfaces)
    {
        alternate_interface = ECU_DNODE_GET_CONST_ENTRY(i, struct cusb_alternate_interface, dnode);
        bytes += cusb_alternate_interface_size(alternate_interface);
    }

    return bytes;
}

/*------------------------------------------------------------*/
/*--------- CUSB ALTERNATE INTERFACE MEMBER FUNCTIONS --------*/
/*------------------------------------------------------------*/

void cusb_alternate_interface_ctor(struct cusb_alternate_interface *me,
                                   const struct cusb_interface_descriptor *descriptor)
{
    ECU_RUNTIME_ASSERT( (me && descriptor) );
    ECU_RUNTIME_ASSERT( (interface_descriptor_valid(descriptor)) );

    ecu_dnode_ctor(&me->dnode, ECU_DNODE_DESTROY_UNUSED, CUSB_INTERFACE_DESCRIPTOR_TYPE);
    memcpy(&me->descriptor, descriptor, sizeof(struct cusb_interface_descriptor));
    ecu_dlist_ctor(&me->endpoints);
    ecu_dlist_ctor(&me->strings);
}

void cusb_alternate_interface_add_endpoint(struct cusb_alternate_interface *me,
                                           struct cusb_endpoint *endpoint)
{
    const struct cusb_endpoint *e = (const struct cusb_endpoint *)0;
    struct ecu_dlist_const_iterator citerator;
    ECU_RUNTIME_ASSERT( (me && endpoint) );

    /* Verify no endpoints are duplicated. I.e. cannot have multiple 
    endpoint1 INs attached to the same interface descriptor. */
    ECU_DLIST_CONST_FOR_EACH(n, &citerator, &me->endpoints)
    {
        e = ECU_DNODE_GET_CONST_ENTRY(n, struct cusb_endpoint, dnode);
        ECU_RUNTIME_ASSERT( (e->descriptor.bEndpointAddress != endpoint->descriptor.bEndpointAddress) );
    }

    /* ECU library asserts if node is already within a list. Descriptor
    valid not asserted since that is done when cusb device starts. It
    loops over everything in the tree and asserts they are valid. */
    ecu_dlist_push_back(&me->endpoints, &endpoint->dnode);
}

void cusb_alternate_interface_add_string(struct cusb_alternate_interface *me,
                                         struct cusb_string *string)
{
    /* ECU library asserts if node is already within a list. String
    valid not asserted since that is done when cusb device starts. It
    loops over everything in the tree and asserts they are valid. */
    ECU_RUNTIME_ASSERT( (me && string) );
    ecu_dlist_push_back(&me->strings, &string->dnode);
}

bool cusb_alternate_interface_valid(const struct cusb_alternate_interface *me)
{
    /* This is a wrapper function in case logic specific to the 
    cusb_alternate_interface object has to be added in the future. */
    ECU_RUNTIME_ASSERT( (me) );
    return interface_descriptor_valid(&me->descriptor);
}

size_t cusb_alternate_interface_size(const struct cusb_alternate_interface *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    size_t bytes = sizeof(struct cusb_interface_descriptor); /* This interface descriptor. */
    bytes += ecu_dlist_size(&me->endpoints) * sizeof(struct cusb_endpoint_descriptor);
    return bytes;
}
