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
#include <stddef.h>
#include <string.h> /* memcpy. */

/* ECU. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_NAME("cusb/configuration.c")

/*------------------------------------------------------------*/
/*--------------------------- DEFINES ------------------------*/
/*------------------------------------------------------------*/

// #define BMATTRIBUTES_

/*------------------------------------------------------------*/
/*------------------- FILE SCOPE VARIABLES -------------------*/
/*------------------------------------------------------------*/

static const struct cusb_interface_descriptor DEFAULT_INTERFACE_DESCRIPTOR = CUSB_INTERFACE_DESCRIPTOR_CTOR(
        0x00, 0x00, 0x00, 0x00
    );

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
    static const uint16_t DEFAULT_WTOTALLENGTH = ECU_CPU_TO_LE16_COMPILETIME(sizeof(struct cusb_configuration_descriptor));
    ECU_RUNTIME_ASSERT( (me && descriptor) );
    ECU_RUNTIME_ASSERT( (descriptor->bLength == sizeof(struct cusb_configuration_descriptor)) );
    ECU_RUNTIME_ASSERT( (descriptor->bDescriptorType == (uint8_t)CUSB_CONFIGURATION_DESCRIPTOR_TYPE) );
    ECU_RUNTIME_ASSERT( (descriptor->wTotalLength == DEFAULT_WTOTALLENGTH) );
    ECU_RUNTIME_ASSERT( (descriptor->bNumInterfaces == (uint8_t)1) );
    ECU_RUNTIME_ASSERT( (descriptor->bConfigurationValue == (uint8_t)0) );
#pragma message("TODO: Figure out iConfiguration, bmAttributes, and bMaxPower.")

    ecu_dnode_ctor(&me->dnode, ECU_DNODE_DESTROY_UNUSED, CUSB_CONFIGURATION_DESCRIPTOR_TYPE);
    memcpy(&me->descriptor, descriptor, sizeof(struct cusb_configuration_descriptor));
    ecu_dlist_ctor(&me->interfaces);
    cusb_interface_ctor(&me->interface0, &DEFAULT_INTERFACE_DESCRIPTOR);

    // todo: cusb_configuration_add_interface??
    ecu_dlist_push_back(&me->interfaces, &me->interface0.dnode);
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
