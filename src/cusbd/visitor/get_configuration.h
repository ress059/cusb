/**
 * @file
 * @brief Concrete visitor that processes GET_CONFIGURATION() request.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-07-04
 * @copyright Copyright (c) 2025
 */

#ifndef CUSBD_VISITOR_GET_CONFIGURATION_H_
#define CUSBD_VISITOR_GET_CONFIGURATION_H_

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* CUSB. */
#include "cusbd/visitor/request.h"
#include "cusbd/visitor/visitor.h"

/*------------------------------------------------------------*/
/*-------------------------- VISITOR -------------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Visitor that processes GET_CONFIGURATION() request.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_visitor_get_configuration
{
    /// @brief Inherit @ref cusbd_cvisitor base class.
    /// @warning MUST be first member.
    struct cusbd_cvisitor base;
    
    /// @brief Translated setup packet for easier use.
    struct cusbd_request request;

    /// @brief Populated with the currently active 
    /// configuration (bConfigurationValue) once request
    /// is processed.
    uint8_t configuration_value;
};

/*------------------------------------------------------------*/
/*------------------- VISITOR MEMBER FUNCTIONS ---------------*/
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
 * @brief Create visitor that processes GET_CONFIGURATION() request.
 * 
 * @param me Visitor to create.
 * @param e Setup packet event received by user. This is an event
 * as opposed to the raw data in @ref cusbd_setup_packet to limit 
 * this visitor's use to only within the device's state machine 
 * implementation.
 * @param state State the USB device was in when the
 * request was received.
 */
extern void cusbd_visitor_get_configuration_ctor(struct cusbd_visitor_get_configuration *me,
                                                 const struct cusbd_setup_packet_rx_event *e,
                                                 enum cusbd_request_state state);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* CUSBD_VISITOR_GET_CONFIGURATION_H_ */
