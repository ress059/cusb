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

/**
 * @brief Type given to endpoint ID (@ref cusbd_endpoint.id).
 * This allows the library to implicitly typecase between
 * @ref cusbd_endpoint_reserved_ids and the user-specified
 * endpoint IDs. Typedeffed incase this has to change in
 * the future.
 * 
 * @warning This must be a signed type so reserved enumerations
 * less than 0 can be stored.
 */
typedef int16_t cusbd_endpoint_id_t;

/**
 * @brief Possible endpoint types, as defined by USB.
 */
enum cusbd_endpoint_type
{
    CUSBD_ENDPOINT_TYPE_CONTROL,     /**<! Endpoint0. Control endpoint. */
    CUSBD_ENDPOINT_TYPE_INTERRUPT,   /**<! Endpoint used for interrupt transfers. */
    CUSBD_ENDPOINT_TYPE_ISOCHRONOUS, /**<! Endpoint used for isochronous transfers. */
    CUSBD_ENDPOINT_TYPE_BULK,        /**<! Endpoint used for bulk transfers. */
    /****************************/
    CUSBD_ENDPOINT_TYPE_COUNT        /**<! Total number of endpoint types defined by USB. */
};

/**
 * @brief Reserved endpoint IDs.
 */
enum cusbd_endpoint_reserved_ids
{
    CUSBD_ENDPOINT0_OUT_ID = -2,     /**<! RESERVED. ID assigned to control endpoint OUT. */
    CUSBD_ENDPOINT0_IN_ID = -1,      /**<! RESERVED. ID assigned to control endpoint IN. */
    /***************************/
    CUSBD_ENDPOINT_USER_ID_BEGIN     /**<! Start of user-specified endpoint IDs. Will always be 0. */
};

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

    /// @brief Each endpoint is parameterized by this user-specified
    /// ID. The value maps to a specific endpoint on the target device. 
    /// I.e. id of 0 == endpoint1 IN, id of 1 == endpoint1 OUT, etc.
    /// @warning This must never be used to identify endpoint0.
    /// The ID must always be >= @ref CUSBD_ENDPOINT_USER_ID_BEGIN.
    cusbd_endpoint_id_t id;
};

/*------------------------------------------------------------*/
/*---------------- CUSBD ENDPOINT MEMBER FUNCTIONS -----------*/
/*------------------------------------------------------------*/

!!!!!!!!! TODO Stopped here. Have to make std requests for endpoint and update API.

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name CUSBD Endpoint Constructors
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
 * @param id Each endpoint is parameterized by this user-specified
 * ID. The value maps to a specific endpoint on the target device. 
 * I.e. id of 0 == endpoint1 IN, id of 1 == endpoint1 OUT, etc.
 * This must never be used to identify endpoint0.
 * This ID must always be >= @ref CUSBD_ENDPOINT_USER_ID_BEGIN.
 */
extern void cusbd_endpoint_ctor(struct cusbd_endpoint *me,
                                const struct cusbd_endpoint_descriptor *descriptor,
                                cusbd_endpoint_id_t id);
/**@}*/

/**
 * @name CUSBD Endpoint Member Functions
 */
/**@{*/
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
