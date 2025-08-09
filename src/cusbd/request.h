/**
 * @file
 * @brief Contains helpers that extract information from the 
 * dispatched request (setup packet).
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-07-03
 * @copyright Copyright (c) 2025
 */

#ifndef CUSBD_REQUEST_H_
#define CUSBD_REQUEST_H_

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <stdbool.h>
#include <stdint.h>

/* CUSB. */
#include "cusbd/event.h"

/*------------------------------------------------------------*/
/*----------------------- CUSBD REQUEST ----------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Bit 7 of bmRequestType bitfield 
 * in @ref cusbd_setup_packet. Translated
 * to enum for easier and more portable use.
 */
enum cusbd_request_direction
{
    CUSBD_REQUEST_DIRECTION_OUT,    /**< OUT. Host to device. Device must only process request. */
    CUSBD_REQUEST_DIRECTION_IN      /**< IN. Device to host. Device must also send data back to host. */
};

/**
 * @brief Bits 4-0 of bmRequestType bitfield 
 * in @ref cusbd_setup_packet. Translated
 * to enum for easier and more portable use.
 */
enum cusbd_request_recipient
{
    CUSBD_REQUEST_RECIPIENT_DEVICE,     /**< Request is for a USB device. */
    CUSBD_REQUEST_RECIPIENT_INTERFACE,  /**< Request is for an interface descriptor. */
    CUSBD_REQUEST_RECIPIENT_ENDPOINT,   /**< Request is for an endpoint descriptor. */
    CUSBD_REQUEST_RECIPIENT_OTHER,      /**< Other. */
    /******************************************/
    CUSBD_REQUEST_RECIPIENT_RESERVED    /**< Values reserved for future use by USB. Currently 4 to 31 is reserved. */
};

/**
 * @brief Bits 6-5 of bmRequestType bitfield 
 * in @ref cusbd_setup_packet. Translated
 * to enum for easier and more portable use.
 */
enum cusbd_request_type
{
    CUSBD_REQUEST_TYPE_STANDARD,    /**< Standard request type. */
    CUSBD_REQUEST_TYPE_CLASS,       /**< Class-specific (HID, printer, etc) request type. */
    CUSBD_REQUEST_TYPE_VENDOR,      /**< Vendor-specific request type. */
    CUSBD_REQUEST_TYPE_RESERVED     /**< Value reserved for future use by USB. */
};

/*------------------------------------------------------------*/
/*--------------- CUSBD REQUEST MEMBER FUNCTIONS -------------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Member Functions
 */
/**@{*/
/**
 * @pre @p me previously constructed via @ref cusbd_request_ctor().
 * @brief Return direction of request.
 * 
 * @param me Request to check.
 */
extern enum cusbd_request_direction cusbd_request_direction(const struct cusbd_request *me);

/**
 * @pre @p me previously constructed via @ref cusbd_request_ctor().
 * @brief Return request recipient. I.e. device, interface, etc.
 * 
 * @param me Request to check.
 */
extern enum cusbd_request_recipient cusbd_request_recipient(const struct cusbd_request *me);

/**
 * @pre @p me previously constructed via @ref cusbd_request_ctor().
 * @brief Return type of request. I.e. standard, class-specific, etc.
 * 
 * @param me Request to check.
 */
extern enum cusbd_request_type cusbd_request_type(const struct cusbd_request *me);

/**
 * @pre @p me previously constructed via @ref cusbd_request_ctor().
 * @brief Return bRequest value of the setup packet which contains
 * the actual request. I.e. CLEAR_FEATURE(), GET_DESCRIPTOR(), etc.
 * Meaning of the value depends on the recipient and request type.
 * I.e. a value of 1 can be different depending on if this is
 * a standard request vs class-specific request. It is the application's
 * responsibility to handle this.
 * 
 * @param me Request to check.
 */
extern uint8_t cusbd_request_brequest(const struct cusbd_request *me);

/**
 * @pre @p me previously constructed via @ref cusbd_request_ctor().
 * @brief Return wIndex field of the setup packet. Value returned
 * in native endianness for easier use, not little endian.
 * 
 * @param me Request to check.
 */
extern uint16_t cusbd_request_windex(const struct cusbd_request *me);

/**
 * @pre @p me previously constructed via @ref cusbd_request_ctor().
 * @brief Return wLength field of the setup packet. Value returned
 * in native endianness for easier use, not little endian.
 * 
 * @param me Request to check.
 */
extern uint16_t cusbd_request_wlength(const struct cusbd_request *me);

/**
 * @pre @p me previously constructed via @ref cusbd_request_ctor().
 * @brief Return wValue field of the setup packet. Value returned
 * in native endianness for easier use, not little endian.
 * 
 * @param me Request to check.
 */
extern uint16_t cusbd_request_wvalue(const struct cusbd_request *me);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* CUSBD_REQUEST_H_ */
