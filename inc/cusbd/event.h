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
#include <stddef.h>
#include <stdint.h>

/* ECU. */
#include "ecu/attributes.h"
#include "ecu/event.h"

/*------------------------------------------------------------*/
/*---------------------- DEFINES AND MACROS ------------------*/
/*------------------------------------------------------------*/

/**
 * @name Base Event
 */
/**@{*/
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
#define CUSBD_EVENT_CONST_BASE_CAST(e_) \
    ((const struct cusbd_event *)(e_))

/*------------------------------------------------------------*/
/*---------------------- BASE EVENT CLASS --------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Identifies the type of event.
 */
enum cusbd_event_id
{
    CUSBD_RESERVED_EVENT_ID = ECU_USER_EVENT_ID_BEGIN, /**< Dummy event ID. Allows library to detect if event was constructed. ID 0-initialized to this value if not. */
    /*****************************/
    CUSBD_RESET_EVENT_ID,           /**< Host issued a reset signal to device. */
    CUSBD_RESUME_EVENT_ID,          /**< Bus activity has resumed. */
    CUSBD_SETUP_PACKET_RX_EVENT_ID, /**< Device received setup packet from host (status stage of control transfer). */
    CUSBD_SUSPEND_EVENT_ID,         /**< Bus activity from the host has stopped for a period of time specified by USB spec. */
    /*****************************/
    CUSBD_TOTAL_EVENT_IDS           /**< Total number of event IDs. */
};
/**@}*/

/*------------------------------------------------------------*/
/*--------------------- CUSBD_RESET_EVENT --------------------*/
/*------------------------------------------------------------*/

/**
 * @name cusbd_reset_event
 */
/**@{*/
/**
 * @brief Application should create this event with
 * @ref cusbd_reset_event_ctor() and dispatch it to
 * the USB device when a USB reset signal is issued 
 * by the host.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_reset_event
{
    /// @brief Inherit base event class.
    /// @warning MUST be first member.
    struct ecu_event base;
};
/**@}*/

/*------------------------------------------------------------*/
/*--------------------- CUSBD_RESUME_EVENT -------------------*/
/*------------------------------------------------------------*/

/**
 * @name cusbd_resume_event
 */
/**@{*/
/**
 * @brief Application should create this event using
 * @ref cusbd_resume_event_ctor() and dispatch
 * it to the USB device when bus activity has resumed
 * after being suspended.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_resume_event
{
    /// @brief Inherit base event class.
    /// @warning MUST be first member.
    struct ecu_event base;
};
/**@}*/

/*------------------------------------------------------------*/
/*----------------- CUSBD_SETUP_PACKET_RX_EVENT --------------*/
/*------------------------------------------------------------*/

/**
 * @name cusbd_setup_packet_rx_event
 */
/**@{*/
/**
 * @brief Data packet contents sent by host in the status 
 * stage of a control transfer.
 * 
 * @warning Must be stored in little endian, not
 * native endianness.
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
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
 * it to the USB device when the data packet (setup packet) 
 * in the status stage of a control transfer is received 
 * from the host.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_setup_packet_rx_event
{
    /// @brief Inherit base event class.
    /// @warning MUST be first member.
    struct ecu_event base;

    /// @brief Data packet contents sent by host in the 
    /// status stage of a control transfer. A local copy 
    /// is stored in case the user's original is destroyed.
    /// @warning This struct is packed and will always 
    /// be in little endian.
    struct cusbd_setup_packet packet;
};
/**@}*/

/*------------------------------------------------------------*/
/*-------------------- CUSBD_SUSPEND_EVENT -------------------*/
/*------------------------------------------------------------*/

/**
 * @name cusbd_suspend_event
 */
/**@{*/
/**
 * @brief Application should create this event using
 * @ref cusbd_suspend_event_ctor() and dispatch
 * it to the USB device when bus activity from the host
 * has stopped.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_suspend_event
{
    /// @brief Inherit base event class.
    /// @warning MUST be first member.
    struct ecu_event base;
};
/**@}*/


// /*------------------------------------------------------------*/
// /*-------------------- CUSBD_BUS_POWERED_EVENT ---------------*/
// /*------------------------------------------------------------*/

// struct cusbd_power_source_change_event
// {
//     /// @brief Inherit @ref cusbd_event base class.
//     /// @warning MUST be first member.
//     struct cusbd_event base;
// };

/*------------------------------------------------------------*/
/*--------------------- CUSBD_RESET_EVENT --------------------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name cusbd_reset_event
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @brief Creates a @ref cusbd_reset_event. The
 * application should create this event using this function
 * and dispatch it to the USB device when a USB reset signal 
 * is issued by the host.
 * 
 * @param me Event to create.
 */
extern void cusbd_reset_event_ctor(struct cusbd_reset_event *me);
/**@}*/

/*------------------------------------------------------------*/
/*--------------------- CUSBD_RESUME_EVENT -------------------*/
/*------------------------------------------------------------*/

/**
 * @name cusbd_resume_event
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @brief Creates a @ref cusbd_resume_event. The
 * application should create this event using this function
 * and dispatch it to the USB device when bus activity has resumed
 * after being suspended.
 * 
 * @param me Event to create.
 */
extern void cusbd_resume_event_ctor(struct cusbd_resume_event *me);
/**@}*/

/*------------------------------------------------------------*/
/*---------------- CUSBD_SETUP_PACKET_RX_EVENT ---------------*/
/*------------------------------------------------------------*/

/**
 * @name cusbd_setup_packet_rx_event
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @brief Creates a @ref cusbd_setup_packet_rx_event. The
 * application should create this event using this function
 * and dispatch it to the USB device when the data packet 
 * (setup packet) in the status stage of a control transfer 
 * is received from the host.
 * 
 * @param me Event to create.
 * @param data The setup packet contents in @ref cusbd_setup_packet.
 * @param len Number of bytes of @p data.
 */
extern void cusbd_setup_packet_rx_event_ctor(struct cusbd_setup_packet_rx_event *me,
                                             const void *data,
                                             size_t len);
/**@}*/

/*------------------------------------------------------------*/
/*-------------------- CUSBD_SUSPEND_EVENT -------------------*/
/*------------------------------------------------------------*/

/**
 * @name cusbd_suspend_event
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @brief Creates a @ref cusbd_suspend_event. The
 * application should create this event using this function
 * and dispatch it to the USB device when bus activity from
 * the host has stopped.
 * 
 * @param me Event to create.
 */
extern void cusbd_suspend_event_ctor(struct cusbd_suspend_event *me);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* CUSBD_EVENT_H_ */
