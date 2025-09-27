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
#include <string.h>

/* CUSB. */
#include "cusbd/setup_packet.h"

/* ECU. */
#include "ecu/asserter.h"
#include "ecu/endian.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_FILE("cusbd/endpoint.c")

/*------------------------------------------------------------*/
/*--------------------- DEFINES - BITMASKS -------------------*/
/*------------------------------------------------------------*/

/**@{*/
/// @brief Bitmasks for endpoint descriptor data fields.
#define BENDPOINTADDRESS_NUMBER_BITMASK ((1U << 3) | (1U << 2) | (1U << 1) | (1U << 0))
#define BENDPOINTADDRESS_RESERVED_BITMASK ((1U << 6) | (1U << 5) | (1U << 4))
#define BENDPOINTADDRESS_DIRECTION_BITMASK (1U << 7)
#define BMATTRIBUTES_TRANSFER_TYPE_BITMASK ((1U << 1) | (1U << 0))
#define BMATTRIBUTES_SYNC_TYPE_BITMASK ((1U << 3) | (1U << 2))
#define BMATTRIBUTES_USAGE_TYPE_BITMASK ((1U << 5) | (1U << 4))
/**@}*/

/*------------------------------------------------------------*/
/*---------------------- DEFINES - VALUES --------------------*/
/*------------------------------------------------------------*/

/// @brief bDescriptorType value of endpoint descriptors.
#define BDESCRIPTORTYPE_ENDPOINT ((uint8_t)0x05)

/**@{*/
/// @brief Values for endpoint descriptor data fields.
#define BMATTRIBUTES_TRANSFER_TYPE_CONTROL (0U)
#define BMATTRIBUTES_TRANSFER_TYPE_ISOCHRONOUS (1U << 0)
#define BMATTRIBUTES_TRANSFER_TYPE_BULK (1U << 1)
#define BMATTRIBUTES_TRANSFER_TYPE_INTERRUPT ((1U << 1) | (1U << 0))
#define BMATTRIBUTES_SYNC_TYPE_NOSYNC (0U)
#define BMATTRIBUTES_SYNC_TYPE_ASYNC (1U << 2)
#define BMATTRIBUTES_SYNC_TYPE_ADAPTIVE (1U << 3)
#define BMATTRIBUTES_SYNC_TYPE_SYNC ((1U << 3) | (1U << 2))
#define BMATTRIBUTES_USAGE_TYPE_DATA (0U)
#define BMATTRIBUTES_USAGE_TYPE_FEEDBACK (1U << 4)
#define BMATTRIBUTES_USAGE_TYPE_IMPLICIT_FEEDBACK (1U << 5)
#define BMATTRIBUTES_USAGE_TYPE_RESERVED ((1U << 5) | (1U << 4))
/**@}*/

/**@{*/
/// @brief bRequest values relevant to standard endpoint descriptors. 
/// This is in the setup packet received in the status stage of 
/// the control transfer.
#define BREQUEST_GET_STATUS ((uint8_t)0)
#define BREQUEST_CLEAR_FEATURE ((uint8_t)1)
#define BREQUEST_SET_FEATURE ((uint8_t)3)
#define BREQUEST_SYNCH_FRAME ((uint8_t)12)
/**@}*/

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

/**@{*/
/// @brief Overrides for descriptors virtual table. See @ref cusbd_descriptor_vtable
static size_t endpoint_v_process_setup_packet_in(const struct cusbd_endpoint *me, 
                                                 const struct cusbd_setup_packet *packet, 
                                                 enum cusbd_descriptor_state state,
                                                 void *buffer, 
                                                 size_t buffer_len);
static bool endpoint_v_process_setup_packet_out(struct cusbd_endpoint *me, 
                                                const struct cusbd_setup_packet *packet,
                                                enum cusbd_descriptor_state state);
static void endpoint_v_string_index_set(struct cusbd_endpoint *me, size_t index);
static size_t endpoint_v_w_total_length(const struct cusbd_endpoint *me);
/**@}*/

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
/*------------------------------------------------------------*/

static bool endpoint_api_valid(const struct cusbd_endpoint_api *api)
{
    ECU_ASSERT( (api) );
    bool status = false;

    if (api->configure && 
        api->deconfigure &&
        api->halt && 
        api->send && 
        api->stall &&
        api->unhalt)
    {
        status = true;
    }

    return status;
}

static bool endpoint_descriptor_valid(const struct cusbd_endpoint_descriptor *descriptor)
{
    ECU_ASSERT( (descriptor) );
    bool status = false;
    uint8_t bEndpointAddress = descriptor->bEndpointAddress;

    /* wMaxPacketSize and bInterval not asserted since they may be different across standards. */
    if ((descriptor->bLength == sizeof(*descriptor)) &&
        (descriptor->bDescriptorType == BDESCRIPTORTYPE_ENDPOINT) &&
        ((descriptor->bEndpointAddress & BENDPOINTADDRESS_RESERVED_BITMASK) == 0U))
    {
        switch (bEndpointAddress & BMATTRIBUTES_TRANSFER_TYPE_BITMASK)
        {
            /* Intentional fallthrough. */
            case BMATTRIBUTES_TRANSFER_TYPE_CONTROL:
            case BMATTRIBUTES_TRANSFER_TYPE_BULK:
            case BMATTRIBUTES_TRANSFER_TYPE_INTERRUPT:
            {
                if (((bEndpointAddress & BMATTRIBUTES_SYNC_TYPE_BITMASK) == 0U) &&
                    ((bEndpointAddress & BMATTRIBUTES_USAGE_TYPE_BITMASK) == 0U))
                {
                    /* Reserved fields and must be set to 0 for non-isochronous endpoints. */
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
                ECU_ASSERT( (false) );
            }
        }
    }

    return status;
}

static size_t endpoint_v_process_setup_packet_in(const struct cusbd_endpoint *me, 
                                                 const struct cusbd_setup_packet *packet, 
                                                 enum cusbd_descriptor_state state,
                                                 void *buffer, 
                                                 size_t buffer_len)
{
    ECU_ASSERT( (me && packet && buffer) );
    ECU_ASSERT( (cusbd_endpoint_valid(me)) );
    ECU_ASSERT( (cusbd_setup_packet_direction(packet) == CUSBD_SETUP_PACKET_DIRECTION_IN) );
    ECU_ASSERT( (state < CUSBD_DESCRIPTOR_STATE_COUNT) );
    ECU_ASSERT( (buffer_len > 0) );
    size_t num_bytes_written = 0;

    if ((cusbd_setup_packet_recipient(packet) == CUSBD_SETUP_PACKET_RECIPIENT_ENDPOINT) &&
        (cusbd_setup_packet_type(packet) == CUSBD_SETUP_PACKET_TYPE_STANDARD))
    {
        /* Packet addressed to standard endpoint. */
        switch (cusbd_setup_packet_b_request(packet))
        {
            case BREQUEST_GET_STATUS:
            {
                if (state == CUSBD_DESCRIPTOR_STATE_ADDRESS)
                {
                    if ((cusbd_endpoint_number(me) == 0) && /* This endpoint is endoint 0. */
                        (cusbd_setup_packet_w_index(packet) == 0) && /* Check first. */
                        (cusbd_setup_packet_w_value(packet) == 0) &&
                        (cusbd_setup_packet_w_length(packet) == 2))
                    {
                        /* Packet for this endpoint (endpoint0) and its contents are valid. Send status
                        which should always be 0 (unhalted) since USB standard does not recommend halting
                        endpoint 0. */
                        ECU_ASSERT( (!cusbd_endpoint_halted(me)) );
                        uint16_t status = 0;
                        ECU_STATIC_ASSERT( (sizeof(status) == sizeof(uint16_t)), "Value must be uint16_t.");
                        ECU_ASSERT( (buffer_len >= sizeof(status)) );
                        memcpy(buffer, &status, sizeof(status));
                        num_bytes_written = sizeof(status);
                    } 
                }
                else if (state == CUSBD_DESCRIPTOR_STATE_CONFIGURED)
                {
                    uint16_t wIndex = cusbd_setup_packet_w_index(packet);

                    if ((uint16_t)cusbd_endpoint_number(me) == wIndex)
                    {
                        /* Packet is for this endpoint. */

                        if ((cusbd_setup_packet_w_value(packet) == 0) &&
                            (cusbd_setup_packet_w_length(packet) == 2))
                        {
                            /* Rest of packet is valid. Send status. */
                            uint16_t status = 0;

                            if (cusbd_endpoint_halted(me))
                            {
                                status |= (1U << 0);
                            }

                            ECU_STATIC_ASSERT( (sizeof(status) == sizeof(uint16_t)), "Value must be uint16_t.");
                            ECU_ASSERT( (buffer_len >= sizeof(status)) );
                            memcpy(buffer, &status, sizeof(status));
                            num_bytes_written = sizeof(status);
                        }
                    }
                }
                else
                {
                    /* Request not valid in default state. */
                }
                break;
            }

            case BREQUEST_SYNCH_FRAME:
            {!!! TODO STopped here !!!!
                break;
            }

            default:
            {
                /* Unsupported request. */
                break;
            }
        }
    }

    return num_bytes_written;
}

static bool endpoint_v_process_setup_packet_out(struct cusbd_endpoint *me, 
                                                const struct cusbd_setup_packet *packet,
                                                enum cusbd_descriptor_state state)
{

}

static void endpoint_v_string_index_set(struct cusbd_endpoint *me, size_t index)
{

}

static size_t endpoint_v_w_total_length(const struct cusbd_endpoint *me)
{

}

/*------------------------------------------------------------*/
/*---------------------- STATIC ASSERTS ----------------------*/
/*------------------------------------------------------------*/

ECU_STATIC_ASSERT( (sizeof(struct cusbd_endpoint_descriptor) == (size_t)7),
                    "Endpoint descriptor is 7 bytes." );

ECU_STATIC_ASSERT( (ECU_FIELD_SIZEOF(struct cusbd_endpoint, descriptor) == (size_t)7),
                    "Endpoint descriptor is 7 bytes." );

ECU_STATIC_ASSERT( (CUSBD_DESCRIPTOR_IS_BASE_OF(base, struct cusbd_endpoint)),
                    "Endpoint descriptor must inherit cusbd_descriptor." );

/*------------------------------------------------------------*/
/*---------------- CUSBD ENDPOINT MEMBER FUNCTIONS -----------*/
/*------------------------------------------------------------*/

void cusbd_endpoint_ctor(struct cusbd_endpoint *me,
                         const struct cusbd_endpoint_api *api,
                         uint8_t bEndpointAddress,
                         uint8_t bmAttributes,
                         uint16_t wMaxPacketSize,
                         uint8_t bInterval)
{
    ECU_ASSERT( (me && api) );
    ECU_ASSERT( (endpoint_api_valid(api)) );

    static const struct cusbd_descriptor_vtable vtable = CUSBD_DESCRIPTOR_VTABLE_CTOR(
        &endpoint_v_process_setup_packet_in, &endpoint_v_process_setup_packet_out, &endpoint_v_string_index_set, &endpoint_v_w_total_length
    );

    /* Base class. */
    cusbd_descriptor_ctor(CUSBD_DESCRIPTOR_BASE_CAST(me), CUSBD_DESCRIPTOR_ID_ENDPOINT, &me->descriptor, sizeof(me->descriptor));
    me->base.vptr = &vtable;

    /* API. */
    me->api = api;

    /* Descriptor data. */
    memset(&me->descriptor, 0, sizeof(me->descriptor));
    me->descriptor.bLength = sizeof(me->descriptor);
    me->descriptor.bDescriptorType = BDESCRIPTORTYPE_ENDPOINT;
    me->descriptor.bEndpointAddress = bEndpointAddress;
    me->descriptor.bmAttributes = bmAttributes;
    me->descriptor.wMaxPacketSize = ECU_CPU_TO_LE16_RUNTIME(wMaxPacketSize);
    me->descriptor.bInterval = bInterval;
    ECU_ASSERT( (endpoint_descriptor_valid(&me->descriptor)) );

    /* Rest of object. */
    me->halted = false;
}

void cusbd_endpoint_configure(struct cusbd_endpoint *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (cusbd_endpoint_valid(me)) );
    (*me->api->configure)(me, me->api->obj);
}

void cusbd_endpoint_deconfigure(struct cusbd_endpoint *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (cusbd_endpoint_valid(me)) );
    (*me->api->deconfigure)(me, me->api->obj);
}

enum cusbd_endpoint_direction cusbd_endpoint_direction(const struct cusbd_endpoint *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (cusbd_endpoint_valid(me)) );
    enum cusbd_endpoint_direction dir = CUSBD_ENDPOINT_DIRECTION_OUT;

    if (me->descriptor.bEndpointAddress & BENDPOINTADDRESS_DIRECTION_BITMASK)
    {
        dir = CUSBD_ENDPOINT_DIRECTION_IN;
    }
    else
    {
        dir = CUSBD_ENDPOINT_DIRECTION_OUT; /* Just incase. */
    }

    return dir;
}

void cusbd_endpoint_halt(struct cusbd_endpoint *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (cusbd_endpoint_valid(me)) );
    ECU_ASSERT( (cusbd_endpoint_number(me) != 0) ); /* Halting for endpoint0 is not recommended by USB standard. */
    (*me->api->halt)(me, me->api->obj);
    me->halted = true;
}

bool cusbd_endpoint_halted(const struct cusbd_endpoint *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (cusbd_endpoint_valid(me)) );
    return (me->halted);
}

uint8_t cusbd_endpoint_number(const struct cusbd_endpoint *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (cusbd_endpoint_valid(me)) );
    return (me->descriptor.bEndpointAddress & BENDPOINTADDRESS_NUMBER_BITMASK);
}

void cusbd_endpoint_send(struct cusbd_endpoint *me, const void *data, size_t len)
{
    ECU_ASSERT( (me && data) );
    ECU_ASSERT( (cusbd_endpoint_valid(me)) );
    (*me->api->send)(me, data, len, me->api->obj);
}

void cusbd_endpoint_stall(struct cusbd_endpoint *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (cusbd_endpoint_valid(me)) );
    (*me->api->stall)(me, me->api->obj);
}

enum cusbd_endpoint_sync_type cusbd_endpoint_sync_type(const struct cusbd_endpoint *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (cusbd_endpoint_valid(me)) );
    enum cusbd_endpoint_sync_type sync_type = CUSBD_ENDPOINT_SYNC_TYPE_NOSYNC;

    switch (me->descriptor.bmAttributes & BMATTRIBUTES_SYNC_TYPE_BITMASK)
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
            ECU_ASSERT( (false) );
            break;
        }
    }

    return sync_type;
}

enum cusbd_endpoint_transfer_type cusbd_endpoint_transfer_type(const struct cusbd_endpoint *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (cusbd_endpoint_valid(me)) );
    enum cusbd_endpoint_transfer_type transfer_type = CUSBD_ENDPOINT_TRANSFER_TYPE_CONTROL;

    switch (me->descriptor.bmAttributes & BMATTRIBUTES_TRANSFER_TYPE_BITMASK)
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
            ECU_ASSERT( (false) );
            break;
        }
    }

    return transfer_type;
}

void cusbd_endpoint_unhalt(struct cusbd_endpoint *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (cusbd_endpoint_valid(me)) );
    (*me->api->unhalt)(me, me->api->obj);
    me->halted = false;
}

enum cusbd_endpoint_usage_type cusbd_endpoint_usage_type(const struct cusbd_endpoint *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (cusbd_endpoint_valid(me)) );
    enum cusbd_endpoint_usage_type usage_type = CUSBD_ENDPOINT_USAGE_TYPE_RESERVED;

    switch (me->descriptor.bmAttributes & BMATTRIBUTES_USAGE_TYPE_BITMASK)
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
            ECU_ASSERT( (false) );
            usage_type = CUSBD_ENDPOINT_USAGE_TYPE_RESERVED;
            break;
        }

        default:
        {
            ECU_ASSERT( (false) );
            break;
        }
    }

    return usage_type;
}

bool cusbd_endpoint_valid(const struct cusbd_endpoint *me)
{
    ECU_ASSERT( (me) );
    bool status = false;

    if ((cusbd_descriptor_valid(CUSBD_DESCRIPTOR_BASE_CAST(me))) &&
        (cusbd_descriptor_id(CUSBD_DESCRIPTOR_BASE_CAST(me)) == CUSBD_DESCRIPTOR_ID_ENDPOINT) &&
        (endpoint_api_valid(me->api)) &&
        (endpoint_descriptor_valid(&me->descriptor)))
    {
        status = true;
    }

    return status;
}
