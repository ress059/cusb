/**
 * @file
 * @brief See @ref setup_packet.h.
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
#include "cusbd/setup_packet.h"

/* ECU. */
#include "ecu/asserter.h"
#include "ecu/endian.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_FILE("cusbd/request.c.")

/*------------------------------------------------------------*/
/*--------------------- DEFINES - BITMASKS -------------------*/
/*------------------------------------------------------------*/

#define BMREQUESTTYPE_DIRECTION_BITMASK (1U << 7)
#define BMREQUESTTYPE_TYPE_BITMASK ((1U << 6) | (1U << 5))
#define BMREQUESTTYPE_RECIPIENT_BITMASK ((1U << 4) | (1U << 3) | (1U << 2) | (1U << 1) | (1U << 0))

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

/*------------------------------------------------------------*/
/*------------ CUSBD_SETUP_PACKET MEMBER FUNCTIONS -----------*/
/*------------------------------------------------------------*/

enum cusbd_setup_packet_direction cusbd_setup_packet_direction(const struct cusbd_setup_packet *me)
{
    ECU_ASSERT( (me) );
    enum cusbd_setup_packet_direction direction = CUSBD_SETUP_PACKET_DIRECTION_IN;
    bool in = (me->bmRequestType & BMREQUESTTYPE_DIRECTION_BITMASK); /* bmRequestType.direction. */

    if (in) 
    {
        direction = CUSBD_SETUP_PACKET_DIRECTION_IN;
    }
    else
    {
        direction = CUSBD_SETUP_PACKET_DIRECTION_OUT;
    }

    return direction;
}

enum cusbd_setup_packet_recipient cusbd_setup_packet_recipient(const struct cusbd_setup_packet *me)
{
    ECU_ASSERT( (me) );
    enum cusbd_setup_packet_recipient recipient = CUSBD_SETUP_PACKET_RECIPIENT_RESERVED;
    uint8_t data = me->bmRequestType & BMREQUESTTYPE_RECIPIENT_BITMASK; /* bmRequestType.recipient. */
    
    switch (data) 
    {
        case BMREQUESTTYPE_RECIPIENT_DEVICE:
        {
            recipient = CUSBD_SETUP_PACKET_RECIPIENT_DEVICE;
            break;
        }

        case BMREQUESTTYPE_RECIPIENT_INTERFACE:
        {
            recipient = CUSBD_SETUP_PACKET_RECIPIENT_INTERFACE;
            break;
        }

        case BMREQUESTTYPE_RECIPIENT_ENDPOINT:
        {
            recipient = CUSBD_SETUP_PACKET_RECIPIENT_ENDPOINT;
            break;
        }

        case BMREQUESTTYPE_RECIPIENT_OTHER:
        {
            recipient = CUSBD_SETUP_PACKET_RECIPIENT_OTHER;
            break;
        }

        default:
        {
            recipient = CUSBD_REQUEST_RECIPIENT_RESERVED;
            break;
        }
    }

    return recipient;
}

enum cusbd_setup_packet_type cusbd_setup_packet_type(const struct cusbd_setup_packet *me)
{
    ECU_ASSERT( (me) );
    enum cusbd_setup_packet_type type = CUSBD_SETUP_PACKET_TYPE_RESERVED;
    uint8_t data = me->bmRequestType & BMREQUESTTYPE_TYPE_BITMASK; /* bmRequestType.type. */

    switch (data)
    {
        case BMREQUESTTYPE_TYPE_STANDARD:
        {
            type = CUSBD_SETUP_PACKET_TYPE_STANDARD;
            break;
        }

        case BMREQUESTTYPE_TYPE_CLASS:
        {
            type = CUSBD_SETUP_PACKET_TYPE_CLASS;
            break;
        }

        case BMREQUESTTYPE_TYPE_VENDOR:
        {
            type = CUSBD_SETUP_PACKET_TYPE_VENDOR;
            break;
        }

        case BMREQUESTTYPE_TYPE_RESERVED:
        {
            type = CUSBD_SETUP_PACKET_TYPE_RESERVED;
            break;
        }

        default:
        {
            /* No other values should be possible since limited to 0b11. */
            ECU_ASSERT( (false) );
            break;
        }
    }

    return type;
}

uint8_t cusbd_setup_packet_b_request(const struct cusbd_setup_packet *me)
{
    ECU_ASSERT( (me) );
    return me->bRequest;
}

uint16_t cusbd_setup_packet_w_index(const struct cusbd_setup_packet *me)
{
    ECU_ASSERT( (me) );
    uint16_t wIndex = ECU_LE16_TO_CPU_RUNTIME(me->wIndex);
    return wIndex;
}

uint16_t cusbd_setup_packet_w_length(const struct cusbd_setup_packet *me)
{
    ECU_ASSERT( (me) );
    uint16_t wLength = ECU_LE16_TO_CPU_RUNTIME(me->wLength);
    return wLength;
}

uint16_t cusbd_setup_packet_w_value(const struct cusbd_setup_packet *me)
{
    ECU_ASSERT( (me) );
    uint16_t wValue = ECU_LE16_TO_CPU_RUNTIME(me->wValue);
    return wValue;
}
