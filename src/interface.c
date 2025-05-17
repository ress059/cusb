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
#include <stddef.h>
#include <string.h> /* memcpy. */

/* CUSB. */
#include "cusb/endpoint.h"

/* ECU. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_NAME("cusb/interface.c")

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
    ECU_RUNTIME_ASSERT( (descriptor->bLength == sizeof(struct cusb_interface_descriptor)) );
    ECU_RUNTIME_ASSERT( (descriptor->bDescriptorType == (uint8_t)CUSB_INTERFACE_DESCRIPTOR_TYPE) );
    /* bInterfaceNumber and bNumEndpoints automatically updated when interface descriptor added to device.*/
#pragma message("TODO: Figure out bAlternateSetting, bInterfaceClass, bInterfaceSubClass, bInterfaceProtocol, and iInterface")

    ecu_dnode_ctor(&me->dnode, ECU_DNODE_DESTROY_UNUSED, CUSB_INTERFACE_DESCRIPTOR_TYPE);
    memcpy(&me->descriptor, descriptor, sizeof(struct cusb_interface_descriptor));
    ecu_dlist_ctor(&me->alternate_settings);
    ecu_dlist_ctor(&me->endpoints);
}

size_t cusb_interface_size(const struct cusb_interface *me)
{
    struct ecu_dlist_const_iterator interface_iterator;
    struct ecu_dlist_const_iterator endpoint_iterator;
    const struct cusb_interface *alternate_setting = (const struct cusb_interface *)0;
    size_t bytes = sizeof(struct cusb_interface_descriptor); /* This interface descriptor. */
    ECU_RUNTIME_ASSERT( (me) );

    /* Add size of all of this interface descriptor's endpoints. */
    bytes = ecu_dlist_size(&me->endpoints) * sizeof(struct cusb_endpoint_descriptor);

    /* Add size of all alternate settings and all of their endpoints. */
    ECU_DLIST_CONST_FOR_EACH(i, &interface_iterator, &me->alternate_settings)
    {
        bytes += sizeof(struct cusb_interface_descriptor);
        alternate_setting = ECU_DNODE_GET_CONST_ENTRY(i, struct cusb_interface, dnode);

        ECU_DLIST_CONST_FOR_EACH(e, &endpoint_iterator, &alternate_setting->endpoints)
        {
            bytes += sizeof(struct cusb_endpoint_descriptor);
        }
    }

    return bytes;
}
