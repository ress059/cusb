/**
 * @file
 * @brief See @ref descriptor_private.h.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-09-26
 * @copyright Copyright (c) 2025
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* Translation unit. */
#include "cusbd/descriptor_private.h"

/*------------------------------------------------------------*/
/*---------------- DESCRIPTOR MEMBER FUNCTIONS ---------------*/
/*------------------------------------------------------------*/

size_t CUSBD_DESCRIPTOR_V_PROCESS_SETUP_PACKET_IN_UNUSED(const struct cusbd_descriptor *me, 
                                                         const struct cusbd_setup_packet *packet, 
                                                         enum cusbd_descriptor_state state,
                                                         void *buffer, 
                                                         size_t buffer_len)
{
    (void)me;
    (void)packet;
    (void)state;
    (void)buffer;
    (void)buffer_len;
    return 0;
}

bool CUSBD_DESCRIPTOR_V_PROCESS_SETUP_PACKET_OUT_UNUSED(struct cusbd_descriptor *me, 
                                                        const struct cusbd_setup_packet *packet,
                                                        enum cusbd_descriptor_state state)
{
    (void)me;
    (void)packet;
    (void)state;
    return false;
}
