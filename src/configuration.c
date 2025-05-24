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
#include "cusb/configuration.h"

/* STDLib. */
#include <string.h> /* memcpy. */

/* ECU. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_NAME("cusb/configuration.c")

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Returns true if supplied descriptor was properly
 * constructed via @ref CUSB_CONFIGURATION_DESCRIPTOR_CTOR().
 * False otherwise.
 * 
 * @param descriptor Descriptor to check.
 */
static bool configuration_descriptor_valid(const struct cusb_configuration_descriptor *descriptor);

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
/*------------------------------------------------------------*/

static bool configuration_descriptor_valid(const struct cusb_configuration_descriptor *descriptor)
{
    bool status = false;
    ECU_RUNTIME_ASSERT( (descriptor) );

#pragma message("TODO: Figure out bmAttributes, and bMaxPower.")
    /* Do not assert wTotalLength, bNumInterfaces, bConfigurationValue, and iConfiguration 
    since these are automatically updated when descriptors are added to the device. */
    if (descriptor->bLength == sizeof(struct cusb_configuration_descriptor) &&
        descriptor->bDescriptorType == (uint8_t)CUSB_CONFIGURATION_DESCRIPTOR_TYPE)
    {
        status = true;
    }

    return status;
}

/*------------------------------------------------------------*/
/*---------------------- STATIC ASSERTS ----------------------*/
/*------------------------------------------------------------*/

ECU_STATIC_ASSERT( (sizeof(struct cusb_configuration_descriptor) == (size_t)9),
                    "Configuration descriptor is 9 bytes." );

/*------------------------------------------------------------*/
/*------------ CUSB CONFIGURATION MEMBER FUNCTIONS -----------*/
/*------------------------------------------------------------*/

void cusb_configuration_ctor(struct cusb_configuration *me,
                             const struct cusb_configuration_descriptor *descriptor)
{
    ECU_RUNTIME_ASSERT( (me && descriptor) );
    ECU_RUNTIME_ASSERT( (configuration_descriptor_valid(&me->descriptor)) );

    ecu_dnode_ctor(&me->dnode, ECU_DNODE_DESTROY_UNUSED, CUSB_CONFIGURATION_DESCRIPTOR_TYPE);
    memcpy(&me->descriptor, descriptor, sizeof(struct cusb_configuration_descriptor));
    ecu_dlist_ctor(&me->interfaces);
    ecu_dlist_ctor(&me->strings);
}

void cusb_configuration_add_interface(struct cusb_configuration *me,
                                      struct cusb_interface *interface)
{
    /* ECU library asserts if node is already within a list. Descriptor
    valid not asserted since that is done when cusb device starts. It
    loops over everything in the tree and asserts they are valid. */
    ECU_RUNTIME_ASSERT( (me && interface) );
    ecu_dlist_push_back(&me->interfaces, &interface->dnode);
}

void cusb_configuration_add_string(struct cusb_configuration *me,
                                   struct cusb_string *string)
{
    /* ECU library asserts if node is already within a list. String
    valid not asserted since that is done when cusb device starts. It
    loops over everything in the tree and asserts they are valid. */
    ECU_RUNTIME_ASSERT( (me && string) );
    ecu_dlist_push_back(&me->strings, &string->dnode);
}

bool cusb_configuration_valid(const struct cusb_configuration *me)
{
    /* This is a wrapper function in case logic specific to the 
    cusb_configuration object has to be added in the future. */
    ECU_RUNTIME_ASSERT( (me) );
    return configuration_descriptor_valid(&me->descriptor);
}

size_t cusb_configuration_interface_count(const struct cusb_configuration *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return ecu_dlist_size(&me->interfaces);
}

size_t cusb_configuration_size(const struct cusb_configuration *me)
{
    struct ecu_dlist_const_iterator interface_iterator;
    const struct cusb_interface *interface = (const struct cusb_interface *)0; 
    size_t bytes = sizeof(struct cusb_configuration_descriptor); /* This configuration descriptor. */
    ECU_RUNTIME_ASSERT( (me) );

    ECU_DLIST_CONST_FOR_EACH(i, &interface_iterator, &me->interfaces)
    {
        interface = ECU_DNODE_GET_CONST_ENTRY(i, struct cusb_interface, dnode);
        bytes += cusb_interface_size(interface);
    }

    return bytes;
}
