/**
 * @file
 * @brief Object representing an endpoint descriptor.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-05-16
 * @copyright Copyright (c) 2025
 */

#ifndef CUSBD_ENDPOINT_H_
#define CUSBD_ENDPOINT_H_

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <stddef.h>
#include <stdint.h>

/* CUSBD. */
#include "cusbd/descriptor.h"

/* ECU. */
#include "ecu/attributes.h"
#include "ecu/dlist.h"
#include "ecu/endian.h"

/*------------------------------------------------------------*/
/*---------------------- DEFINES AND MACROS ------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Value of bDescriptorType in a standard
 * endpoint descriptor.
 */
#define CUSBD_ENDPOINT_BDESCRIPTORTYPE \
    ((uint8_t)0x05)

/**
 * @brief Creates a @ref cusbd_endpoint_descriptor at
 * either compile-time or run-time. Example usage below
 * creates a bulk IN endpoint1 with a max packet size of 
 * 64 bytes:
 * @code{.c}
 * static const struct cusbd_endpoint_descriptor endpoint = CUSBD_ENDPOINT_DESCRIPTOR_CTOR(
 *      1, ((1U << 0) | (1U << 1)), 64, 0
 * );
 * @endcode
 * 
 * @warning This macro performs all necessary byte swapping 
 * to store multi-byte values in little endian. Do not attempt 
 * to send multi-byte values in little endian. Raw numbers 
 * must be supplied.
 * 
 * @param bEndpointAddress_ Specifies the endpoint's number and 
 * if it is OUT or IN. The endpoint number cannot be 0 since there 
 * is never a descriptor for endpoint0. See USB spec. 
 * @param bmAttributes_ Specifies the endpoint's type (control,
 * isochronous, etc). See USB spec.
 * @param wMaxPacketSize_ Maximum packet size, in bytes, the
 * endpoint is capable of sending or receiving in a single
 * transaction. See USB spec.
 * @param bInterval_ Time interval for polling endpoints.
 * See USB spec.
 */
#define CUSBD_ENDPOINT_DESCRIPTOR_CTOR(bEndpointAddress_,               \
                                       bmAttributes_,                   \
                                       wMaxPacketSize_,                 \
                                       bInterval_)                      \
    {                                                                   \
        .bLength = sizeof(struct cusbd_endpoint_descriptor),            \
        .bDescriptorType = CUSBD_DESCRIPTOR_TYPE_ENDPOINT,              \
        .bEndpointAddress = (bEndpointAddress_),                        \
        .bmAttributes = (bmAttributes_),                                \
        .wMaxPacketSize = ECU_CPU_TO_LE16_COMPILETIME(wMaxPacketSize_), \
        .bInterval = (bInterval_)                                       \
    }

/*------------------------------------------------------------*/
/*----------------------- CUSBD ENDPOINT ---------------------*/
/*------------------------------------------------------------*/

// /**
//  * @brief Type given to endpoint ID (@ref cusbd_endpoint.id).
//  * This allows the library to implicitly typecast between
//  * @ref cusbd_endpoint_reserved_ids and the user-specified
//  * endpoint IDs. Typedeffed incase this has to change in
//  * the future.
//  * 
//  * @warning This must be a signed type so reserved enumerations
//  * less than 0 can be stored.
//  */
// typedef int16_t cusbd_endpoint_id_t;

/**
 * @brief Endpoint direction. Bit 7 of bEndpointAddress.
 */
enum cusbd_endpoint_direction
{
    CUSBD_ENDPOINT_DIRECTION_OUT,   /**< [7] = 0. Host to device. */
    CUSBD_ENDPOINT_DIRECTION_IN     /**< [7] = 1. Device to host. */
};

/**
 * @brief Possible endpoint transfer types, as defined by USB.
 * Bits [1:0] of bmAttributes.
 */
enum cusbd_endpoint_transfer_type
{
    CUSBD_ENDPOINT_TRANSFER_TYPE_CONTROL,       /**<! [1:0] = 00. Endpoint0. Control endpoint. */
    CUSBD_ENDPOINT_TRANSFER_TYPE_ISOCHRONOUS,   /**<! [1:0] = 01. Endpoint used for isochronous transfers. */
    CUSBD_ENDPOINT_TRANSFER_TYPE_BULK,          /**<! [1:0] = 10. Endpoint used for bulk transfers. */
    CUSBD_ENDPOINT_TRANSFER_TYPE_INTERRUPT      /**<! [1:0] = 11. Endpoint used for interrupt transfers. */
};

/**
 * @brief Possible endpoint transfer types, as defined by USB.
 * Bits [3:2] of bmAttributes.
 * 
 * @warning Only applicable to isochronous endpoints.
 */
enum cusbd_endpoint_sync_type
{
    CUSBD_ENDPOINT_SYNC_TYPE_NOSYNC,        /**< [3:2] = 00 */
    CUSBD_ENDPOINT_SYNC_TYPE_ASYNC,         /**< [3:2] = 01 */
    CUSBD_ENDPOINT_SYNC_TYPE_ADAPTIVE,      /**< [3:2] = 10 */
    CUSBD_ENDPOINT_SYNC_TYPE_SYNC           /**< [3:2] = 11 */
};

/**
 * @brief Possible endpoint usage types, as defined by USB.
 * Bits [5:4] of bmAttributes.
 * 
 * @warning Only applicable to isochronous endpoints.
 */
enum cusbd_endpoint_usage_type
{
    CUSBD_ENDPOINT_USAGE_TYPE_DATA,                 /**< [5:4] = 00 */
    CUSBD_ENDPOINT_USAGE_TYPE_FEEDBACK,             /**< [5:4] = 01 */
    CUSBD_ENDPOINT_USAGE_TYPE_IMPLICIT_FEEDBACK,    /**< [5:4] = 10 */
    CUSBD_ENDPOINT_USAGE_TYPE_RESERVED              /**< [5:4] = 11 */
};

// /**
//  * @brief Reserved endpoint IDs.
//  */
// enum cusbd_endpoint_reserved_ids
// {
//     CUSBD_ENDPOINT0_OUT_ID = -2,     /**<! RESERVED. ID assigned to control endpoint OUT. */
//     CUSBD_ENDPOINT0_IN_ID = -1,      /**<! RESERVED. ID assigned to control endpoint IN. */
//     /***************************/
//     CUSBD_ENDPOINT_USER_ID_BEGIN     /**<! Start of user-specified endpoint IDs. Will always be 0. */
// };

/**
 * @brief Data in a standard endpoint descriptor.
 * This will always be in little endian format.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_endpoint_descriptor
{
    /// @brief Number of bytes of this descriptor.
    uint8_t bLength;

    /// @brief Descriptor type. Always 0x05 == Endpoint Descriptor.
    uint8_t bDescriptorType;

    /// @brief Specifies the endpoint's number and if it is OUT or IN. 
    /// The endpoint number cannot be 0 since there is never a descriptor 
    /// for endpoint0. See USB spec. 
    uint8_t bEndpointAddress;

    /// @brief Specifies the endpoint's type (control, isochronous, 
    /// etc). See USB spec.
    uint8_t bmAttributes;

    /// @brief Maximum packet size, in bytes, the endpoint is capable 
    /// of sending or receiving in a single transaction. See USB spec.
    uint16_t wMaxPacketSize;

    /// @brief Time interval for polling endpoints. See USB spec.
    uint8_t bInterval;
} ECU_ATTRIBUTE_PACKED;

/**
 * @brief Object representing a USB endpoint descriptor.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 * @warning This should not be created for endpoint0
 * since there is never a descriptor for endpoint0.
 */
struct cusbd_endpoint
{
    /// @brief Inherit cusbd_descriptor base class.
    /// @warning MUST be first member.
    struct cusbd_descriptor base;

    /// @brief Descriptor data. A copy is stored so the API can
    /// automatically adjust it as the device is updated.
    /// @warning This struct is packed and will always be in 
    /// little endian.
    struct cusbd_endpoint_descriptor descriptor;
};

/*------------------------------------------------------------*/
/*---------------- CUSBD ENDPOINT MEMBER FUNCTIONS -----------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif
#pragma message("TODO: Update descriptions!!")
/**
 * @name Constructor
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @pre @p descriptor previously constructed via @ref CUSBD_ENDPOINT_DESCRIPTOR_CTOR().
 * @brief Endpoint descriptor constructor.
 * 
 * @warning This cannot be called on an active endpoint 
 * descriptor. Doing so is undefined behavior.
 * @warning This should not be called to initialize endpoint0
 * since there is never a descriptor for endpoint0.
 * 
 * @param me Endpoint descriptor to construct.
 * @param descriptor The endpoint descriptor's data.
 */
extern void cusbd_endpoint_ctor(struct cusbd_endpoint *me,
                                const struct cusbd_endpoint_descriptor *descriptor);
/**@}*/

/**
 * @name Member Functions
 */
/**@{*/
extern enum cusbd_endpoint_direction cusbd_endpoint_direction(const struct cusbd_endpoint *me);

extern size_t cusbd_endpoint_number(const struct cusbd_endpoint *me);


// Only allowed for isochronous endpoints!!!!
extern enum cusbd_endpoint_sync_type cusbd_endpoint_sync_type(const struct cusbd_endpoint *me);

extern enum cusbd_endpoint_transfer_type cusbd_endpoint_transfer_type(const struct cusbd_endpoint *me);

// Only allowed for isochronous endpoints!!!!
extern enum cusbd_endpoint_usage_type cusbd_endpoint_usage_type(const struct cusbd_endpoint *me);

/**
 * @pre @p me previously constructed via @ref cusbd_endpoint().
 * @brief Returns true if the supplied endpoint descriptor contains
 * valid data and was properly constructed via @ref cusbd_endpoint(). 
 * False otherwise.
 * 
 * @param me Interface descriptor to check.
 */
extern bool cusbd_endpoint_valid(const struct cusbd_endpoint *me);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* CUSBD_ENDPOINT_H_ */
