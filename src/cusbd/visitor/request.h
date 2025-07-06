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

#ifndef CUSBD_VISITOR_STD_REQUEST_H_
#define CUSBD_VISITOR_STD_REQUEST_H_

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

/// @brief Feature selector value.
#define CUSBD_FEATURE_DEVICE_REMOTE_WAKEUP (1U)

/// @brief Feature selector value.
#define CUSBD_FEATURE_ENDPOINT_HALT (0U)

/// @brief Feature selector value.
#define CUSBD_FEATURE_TEST_MODE (2U)

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
    /********************************/
    CUSBD_REQUEST_RECIPIENT_RESERVED    /**< Values reserved for future use by USB. Currently 4 to 31 is reserved. */
};

/**
 * @brief State the USB device was in when the 
 * request was received.
 */
enum cusbd_request_state
{
    CUSBD_REQUEST_STATE_RESERVED,           /**< Default initialiation value to detect if request was constructed or not. Default case asserts. */
    /***********************************/
    CUSBD_REQUEST_STATE_DEFAULT_STATE,      /**< Device is in default state. */
    CUSBD_REQUEST_STATE_ADDRESS_STATE,      /**< Device is in address state. */
    CUSBD_REQUEST_STATE_CONFIGURED_STATE,   /**< Device is in configured state. */
    /***********************************/
    CUSBD_REQUEST_STATE_COUNT               /**< Total number of device states. */
};

/**
 * @brief Current status of request being processed.
 * Device should indicate status back to host based 
 * on this value once the request has been processed
 * or accepted by all descriptors.
 */
enum cusbd_request_status
{
    CUSBD_REQUEST_STATUS_UNPROCESSED,   /**< Default initialization value. Request not yet processed. */
    /********************************/
    CUSBD_REQUEST_STATUS_ACK,           /**< Request was successfully processed. */
    CUSBD_REQUEST_STATUS_NAK,           /**< Request cannot be processed. Device is currently busy. */
    CUSBD_REQUEST_STATUS_STALL,         /**< Request successfully processed but invalid request. Request error. */
    /********************************/
    CUSBD_REQUEST_STATUS_COUNT          /**< Total number of statuses. */
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

/**
 * @brief bRequest in @ref cusbd_setup_packet.
 * Translated to enum for easier and more portable 
 * use.
 */
enum cusbd_request_value
{
    CUSBD_REQUEST_VALUE_GET_STATUS,
    CUSBD_REQUEST_VALUE_CLEAR_FEATURE,
    CUSBD_REQUEST_VALUE_SET_FEATURE,
    CUSBD_REQUEST_VALUE_SET_ADDRESS,
    CUSBD_REQUEST_VALUE_GET_DESCRIPTOR,
    CUSBD_REQUEST_VALUE_SET_DESCRIPTOR,
    CUSBD_REQUEST_VALUE_GET_CONFIGURATION,
    CUSBD_REQUEST_VALUE_SET_CONFIGURATION,
    CUSBD_REQUEST_VALUE_GET_INTERFACE,
    CUSBD_REQUEST_VALUE_SET_INTERFACE,
    CUSBD_REQUEST_VALUE_SYNCH_FRAME,
    /************************************/
    CUSBD_REQUEST_VALUE_RESERVED
};

/**
 * @brief Contains common request info and the
 * translated setup packet of a USB request.
 * Values are translated to enums and native endianness
 * for easier use. This protects the library from any
 * changes to the USB standard. The changepoint would be
 * limited to just the translation implementation in this
 * module as opposed to throughout the library.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_request
{
    /// @brief Bit 7 of bmRequestType. OUT or IN. Translated
    /// to enum for easier and more portable use.
    enum cusbd_request_direction direction;

    /// @brief Bits 6-5 of bmRequestType. Standard, class-specific,
    /// etc. Translated to enum for easier and more portable use.
    enum cusbd_request_type type;

    /// @brief Bits 4-0 of bmRequestType. Device, interface, endpoint,
    /// etc. Translated to enum for easier and more portable use.
    enum cusbd_request_recipient recipient;

    /// @brief The actual request. I.e. CLEAR_FEATURE(), GET_DESCRIPTOR(),
    /// etc. Translated to enum for easier and more portable use.
    enum cusbd_request_value value;

    /// @brief Unique to each request. Translated to native endianness for easier use.
    /// @warning Will always be in native endianness, not little endian.
    uint16_t wValue;

    /// @brief Unique to each request. Translated to native endianness for easier use.
    /// @warning Will always be in native endianness, not little endian.
    uint16_t wIndex;

    /// @brief Unique to each request. Translated to native endianness for easier use.
    /// @warning Will always be in native endianness, not little endian.
    uint16_t wLength;

    /// @brief State the USB device was in when the request was received.
    enum cusbd_request_state state;

    /// @brief Current status of request being processed. Device should 
    /// indicate status back to host based on this value once the request
    /// has been processed or accepted by all descriptors.
    enum cusbd_request_status status;
};

/*------------------------------------------------------------*/
/*--------------- CUSBD REQUEST MEMBER FUNCTIONS -------------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Constructor
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @pre @p e previously constructed via @ref cusbd_setup_packet_rx_event_ctor().
 * @brief Creates a translated setup packet for easier use
 * by request-specific visitors. This translation protects the
 * library from any changes to the USB standard, etc. 
 * Changepoint would be limited to only this function as opposed to
 * throughout the library code.
 * 
 * @param me Setup packet translation to construct.
 * @param e Setup packet event received by user. This is an event
 * as opposed to the raw data in @ref cusbd_setup_packet to limit 
 * this visitor's use to only within the device's state machine 
 * implementation.
 * @param state State the USB device was in when the
 * request was received.
 */
extern void cusbd_request_ctor(struct cusbd_request *me, 
                               const struct cusbd_setup_packet_rx_event *e,
                               enum cusbd_request_state state);
/**@}*/

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
 * @brief Returns true if the request has been processed.
 * False otherwise. The device implementation can poll this
 * function to know when the request visitor can stop being
 * propagated within the descriptor tree. Once the request
 * has been processed the implementation should call @ref cusbd_request_status()
 * to know how to respond to the host.
 * 
 * @warning Do not block on this function as it is possible
 * for a request to never be processed. I.e. a request that
 * does not apply to any descriptors in the descriptor tree.
 * 
 * @param me Request to check.
 */
extern bool cusbd_request_done(const struct cusbd_request *me);

/**
 * @pre @p me previously constructed via @ref cusbd_request_ctor().
 * @brief Return request recipient. I.e. device, interface, etc.
 * 
 * @param me Request to check.
 */
extern enum cusbd_request_recipient cusbd_request_recipient(const struct cusbd_request *me);

/**
 * @pre @p me previously constructed via @ref cusbd_request_ctor().
 * @brief Returns the state the USB device was in when the request
 * was received.
 * 
 * @param me Request to check.
 */
extern enum cusbd_request_state cusbd_request_state(const struct cusbd_request *me);

/**
 * @pre @p me previously constructed via @ref cusbd_request_ctor().
 * @brief Set the status returned by @ref cusbd_request_status().
 * Once the request has been processed, the status should be
 * updated by calling this function.
 * 
 * @param me Request to update.
 */
extern void cusbd_request_set_status(struct cusbd_request *me, enum cusbd_request_status status);

/**
 * @pre @p me previously constructed via @ref cusbd_request_ctor().
 * @brief Return request status. Once @ref cusbd_request_done()
 * returns true, the device implementation should use the return
 * value of this function to know how to respond to the host.
 * 
 * @param me Request to check.
 */
extern enum cusbd_request_status cusbd_request_status(const struct cusbd_request *me);

/**
 * @pre @p me previously constructed via @ref cusbd_request_ctor().
 * @brief Return type of request. I.e. standard, class-specific, etc.
 * 
 * @param me Request to check.
 */
extern enum cusbd_request_type cusbd_request_type(const struct cusbd_request *me);

/**
 * @pre @p me previously constructed via @ref cusbd_request_ctor().
 * @brief Return the actual request. I.e. CLEAR_FEATURE(), GET_DESCRIPTOR(), etc.
 * 
 * @param me Request to check.
 */
extern enum cusbd_request_value cusbd_request_value(const struct cusbd_request *me);

/**
 * @pre @p me previously constructed via @ref cusbd_request_ctor().
 * @brief Return wIndex field of the setup packet. Value returned
 * in native endianness for easier use, not little endian.
 * 
 * @param me Request to check.
 */
extern uint16_t cusbd_request_w_index(const struct cusbd_request *me);

/**
 * @pre @p me previously constructed via @ref cusbd_request_ctor().
 * @brief Return wLndex field of the setup packet. Value returned
 * in native endianness for easier use, not little endian.
 * 
 * @param me Request to check.
 */
extern uint16_t cusbd_request_w_length(const struct cusbd_request *me);

/**
 * @pre @p me previously constructed via @ref cusbd_request_ctor().
 * @brief Return wValue field of the setup packet. Value returned
 * in native endianness for easier use, not little endian.
 * 
 * @param me Request to check.
 */
extern uint16_t cusbd_request_w_value(const struct cusbd_request *me);


/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* CUSBD_VISITOR_STD_REQUEST_H_ */
