/**
 * @file
 * @brief See @ref endpoint.h.
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
#include "cusbd/endpoint.h"

/* STDLib. */
#include <string.h> /* memcpy. */

/* ECU. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_NAME("cusbd/endpoint.c")

/*------------------------------------------------------------*/
/*--------------------- DEFINES - BITMASKS -------------------*/
/*------------------------------------------------------------*/

#define BENDPOINTADDRESS_NUMBER_BITMASK \
    ((1U << 3) | (1U << 2) | (1U << 1) | (1U << 0))

#define BENDPOINTADDRESS_RESERVED_BITMASK \
    ((1U << 6) | (1U << 5) | (1U << 4))

#define BENDPOINTADDRESS_DIRECTION_BITMASK \
    (1U << 7)

#define BMATTRIBUTES_TRANSFER_TYPE_BITMASK \
    ((1U << 1) | (1U << 0))

#define BMATTRIBUTES_SYNC_TYPE_BITMASK \
    ((1U << 3) | (1U << 2))

#define BMATTRIBUTES_USAGE_TYPE_BITMASK \
    ((1U << 5) | (1U << 4))

/*------------------------------------------------------------*/
/*---------------------- DEFINES - VALUES --------------------*/
/*------------------------------------------------------------*/

#define BMATTRIBUTES_TRANSFER_TYPE_CONTROL \
    (0U)

#define BMATTRIBUTES_TRANSFER_TYPE_ISOCHRONOUS \
    (1U << 0)

#define BMATTRIBUTES_TRANSFER_TYPE_BULK \
    (1U << 1)

#define BMATTRIBUTES_TRANSFER_TYPE_INTERRUPT \
    ((1U << 1) | (1U << 0))

#define BMATTRIBUTES_SYNC_TYPE_NOSYNC \
    (0U)

#define BMATTRIBUTES_SYNC_TYPE_ASYNC \
    (1U << 2)

#define BMATTRIBUTES_SYNC_TYPE_ADAPTIVE \
    (1U << 3)

#define BMATTRIBUTES_SYNC_TYPE_SYNC \
    ((1U << 3) | (1U << 2))

#define BMATTRIBUTES_USAGE_TYPE_DATA \
    (0U)

#define BMATTRIBUTES_USAGE_TYPE_FEEDBACK \
    (1U << 4)

#define BMATTRIBUTES_USAGE_TYPE_IMPLICIT_FEEDBACK \
    (1U << 5)

#define BMATTRIBUTES_USAGE_TYPE_RESERVED \
    ((1U << 5) | (1U << 4))

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Returns true if supplied endpoint API was 
 * properly constructed via @ref CUSBD_ENDPOINT_API_CTOR().
 * False otherwise.
 * 
 * @param api API to check.
 */
static bool endpoint_api_valid(const struct cusbd_endpoint_api *api);

/**
 * @brief Returns true if supplied descriptor was properly
 * constructed via @ref CUSBD_ENDPOINT_DESCRIPTOR_CTOR().
 * False otherwise.
 * 
 * @param descriptor Descriptor to check.
 */
static bool endpoint_descriptor_valid(const struct cusbd_endpoint_descriptor *descriptor);

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
/*------------------------------------------------------------*/

static bool endpoint_api_valid(const struct cusbd_endpoint_api *api)
{
    ECU_RUNTIME_ASSERT( (api) );
    bool status = false;

    if (api->configure && 
        api->halt && 
        api->send && 
        api->stall)
    {
        status = true;
    }

    return status;
}

static bool endpoint_descriptor_valid(const struct cusbd_endpoint_descriptor *descriptor)
{
    ECU_RUNTIME_ASSERT( (descriptor) );
    bool status = false;
    uint8_t bEndpointAddress = descriptor->bEndpointAddress;

#pragma message("TODO: Figure out wMaxPacketSize and bInterval")
    if ((descriptor->bLength == sizeof(struct cusbd_endpoint_descriptor)) &&
        (descriptor->bDescriptorType == CUSBD_ENDPOINT_BDESCRIPTORTYPE) &&
        ((descriptor->bEndpointAddress & BENDPOINTADDRESS_NUMBER_BITMASK) > 0U) &&
        ((descriptor->bEndpointAddress & BENDPOINTADDRESS_RESERVED_BITMASK) == 0U))
    {
        switch (bEndpointAddress & BMATTRIBUTES_TRANSFER_TYPE_BITMASK)
        {
            /* Intentional fallthrough. */
            case BMATTRIBUTES_TRANSFER_TYPE_CONTROL:
            case BMATTRIBUTES_TRANSFER_TYPE_BULK:
            case BMATTRIBUTES_TRANSFER_TYPE_INTERRUPT:
            {
                if (((bEndpointAddress & BMATTRIBUTES_SYNC_TYPE_BITMASK) == 0U) ||
                    ((bEndpointAddress & BMATTRIBUTES_USAGE_TYPE_BITMASK) == 0U))
                {
                    status = true;
                }
                break;
            }
    
            case BMATTRIBUTES_TRANSFER_TYPE_ISOCHRONOUS:
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

ECU_STATIC_ASSERT( (sizeof(struct cusbd_endpoint_descriptor) == (size_t)7),
                    "Endpoint descriptor is 7 bytes." );

/*------------------------------------------------------------*/
/*---------------- CUSBD ENDPOINT MEMBER FUNCTIONS -----------*/
/*------------------------------------------------------------*/

void cusbd_endpoint_ctor(struct cusbd_endpoint *me,
                         const struct cusbd_endpoint_api *api,
                         const struct cusbd_endpoint_descriptor *descriptor)
{
    ECU_RUNTIME_ASSERT( (me && descriptor) );
    ECU_RUNTIME_ASSERT( (endpoint_api_valid(api)) );
    ECU_RUNTIME_ASSERT( (endpoint_descriptor_valid(descriptor)) );

    ecu_ntnode_ctor(&me->ntnode, ECU_NTNODE_DESTROY_UNUSED, (ecu_object_id)CUSBD_ENDPOINT_BDESCRIPTORTYPE);
    me->api = api;
    memcpy(&me->descriptor, descriptor, sizeof(struct cusbd_endpoint_descriptor));
    me->halted = false;
}

enum cusbd_endpoint_direction cusbd_endpoint_direction(const struct cusbd_endpoint *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (cusbd_endpoint_valid(me)) );
    enum cusbd_endpoint_direction dir = CUSBD_ENDPOINT_DIRECTION_OUT;

    if (me->descriptor->bEndpointAddress & BENDPOINTADDRESS_DIRECTION_BITMASK)
    {
        dir = CUSBD_ENDPOINT_DIRECTION_IN;
    }
    else
    {
        dir = CUSBD_ENDPOINT_DIRECTION_OUT; /* Just incase. */
    }

    return dir;
}

bool cusbd_endpoint_halted(const struct cusbd_endpoint *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (cusbd_endpoint_valid(me)) );
    return (me->halted);
}

size_t cusbd_endpoint_number(const struct cusbd_endpoint *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (cusbd_endpoint_valid(me)) );
    return (me->descriptor->bEndpointAddress & BENDPOINTADDRESS_NUMBER_BITMASK);
}

enum cusbd_endpoint_sync_type cusbd_endpoint_sync_type(const struct cusbd_endpoint *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (cusbd_endpoint_valid(me)) );
    ECU_RUNTIME_ASSERT( (cusbd_endpoint_transfer_type(me) == CUSBD_ENDPOINT_TRANSFER_TYPE_ISOCHRONOUS) );
    enum cusbd_endpoint_sync_type sync_type = CUSBD_ENDPOINT_SYNC_TYPE_NOSYNC;

    switch (me->descriptor->bmAttributes & BMATTRIBUTES_SYNC_TYPE_BITMASK)
    {
        case BMATTRIBUTES_SYNC_TYPE_NOSYNC:
        {
            sync_type = CUSBD_ENDPOINT_SYNC_TYPE_NOSYNC;
            break;
        }

        case BMATTRIBUTES_SYNC_TYPE_ASYNC:
        {
            sync_type = CUSBD_ENDPOINT_SYNC_TYPE_ASYNC;
            break;
        }

        case BMATTRIBUTES_SYNC_TYPE_ADAPTIVE:
        {
            sync_type = CUSBD_ENDPOINT_SYNC_TYPE_ADAPTIVE;
            break;
        }

        case BMATTRIBUTES_SYNC_TYPE_SYNC:
        {
            sync_type = CUSBD_ENDPOINT_SYNC_TYPE_SYNC;
            break;
        }

        default:
        {
            ECU_RUNTIME_ASSERT( (false) );
            break;
        }
    }

    return sync_type;
}

enum cusbd_endpoint_transfer_type cusbd_endpoint_transfer_type(const struct cusbd_endpoint *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (cusbd_endpoint_valid(me)) );
    enum cusbd_endpoint_transfer_type transfer_type = CUSBD_ENDPOINT_TRANSFER_TYPE_CONTROL;

    switch (me->descriptor->bmAttributes & BMATTRIBUTES_TRANSFER_TYPE_BITMASK)
    {
        case BMATTRIBUTES_TRANSFER_TYPE_CONTROL:
        {
            transfer_type = CUSBD_ENDPOINT_TRANSFER_TYPE_CONTROL;
            break;
        }

        case BMATTRIBUTES_TRANSFER_TYPE_ISOCHRONOUS:
        {
            transfer_type = CUSBD_ENDPOINT_TRANSFER_TYPE_ISOCHRONOUS;
            break;
        }

        case BMATTRIBUTES_TRANSFER_TYPE_BULK:
        {
            transfer_type = CUSBD_ENDPOINT_TRANSFER_TYPE_BULK;
            break;
        }

        case BMATTRIBUTES_TRANSFER_TYPE_INTERRUPT:
        {
            transfer_type = CUSBD_ENDPOINT_TRANSFER_TYPE_INTERRUPT;
            break;
        }

        default:
        {
            ECU_RUNTIME_ASSERT( (false) );
            break;
        }
    }

    return transfer_type;
}

enum cusbd_endpoint_usage_type cusbd_endpoint_usage_type(const struct cusbd_endpoint *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (cusbd_endpoint_valid(me)) );
    ECU_RUNTIME_ASSERT( (cusbd_endpoint_transfer_type(me) == CUSBD_ENDPOINT_TRANSFER_TYPE_ISOCHRONOUS) );
    enum cusbd_endpoint_usage_type usage_type = CUSBD_ENDPOINT_USAGE_TYPE_RESERVED;

    switch (me->descriptor->bmAttributes & BMATTRIBUTES_USAGE_TYPE_BITMASK)
    {
        case BMATTRIBUTES_USAGE_TYPE_DATA:
        {
            usage_type = CUSBD_ENDPOINT_USAGE_TYPE_DATA;
            break;
        }

        case BMATTRIBUTES_USAGE_TYPE_FEEDBACK:
        {
            usage_type = CUSBD_ENDPOINT_USAGE_TYPE_FEEDBACK;
            break;
        }

        case BMATTRIBUTES_USAGE_TYPE_IMPLICIT_FEEDBACK:
        {
            usage_type = CUSBD_ENDPOINT_USAGE_TYPE_IMPLICIT_FEEDBACK;
            break;
        }

        case BMATTRIBUTES_USAGE_TYPE_RESERVED:
        {
            ECU_RUNTIME_ASSERT( (false) );
            usage_type = CUSBD_ENDPOINT_USAGE_TYPE_RESERVED;
            break;
        }

        default:
        {
            ECU_RUNTIME_ASSERT( (false) );
            break;
        }
    }

    return usage_type;
}

bool cusbd_endpoint_valid(const struct cusbd_endpoint *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return (ecu_ntnode_valid(&me->ntnode) &&
            ecu_ntnode_id(&me->ntnode) == (ecu_object_id)CUSBD_ENDPOINT_BDESCRIPTORTYPE &&
            endpoint_api_valid(&me->api) &&
            endpoint_descriptor_valid(&me->descriptor));
}
