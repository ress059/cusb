/**
 * @file
 * @brief See @ref src/cusbd/visitor/request.h.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-07-03
 * @copyright Copyright (c) 2025
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* Translation unit. */
#include "cusbd/visitor/request.h"

/* ECU. */
#include "ecu/asserter.h"
#include "ecu/endian.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_NAME("cusbd/visitor/request.c.")

/*------------------------------------------------------------*/
/*--------------------- DEFINES - BITMASKS -------------------*/
/*------------------------------------------------------------*/

#define BMREQUESTTYPE_DIRECTION_BITMASK \ 
    (1U << 7)

#define BMREQUESTTYPE_TYPE_BITMASK \
    ((1U << 6) | (1U << 5))

#define BMREQUESTTYPE_RECIPIENT_BITMASK \
    ((1U << 4) | (1U << 3) | (1U << 2) | (1U << 1) | (1U << 0))

/*------------------------------------------------------------*/
/*---------------------- DEFINES - VALUES --------------------*/
/*------------------------------------------------------------*/

#define BMREQUESTTYPE_TYPE_STANDARD (0U)
#define BMREQUESTTYPE_TYPE_CLASS (1U << 5)
#define BMREQUESTTYPE_TYPE_VENDOR (2U << 5)
#define BMREQUESTTYPE_TYPE_RESERVED (3U << 5)
#define BMREQUESTTYPE_RECIPIENT_DEVICE (0U)
#define BMREQUESTTYPE_RECIPIENT_INTERFACE (1U)
#define BMREQUESTTYPE_RECIPIENT_ENDPOINT (2U)
#define BMREQUESTTYPE_RECIPIENT_OTHER (3U)
#define BREQUEST_GET_STATUS (0U)
#define BREQUEST_CLEAR_FEATURE (1U)
#define BREQUEST_SET_FEATURE (3U)
#define BREQUEST_SET_ADDRESS (5U)
#define BREQUEST_GET_DESCRIPTOR (6U)
#define BREQUEST_SET_DESCRIPTOR (7U)
#define BREQUEST_GET_CONFIGURATION (8U)
#define BREQUEST_SET_CONFIGURATION (9U)
#define BREQUEST_GET_INTERFACE (10U)
#define BREQUEST_SET_INTERFACE (11U)
#define BREQUEST_SYNCH_FRAME (12U)

/*------------------------------------------------------------*/
/*---------------------- STATIC ASSERTS ----------------------*/
/*------------------------------------------------------------*/

ECU_STATIC_ASSERT( (CUSBD_REQUEST_STATE_RESERVED == 0), 
                    "cusbd_request_state enum must default initialize to CUSBD_REQUEST_STATE_RESERVED so visitors "
                    "can detect if request was not constructed (default case for state variable)." );

/*------------------------------------------------------------*/
/*--------------- CUSBD REQUEST MEMBER FUNCTIONS -------------*/
/*------------------------------------------------------------*/

void cusbd_request_ctor(struct cusbd_request *me, 
                        const struct cusbd_setup_packet_rx_event *e,
                        enum cusbd_request_state state)
{
    ECU_RUNTIME_ASSERT( (me && e) );
    ECU_RUNTIME_ASSERT( (state > CUSBD_REQUEST_STATE_RESERVED && state < CUSBD_REQUEST_STATE_COUNT) );

    /* Perform translations. Limiting it here protects library from any changes
    to USB standard, etc. Changepoint would be limited to only here as opposed to
    throughout the library code. */
    /* bmRequestType.direction. */
    if (e->packet.bmRequestType & BMREQUESTTYPE_DIRECTION_BITMASK)
    {
        me->direction = CUSBD_REQUEST_DIRECTION_IN;
    }
    else
    {
        me->direction = CUSBD_REQUEST_DIRECTION_OUT;
    }

    /* bmRequestType.type. */
    switch (e->packet.bmRequestType & BMREQUESTTYPE_TYPE_BITMASK)
    {
        case BMREQUESTTYPE_TYPE_STANDARD:
        {
            me->type = CUSBD_REQUEST_TYPE_STANDARD;
            break;
        }

        case BMREQUESTTYPE_TYPE_CLASS:
        {
            me->type = CUSBD_REQUEST_TYPE_CLASS;
            break;
        }

        case BMREQUESTTYPE_TYPE_VENDOR:
        {
            me->type = CUSBD_REQUEST_TYPE_VENDOR;
            break;
        }

        case BMREQUESTTYPE_TYPE_RESERVED:
        {
            me->type = CUSBD_REQUEST_TYPE_RESERVED;
            break;
        }

        default:
        {
            /* No other values should be possible since limited to 0b11. */
            ECU_RUNTIME_ASSERT( (false) );
            break;
        }
    }

    /* bmRequestType.recipient. */
    switch (e->packet.bmRequestType & BMREQUESTTYPE_RECIPIENT_BITMASK)
    {
        case BMREQUESTTYPE_RECIPIENT_DEVICE:
        {
            me->recipient = CUSBD_REQUEST_RECIPIENT_DEVICE;
            break;
        }

        case BMREQUESTTYPE_RECIPIENT_INTERFACE:
        {
            me->recipient = CUSBD_REQUEST_RECIPIENT_INTERFACE;
            break;
        }

        case BMREQUESTTYPE_RECIPIENT_ENDPOINT:
        {
            me->recipient = CUSBD_REQUEST_RECIPIENT_ENDPOINT;
            break;
        }

        case BMREQUESTTYPE_RECIPIENT_OTHER:
        {
            me->recipient = CUSBD_REQUEST_RECIPIENT_OTHER;
            break;
        }

        default:
        {
            me->recipient = CUSBD_REQUEST_TYPE_RESERVED;
            break;
        }
    }

    /* bRequest. */
    switch (e->packet.bRequest)
    {
        case BREQUEST_GET_STATUS:
        {
            me->value = CUSBD_REQUEST_VALUE_GET_STATUS;
            break;
        }

        case BREQUEST_CLEAR_FEATURE:
        {
            me->value = CUSBD_REQUEST_VALUE_CLEAR_FEATURE;
            break;
        }

        case BREQUEST_SET_FEATURE:
        {
            me->value = CUSBD_REQUEST_VALUE_SET_FEATURE;
            break;
        }

        case BREQUEST_SET_ADDRESS:
        {
            me->value = CUSBD_REQUEST_VALUE_SET_ADDRESS;
            break;
        }
        
        case BREQUEST_GET_DESCRIPTOR:
        {
            me->value = CUSBD_REQUEST_VALUE_GET_DESCRIPTOR;
            break;
        }

        case BREQUEST_SET_DESCRIPTOR:
        {
            me->value = CUSBD_REQUEST_VALUE_SET_DESCRIPTOR;
            break;
        }

        case BREQUEST_GET_CONFIGURATION:
        {
            me->value = CUSBD_REQUEST_VALUE_GET_CONFIGURATION;
            break;
        }

        case BREQUEST_SET_CONFIGURATION:
        {
            me->value = CUSBD_REQUEST_VALUE_SET_CONFIGURATION;
            break;
        }

        case BREQUEST_GET_INTERFACE:
        {
            me->value = CUSBD_REQUEST_VALUE_GET_INTERFACE;
            break;
        }

        case BREQUEST_SET_INTERFACE:
        {
            me->value = CUSBD_REQUEST_VALUE_SET_INTERFACE;
            break;
        }

        default:
        {
            me->value = CUSBD_REQUEST_VALUE_RESERVED;
            break;
        }
    }

    me->wValue = ECU_LE16_TO_CPU_RUNTIME(e->packet.wValue);
    me->wIndex = ECU_LE16_TO_CPU_RUNTIME(e->packet.wIndex);
    me->wLength = ECU_LE16_TO_CPU_RUNTIME(e->packet.wLength);
    me->state = state;
    me->status = CUSBD_REQUEST_STATUS_UNPROCESSED;
}

enum cusbd_request_direction cusbd_request_direction(const struct cusbd_request *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return (me->direction);
}

bool cusbd_request_done(const struct cusbd_request *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return (me->status != CUSBD_REQUEST_STATUS_UNPROCESSED);
}

enum cusbd_request_recipient cusbd_request_recipient(const struct cusbd_request *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return (me->recipient);
}

enum cusbd_request_state cusbd_request_state(const struct cusbd_request *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return (me->state);
}

void cusbd_request_set_status(struct cusbd_request *me, enum cusbd_request_status status)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (status >= 0 && status < CUSBD_REQUEST_STATUS_COUNT) );
    me->status = status;
}

enum cusbd_request_status cusbd_request_status(const struct cusbd_request *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return (me->status);
}

enum cusbd_request_type cusbd_request_type(const struct cusbd_request *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return (me->type);
}

enum cusbd_request_value cusbd_request_value(const struct cusbd_request *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return (me->value);
}

uint16_t cusbd_request_w_index(const struct cusbd_request *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return (me->wIndex);
}

uint16_t cusbd_request_w_length(const struct cusbd_request *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return (me->wLength);
}

uint16_t cusbd_request_w_value(const struct cusbd_request *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return (me->wValue);
}
