/**
 * @file
 * @brief Object representing an endpoint descriptor.
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
#include "cusb/endpoint.h"

/* STDLib. */
#include <stddef.h>
#include <string.h> /* memcpy. */

/* ECU. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_NAME("cusb/endpoint.c")

/*------------------------------------------------------------*/
/*---------------------- STATIC ASSERTS ----------------------*/
/*------------------------------------------------------------*/

ECU_STATIC_ASSERT( (sizeof(struct cusb_endpoint_descriptor) == (size_t)7),
                    "Endpoint descriptor is 7 bytes." );

/*------------------------------------------------------------*/
/*----------------- CUSB ENDPOINT MEMBER FUNCTIONS -----------*/
/*------------------------------------------------------------*/

void cusb_endpoint_ctor(struct cusb_endpoint *me,
                        const struct cusb_endpoint_descriptor *descriptor)
{
    ECU_RUNTIME_ASSERT( (me && descriptor) );
    ECU_RUNTIME_ASSERT( (descriptor->bLength == sizeof(struct cusb_endpoint_descriptor)) );
    ECU_RUNTIME_ASSERT( (descriptor->bDescriptorType == (uint8_t)CUSB_ENDPOINT_DESCRIPTOR_TYPE) );
#pragma message("TODO: Figure out bEndpointAddress, bmAttributes, wMaxPacketSize, and bInterval.")

    ecu_dnode_ctor(&me->dnode, ECU_DNODE_DESTROY_UNUSED, CUSB_ENDPOINT_DESCRIPTOR_TYPE);
    memcpy(&me->descriptor, descriptor, sizeof(struct cusb_endpoint_descriptor));
}
