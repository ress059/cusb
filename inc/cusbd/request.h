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
    CUSBD_REQUEST_RECIPIENT_DEVICE,     /**<! Request is for a USB device. */
    CUSBD_REQUEST_RECIPIENT_INTERFACE,  /**<! Request is for an interface descriptor. */
    CUSBD_REQUEST_RECIPIENT_ENDPOINT,   /**<! Request is for an endpoint descriptor. */
    CUSBD_REQUEST_RECIPIENT_OTHER,      /**<! Other. */
    /********************************/
    CUSBD_REQUEST_RECIPIENT_RESERVED    /**<! Values reserved for future use by USB. Currently 4 to 31 is reserved. */
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

#ifdef __cplusplus
}
#endif

#endif /* CUSBD_REQUEST_H_ */
