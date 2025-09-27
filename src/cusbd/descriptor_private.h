/**
 * @file
 * @brief Private contents of @ref descriptor.h.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-09-26
 * @copyright Copyright (c) 2025
 */

#ifndef CUSBD_DESCRIPTOR_PRIVATE_H_
#define CUSBD_DESCRIPTOR_PRIVATE_H_

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* Public portion. */
#include "cusbd/descriptor.h"

/*------------------------------------------------------------*/
/*---------------- DESCRIPTOR MEMBER FUNCTIONS ---------------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Member Functions
 */
/**@{*/
/**
 * @brief Used if @ref cusbd_descriptor_v_process_setup_packet() does
 * not apply to the descriptor.
 */
extern size_t CUSBD_DESCRIPTOR_V_PROCESS_SETUP_PACKET_IN_UNUSED(const struct cusbd_descriptor *me, 
                                                                const struct cusbd_setup_packet *packet, 
                                                                enum cusbd_descriptor_state state,
                                                                void *buffer, 
                                                                size_t buffer_len);
/**
 * @brief Used if @ref cusbd_descriptor_v_process_setup_packet_out() does
 * not apply to the descriptor.
 */
extern bool CUSBD_DESCRIPTOR_V_PROCESS_SETUP_PACKET_OUT_UNUSED(struct cusbd_descriptor *me, 
                                                               const struct cusbd_setup_packet *packet,
                                                               enum cusbd_descriptor_state state);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* CUSBD_DESCRIPTOR_PRIVATE_H_ */
