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
/*--------------------- DEFINES - BITMASKS -------------------*/
/*------------------------------------------------------------*/

#define BENDPOINTADDRESS_NUMBER_BITMASK \
    ((1U << 3) | (1U << 2) | (1U << 1) | (1U << 0))

#define BENDPOINTADDRESS_RESERVED_BITMASK \
    ((1U << 6) | (1U << 5) | (1U << 4))

#define BMATTRIBUTES_TRANSFER_TYPE_BITMASK \
    ((1U << 1) | (1U << 0))

#define BMATTRIBUTES_SYNC_TYPE_BITMASK \
    ((1U << 3) | (1U << 2))

#define BMATTRIBUTES_USAGE_TYPE_BITMASK \
    ((1U << 5) | (1U << 4))

/*------------------------------------------------------------*/
/*---------------------- DEFINES - VALUES --------------------*/
/*------------------------------------------------------------*/

#define BMATTRIBUTES_CONTROL_TYPE \
    (0U)

#define BMATTRIBUTES_ISOCHRONOUS_TYPE \
    (1U << 0)

#define BMATTRIBUTES_BULK_TYPE \
    (1U << 1)

#define BMATTRIBUTES_INTERRUPT_TYPE \
    ((1U << 1) | (1U << 0))

#define BMATTRIBUTES_USAGE_TYPE_RESERVED \
    ((1U << 5) | (1U << 4))

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Returns true if supplied descriptor was properly
 * constructed via @ref CUSB_ENDPOINT_DESCRIPTOR_CTOR().
 * False otherwise.
 * 
 * @param descriptor Descriptor to check.
 */
static bool endpoint_descriptor_valid(const struct cusb_endpoint_descriptor *descriptor);

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
/*------------------------------------------------------------*/

static bool endpoint_descriptor_valid(const struct cusb_endpoint_descriptor *descriptor)
{
    bool status = false;
    ECU_RUNTIME_ASSERT( (descriptor) );
    uint8_t bEndpointAddress = descriptor->bEndpointAddress;

#pragma message("TODO: Figure out wMaxPacketSize and bInterval")
    if ((descriptor->bLength == sizeof(struct cusb_endpoint_descriptor)) &&
        (descriptor->bDescriptorType == (uint8_t)CUSB_ENDPOINT_DESCRIPTOR_TYPE) &&
        ((descriptor->bEndpointAddress & BENDPOINTADDRESS_NUMBER_BITMASK) > 0U) &&
        ((descriptor->bEndpointAddress & BENDPOINTADDRESS_RESERVED_BITMASK) == 0U))
    {
        switch (bEndpointAddress & BMATTRIBUTES_TRANSFER_TYPE_BITMASK)
        {
            /* Intentional fallthrough. */
            case BMATTRIBUTES_CONTROL_TYPE:
            case BMATTRIBUTES_BULK_TYPE:
            case BMATTRIBUTES_INTERRUPT_TYPE:
            {
                if (((bEndpointAddress & BMATTRIBUTES_SYNC_TYPE_BITMASK) == 0U) ||
                    ((bEndpointAddress & BMATTRIBUTES_USAGE_TYPE_BITMASK) == 0U))
                {
                    status = true;
                }
                break;
            }
    
            case BMATTRIBUTES_ISOCHRONOUS_TYPE:
            {
                if ((bEndpointAddress & BMATTRIBUTES_USAGE_TYPE_BITMASK) != BMATTRIBUTES_USAGE_TYPE_RESERVED)
                {
                    status = true;
                }
                break;
            }
    
            default:
            {
                ECU_RUNTIME_ASSERT( (false) );
            }
        }
    }

    return status;
}

/*------------------------------------------------------------*/
/*---------------------- STATIC ASSERTS ----------------------*/
/*------------------------------------------------------------*/

ECU_STATIC_ASSERT( ((endpoint_id_t)-1 < (endpoint_id_t)0), 
                    "endpoint_id_t must be a signed type." );

ECU_STATIC_ASSERT( (CUSB_ENDPOINT_USER_ID_BEGIN == 0), 
                    "CUSB_ENDPOINT_USER_ID_BEGIN must always equal 0 for future compatibility." );

ECU_STATIC_ASSERT( (sizeof(struct cusb_endpoint_descriptor) == (size_t)7),
                    "Endpoint descriptor is 7 bytes." );

/*------------------------------------------------------------*/
/*----------------- CUSB ENDPOINT MEMBER FUNCTIONS -----------*/
/*------------------------------------------------------------*/

void cusb_endpoint_ctor(struct cusb_endpoint *me,
                        const struct cusb_endpoint_descriptor *descriptor,
                        endpoint_id_t id)
{
    ECU_RUNTIME_ASSERT( (me && descriptor) );
    ECU_RUNTIME_ASSERT( (endpoint_descriptor_valid(descriptor)) );
    ECU_RUNTIME_ASSERT( (id >= CUSB_ENDPOINT_USER_ID_BEGIN) );

    ecu_dnode_ctor(&me->dnode, ECU_DNODE_DESTROY_UNUSED, CUSB_ENDPOINT_DESCRIPTOR_TYPE);
    memcpy(&me->descriptor, descriptor, sizeof(struct cusb_endpoint_descriptor));
    me->id = id;
}

bool cusb_endpoint_valid(const struct cusb_endpoint *me)
{
    /* This is a wrapper function in case logic specific to the 
    cusb_endpoint object has to be added in the future. */
    ECU_RUNTIME_ASSERT( (me) );
    return endpoint_descriptor_valid(&me->descriptor);
}
