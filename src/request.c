/**
 * @file
 * @brief See @ref request.h.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-06-02
 * @copyright Copyright (c) 2025
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* Translation unit. */
#include "cusbd/request.h"

/* ECU. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_NAME("cusbd/request.c")

/*------------------------------------------------------------*/
/*----------------------- PUBLIC FUNCTIONS -------------------*/
/*------------------------------------------------------------*/

enum cusbd_request_direction cusbd_request_direction(const struct cusb_request *request)
{
    enum cusbd_request_direction direction = CUSBD_REQUEST_DIRECTION_HOST_TO_DEVICE;
    ECU_RUNTIME_ASSERT( (request) );

    if (request->bmRequestType & (1U << 7))
    {
        direction = CUSBD_REQUEST_DIRECTION_DEVICE_TO_HOST;
    }

    return direction;
}

enum cusbd_request_type cusbd_request_type(const struct cusb_request *request)
{
    static const uint8_t BITMAP = ((1U << 6) | (1U << 5));
    ECU_RUNTIME_ASSERT( (request) );
    /* Value can always be safely casted to enumeration since values constrained to 0b11. */
    return ((enum cusbd_request_type)((request->bmRequestType & BITMAP) >> 5U));
}

enum cusbd_request_recipient cusbd_request_recipient(const struct cusb_request *request)
{
    static const uint8_t BITMAP = ((1U << 4) | (1U << 3) | (1U << 2) | (1U << 1) | (1U << 0));
    enum cusbd_request_recipient recipient = CUSBD_REQUEST_RECIPIENT_RESERVED;
    ECU_RUNTIME_ASSERT( (request) );
    uint8_t data = request->bmRequestType & BITMAP;

    if (data < (uint8_t)CUSBD_REQUEST_RECIPIENT_RESERVED)
    {
        recipient = (enum cusbd_request_recipient)data;
    }

    return recipient;
}

enum cusbd_request_value cusbd_request_value(const struct cusb_request *request)
{
    enum cusbd_request_value value = CUSBD_REQUEST_VALUE_UNDEFINED;
    ECU_RUNTIME_ASSERT( (request) );
    uint8_t data = request->bRequest;

    if (data < (uint8_t)CUSBD_REQUEST_VALUE_UNDEFINED)
    {
        value = (enum cusbd_request_recipient)data;
    }
    
    return value;
}
