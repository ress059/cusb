/**
 * @file
 * @brief Collection of all events user can dispatch to
 * a USB device. Also defines a common base event class.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-07-03
 * @copyright Copyright (c) 2025
 */

#ifndef CUSBD_EVENT_H_
#define CUSBD_EVENT_H_

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <stdbool.h>
#include <stdint.h>

/* ECU. */
#include "ecu/attributes.h"

/*------------------------------------------------------------*/
/*---------------------- DEFINES AND MACROS ------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Upcasts derived event pointer into
 * a @ref cusbd_event base class pointer. This macro 
 * encapsulates the cast. It allows the application to 
 * dispatch derived events to library functions that take
 * in a base class pointer.
 * 
 * @param me_ Pointer to derived event. This must inherit
 * @ref cusbd_event base class.
 */
#define CUSBD_EVENT_BASE_CAST(e_) \
    ((const struct cusbd_event *)(e_))

/*------------------------------------------------------------*/
/*---------------------- BASE EVENT CLASS --------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Identifies the type of event.
 */
enum cusbd_event_id
{
    CUSBD_EVENT_ID_RESERVED,            /**< Dummy event ID. Allows library to detect if event was constructed. ID 0-initialized to this value if not. */
    /*****************************/
    CUSBD_EVENT_ID_SETUP_PACKET_RX,     /**< Device received a request (setup packet in a control transfer) from the host. */
    /*****************************/
    CUSBD_EVENT_ID_COUNT                /**< Total number of event IDs. */
};

/**
 * @brief Base event class.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_event
{
    /// @brief Identifies the type of event.
    enum cusbd_event_id id;
};

/*------------------------------------------------------------*/
/*----------------- CUSBD_STD_REQUEST_RX_EVENT ---------------*/
/*------------------------------------------------------------*/

/**
 * @brief Setup packet contents for a USB device request.
 * End user will memcopy setup packets received on endpoint 
 * zero into this struct.
 * 
 * @warning Will always be stored in little endian, not
 * native endianness.
 */
struct cusbd_setup_packet
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

/**
 * @brief Application should create this event using
 * @ref cusbd_setup_packet_rx_event_ctor() and dispatch
 * it to the USB device when a request (setup packet from
 * a control transfer) is received from the host on endpoint 
 * zero.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_setup_packet_rx_event
{
    /// @brief Inherit @ref cusbd_event base class.
    /// @warning MUST be first member.
    struct cusbd_event base;

    /// @brief Request data. A local copy is stored
    /// in case the user's original is destroyed.
    /// @warning This struct is packed and will always 
    /// be in little endian.
    struct cusbd_setup_packet packet;
};

/*------------------------------------------------------------*/
/*----------------------- PUBLIC FUNCTIONS -------------------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Returns true if the supplied event was properly
 * constructed. False otherwise.
 * 
 * @param me Derived event to check.
 */
extern bool cusbd_event_valid(const struct cusbd_event *me);

/*------------------------------------------------------------*/
/*----------------- CUSBD_STD_REQUEST_RX_EVENT ---------------*/
/*------------------------------------------------------------*/
/**
 * @name cusbd_setup_packet_rx_event
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @pre Setup packet mem-copied from endpoint zero buffer into @p request.
 * @brief Creates a @ref cusbd_setup_packet_rx_event. The
 * application should create this event using this function
 * and dispatch it to the USB device when a request (setup packet 
 * from a control transfer) is received from the host on endpoint 
 * zero.
 * 
 * @param me Event to create.
 * @param packet Setup packet received from host. Must
 * be directly mem-copied. Library will handle endianness.
 */
extern void cusbd_setup_packet_rx_event_ctor(struct cusbd_setup_packet_rx_event *me,
                                             const struct cusbd_setup_packet *packet);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* CUSBD_EVENT_H_ */
