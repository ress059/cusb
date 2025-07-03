/**
 * @file
 * @brief Contains specific logic for processing standard USB device requests.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-06-15
 * @copyright Copyright (c) 2025
 */

#ifndef CUSBD_STD_REQUEST_H_
#define CUSBD_STD_REQUEST_H_

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <stdbool.h>
#include <stddef.h>

/* CUSB. */
#include "cusbd/visitor.h"

/*------------------------------------------------------------*/
/*-------------- CUSBD STANDARD REQUESTS COMMON --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Status returned when USB device request is
 * processed. Return values < CUSBD_REQUEST_STATUS_PROPAGATE
 * means the request was processed and should not be
 * propagated to the rest of the descriptor tree.
 */
enum cusbd_std_request_status
{
    CUSBD_STD_REQUEST_STATUS_ACK,           /**<! Request was successfully processed. */
    CUSBD_STD_REQUEST_STATUS_STALL,         /**<! Request processed but invalid. Device should indicate error to host via STALL. */
    /*******************************/
    CUSBD_STD_REQUEST_STATUS_UNPROCESSED    /**<! Default initialization value. Request not yet processed. */
};

/**
 * @brief State the USB device is in when the 
 * standard request was received.
 */
enum cusbd_std_request_state
{
    CUSBD_STD_REQUEST_DEFAULT_STATE,    /**<! Device is in default state. */
    CUSBD_STD_REQUEST_ADDRESS_STATE,    /**<! Device is in address state. */
    CUSBD_STD_REQUEST_CONFIGURED_STATE, /**<! Device is in configured state. */
    /*********************************/
    CUSBD_STD_REQUEST_STATE_COUNT       /**<! Number of device states. */
};

/**
 * @brief Common data for processing standard host-to-device 
 * requests. Not meant to be used directly.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_std_request_out
{
    /// @brief The USB device request packet.
    /// @warning Must be in little endian.
    const struct cusb_request *packet;

    /// @brief State the USB device is in when the standard 
    /// request was received.
    enum cusbd_std_request_state state;

    /// @brief Stored for easy access. Initialized in constructor 
    /// and asserted this is always OUT. 
    enum cusbd_request_direction direction;

    /// @brief Stored for easy access. Initialized in constructor.
    enum cusbd_request_type type;

    /// @brief Stored for easy access. Initialized in constructor.
    enum cusbd_request_recipient recipient;

    /// @brief Request done procesing and descriptor tree iteration
    /// can be terminated if value less than @ref CUSBD_REQUEST_STATUS_UNPROCESSED
    /// Device responsible for polling status.
    enum cusbd_std_request_status status;
};

/**
 * @brief Common data for processing standard device-to-host 
 * requests. Not meant to be used directly.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_std_request_in
{
    /// @brief The USB device request packet.
    /// @warning Must be in little endian.
    const struct cusb_request *packet;

    /// @brief State the USB device is in when the standard 
    /// request was received.
    enum cusbd_std_request_state state;

    /// @brief User-supplied buffer. Request populates data into
    /// this buffer which must be sent to the host.
    /// @warning Data always populatd in little endian.
    void *buf;

    /// @brief Stored for easy access. Initialized in constructor 
    /// and asserted this is always OUT. 
    enum cusbd_request_direction direction;

    /// @brief Stored for easy access. Initialized in constructor.
    enum cusbd_request_type type;

    /// @brief Stored for easy access. Initialized in constructor.
    enum cusbd_request_recipient recipient;

    /// @brief Request done procesing and descriptor tree iteration
    /// can be terminated if value less than @ref CUSBD_REQUEST_STATUS_UNPROCESSED
    /// Device responsible for polling status.
    enum cusbd_std_request_status status;
};

/*------------------------------------------------------------*/
/*------------------ CUSBD STANDARD REQUESTS -----------------*/
/*------------------------------------------------------------*/

struct cusbd_std_request_clear_feature
{
!!!! TODO Stopped here
};


/**
 * @brief Concrete visitor that processes standard
 * host-to-device requests while device is in the
 * default state.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_std_request_out_default_state
{
    /// @brief Inherit base visitor class.
    /// @warning MUST be first member.
    struct cusbd_visitor visitor;

    /// @brief Common data for all standard host-to-device requests.
    struct cusbd_std_request_out request;
};

/**
 * @brief Concrete visitor that processes standard
 * host-to-device requests while device is in the
 * address state.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_std_request_out_address_state
{
    /// @brief Inherit base visitor class.
    /// @warning MUST be first member.
    struct cusbd_visitor visitor;

    /// @brief Common data for all standard host-to-device requests.
    struct cusbd_std_request_out request;
};

/**
 * @brief Concrete visitor that processes standard
 * host-to-device requests while device is in the
 * configured state.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_std_request_out_configured_state
{
    /// @brief Inherit base visitor class.
    /// @warning MUST be first member.
    struct cusbd_visitor visitor;

    /// @brief Common data for all standard host-to-device requests.
    struct cusbd_std_request_out request;
};

/*------------------------------------------------------------*/
/*------------------ CUSBD STANDARD REQUEST IN ---------------*/
/*------------------------------------------------------------*/



struct asdf
{
    /// @brief Inherit base visitor class.
    /// @warning MUST be first member.
    struct cusbd_visitor visitor;
};





#endif /* CUSBD_STD_REQUEST_H_ */
























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
 * @pre End user mem-copied control endpoint buffer into @p request. 
 * @brief Returns the actual request. I.e. CLEAR_FEATURE, GET_CONFIGURATION,
 * etc. Returns @ref CUSBD_REQUEST_VALUE_UNDEFINED if setup packet contains
 * garbage value.
 * 
 * @param request Setup packet of request to check.
 */
extern enum cusbd_request_value cusbd_request_value(const struct cusb_request *request);

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