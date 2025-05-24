/**
 * @file
 * @brief Object representing an endpoint descriptor.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-05-16
 * @copyright Copyright (c) 2025
 */

#ifndef CUSB_ENDPOINT_H_
#define CUSB_ENDPOINT_H_

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <stdint.h>

/* CUSB. */
#include "cusb/descriptors.h"

/* ECU. */
#include "ecu/attributes.h"
#include "ecu/dlist.h"
#include "ecu/endian.h"

/*------------------------------------------------------------*/
/*---------------------- DEFINES AND MACROS ------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Creates a @ref cusb_endpoint_descriptor at
 * either compile-time or run-time. Example usage below
 * creates a bulk IN endpoint1 with a max packet size of 
 * 64 bytes:
 * @code{.c}
 * static const struct cusb_endpoint_descriptor endpoint = CUSB_ENDPOINT_DESCRIPTOR_CTOR(
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
#define CUSB_ENDPOINT_DESCRIPTOR_CTOR(bEndpointAddress_,                \
                                      bmAttributes_,                    \
                                      wMaxPacketSize_,                  \
                                      bInterval_)                       \
    {                                                                   \
        .bLength = sizeof(struct cusb_endpoint_descriptor),             \
        .bDescriptorType = CUSB_ENDPOINT_DESCRIPTOR_TYPE,               \
        .bEndpointAddress = (bEndpointAddress_),                        \
        .bmAttributes = (bmAttributes_),                                \
        .wMaxPacketSize = ECU_CPU_TO_LE16_COMPILETIME(wMaxPacketSize_), \
        .bInterval = (bInterval_)                                       \
    }

/*------------------------------------------------------------*/
/*------------------------ CUSB ENDPOINT ---------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Type given to endpoint ID (@ref cusb_endpoint.id).
 * This allows the library to implicitly typecase between
 * @ref cusb_endpoint_reserved_ids and the user-specified
 * endpoint IDs. Typedeffed incase this has to change in
 * the future.
 * 
 * @warning This must be a signed type so reserved enumerations
 * less than 0 can be stored.
 */
typedef int16_t endpoint_id_t;

/**
 * @brief Possible endpoint types, as defined by USB.
 */
enum cusb_endpoint_type
{
    CUSB_ENDPOINT_CONTROL_TYPE,     /**<! Endpoint0. Control endpoint. */
    CUSB_ENDPOINT_INTERRUPT_TYPE,   /**<! Endpoint used for interrupt transfers. */
    CUSB_ENDPOINT_ISOCHRONOUS_TYPE, /**<! Endpoint used for isochronous transfers. */
    CUSB_ENDPOINT_BULK_TYPE,        /**<! Endpoint used for bulk transfers. */
    /****************************/
    CUSB_ENDPOINT_TYPE_COUNT        /**<! Total number of endpoint types defined by USB. */
};

/**
 * @brief Reserved endpoint IDs.
 */
enum cusb_endpoint_reserved_ids
{
    CUSB_ENDPOINT0_OUT_ID = -2,     /**<! RESERVED. ID assigned to control endpoint OUT. */
    CUSB_ENDPOINT0_IN_ID = -1,      /**<! RESERVED. ID assigned to control endpoint IN. */
    /***************************/
    CUSB_ENDPOINT_USER_ID_BEGIN     /**<! Start of user-specified endpoint IDs. Will always be 0. */
};

/**
 * @brief Data in a standard endpoint descriptor.
 * Using the API ensures this is always encoded in
 * little endian format.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusb_endpoint_descriptor
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
struct cusb_endpoint
{
    /// @brief Node in linked list.
    struct ecu_dnode dnode;

    /// @brief Descriptor data. A copy is stored so the API can
    /// automatically adjust it as the device is updated.
    /// @warning This struct is packed and will always be in 
    /// little endian.
    struct cusb_endpoint_descriptor descriptor;

    /// @brief Each endpoint is parameterized by this user-specified
    /// ID. The value maps to a specific endpoint on the target device. 
    /// I.e. id of 0 == endpoint1 IN, id of 1 == endpoint1 OUT, etc.
    /// @warning This must never be used to identify endpoint0.
    /// The ID must always be >= @ref CUSB_ENDPOINT_USER_ID_BEGIN.
    endpoint_id_t id;
};

/*------------------------------------------------------------*/
/*----------------- CUSB ENDPOINT MEMBER FUNCTIONS -----------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name CUSB Endpoint Constructors
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @pre @p descriptor previously constructed via @ref CUSB_ENDPOINT_DESCRIPTOR_CTOR().
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
 * This ID must always be >= @ref CUSB_ENDPOINT_USER_ID_BEGIN.
 */
extern void cusb_endpoint_ctor(struct cusb_endpoint *me,
                               const struct cusb_endpoint_descriptor *descriptor,
                               endpoint_id_t id);
/**@}*/

/**
 * @name CUSB Endpoint Member Functions
 */
/**@{*/
/**
 * @pre @p me previously constructed via @ref cusb_endpoint().
 * @brief Returns true if the supplied endpoint descriptor contains
 * valid data and was properly constructed via @ref cusb_endpoint(). 
 * False otherwise.
 * 
 * @param me Interface descriptor to check.
 */
extern bool cusb_endpoint_valid(const struct cusb_endpoint *me);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* CUSB_ENDPOINT_H_ */
