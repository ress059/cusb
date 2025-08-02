/**
 * @file
 * @brief Common functionality for all USB requests.
 * This does not (and should not) be inherited. It can just
 * be a composite inside request-specific visitors.
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
/*---------------------- DEFINES AND MACROS ------------------*/
/*------------------------------------------------------------*/

/// @brief Feature selector value. Addressed to device.
#define CUSBD_FEATURE_SELECTOR_DEVICE_REMOTE_WAKEUP (1U)

/// @brief Feature selector value. Addressed to endpoint.
#define CUSBD_FEATURE_SELECTOR_ENDPOINT_HALT (0U)

/// @brief Feature selector value. Addressed to device.
#define CUSBD_FEATURE_SELECTOR_TEST_MODE (2U)

/*------------------------------------------------------------*/
/*----------------------- CUSBD REQUEST ----------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Bit 7 of bmRequestType bitfield 
 * in @ref cusbd_setup_packet. Translated
 * to enum for easier and more portable use.
 */
enum cusbd_rx_request_event_direction
{
    CUSBD_RX_REQUEST_EVENT_DIRECTION_OUT,    /**< OUT. Host to device. Device must only process request. */
    CUSBD_RX_REQUEST_EVENT_DIRECTION_IN      /**< IN. Device to host. Device must also send data back to host. */
};

/**
 * @brief Bits 4-0 of bmRequestType bitfield 
 * in @ref cusbd_setup_packet. Translated
 * to enum for easier and more portable use.
 */
enum cusbd_rx_request_event_recipient
{
    CUSBD_RX_REQUEST_EVENT_RECIPIENT_DEVICE,     /**< Request is for a USB device. */
    CUSBD_RX_REQUEST_EVENT_RECIPIENT_INTERFACE,  /**< Request is for an interface descriptor. */
    CUSBD_RX_REQUEST_EVENT_RECIPIENT_ENDPOINT,   /**< Request is for an endpoint descriptor. */
    CUSBD_RX_REQUEST_EVENT_RECIPIENT_OTHER,      /**< Other. */
    /******************************************/
    CUSBD_RX_REQUEST_EVENT_RECIPIENT_RESERVED    /**< Values reserved for future use by USB. Currently 4 to 31 is reserved. */
};

/**
 * @brief Bits 6-5 of bmRequestType bitfield 
 * in @ref cusbd_setup_packet. Translated
 * to enum for easier and more portable use.
 */
enum cusbd_rx_request_event_type
{
    CUSBD_RX_REQUEST_EVENT_TYPE_STANDARD,    /**< Standard request type. */
    CUSBD_RX_REQUEST_EVENT_TYPE_CLASS,       /**< Class-specific (HID, printer, etc) request type. */
    CUSBD_RX_REQUEST_EVENT_TYPE_VENDOR,      /**< Vendor-specific request type. */
    CUSBD_RX_REQUEST_EVENT_TYPE_RESERVED     /**< Value reserved for future use by USB. */
};

/**
 * @brief bRequest in @ref cusbd_setup_packet.
 * Translated to enum for easier and more portable 
 * use.
 */
enum cusbd_rx_request_event_value
{
    CUSBD_RX_REQUEST_EVENT_VALUE_GET_STATUS,
    CUSBD_RX_REQUEST_EVENT_VALUE_CLEAR_FEATURE,
    CUSBD_RX_REQUEST_EVENT_VALUE_SET_FEATURE,
    CUSBD_RX_REQUEST_EVENT_VALUE_SET_ADDRESS,
    CUSBD_RX_REQUEST_EVENT_VALUE_GET_DESCRIPTOR,
    CUSBD_RX_REQUEST_EVENT_VALUE_SET_DESCRIPTOR,
    CUSBD_RX_REQUEST_EVENT_VALUE_GET_CONFIGURATION,
    CUSBD_RX_REQUEST_EVENT_VALUE_SET_CONFIGURATION,
    CUSBD_RX_REQUEST_EVENT_VALUE_GET_INTERFACE,
    CUSBD_RX_REQUEST_EVENT_VALUE_SET_INTERFACE,
    CUSBD_RX_REQUEST_EVENT_VALUE_SYNCH_FRAME,
    /*****************************************/
    CUSBD_RX_REQUEST_EVENT_VALUE_RESERVED
};

/*------------------------------------------------------------*/
/*---------- CUSBD_RX_REQUEST_EVENT MEMBER FUNCTIONS ---------*/
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
extern enum cusbd_rx_request_event_direction cusbd_rx_request_event_direction(const struct cusbd_rx_request_event *me);

/**
 * @pre @p me previously constructed via @ref cusbd_request_ctor().
 * @brief Return request recipient. I.e. device, interface, etc.
 * 
 * @param me Request to check.
 */
extern enum cusbd_rx_request_event_recipient cusbd_rx_request_event_recipient(const struct cusbd_rx_request_event *me);

/**
 * @pre @p me previously constructed via @ref cusbd_request_ctor().
 * @brief Return type of request. I.e. standard, class-specific, etc.
 * 
 * @param me Request to check.
 */
extern enum cusbd_rx_request_event_type cusbd_rx_request_event_type(const struct cusbd_rx_request_event *me);

/**
 * @pre @p me previously constructed via @ref cusbd_request_ctor().
 * @brief Return the actual request. I.e. CLEAR_FEATURE(), GET_DESCRIPTOR(), etc.
 * 
 * @param me Request to check.
 */
extern enum cusbd_rx_request_event_value cusbd_rx_request_event_value(const struct cusbd_rx_request_event *me);

/**
 * @pre @p me previously constructed via @ref cusbd_request_ctor().
 * @brief Return wIndex field of the setup packet. Value returned
 * in native endianness for easier use, not little endian.
 * 
 * @param me Request to check.
 */
extern uint16_t cusbd_rx_request_event_w_index(const struct cusbd_rx_request_event *me);

/**
 * @pre @p me previously constructed via @ref cusbd_request_ctor().
 * @brief Return wLndex field of the setup packet. Value returned
 * in native endianness for easier use, not little endian.
 * 
 * @param me Request to check.
 */
extern uint16_t cusbd_rx_request_event_w_length(const struct cusbd_rx_request_event *me);

/**
 * @pre @p me previously constructed via @ref cusbd_request_ctor().
 * @brief Return wValue field of the setup packet. Value returned
 * in native endianness for easier use, not little endian.
 * 
 * @param me Request to check.
 */
extern uint16_t cusbd_rx_request_event_w_value(const struct cusbd_rx_request_event *me);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* CUSBD_REQUEST_H_ */
