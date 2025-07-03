/**
 * @file
 * @brief See @ref event.h.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-07-03
 * @copyright Copyright (c) 2025
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* Translation unit. */
#include "cusbd/event.h"

/* STDLib. */
#include <stddef.h>
#include <string.h> /* memcpy. */

/* ECU. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_NAME("cusbd/event.c")

/*------------------------------------------------------------*/
/*---------------------- DEFINES AND MACROS ------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Verifies, at compile-time, the concrete event
 * correctly inherits @ref cusbd_event base class. 
 * Returns true if correctly inherited. False otherwise.
 * 
 * @param base_ Name of @ref cusbd_event <b>member</b> 
 * within concrete event type.
 * @param derived_ Concrete event type to check.
 */
#define CUSBD_EVENT_IS_BASEOF(base_, derived_) \
    ((bool)(offsetof(derived_, base_) == (size_t)0))

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Base event constructor. Hidden to prevent
 * user from dispatching base event directly.
 */
static void cusbd_event_ctor(struct cusbd_event *me, enum cusbd_event_id id);

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
/*------------------------------------------------------------*/

static void cusbd_event_ctor(struct cusbd_event *me, enum cusbd_event_id id)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (id > CUSBD_EVENT_ID_RESERVED && id < CUSBD_EVENT_ID_COUNT) );
    me->id = id;
}

/*------------------------------------------------------------*/
/*---------------------- STATIC ASSERTS ----------------------*/
/*------------------------------------------------------------*/

ECU_STATIC_ASSERT( (sizeof(struct cusbd_setup_packet) == (size_t)8),
                    "Setup packet is 8 bytes." );

ECU_STATIC_ASSERT( (CUSBD_EVENT_IS_BASEOF(base, struct cusbd_setup_packet_rx_event)),
                    "cusbd_setup_packet_rx_event must inherit cusbd_event." );

/*------------------------------------------------------------*/
/*----------------------- PUBLIC FUNCTIONS -------------------*/
/*------------------------------------------------------------*/

bool cusbd_event_valid(const struct cusbd_event *me)
{
    /* This should be enough to verify any derived event was properly constructed
    since ID default-initializes to 0 which is CUSBD_EVENT_ID_RESERVED. The base
    event constructor is private so this only fails if user manually changes
    struct contents, which the API clearly states is forbidden. */
    ECU_RUNTIME_ASSERT( (me) );
    return (me->id > CUSBD_EVENT_ID_RESERVED && me->id < CUSBD_EVENT_ID_COUNT);
}

/*------------------------------------------------------------*/
/*----------------- CUSBD_STD_REQUEST_RX_EVENT ---------------*/
/*------------------------------------------------------------*/

void cusbd_setup_packet_rx_event_ctor(struct cusbd_setup_packet_rx_event *me,
                                      const struct cusbd_setup_packet *packet)
{
    ECU_RUNTIME_ASSERT( (me && packet) );
    cusbd_event_ctor(&me->base, CUSBD_EVENT_ID_SETUP_PACKET_RX);
    memcpy(&me->packet, packet, sizeof(struct cusbd_setup_packet));
}
