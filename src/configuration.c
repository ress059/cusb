/**
 * @file
 * @brief Object representing a configuration descriptor.
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
#include "cusbd/configuration.h"

/* STDLib. */
#include <string.h> /* memcpy. */

/* ECU. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_NAME("cusbd/configuration.c")

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Returns true if supplied descriptor was properly
 * constructed via @ref CUSBD_CONFIGURATION_DESCRIPTOR_CTOR().
 * False otherwise.
 * 
 * @param descriptor Descriptor to check.
 */
static bool configuration_descriptor_valid(const struct cusbd_configuration_descriptor *descriptor);

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
/*------------------------------------------------------------*/

static bool configuration_descriptor_valid(const struct cusbd_configuration_descriptor *descriptor)
{
    bool status = false;
    ECU_RUNTIME_ASSERT( (descriptor) );

#pragma message("TODO: Figure out bmAttributes, and bMaxPower.")
    /* Do not assert wTotalLength, bNumInterfaces, bConfigurationValue, and iConfiguration 
    since these are automatically updated when descriptors are added to the device. */
    if (descriptor->bLength == sizeof(struct cusbd_configuration_descriptor) &&
        descriptor->bDescriptorType == (uint8_t)CUSBD_DESCRIPTOR_TYPE_CONFIGURATION)
    {
        status = true;
    }

    return status;
}

/*------------------------------------------------------------*/
/*---------------------- STATIC ASSERTS ----------------------*/
/*------------------------------------------------------------*/

ECU_STATIC_ASSERT( (sizeof(struct cusbd_configuration_descriptor) == (size_t)9),
                    "Configuration descriptor is 9 bytes." );

/*------------------------------------------------------------*/
/*------------ CUSBD CONFIGURATION MEMBER FUNCTIONS ----------*/
/*------------------------------------------------------------*/

void cusbd_configuration_ctor(struct cusbd_configuration *me,
                              const struct cusbd_configuration_descriptor *descriptor)
{
    ECU_RUNTIME_ASSERT( (me && descriptor) );
    ECU_RUNTIME_ASSERT( (configuration_descriptor_valid(&me->descriptor)) );

    ecu_dnode_ctor(&me->dnode, ECU_DNODE_DESTROY_UNUSED, CUSBD_DESCRIPTOR_TYPE_CONFIGURATION);
    memcpy(&me->descriptor, descriptor, sizeof(struct cusbd_configuration_descriptor));
    ecu_dlist_ctor(&me->interfaces);
    ecu_dlist_ctor(&me->strings);
}

void cusbd_configuration_add_interface(struct cusbd_configuration *me,
                                       struct cusbd_interface *interface)
{
    /* ECU library asserts if node is already within a list. Descriptor
    valid not asserted since that is done when cusbd device starts. It
    loops over everything in the tree and asserts they are valid. */
    ECU_RUNTIME_ASSERT( (me && interface) );
    ecu_dlist_push_back(&me->interfaces, &interface->dnode);
}

void cusbd_configuration_add_string(struct cusbd_configuration *me,
                                   struct cusbd_string *string)
{
    /* ECU library asserts if node is already within a list. String
    valid not asserted since that is done when cusbd device starts. It
    loops over everything in the tree and asserts they are valid. */
    ECU_RUNTIME_ASSERT( (me && string) );
    ecu_dlist_push_back(&me->strings, &string->dnode);
}

bool cusbd_configuration_valid(const struct cusbd_configuration *me)
{
    /* This is a wrapper function in case logic specific to the 
    cusbd_configuration object has to be added in the future. */
    ECU_RUNTIME_ASSERT( (me) );
    return configuration_descriptor_valid(&me->descriptor);
}

size_t cusbd_configuration_interface_count(const struct cusbd_configuration *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return ecu_dlist_size(&me->interfaces);
}

size_t cusbd_configuration_size(const struct cusbd_configuration *me)
{
    struct ecu_dlist_citerator interface_iterator;
    const struct cusbd_interface *interface = (const struct cusbd_interface *)0; 
    size_t bytes = sizeof(struct cusbd_configuration_descriptor); /* This configuration descriptor. */
    ECU_RUNTIME_ASSERT( (me) );

    ECU_DLIST_CONST_FOR_EACH(i, &interface_iterator, &me->interfaces)
    {
        interface = ECU_DNODE_GET_CONST_ENTRY(i, struct cusbd_interface, dnode);
        bytes += cusbd_interface_size(interface);
    }

    return bytes;
}
