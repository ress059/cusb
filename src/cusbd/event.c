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
#include <string.h> /* memcpy. */

/* ECU. */
#include "ecu/asserter.h"
#include "ecu/utils.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_FILE("cusbd/event.c")

/*------------------------------------------------------------*/
/*---------------------- STATIC ASSERTS ----------------------*/
/*------------------------------------------------------------*/

ECU_STATIC_ASSERT( (sizeof(struct cusbd_setup_packet) == (size_t)8),
                    "Setup packet is 8 bytes." );

ECU_STATIC_ASSERT( (ECU_EVENT_IS_BASE_OF(base, struct cusbd_reset_event)),
                    "Event must inherit ecu_event." );

ECU_STATIC_ASSERT( (ECU_EVENT_IS_BASE_OF(base, struct cusbd_resume_event)),
                    "Event must inherit ecu_event." );

ECU_STATIC_ASSERT( (ECU_EVENT_IS_BASE_OF(base, struct cusbd_setup_packet_rx_event)),
                    "Event must inherit ecu_event." );

ECU_STATIC_ASSERT( (ECU_EVENT_IS_BASE_OF(base, struct cusbd_suspend_event)),
                    "Event must inherit ecu_event." );

/*------------------------------------------------------------*/
/*--------------------- CUSBD_RESET_EVENT --------------------*/
/*------------------------------------------------------------*/

void cusbd_reset_event_ctor(struct cusbd_reset_event *me)
{
    ECU_ASSERT( (me) );
    ecu_event_ctor(ECU_EVENT_BASE_CAST(me), CUSBD_RESET_EVENT_ID, sizeof(*me));
}

/*------------------------------------------------------------*/
/*--------------------- CUSBD_RESUME_EVENT -------------------*/
/*------------------------------------------------------------*/

void cusbd_resume_event_ctor(struct cusbd_resume_event *me)
{
    ECU_ASSERT( (me) );
    ecu_event_ctor(ECU_EVENT_BASE_CAST(me), CUSBD_RESUME_EVENT_ID, sizeof(*me));
}

/*------------------------------------------------------------*/
/*----------------- CUSBD_SETUP_PACKET_RX_EVENT --------------*/
/*------------------------------------------------------------*/

void cusbd_setup_packet_rx_event_ctor(struct cusbd_setup_packet_rx_event *me,
                                      const void *data,
                                      size_t len)
{
    ECU_ASSERT( (me && data) );
    ECU_ASSERT( (len == sizeof(me->packet)) );
    ecu_event_ctor(ECU_EVENT_BASE_CAST(me), CUSBD_SETUP_PACKET_RX_EVENT_ID, sizeof(*me));
    memcpy(&me->packet, data, sizeof(me->packet));
}

/*------------------------------------------------------------*/
/*-------------------- CUSBD_SUSPEND_EVENT -------------------*/
/*------------------------------------------------------------*/

void cusbd_suspend_event_ctor(struct cusbd_suspend_event *me)
{
    ECU_ASSERT( (me) );
    ecu_event_ctor(ECU_EVENT_BASE_CAST(me), CUSBD_SUSPEND_EVENT_ID, sizeof(*me));
}
