/**
 * @file
 * @brief Contains common definitions for USB device requests.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-06-02
 * @copyright Copyright (c) 2025
 */

#ifndef CUSBD_REQUEST_H_
#define CUSBD_REQUEST_H_

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <stdint.h>

/* ECU. */
#include "ecu/attributes.h"

/*------------------------------------------------------------*/
/*------------------------ COMMON TYPES ----------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Status returned when USB device request is
 * processed. Return values < CUSBD_REQUEST_STATUS_PROPAGATE
 * means the request was processed and should not be
 * propagated to the rest of the descriptor tree.
 */
enum cusbd_request_status
{
    /// @brief Request was successfully processed.
    CUSBD_REQUEST_STATUS_ACK,

    /// @brief  Request relevant but invalid. Device should indicate 
    /// error to host via STALL.
    CUSBD_REQUEST_STATUS_STALL,

    /// @brief Endpoint temporarily busy. Host will retry at another time.
    CUSBD_REQUEST_STATUS_NAK,

    /// @brief CUSB-specific. Device in state where behavior on request 
    /// is undefined by USB. I.e. in Default State during CLEAR_FEATURE
    /// request. Device should ignore request and stop propogating it.
    CUSBD_REQUEST_STATUS_UNDEFINED,
    /************************************************************************/
    /************************************************************************/
    /************************************************************************/
    /// @brief CUSB-specific. Delimiter. Return statuses less than
    /// this value mean the request has been processed and should not be
    /// propagated to the rest of the descriptor tree. Return statuses
    /// >= this value means the request should be propagated.
    CUSBD_REQUEST_STATUS_PROPAGATE,

    /// @brief CUSB-specific. Request irrelevant to descriptor and 
    /// ignored. I.e. GET_INTERFACE() on configuration descriptor.
    CUSBD_REQUEST_STATUS_IGNORED = CUSBD_REQUEST_STATUS_PROPAGATE,

    /// @brief Maximum value this enumeration can be.
    CUSBD_REQUEST_STATUS_MAX = CUSBD_REQUEST_STATUS_PROPAGATE
};

/**
 * @brief Bit 7 of bmRequestType bitfield in
 * request setup packet.
 */
enum cusbd_request_direction
{
    CUSBD_REQUEST_DIRECTION_HOST_TO_DEVICE, /**<! OUT. Device must only process request. */
    CUSBD_REQUEST_DIRECTION_DEVICE_TO_HOST  /**<! IN. Device must also send data back to host. */
};

/**
 * @brief Bits 6-5 of bmRequestType bitfield in
 * request setup packet. 
 */
enum cusbd_request_type
{
    CUSBD_REQUEST_TYPE_STANDARD,    /**<! Standard request type. */
    CUSBD_REQUEST_TYPE_CLASS,       /**<! Class-specific (HID, printer, etc) request type. */
    CUSBD_REQUEST_TYPE_VENDOR,      /**<! Vendor-specific request type. */
    CUSBD_REQUEST_TYPE_RESERVED     /**<! Value reserved for future use by USB. */
};

/**
 * @brief Bits 4-0 of bmRequestType bitfield in
 * request setup packet.
 */
enum cusbd_request_recipient
{
    CUSBD_REQUEST_RECIPIENT_DEVICE,     /**<!  */
    CUSBD_REQUEST_RECIPIENT_INTERFACE,  /**<! Request is for an interface descriptor. */
    CUSBD_REQUEST_RECIPIENT_ENDPOINT,   /**<! Request is for an endpoint descriptor. */
    CUSBD_REQUEST_RECIPIENT_OTHER,      /**<! Other. */
    /********************************/
    CUSBD_REQUEST_RECIPIENT_RESERVED    /**<! Values reserved for future use by USB. Currently 4 to 31 is reserved. */
};

/**
 * @brief Value of bRequest in request setup packet.
 */
enum cusbd_request_value
{
    CUSBD_REQUEST_VALUE_GET_STATUS,             /**<! Standard request. Get status. */
    CUSBD_REQUEST_VALUE_CLEAR_FEATURE,          /**!< Standard request. Clear feature. */
    CUSBD_REQUEST_VALUE_RESERVED0,              /**!< Reserved for future use by USB. */
    CUSBD_REQUEST_VALUE_SET_FEATURE,            /**!< Standard request. Set feature. */
    CUSBD_REQUEST_VALUE_RESERVED1,              /**!< Reserved for future use by USB. */
    CUSBD_REQUEST_VALUE_SET_ADDRESS,            /**!< Standard request. Set address. */
    CUSBD_REQUEST_VALUE_GET_DESCRIPTOR,         /**!< Standard request. Get descriptor. */
    CUSBD_REQUEST_VALUE_SET_DESCRIPTOR,         /**!< Standard request. Set descriptor. */
    CUBSD_REQUEST_VALUE_GET_CONFIGURATION,      /**!< Standard request. Get configuration. */
    CUSBD_REQUEST_VALUE_SET_CONFIGURATION,      /**!< Standard request. Set configuration. */
    CUSBD_REQUEST_VALUE_GET_INTERFACE,          /**!< Standard request. Get interface. */
    CUSBD_REQUEST_VALUE_SET_INTERFACE,          /**!< Standard request. Set interface. */
    CUSBD_REQUEST_VALUE_SYNCH_FRAME,            /**!< Standard request. Synch frame. */
    /*************************************/
    CUSBD_REQUEST_VALUE_UNDEFINED,              /**!< Undefined (garbage) value. */
    CUSBD_REQUEST_VALUE_COUNT = CUSBD_REQUEST_VALUE_UNDEFINED /**!< Number of requests. */
};

/**
 * @brief Setup packet contents for a USB device request.
 * End user will memcopy received requests into this struct
 * directly.
 * 
 * @warning Will always be stored in little endian, not
 * native endianness.
 */
struct cusbd_request
{
    /// @brief Bitmap. Type of request.
    uint8_t bmRequestType;

    /// @brief The specific request. I.e. CLEAR_FEATURE(), GET_INTERFACE(), etc.
    uint8_t bRequest;

    /// @brief Varies according to the request.
    uint16_t wValue;

    /// @brief Varies according to the request.
    uint16_t wIndex;

    /// @brief Number of bytes of data transferred. If 0, no data is
    /// transferred.
    uint16_t wLength;
} ECU_ATTRIBUTE_PACKED;

/*------------------------------------------------------------*/
/*----------------------- PUBLIC FUNCTIONS -------------------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @pre End user mem-copied control endpoint buffer into @p request. 
 * @brief Returns if request is host-to-device or device-to-host.
 * Device must send data back to host if device-to-host.
 * 
 * @param request Setup packet of request to check.
 */
extern enum cusbd_request_direction cusbd_request_direction(const struct cusb_request *request);

/**
 * @pre End user mem-copied control endpoint buffer into @p request. 
 * @brief Returns if request type is standard, class-specific,
 * vendor-specific, or reserved.
 * 
 * @param request Setup packet of request to check.
 */
extern enum cusbd_request_type cusbd_request_type(const struct cusb_request *request);

/**
 * @pre End user mem-copied control endpoint buffer into @p request. 
 * @brief Returns if request recipient is for a device, interface,
 * endpoint, other, or reserved.
 * 
 * @param request Setup packet of request to check.
 */
extern enum cusbd_request_recipient cusbd_request_recipient(const struct cusb_request *request);

/**
 * @pre End user mem-copied control endpoint buffer into @p request. 
 * @brief Returns the actual request. I.e. CLEAR_FEATURE, GET_CONFIGURATION,
 * etc. Returns @ref CUSBD_REQUEST_VALUE_UNDEFINED if setup packet contains
 * garbage value.
 * 
 * @param request Setup packet of request to check.
 */
extern enum cusbd_request_value cusbd_request_value(const struct cusb_request *request);

#ifdef __cplusplus
}
#endif

#endif /* CUSBD_REQUEST_H_ */
