/**
 * @file
 * @brief Contains private helpers that extract information from
 * a setup packet.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-07-03
 * @copyright Copyright (c) 2025
 */

#ifndef CUSBD_SETUP_PACKET_H_
#define CUSBD_SETUP_PACKET_H_

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <stdint.h>

/* CUSB. */
#include "cusbd/event.h"

/*------------------------------------------------------------*/
/*-------------------- CUSBD_SETUP_PACKET --------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Bit 7 of bmRequestType bitfield 
 * in @ref cusbd_setup_packet. Translated
 * to enum for easier and more portable use.
 */
enum cusbd_setup_packet_direction
{
    CUSBD_SETUP_PACKET_DIRECTION_OUT,    /**< OUT. Host to device. Device must only process request. */
    CUSBD_SETUP_PACKET_DIRECTION_IN      /**< IN. Device to host. Device must also send data back to host. */
};

/**
 * @brief Bits 4-0 of bmRequestType bitfield 
 * in @ref cusbd_setup_packet. Translated
 * to enum for easier and more portable use.
 */
enum cusbd_setup_packet_recipient
{
    CUSBD_SETUP_PACKET_RECIPIENT_DEVICE,     /**< Request is for a USB device. */
    CUSBD_SETUP_PACKET_RECIPIENT_INTERFACE,  /**< Request is for an interface descriptor. */
    CUSBD_SETUP_PACKET_RECIPIENT_ENDPOINT,   /**< Request is for an endpoint descriptor. */
    CUSBD_SETUP_PACKET_RECIPIENT_OTHER,      /**< Other. */
    /******************************************/
    CUSBD_SETUP_PACKET_RECIPIENT_RESERVED    /**< Values reserved for future use by USB. Currently 4 to 31 are reserved. */
};

/**
 * @brief Bits 6-5 of bmRequestType bitfield 
 * in @ref cusbd_setup_packet. Translated
 * to enum for easier and more portable use.
 */
enum cusbd_setup_packet_type
{
    CUSBD_SETUP_PACKET_TYPE_STANDARD,    /**< Standard request type. */
    CUSBD_SETUP_PACKET_TYPE_CLASS,       /**< Class-specific (HID, printer, etc) request type. */
    CUSBD_SETUP_PACKET_TYPE_VENDOR,      /**< Vendor-specific request type. */
    CUSBD_SETUP_PACKET_TYPE_RESERVED     /**< Value reserved for future use by USB. */
};

/*------------------------------------------------------------*/
/*------------ CUSBD_SETUP_PACKET MEMBER FUNCTIONS -----------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Member Functions
 */
/**@{*/
/**
 * @pre Contents of @p me populated.
 * @brief Indicates whether setup packet is IN or OUT.
 * 
 * @param me Setup packet to check.
 */
extern enum cusbd_setup_packet_direction cusbd_setup_packet_direction(const struct cusbd_setup_packet *me);

/**
 * @pre Contents of @p me populated.
 * @brief Return setup packet's recipient. I.e. device, interface, etc.
 * 
 * @param me Setup packet to check.
 */
extern enum cusbd_setup_packet_recipient cusbd_setup_packet_recipient(const struct cusbd_setup_packet *me);

/**
 * @pre Contents of @p me populated.
 * @brief Return type of setup packet. I.e. standard, class-specific, etc.
 * 
 * @param me Setup packet to check.
 */
extern enum cusbd_setup_packet_type cusbd_setup_packet_type(const struct cusbd_setup_packet *me);

/**
 * @pre Contents of @p me populated.
 * @brief Return bRequest value of the setup packet which contains
 * the actual request. I.e. CLEAR_FEATURE(), GET_DESCRIPTOR(), etc.
 * Meaning of the value depends on the recipient and request type.
 * I.e. a value of 1 can be different depending on if this is
 * a standard request vs class-specific request. It is the application's
 * responsibility to handle this.
 * 
 * @param me Setup packet to check.
 */
extern uint8_t cusbd_setup_packet_b_request(const struct cusbd_setup_packet *me);

/**
 * @pre Contents of @p me populated.
 * @brief Return wIndex field of the setup packet. Value returned
 * in native endianness for easier use, not little endian.
 * 
 * @param me Setup packet to check.
 */
extern uint16_t cusbd_setup_packet_w_index(const struct cusbd_setup_packet *me);

/**
 * @pre Contents of @p me populated.
 * @brief Return wLength field of the setup packet. Value returned
 * in native endianness for easier use, not little endian.
 * 
 * @param me Setup packet to check.
 */
extern uint16_t cusbd_setup_packet_w_length(const struct cusbd_setup_packet *me);

/**
 * @pre Contents of @p me populated.
 * @brief Return wValue field of the setup packet. Value returned
 * in native endianness for easier use, not little endian.
 * 
 * @param me Setup packet to check.
 */
extern uint16_t cusbd_setup_packet_w_value(const struct cusbd_setup_packet *me);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* CUSBD_SETUP_PACKET_H_ */
