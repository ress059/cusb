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
#include "cusbd/request.h"

/* ECU. */
#include "ecu/asserter.h"
#include "ecu/endian.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_NAME("cusbd/request.c.")

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
/*--------------- CUSBD REQUEST MEMBER FUNCTIONS -------------*/
/*------------------------------------------------------------*/

enum cusbd_rx_request_event_direction cusbd_rx_request_event_direction(const struct cusbd_rx_request_event *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    enum cusbd_rx_request_event_direction direction = CUSBD_RX_REQUEST_EVENT_DIRECTION_IN;
    bool in = (me->packet.bmRequestType & BMREQUESTTYPE_DIRECTION_BITMASK); /* bmRequestType.direction. */

    if (in) 
    {
        direction = CUSBD_RX_REQUEST_EVENT_DIRECTION_IN;
    }
    else
    {
        direction = CUSBD_RX_REQUEST_EVENT_DIRECTION_OUT;
    }

    return direction;
}

enum cusbd_rx_request_event_recipient cusbd_rx_request_event_recipient(const struct cusbd_rx_request_event *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    enum cusbd_rx_request_event_recipient recipient = CUSBD_RX_REQUEST_EVENT_RECIPIENT_RESERVED;
    uint8_t data = me->packet.bmRequestType & BMREQUESTTYPE_RECIPIENT_BITMASK; /* bmRequestType.recipient. */
    
    switch (data) 
    {
        case BMREQUESTTYPE_RECIPIENT_DEVICE:
        {
            recipient = CUSBD_RX_REQUEST_EVENT_RECIPIENT_DEVICE;
            break;
        }

        case BMREQUESTTYPE_RECIPIENT_INTERFACE:
        {
            recipient = CUSBD_RX_REQUEST_EVENT_RECIPIENT_INTERFACE;
            break;
        }

        case BMREQUESTTYPE_RECIPIENT_ENDPOINT:
        {
            recipient = CUSBD_RX_REQUEST_EVENT_RECIPIENT_ENDPOINT;
            break;
        }

        case BMREQUESTTYPE_RECIPIENT_OTHER:
        {
            recipient = CUSBD_RX_REQUEST_EVENT_RECIPIENT_OTHER;
            break;
        }

        default:
        {
            recipient = CUSBD_RX_REQUEST_EVENT_RECIPIENT_RESERVED;
            break;
        }
    }

    return recipient;
}

enum cusbd_rx_request_event_type cusbd_rx_request_event_type(const struct cusbd_rx_request_event *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    enum cusbd_rx_request_event_type type = CUSBD_RX_REQUEST_EVENT_TYPE_RESERVED;
    uint8_t data = me->packet.bmRequestType & BMREQUESTTYPE_TYPE_BITMASK; /* bmRequestType.type. */

    switch (data)
    {
        case BMREQUESTTYPE_TYPE_STANDARD:
        {
            type = CUSBD_RX_REQUEST_EVENT_TYPE_STANDARD;
            break;
        }

        case BMREQUESTTYPE_TYPE_CLASS:
        {
            type = CUSBD_RX_REQUEST_EVENT_TYPE_CLASS;
            break;
        }

        case BMREQUESTTYPE_TYPE_VENDOR:
        {
            type = CUSBD_RX_REQUEST_EVENT_TYPE_VENDOR;
            break;
        }

        case BMREQUESTTYPE_TYPE_RESERVED:
        {
            type = CUSBD_RX_REQUEST_EVENT_TYPE_RESERVED;
            break;
        }

        default:
        {
            /* No other values should be possible since limited to 0b11. */
            ECU_RUNTIME_ASSERT( (false) );
            break;
        }
    }

    return type;
}

enum cusbd_rx_request_event_value cusbd_rx_request_event_value(const struct cusbd_rx_request_event *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    enum cusbd_rx_request_event_value value = CUSBD_RX_REQUEST_EVENT_VALUE_RESERVED;

    /* bRequest. */
    switch (me->packet.bRequest)
    {
        case BREQUEST_GET_STATUS:
        {
            value = CUSBD_RX_REQUEST_EVENT_VALUE_GET_STATUS;
            break;
        }

        case BREQUEST_CLEAR_FEATURE:
        {
            value = CUSBD_RX_REQUEST_EVENT_VALUE_CLEAR_FEATURE;
            break;
        }

        case BREQUEST_SET_FEATURE:
        {
            value = CUSBD_RX_REQUEST_EVENT_VALUE_SET_FEATURE;
            break;
        }

        case BREQUEST_SET_ADDRESS:
        {
            value = CUSBD_RX_REQUEST_EVENT_VALUE_SET_ADDRESS;
            break;
        }
        
        case BREQUEST_GET_DESCRIPTOR:
        {
            value = CUSBD_RX_REQUEST_EVENT_VALUE_GET_DESCRIPTOR;
            break;
        }

        case BREQUEST_SET_DESCRIPTOR:
        {
            value = CUSBD_RX_REQUEST_EVENT_VALUE_SET_DESCRIPTOR;
            break;
        }

        case BREQUEST_GET_CONFIGURATION:
        {
            value = CUSBD_RX_REQUEST_EVENT_VALUE_GET_CONFIGURATION;
            break;
        }

        case BREQUEST_SET_CONFIGURATION:
        {
            value = CUSBD_RX_REQUEST_EVENT_VALUE_SET_CONFIGURATION;
            break;
        }

        case BREQUEST_GET_INTERFACE:
        {
            value = CUSBD_RX_REQUEST_EVENT_VALUE_GET_INTERFACE;
            break;
        }

        case BREQUEST_SET_INTERFACE:
        {
            value = CUSBD_RX_REQUEST_EVENT_VALUE_SET_INTERFACE;
            break;
        }

        default:
        {
            value = CUSBD_RX_REQUEST_EVENT_VALUE_RESERVED;
            break;
        }
    }

    return value;
}

uint16_t cusbd_rx_request_event_w_index(const struct cusbd_rx_request_event *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    uint16_t wIndex = ECU_LE16_TO_CPU_RUNTIME(me->packet.wIndex);
    return wIndex;
}

uint16_t cusbd_rx_request_event_w_length(const struct cusbd_rx_request_event *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    uint16_t wLength = ECU_LE16_TO_CPU_RUNTIME(me->packet.wLength);
    return wLength;
}

uint16_t cusbd_rx_request_event_w_value(const struct cusbd_rx_request_event *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    uint16_t wValue = ECU_LE16_TO_CPU_RUNTIME(me->packet.wValue);
    return wValue;
}
