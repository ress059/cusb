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
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* CUSB. */
#include "cusbd/descriptor.h"

/* ECU. */
#include "ecu/attributes.h"

/*------------------------------------------------------------*/
/*---------------------- DEFINES AND MACROS ------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Creates an @ref cusbd_endpoint_api instance at compile-time.
 * Example usage:
 * @code{.c}
 * static const struct cusbd_endpoint_api api = CUSBD_ENDPOINT_API_CTOR(
 *      &ep_configure, &ep_deconfigure, ...
 * );
 * @endcode
 * 
 * See description of @ref cusbd_endpoint_api members for function
 * details. For the object parameter, supply @ref CUSBD_ENDPOINT_OBJ_UNUSED
 * if unused.
 */
#define CUSBD_ENDPOINT_API_CTOR(configure_,     \
                                deconfigure_,   \
                                halt_,          \
                                send_,          \
                                stall_,         \
                                obj_)           \
    {                                           \
        .configure = (configure_),              \
        .deconfigure = (deconfigure_),          \
        .halt = (halt_),                        \
        .send = (send_),                        \
        .stall = (stall_),                      \
        .obj = (obj_)                           \
    }

/**
 * @brief Supplied when optional objects in endpoint callbacks
 * are unused.
 */
#define CUSBD_ENDPOINT_OBJ_UNUSED \
    ((void *)0)

/*------------------------------------------------------------*/
/*----------------------- CUSBD ENDPOINT ---------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Status of endpoint.
 */
enum cusbd_endpoint_status
{
    CUSBD_ENDPOINT_STATUS_ACK,      /**< Request was successfully processed. */
    CUSBD_ENDPOINT_STATUS_NAK,      /**< Request cannot be processed. Device is currently busy. */
    CUSBD_ENDPOINT_STATUS_STALL,    /**< Request successfully processed but invalid request. Request error. */
};

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
    CUSBD_ENDPOINT_TRANSFER_TYPE_CONTROL,       /**< [1:0] = 00. Endpoint0. Control endpoint. */
    CUSBD_ENDPOINT_TRANSFER_TYPE_ISOCHRONOUS,   /**< [1:0] = 01. Endpoint used for isochronous transfers. */
    CUSBD_ENDPOINT_TRANSFER_TYPE_BULK,          /**< [1:0] = 10. Endpoint used for bulk transfers. */
    CUSBD_ENDPOINT_TRANSFER_TYPE_INTERRUPT      /**< [1:0] = 11. Endpoint used for interrupt transfers. */
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
 * @brief Dependency injection. Links library with user's 
 * hardware-specific code that controls the endpoint. Must 
 * be initialized at compile-time. The supplied endpoint can
 * be passed to the cusbd_endpoint() API to get its characteristics.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_endpoint_api
{
    /// @brief Called when device first starts up or the characteristics 
    /// of an endpoint must change due to a SET_CONFIGURATION() or SET_INTERFACE()
    /// being processed.
    void (*const configure)(const struct cusbd_endpoint *me, void *obj);

    /// @brief Called when the endpoint must be deconfigured (stopped) due to 
    /// a SET_INTERFACE() request.
    void (*const deconfigure)(const struct cusbd_endpoint *me, void *obj);

    /// @brief Called when endpoint must be halted due to a SET_FEATURE(ENDPOINT_HALT)
    /// request or some other event. The halted endpoint must always return STALL
    /// when it is addressed until it is unhalted.
    void (*const halt)(const struct cusbd_endpoint *me, void *obj);

    /// @brief Called when data has to be sent across the supplied 
    /// endpoint's pipe.
    void (*const send)(const struct cusbd_endpoint *me, const void *data, size_t len, void *obj);

    /// @brief Called when endpoint must reply back to host with STALL
    /// in the data or status stage of a transfer. The STALL only occurs
    /// once (in response to the host's request) and is NOT permanent.
    void (*const stall)(const struct cusbd_endpoint *me, void *obj);

    /// @brief Called when the endpoint must be unhalted due to a CLEAR_FEATURE(ENDPOINT_HALT),
    /// request, SET_CONFIGURATION() request, SET_INTERFACE() request or some other
    /// event. The endpoint should no longer STALL when it is addressed.
    void (*const unhalt)(const struct cusbd_endpoint *me, void *obj);

    /// @brief Optional object passed into endpoint API.
    void *const obj;
};

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
    /// @brief Inherit base descriptor class
    /// @warning MUST be first member.
    struct cusbd_descriptor base;

    /// @brief Dependency injection. Links library with user's
    /// hardware-specific code that controls the endpoint.
    const struct cusbd_endpoint_api *api;

    /// @brief Descriptor data. A copy is stored so the API can
    /// automatically adjust it as the device is updated.
    /// @warning This struct is packed and will always be in 
    /// little endian.
    struct cusbd_endpoint_descriptor descriptor;

    /// @brief True = endpoint halted and always returns STALL.
    /// False = endpoint active.
    bool halted;
};

/*------------------------------------------------------------*/
/*---------------- CUSBD ENDPOINT MEMBER FUNCTIONS -----------*/
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
 * @pre @p api previously constructed via @ref CUSBD_ENDPOINT_API_CTOR().
 * @brief Endpoint descriptor constructor.
 * 
 * @warning This cannot be called on an active endpoint 
 * descriptor. Doing so is undefined behavior.
 * @warning This should not be called to initialize endpoint0
 * since there is never a descriptor for endpoint0.
 * 
 * @param me Endpoint descriptor to construct.
 * @param api Collection of user-defined functions that control the endpoint.
 * See @ref cusbd_endpoint_api.
 * @param bEndpointAddress bEndpointAddress field in the endpoint descriptor.
 * See USB spec.
 * @param bmAttributes bmAttributes field in the endpoint descriptor.
 * See USB spec.
 * @param wMaxPacketSize wMaxPacketSize field in the endpoint descriptor.
 * See USB spec. Library automatically handles endianness so supply a raw
 * value. Do not try to supply this in little endian.
 * @param bInterval bInterval field in the endpoint descriptor.
 * See USB spec.
 */
extern void cusbd_endpoint_ctor(struct cusbd_endpoint *me,
                                const struct cusbd_endpoint_api *api,
                                uint8_t bEndpointAddress,
                                uint8_t bmAttributes,
                                uint16_t wMaxPacketSize,
                                uint8_t bInterval);
/**@}*/

/**
 * @name Member Functions
 */
/**@{*/
/**
 * @pre @p me previously constructed via @ref cusbd_endpoint_ctor().
 * @brief Application should not use - called automatically by the library.
 * Calls @ref cusbd_endpoint_api.configure. See @ref cusbd_endpoint_api.configure.
 * 
 * @param me Endpoint to configure.
 */
extern void cusbd_endpoint_configure(struct cusbd_endpoint *me);

/**
 * @pre @p me previously constructed via @ref cusbd_endpoint_ctor().
 * @brief Application should not use - called automatically by the library.
 * Calls @ref cusbd_endpoint_api.deconfigure. See @ref cusbd_endpoint_api.deconfigure.
 * 
 * @param me Endpoint to deconfigure.
 */
extern void cusbd_endpoint_deconfigure(struct cusbd_endpoint *me);

/**
 * @pre @p me previously constructed via @ref cusbd_endpoint_ctor().
 * @brief Returns the endpoint's direction, as specified in
 * bEndpointAddress.
 * 
 * @param me Endpoint to check.
 */
extern enum cusbd_endpoint_direction cusbd_endpoint_direction(const struct cusbd_endpoint *me);

/**
 * @pre @p me previously constructed via @ref cusbd_endpoint_ctor().
 * @brief Application should not use - called automatically by the library.
 * Calls @ref cusbd_endpoint_api.halt. See @ref cusbd_endpoint_api.halt.
 * 
 * @param me Endpoint to halt.
 */
extern void cusbd_endpoint_halt(struct cusbd_endpoint *me);

/**
 * @pre @p me previously constructed via @ref cusbd_endpoint_ctor().
 * @brief Returns true if the endpoint is currently halted. False
 * otherwise.
 * 
 * @param me Endpoint to check.
 */
extern bool cusbd_endpoint_halted(const struct cusbd_endpoint *me);

/**
 * @pre @p me previously constructed via @ref cusbd_endpoint_ctor().
 * @brief Returns the endpoint's number, as specified in bEndpointAddress.
 * 
 * @param me Endpoint to check.
 */
extern uint8_t cusbd_endpoint_number(const struct cusbd_endpoint *me);

/**
 * @pre @p me previously constructed via @ref cusbd_endpoint_ctor().
 * @brief Application should not use - called automatically by the library.
 * Calls @ref cusbd_endpoint_api.send. See @ref cusbd_endpoint_api.send.
 * 
 * @param me Endpoint to configure.
 * @param data Data to send.
 * @param len Number of bytes of @p data.
 */
extern void cusbd_endpoint_send(struct cusbd_endpoint *me, const void *data, size_t len);

/**
 * @pre @p me previously constructed via @ref cusbd_endpoint_ctor().
 * @brief Application should not use - called automatically by the library.
 * Calls @ref cusbd_endpoint_api.stall. See @ref cusbd_endpoint_api.stall.
 * 
 * @param me Endpoint to stall.
 */
extern void cusbd_endpoint_stall(struct cusbd_endpoint *me);

/**
 * @pre @p me previously constructed via @ref cusbd_endpoint_ctor().
 * @brief Returns the endpoint's sync type, as specified in bmAttributes.
 * Only relevant for isochronous endpoints.
 * 
 * @param me Endpoint to check.
 */
extern enum cusbd_endpoint_sync_type cusbd_endpoint_sync_type(const struct cusbd_endpoint *me);

/**
 * @pre @p me previously constructed via @ref cusbd_endpoint_ctor().
 * @brief Returns the endpoint's transfer type, as specified in 
 * bmAttributes.
 * 
 * @param me Endpoint to check.
 */
extern enum cusbd_endpoint_transfer_type cusbd_endpoint_transfer_type(const struct cusbd_endpoint *me);

/**
 * @pre @p me previously constructed via @ref cusbd_endpoint_ctor().
 * @brief Application should not use - called automatically by the library.
 * Calls @ref cusbd_endpoint_api.unhalt. See @ref cusbd_endpoint_api.unhalt.
 * 
 * @param me Endpoint to unhalt.
 */
extern void cusbd_endpoint_unhalt(struct cusbd_endpoint *me);

/**
 * @pre @p me previously constructed via @ref cusbd_endpoint_ctor().
 * @brief Returns the endpoint's usage type, as specified in 
 * bmAttributes. Only relevant for isochronous endpoints.
 * 
 * @param me Endpoint to check.
 */
extern enum cusbd_endpoint_usage_type cusbd_endpoint_usage_type(const struct cusbd_endpoint *me);

/**
 * @brief Returns true if the supplied endpoint descriptor contains
 * valid data and was properly constructed via @ref cusbd_endpoint_ctor(). 
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
