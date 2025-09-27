/**
 * @file
 * @brief Object representing USB device. Device descriptor
 * is contained within this object.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-05-16
 * @copyright Copyright (c) 2025
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* Translation unit. */
#include "cusbd/cusbd.h"

/* STDLib. */
#include <stddef.h>
#include <string.h> /* memcpy. */

/* CUSB. */
#include "cusbd/setup_packet.h"

/* ECU. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_FILE("cusbd/cusbd.c")

/*------------------------------------------------------------*/
/*--------------------------- DEFINES ------------------------*/
/*------------------------------------------------------------*/

/// @brief All addresses of USB devices must be less than this value.
#define MAX_ADDRESS (128U)

/// @brief High byte of wValue in GET_DESCRIPTOR(DEVICE) request when device descriptor requested.
#define BREQUEST_GET_DESCRIPTOR_WVALUE_TYPE_DEVICE ((uint8_t)1)

/// @brief High byte of wValue in GET_DESCRIPTOR(CONFIGURATION) request when configuration descriptor requested.
#define BREQUEST_GET_DESCRIPTOR_WVALUE_TYPE_CONFIGURATION ((uint8_t)2)

/// @brief High byte of wValue in GET_DESCRIPTOR(STRING) request when string descriptor requested.
#define BREQUEST_GET_DESCRIPTOR_WVALUE_TYPE_STRING ((uint8_t)3)

/**@{*/
/// @brief bRequest value in setup packet received in status stage of
/// control transfer. Identifies the actual request. I.e. CLEAR_FEATURE(),
/// GET_DESCRIPTOR(), etc.
#define BREQUEST_GET_STATUS ((uint8_t)0)
#define BREQUEST_CLEAR_FEATURE ((uint8_t)1)
#define BREQUEST_SET_FEATURE ((uint8_t)3)
#define BREQUEST_SET_ADDRESS ((uint8_t)5)
#define BREQUEST_GET_DESCRIPTOR ((uint8_t)6)
#define BREQUEST_SET_DESCRIPTOR ((uint8_t)7)
#define BREQUEST_GET_CONFIGURATION ((uint8_t)8)
#define BREQUEST_SET_CONFIGURATION ((uint8_t)9)
#define BREQUEST_GET_INTERFACE ((uint8_t)10)
#define BREQUEST_SET_INTERFACE ((uint8_t)11)
#define BREQUEST_SYNCH_FRAME ((uint8_t)12)
/**@}*/

// /*------------------------------------------------------------*/
// /*---------------- STATIC FUNCTION DECLARATIONS --------------*/
// /*------------------------------------------------------------*/

// /**
//  * @brief Returns true if supplied descriptor was properly
//  * constructed via @ref CUSBD_DEVICE_DESCRIPTOR_CTOR().
//  * False otherwise.
//  * 
//  * @param descriptor Descriptor to check.
//  */
// static bool device_descriptor_valid(const struct cusbd_device_descriptor *descriptor);

/*------------------------------------------------------------*/
/*------ STATIC FUNCTION DECLARATIONS - DEVICE CONTROL -------*/
/*------------------------------------------------------------*/

/// @brief Calls @ref cusbd_api function supplied by user. Purely a wrapper to protect against API changes.
static void device_set_address(const struct cusbd *device);

// /// @brief Calls @ref cusbd_api function supplied by user. Purely a wrapper to protect against API changes.
// static void endpoint_zero_configure(const struct cusbd *device);

// /// @brief Calls @ref cusbd_api function supplied by user. Purely a wrapper to protect against API changes.
// static void endpoint_zero_halt(const struct cusbd *device);

/// @brief Calls @ref cusbd_api function supplied by user. Purely a wrapper to protect against API changes.
static void endpoint_zero_send(const struct cusbd *device, const void *data, size_t len);

/// @brief Calls @ref cusbd_api function supplied by user. Purely a wrapper to protect against API changes.
static void endpoint_zero_stall(const struct cusbd *device);

// /*------------------------------------------------------------*/
// /*------ STATIC FUNCTION DECLARATIONS - ENDPOINT CONTROL -----*/
// /*------------------------------------------------------------*/

// /// @brief Calls @ref cusbd_endpoint_api function supplied by user. 
// /// Purely a wrapper to protect against API changes.
// static void endpoint_configure(const struct cusbd_endpoint *endpoint);

// /// @brief Calls @ref cusbd_endpoint_api function supplied by user. 
// /// Purely a wrapper to protect against API changes.
// static void endpoint_halt(const struct cusbd_endpoint *endpoint);

// /// @brief Calls @ref cusbd_endpoint_api function supplied by user. 
// /// Purely a wrapper to protect against API changes.
// static void endpoint_send(const struct cusbd_endpoint *endpoint, const void *data, size_t len);

// /// @brief Calls @ref cusbd_endpoint_api function supplied by user. 
// /// Purely a wrapper to protect against API changes.
// static void endpoint_stall(const struct cusbd_endpoint *endpoint);

/*------------------------------------------------------------*/
/*--- STATIC FUNCTION DECLARATIONS - STATE MACHINE HELPERS ---*/
/*------------------------------------------------------------*/

/**
 * @brief Processes setup packet received in status stage of 
 * control transfer when device is in the default state.
 * 
 * @warning Can change device's HSM state.
 * 
 * @param me Device that received setup packet.
 * @param packet Received setup packet.
 */
static void process_setup_packet_default_state(struct cusbd *me, 
                                               const struct cusbd_setup_packet *packet);

// /**
//  * @brief Processes setup packet received in status stage of 
//  * control transfer when device is in the address state.
//  * 
//  * @warning Can change device's HSM state.
//  * 
//  * @param me Device that received setup packet.
//  * @param event Setup packet rx event dispatched to hsm.
//  */
// static void process_request_address_state(struct cusbd *me,
//                                           const struct cusbd_setup_packet_rx_event *event);

// /**
//  * @brief Same as @ref process_setup_packet_default_state(), 
//  * however this function processes the packet while the device
//  * is in the configured state.
//  */
// static void process_setup_packet_configured_state(struct cusbd *me,
//                                                   const struct cusbd_setup_packet_rx_event *event);

/*------------------------------------------------------------*/
/*------------ STATIC FUNCTION DECLARATIONS - STATES ---------*/
/*------------------------------------------------------------*/

/**@{*/
/// @brief States in USB device's hierarchical state machine.
/// Modeled from Section 9.1 - Device States in USB 2.0 standard.
/**@}*/
static bool running_state_handler(struct ecu_hsm *hsm, const void *event);
static void running_state_initial(struct ecu_hsm *hsm);
static bool default_state_handler(struct ecu_hsm *hsm, const void *event);
static bool default_suspended_state_handler(struct ecu_hsm *hsm, const void *event);
static bool address_state_handler(struct ecu_hsm *hsm, const void *event);
static bool address_suspended_state_handler(struct ecu_hsm *hsm, const void *event);
static bool configured_state_handler(struct ecu_hsm *hsm, const void *event);
static bool configured_suspended_state_handler(struct ecu_hsm *hsm, const void *event);

/*------------------------------------------------------------*/
/*-------------------- STATIC VARIABLES ----------------------*/
/*------------------------------------------------------------*/

static const struct ecu_hsm_state RUNNING_STATE = ECU_HSM_STATE_CTOR(
    ECU_HSM_STATE_ENTRY_UNUSED, ECU_HSM_STATE_EXIT_UNUSED, &running_state_initial, &running_state_handler, &ECU_HSM_TOP_STATE
);

static const struct ecu_hsm_state DEFAULT_STATE = ECU_HSM_STATE_CTOR(
    ECU_HSM_STATE_ENTRY_UNUSED, ECU_HSM_STATE_EXIT_UNUSED, ECU_HSM_STATE_INITIAL_UNUSED, &default_state_handler, &RUNNING_STATE
);

static const struct ecu_hsm_state DEFAULT_SUSPENDED_STATE = ECU_HSM_STATE_CTOR(
    ECU_HSM_STATE_ENTRY_UNUSED, ECU_HSM_STATE_EXIT_UNUSED, ECU_HSM_STATE_INITIAL_UNUSED, &default_suspended_state_handler, &RUNNING_STATE
);

static const struct ecu_hsm_state ADDRESS_STATE = ECU_HSM_STATE_CTOR(
    ECU_HSM_STATE_ENTRY_UNUSED, ECU_HSM_STATE_EXIT_UNUSED, ECU_HSM_STATE_INITIAL_UNUSED, &address_state_handler, &RUNNING_STATE
);

static const struct ecu_hsm_state ADDRESS_SUSPENDED_STATE = ECU_HSM_STATE_CTOR(
    ECU_HSM_STATE_ENTRY_UNUSED, ECU_HSM_STATE_EXIT_UNUSED, ECU_HSM_STATE_INITIAL_UNUSED, &address_suspended_state_handler, &RUNNING_STATE
);

static const struct ecu_hsm_state CONFIGURED_STATE = ECU_HSM_STATE_CTOR(
    ECU_HSM_STATE_ENTRY_UNUSED, ECU_HSM_STATE_EXIT_UNUSED, ECU_HSM_STATE_INITIAL_UNUSED, &configured_state_handler, &RUNNING_STATE
);

static const struct ecu_hsm_state CONFIGURED_SUSPENDED_STATE = ECU_HSM_STATE_CTOR(
    ECU_HSM_STATE_ENTRY_UNUSED, ECU_HSM_STATE_EXIT_UNUSED, ECU_HSM_STATE_INITIAL_UNUSED, &configured_suspended_state_handler, &RUNNING_STATE
);

// /*------------------------------------------------------------*/
// /*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
// /*------------------------------------------------------------*/

// static bool device_descriptor_valid(const struct cusbd_device_descriptor *descriptor)
// {
//     bool status = false;
//     ECU_ASSERT( (descriptor) );

// #pragma message("TODO: Figure out bcdUSB, bDeviceClass, bDeviceSubClass, bDeviceProtocol, "\
//         "and valid bcdUSB and bMaxPacketSize0 combinations.")
//     /* Do not assert idVendor, idProduct, and bcdDevice since these are user-specific. 
//     Do not assert iManufacturer, iProduct, iSerialNumber, and bNumConfigurations 
//     since these are automatically updated when descriptors are added to the device. */
//     if ((descriptor->bLength == sizeof(struct cusbd_device_descriptor)) &&
//         (descriptor->bDescriptorType == CUSBD_BDESCRIPTORTYPE) &&
//         (descriptor->bMaxPacketSize0 == (uint8_t)8 || descriptor->bMaxPacketSize0 == (uint8_t)16 ||
//          descriptor->bMaxPacketSize0 == (uint8_t)32 || descriptor->bMaxPacketSize0 == (uint8_t)64))
//     {
//         status = true;
//     }

//     return status;
// }

/*------------------------------------------------------------*/
/*------ STATIC FUNCTION DEFINITIONS - DEVICE CONTROL --------*/
/*------------------------------------------------------------*/

static void device_set_address(const struct cusbd *device)
{
    ECU_ASSERT( (device) );
    ECU_ASSERT( (device->api) );
    ECU_ASSERT( (device->api->device_set_address) );
    ECU_ASSERT( (device->address < MAX_ADDRESS) );
    (*device->api->device_set_address)(device->address, device->api->device_obj);
}

// static void endpoint_zero_configure(const struct cusbd *device)
// {
//     ECU_ASSERT( (device) );
//     ECU_ASSERT( (device->api) );
//     ECU_ASSERT( (device->api->endpoint0_configure) );
//     uint8_t bMaxPacketSize0 = device->descriptor.bMaxPacketSize0;

//     (*device->api->endpoint0_configure)(bMaxPacketSize0, device->api->endpoint0_obj);
// }

// static void endpoint_zero_halt(const struct cusbd *device)
// {
//     ECU_ASSERT( (device) );
//     ECU_ASSERT( (device->api) );
//     ECU_ASSERT( (device->api->endpoint0_halt) );
//     (*device->api->endpoint0_halt)(device->api->endpoint0_obj);
// }

static void endpoint_zero_send(const struct cusbd *device, const void *data, size_t len)
{
    ECU_ASSERT( (device) );
    ECU_ASSERT( (device->api) );
    ECU_ASSERT( (device->api->endpoint_zero_send) );
    ECU_ASSERT( (data) );
    ECU_ASSERT( (len > 0) );
    (*device->api->endpoint_zero_send)(data, len, device->api->endpoint_zero_obj);
}

static void endpoint_zero_stall(const struct cusbd *device)
{
    ECU_ASSERT( (device) );
    ECU_ASSERT( (device->api) );
    ECU_ASSERT( (device->api->endpoint_zero_stall) );
    (*device->api->endpoint_zero_stall)(device->api->endpoint_zero_obj);
}

// /*------------------------------------------------------------*/
// /*------ STATIC FUNCTION DEFINITIONS - ENDPOINT CONTROL ------*/
// /*------------------------------------------------------------*/

// static void endpoint_configure(const struct cusbd_endpoint *endpoint)
// {
//     ECU_ASSERT( (endpoint) );
//     ECU_ASSERT( (cusbd_endpoint_valid(endpoint)) );
//     (*endpoint->api->configure)(endpoint, endpoint->api->obj);
// }

// static void endpoint_halt(const struct cusbd_endpoint *endpoint)
// {
//     ECU_ASSERT( (endpoint) );
//     ECU_ASSERT( (cusbd_endpoint_valid(endpoint)) );
//     (*endpoint->api->halt)(endpoint, endpoint->api->obj);
// }

// static void endpoint_send(const struct cusbd_endpoint *endpoint, const void *data, size_t len)
// {
//     ECU_ASSERT( (endpoint && data) );
//     ECU_ASSERT( (cusbd_endpoint_valid(endpoint)) );
//     ECU_ASSERT( (len > 0) );
//     (*endpoint->api->send)(endpoint, data, len, endpoint->api->obj);
// }

// static void endpoint_stall(const struct cusbd_endpoint *endpoint)
// {
//     ECU_ASSERT( (endpoint) );
//     ECU_ASSERT( (cusbd_endpoint_valid(endpoint)) );
//     (*endpoint->api->stall)(endpoint, endpoint->api->obj);
// }

/*------------------------------------------------------------*/
/*--- STATIC FUNCTION DEFINITIONS - STATE MACHINE HELPERS ----*/
/*------------------------------------------------------------*/

static void process_setup_packet_default_state(struct cusbd *me, 
                                               const struct cusbd_setup_packet *packet)
{
    ECU_ASSERT( (me && packet) );

    switch (cusbd_setup_packet_b_request(packet)) /* Cases ordered based on how they're presented in Section 9.4. */
    {
        case BREQUEST_CLEAR_FEATURE:
        {
            endpoint_zero_stall(me); /* Request not supported in default state. */
            break;
        }

        case BREQUEST_GET_CONFIGURATION:
        {
            endpoint_zero_stall(me); /* Request not supported in default state. */
            break;
        }

        case BREQUEST_GET_DESCRIPTOR:
        {
            if (cusbd_setup_packet_direction(packet) == CUSBD_SETUP_PACKET_DIRECTION_IN &&
                cusbd_setup_packet_type(packet) == CUSBD_SETUP_PACKET_TYPE_STANDARD &&
                cusbd_setup_packet_recipient(packet) == CUSBD_SETUP_PACKET_RECIPIENT_DEVICE)
            {
                uint16_t wValue = cusbd_setup_packet_w_value(packet);
                wValue = ECU_CPU_TO_LE16_RUNTIME(wValue); /* Do this so member access is still only done through API. */
                uint8_t descriptor_type = ((wValue & 0xFF00U) >> 8);
                uint8_t descriptor_index = wValue & 0xFFU;
                uint16_t wIndex = cusbd_setup_packet_w_index(packet);
                uint16_t wLength = cusbd_setup_packet_w_length(packet);

                switch (descriptor_type)
                {
                    case BREQUEST_GET_DESCRIPTOR_WVALUE_TYPE_DEVICE:
                    {
                        if (descriptor_index == 0 &&
                            wIndex == 0)
                        {
                            ECU_STATIC_ASSERT( (ECU_FIELD_SIZEOF(struct cusbd, descriptor) == sizeof(struct cusbd_device_descriptor)),
                                                "cusbd::descriptor must be size of standard device descriptor." );
                            
                            /* Only send descriptor if wLength > sizeof(descriptor). Otherwise only send up to wLength bytes. */
                            if (wLength > sizeof(me->descriptor))
                            {
                                wLength = sizeof(me->descriptor);
                            }

                            uint8_t device_descriptor[sizeof(struct cusbd_device_descriptor)];
                            memcpy(&device_descriptor[0], &me->descriptor, sizeof(me->descriptor));
                            endpoint_zero_send(me, &device_descriptor[0], wLength); /* wLength, NOT sizeof()!! */
                        }
                        else
                        {
                            endpoint_zero_stall(me); /* Format of request is invalid. */
                        }
                        break;
                    }

                    case BREQUEST_GET_DESCRIPTOR_WVALUE_TYPE_CONFIGURATION:
                    {
                    #pragma message("TODO:")
                        break;
                    }

                    case BREQUEST_GET_DESCRIPTOR_WVALUE_TYPE_STRING:
                    {
                        
                        break;
                    }

            #pragma message("TODO: Add support for device_qualifier and other_speed_configuration")

                    default:
                    {
                        endpoint_zero_stall(me); /* Unsupported descriptor type. */
                        break;
                    }
                }
            }
            else
            {
                endpoint_zero_stall(me); /* Format of request is invalid. */
            }
            break;
        }

        case BREQUEST_GET_INTERFACE:
        {
            endpoint_zero_stall(me); /* Request not supported in default state. */
            break;
        }

        case BREQUEST_SET_ADDRESS:
        {
            uint16_t address = cusbd_setup_packet_w_value(packet);

            if (cusbd_setup_packet_direction(packet) == CUSBD_SETUP_PACKET_DIRECTION_OUT &&
                cusbd_setup_packet_type(packet) == CUSBD_SETUP_PACKET_TYPE_STANDARD &&
                cusbd_setup_packet_recipient(packet) == CUSBD_SETUP_PACKET_RECIPIENT_DEVICE &&
                address < MAX_ADDRESS &&
                cusbd_setup_packet_w_index(packet) == 0 &&
                cusbd_setup_packet_w_length(packet) == 0)
            {
                if (address > 0)
                {
                    me->address = address & 0xFFU;
                    device_set_address(me);
                    ecu_hsm_change_state(&me->hsm, &ADDRESS_STATE);
                }
            }
            else
            {
                endpoint_zero_stall(me); /* Format of request is invalid. */
            }
            break;
        }

        case BREQUEST_SET_CONFIGURATION:
        {
            endpoint_zero_stall(me); /* Request not supported in default state. */
            break;
        }

        case BREQUEST_SET_DESCRIPTOR:
        {
            endpoint_zero_stall(me); /* Request not supported in default state. */
            break;
        }

        case BREQUEST_SET_FEATURE:
        {
#pragma message("TODO")
            break;
        }

        case BREQUEST_SET_INTERFACE:
        {
            endpoint_zero_stall(me); /* Request not supported in default state. */
            break;
        }

        case BREQUEST_SYNCH_FRAME:
        {
            endpoint_zero_stall(me); /* Request not supported in default state. */
            break;
        }

        /*!!! FUTURE REQUESTS ADDED HERE. !!!*/

        default:
        {
            /* Any other unsupported requests. */
            endpoint_zero_stall(me);
            break;
        }
    }
}

// static void process_request_address_state(struct cusbd *me,
//                                           const struct cusbd_rx_request_event *event)
// {
//     ECU_ASSERT( (me && event) );

//     switch (cusbd_rx_request_event_value(event))
//     {
//         case CUSBD_RX_REQUEST_EVENT_VALUE_CLEAR_FEATURE:
//         {
//             if (cusbd_rx_request_event_direction(event) == CUSBD_RX_REQUEST_EVENT_DIRECTION_OUT &&
//                 cusbd_rx_request_event_type(event) == CUSBD_RX_REQUEST_EVENT_TYPE_STANDARD &&
//                 cusbd_rx_request_event_w_index(event) == 0 && /* Only interface0 and endpoint0 allowed in configured state. */
//                 cusbd_rx_request_event_w_length(event) == 0)
//             {
//                 uint16_t feature_selector = cusbd_rx_request_event_w_value(event);

//                 switch (cusbd_rx_request_event_recipient(event))
//                 {
//                     case CUSBD_RX_REQUEST_EVENT_RECIPIENT_DEVICE:
//                     {
//                         if (feature_selector == CUSBD_FEATURE_SELECTOR_DEVICE_REMOTE_WAKEUP)
//                         {
//                             me->remote_wakeup = false;
//                         }
//                         else
//                         {
//                             /* Unsupported feature. Note TEST_MODE feature selector cannot be cleared with CLEAR_FEATURE(). */
//                             endpoint_zero_stall(me);
//                         }
//                         break;
//                     }

//                     case CUSBD_RX_REQUEST_EVENT_RECIPIENT_INTERFACE:
//                     {
//                         /* Interfaces cannot be recipients while device is in configured state. */
//                         endpoint_zero_stall(me);
//                         break;
//                     }

//                     case CUSBD_RX_REQUEST_EVENT_RECIPIENT_ENDPOINT:
//                     {
//                         /* Assume endpoint halting is user-controlled and a feature that cannot be cleared. */
//                         endpoint_zero_stall(me);
//                         break;
//                     }

//                     default:
//                     {
//                         /* Invalid recipient. */
//                         endpoint_zero_stall(me);
//                         break;
//                     }
//                 }
//             }
//             else
//             {
//                 /* Format of request is invalid. */
//                 endpoint_zero_stall(me);
//             }
//             break;
//         }

//         case CUSBD_RX_REQUEST_EVENT_VALUE_GET_CONFIGURATION:
//         {
//             if (cusbd_rx_request_event_direction(event) == CUSBD_RX_REQUEST_EVENT_DIRECTION_IN &&
//                 cusbd_rx_request_event_type(event) == CUSBD_RX_REQUEST_EVENT_TYPE_STANDARD &&
//                 cusbd_rx_request_event_recipient(event) == CUSBD_RX_REQUEST_EVENT_RECIPIENT_DEVICE &&
//                 cusbd_rx_request_event_w_value(event) == 0 &&
//                 cusbd_rx_request_event_w_index(event) == 0 &&
//                 cusbd_rx_request_event_w_length(event) == 1)
//             {
//                 uint8_t configuration = 0; /* 0 is always sent since device unconfigured while in address state. */
//                 endpoint_zero_send(me, &configuration, sizeof(configuration));
//             }
//             else
//             {
//                 /* Format of request is invalid. */
//                 endpoint_zero_stall(me);
//             }
//             break;
//         }

//         case CUSBD_RX_REQUEST_EVENT_VALUE_GET_DESCRIPTOR:
//         {
//     #pragma message("TODO: Going to need some sort of ring buffer. Buffer all descriptor data. Send up to wLength")
//             if (cusbd_rx_request_event_direction(event) == CUSBD_RX_REQUEST_EVENT_DIRECTION_IN &&
//                 cusbd_rx_request_event_type(event) == CUSBD_RX_REQUEST_EVENT_TYPE_STANDARD &&
//                 cusbd_rx_request_event_recipient(event) == CUSBD_RX_REQUEST_EVENT_RECIPIENT_DEVICE)
//             {
//                 uint16_t wValue = cusbd_rx_request_event_w_value(event);
//                 uint8_t descriptor_type = ((wValue & 0xFF00U) >> 8);
//                 uint8_t descriptor_index = wValue & 0xFFU;

//                 switch (descriptor_type)
//                 {
//                     case GET_DESCRIPTOR_WVALUE_TYPE_DEVICE:
//                     {
//                         break;
//                     }

//                     case GET_DESCRIPTOR_WVALUE_TYPE_CONFIGURATION:
//                     {
//                         // preorder iterator should guarantee descriptors sent in proper order!!
//                         break;
//                     }

//                     case GET_DESCRIPTOR_WVALUE_TYPE_STRING:
//                     {
//                         break;
//                     }

//                     default:
//                     {
//                         /* Unsupported descriptor. */
//                         endpoint_zero_stall(me);
//                         break;
//                     }
//                 }
//             }
//             else
//             {
//                 /* Format of request is invalid. */
//                 endpoint_zero_stall(me);
//             }
//             break;
//         }

//         case CUSBD_RX_REQUEST_EVENT_VALUE_GET_INTERFACE:
//         {
//             /* GET_INTERFACE() requests invalid while in address state. */
//             endpoint_zero_stall(me);
//             break;
//         }

//         case CUSBD_RX_REQUEST_EVENT_VALUE_GET_STATUS:
//         {
//             if (cusbd_rx_request_event_direction(event) == CUSBD_RX_REQUEST_EVENT_DIRECTION_IN &&
//                 cusbd_rx_request_event_type(event) == CUSBD_RX_REQUEST_EVENT_TYPE_STANDARD &&
//                 cusbd_rx_request_event_w_value(event) == 0 &&
//                 cusbd_rx_request_event_w_index(event) == 0 && /* Only device or ep0 can only be addressed while in configured state. */
//                 cusbd_rx_request_event_w_length(event) == 2)
//             {
//                 switch (cusbd_rx_request_event_recipient(event))
//                 {
//                     case CUSBD_RX_REQUEST_EVENT_RECIPIENT_DEVICE:
//                     {
//                         /* Note that remote wakeup and self-powered statuses are properties of the 
//                         device. These values are not taken from the configuration descriptor. */
//                         uint16_t status = 0;

//                         if (me->self_powered)
//                         {
//                             status |= (1U << 0);
//                         }
//                         if (me->remote_wakeup)
//                         {
//                             status |= (1U << 1);
//                         }

//                         status = ECU_CPU_TO_LE16_RUNTIME(status);
//                         endpoint_zero_send(me, &status, sizeof(status));
//                         break;
//                     }

//                     case CUSBD_RX_REQUEST_EVENT_RECIPIENT_INTERFACE:
//                     {
//                         /* Only device or ep0 can only be addressed while in configured state. */
//                         endpoint_zero_stall(me);
//                         break;
//                     }

//                     case CUSBD_RX_REQUEST_EVENT_RECIPIENT_ENDPOINT:
//                     {
//                         uint16_t status = 0; /* Do not support halting for endpoint 0 so always send back a halt bit of 0. */
//                         endpoint_zero_send(me, &status, sizeof(status));
//                         break;
//                     }

//                     default:
//                     {
//                         /* Invalid recipient. */
//                         endpoint_zero_stall(me);
//                         break;
//                     }
//                 }
//             }
//             else
//             {
//                 /* Format of request is invalid. */
//                 endpoint_zero_stall(me);
//             }
//             break;
//         }

//         case CUSBD_RX_REQUEST_EVENT_VALUE_SET_ADDRESS:
//         {
//             uint16_t address = cusbd_rx_request_event_w_value(event);

//             if (cusbd_rx_request_event_direction(event) == CUSBD_RX_REQUEST_EVENT_DIRECTION_OUT &&
//                 cusbd_rx_request_event_type(event) == CUSBD_RX_REQUEST_EVENT_TYPE_STANDARD &&
//                 cusbd_rx_request_event_recipient(event) == CUSBD_RX_REQUEST_EVENT_RECIPIENT_DEVICE &&
//                 address < MAX_ADDRESS &&
//                 cusbd_rx_request_event_w_index(event) == 0 &&
//                 cusbd_rx_request_event_w_length(event) == 0)
//             {
//                 if (address == 0)
//                 {
//                     me->address = 0;
//                     device_set_address(me);
//                     ecu_hsm_change_state(me, &DEFAULT_STATE);
//                 }
//                 else
//                 {
//                     me->address = address & 0xFFU;
//                     device_set_address(me);
//                 }
//             }
//             else
//             {
//                 /* Format of request is invalid. */
//                 endpoint_zero_stall(me);
//             }
//             break;
//         }

//         case CUSBD_RX_REQUEST_EVENT_VALUE_SET_CONFIGURATION:
//         {
//             !!!!!!!!!!!! TODO Stopped here!!
//             if (config > 0)
//             {
//                 ecu_hsm_change_state(me, &CONFIGURED_STATE);
//             }
//             break;
//         }

//         case CUSBD_RX_REQUEST_EVENT_VALUE_SET_DESCRIPTOR:
//         {
//             break;
//         }

//         case CUSBD_RX_REQUEST_EVENT_VALUE_SET_FEATURE:
//         {
//             break;
//         }

//         /* !! FUTURE SUPPORTED REQUESTS ADDED HERE. ONLY ADD 
//         REQUESTS THAT ARE VALID IN THE ADDRESS STATE !! */

//         default:
//         {
//             /* Any unsupported requests. */
//             (*me->ep0.handshake)(CUSBD_ENDPOINT_STATUS_STALL, me->ep0.obj);
//             break;
//         }
//     }
// }

// static void process_setup_packet_configured_state(struct cusbd *me,
//                                                   const struct cusbd_setup_packet_rx_event *event)
// {
//     ECU_ASSERT( (me && event) );
//     /* Use variable to avoid passing wrong state. */
//     static const enum cusbd_request_state STATE = CUSBD_REQUEST_STATE_CONFIGURED_STATE;

//     enum cusbd_request_status status = CUSBD_REQUEST_STATUS_UNPROCESSED;
//     struct cusbd_request request;
//     cusbd_request_ctor(&request, event, STATE);

//     #pragma message("TODO")

//     switch (cusbd_request_value(&request))
//     {
//         case CUSBD_REQUEST_VALUE_CLEAR_FEATURE:
//         {
//             struct cusbd_visitor_clear_feature clear_feature;
//             cusbd_visitor_clear_feature_ctor(&clear_feature, event, STATE);
//             (void)cusbd_descriptor_accept_before(&me->base, &clear_feature.base, &setup_packet_processed, &clear_feature.request);
//             status = cusbd_request_status(&clear_feature.request);
//             break;
//         }

//         case CUSBD_REQUEST_VALUE_GET_CONFIGURATION:
//         {
//             struct cusbd_visitor_get_configuration get_configuration;
//             cusbd_visitor_get_configuration_ctor(&get_configuration, event, STATE);
//             v_cusbd_descriptor_caccept(&me->base, &get_configuration.base); /* Can get away with only running on device. */
//             status = cusbd_request_status(&get_configuration.request);
//             break;
//         }

//         case CUSBD_REQUEST_VALUE_GET_DESCRIPTOR:
//         {
//             break;
//         }

//         case CUSBD_REQUEST_VALUE_GET_INTERFACE:
//         {
//             break;
//         }

//         case CUSBD_REQUEST_VALUE_GET_STATUS:
//         {
//             break;
//         }

//         case CUSBD_REQUEST_VALUE_SET_CONFIGURATION:
//         {
//             if (config == 0)
//             {
//                 ecu_hsm_change_state(me, &ADDRESS_STATE);
//             }
//             break;
//         }

//         case CUSBD_REQUEST_VALUE_SET_DESCRIPTOR:
//         {
//             break;
//         }

//         case CUSBD_REQUEST_VALUE_SET_FEATURE:
//         {
//             break;
//         }

//         case CUSBD_REQUEST_VALUE_SET_INTERFACE:
//         {
//             break;
//         }

//         case CUSBD_REQUEST_VALUE_SYNCH_FRAME:
//         {
//             break;
//         }

//         /* !! FUTURE SUPPORTED REQUESTS ADDED HERE. ONLY ADD 
//         REQUESTS THAT ARE VALID IN THE CONFIGURED STATE !! */

//         default:
//         {
//             /* Any unsupported requests. */
//             status = CUSBD_REQUEST_STATUS_STALL;
//             break;
//         }
//     }

// #pragma message("TODO: Need to know how to ACK, NAK, and STALL.")

//     if (status == CUSBD_REQUEST_STATUS_ACK)
//     {
//         // ACK.
//     }
//     else if (status == CUSBD_REQUEST_STATUS_NAK)
//     {
//         // NAK
//     }
//     else
//     {
//         // STALL if STALL or UNPROCESSED.
//     }
// }                

/*------------------------------------------------------------*/
/*------------- STATIC FUNCTION DEFINITIONS - STATES ---------*/
/*------------------------------------------------------------*/

static bool running_state_handler(struct ecu_hsm *hsm, const void *event)
{
    ECU_ASSERT( (hsm && event) );
    struct cusbd *me = ECU_HSM_GET_CONTEXT(hsm, struct cusbd, hsm);
    bool status = true;
    const struct ecu_event *base_event = event;

    switch (cusbd_event_id(&base_event))
    {
        case CUSBD_RESET_EVENT_ID:
        {
            ecu_hsm_change_state(&me->hsm, &RUNNING_STATE);
            break;
        }

        default:
        {
            /* Propagate event up state hierarchy. */
            status = false;
            break;
        }
    }

    return status;
}

static void running_state_initial(struct ecu_hsm *hsm)
{
    ECU_ASSERT( (hsm) );
    ecu_hsm_change_state(hsm, &DEFAULT_STATE);
}

static bool default_state_handler(struct ecu_hsm *hsm, const void *event)
{
    ECU_ASSERT( (hsm && event) );
    struct cusbd *me = ECU_HSM_GET_CONTEXT(hsm, struct cusbd, hsm);
    bool status = true;
    const struct ecu_event *base_event = event;

    switch (cusbd_event_id(&base_event))
    {
        case CUSBD_SETUP_PACKET_RX_EVENT_ID:
        {
            const struct cusbd_setup_packet_rx_event *e = (const struct cusbd_setup_packet_rx_event)base_event;
            process_setup_packet_default_state(me, &e->packet);
            break;
        }

        case CUSBD_SUSPEND_EVENT_ID:
        {
            ecu_hsm_change_state(&me->hsm, &DEFAULT_SUSPENDED_STATE);
            break;
        }

        default:
        {
            /* Propagate event up state hierarchy. */
            status = false;
            break;
        }
    }

    return status;
}

static bool default_suspended_state_handler(struct ecu_hsm *hsm, const void *event)
{
    ECU_ASSERT( (hsm && event) );
    struct cusbd *me = ECU_HSM_GET_CONTEXT(hsm, struct cusbd, hsm);
    bool status = true;
    const struct ecu_event *base_event = event;

    switch (cusbd_event_id(&base_event))
    {
        case CUSBD_RESUME_EVENT_ID:
        {
            ecu_hsm_change_state(&me->hsm, &DEFAULT_STATE);
            break;
        }

        default:
        {
            /* Propagate event up state hierarchy. */
            status = false;
            break;
        }
    }

    return status;
}

static bool address_state_handler(struct ecu_hsm *hsm, const void *event)
{
    ECU_ASSERT( (hsm && event) );
    struct cusbd *me = ECU_HSM_GET_CONTEXT(hsm, struct cusbd, hsm);
    bool status = true;
    const struct ecu_event *base_event = event;

    switch (cusbd_event_id(&base_event))
    {
        case CUSBD_SETUP_PACKET_RX_EVENT_ID:
        {
            const struct cusbd_setup_packet_rx_event *e = (const struct cusbd_setup_packet_rx_event)base_event;
            process_setup_packet_address_state(me, &e->packet);
            break;
        }

        case CUSBD_SUSPEND_EVENT_ID:
        {
            ecu_hsm_change_state(&me->hsm, &ADDRESS_SUSPENDED_STATE);
            break;
        }

        default:
        {
            /* Propagate event up state hierarchy. */
            status = false;
            break;
        }
    }

    return status;
}

static bool address_suspended_state_handler(struct ecu_hsm *hsm, const void *event)
{
    ECU_ASSERT( (hsm && event) );
    struct cusbd *me = ECU_HSM_GET_CONTEXT(hsm, struct cusbd, hsm);
    bool status = true;
    const struct ecu_event *base_event = event;

    switch (cusbd_event_id(&base_event))
    {
        case CUSBD_RESUME_EVENT_ID:
        {
            ecu_hsm_change_state(&me->hsm, &ADDRESS_STATE);
            break;
        }

        default:
        {
            /* Propagate event up state hierarchy. */
            status = false;
            break;
        }
    }

    return status;
}

static bool configured_state_handler(struct ecu_hsm *hsm, const void *event)
{
    ECU_ASSERT( (hsm && event) );
    struct cusbd *me = ECU_HSM_GET_CONTEXT(hsm, struct cusbd, hsm);
    bool status = true;
    const struct ecu_event *base_event = event;

    switch (cusbd_event_id(&base_event))
    {
        case CUSBD_SETUP_PACKET_RX_EVENT_ID:
        {
            const struct cusbd_setup_packet_rx_event *e = (const struct cusbd_setup_packet_rx_event)base_event;
            process_setup_packet_configured_state(me, &e->packet);
            break;
        }

        case CUSBD_SUSPEND_EVENT_ID:
        {
            ecu_hsm_change_state(&me->hsm, &CONFIGURED_SUSPENDED_STATE);
            break;
        }

        default:
        {
            /* Propagate event up state hierarchy. */
            status = false;
            break;
        }
    }

    return status;
}

static bool configured_suspended_state_handler(struct ecu_hsm *hsm, const void *event)
{
    ECU_ASSERT( (hsm && event) );
    struct cusbd *me = ECU_HSM_GET_CONTEXT(hsm, struct cusbd, hsm);
    bool status = true;
    const struct ecu_event *base_event = event;

    switch (cusbd_event_id(&base_event))
    {
        case CUSBD_RESUME_EVENT_ID:
        {
            ecu_hsm_change_state(&me->hsm, &CONFIGURED_STATE);
            break;
        }

        default:
        {
            /* Propagate event up state hierarchy. */
            status = false;
            break;
        }
    }

    return status;
}

/*------------------------------------------------------------*/
/*---------------------- STATIC ASSERTS ----------------------*/
/*------------------------------------------------------------*/

ECU_STATIC_ASSERT( (sizeof(struct cusbd_device_descriptor) == (size_t)18),
                    "Device descriptor is 18 bytes." );

/*------------------------------------------------------------*/
/*------- CUSBD DEVICE MEMBER FUNCTIONS - CONFIGURATION ------*/
/*------------------------------------------------------------*/

// void cusbd_ctor(struct cusbd *me,
//                 const struct cusbd_device_descriptor *descriptor,
//                 const struct cusbd_string_zero *string0,
//                 void (*configure)(cusbd_endpoint_id_t id, enum cusbd_endpoint_type type, uint16_t packet_size, void *obj),
//                 void (*post)(cusbd_endpoint_id_t id, const void *data, size_t len, void *obj),
//                 void *obj)
// {
//     ECU_ASSERT( (me && descriptor && configure && post) );
//     ECU_ASSERT( (device_descriptor_valid(descriptor)) );
//     if (string0 != CUSBD_STRING_ZERO_UNUSED)
//     {
//         ECU_ASSERT( (cusbd_string_zero_valid(string0)) );
//     }

// #pragma message("TODO: USB HSM needs to be implemented.")
//     // ecu_hsm_ctor(ECU_HSM_BASE_CAST(me), &INIT_STATE);

//     memcpy(&me->descriptor, descriptor, sizeof(struct cusbd_device_descriptor));
//     me->string0 = string0;
//     ecu_dlist_ctor(&me->configurations);
//     ecu_dlist_ctor(&me->manufacturer_strings);
//     ecu_dlist_ctor(&me->product_strings);
//     ecu_dlist_ctor(&me->serial_number_strings);
//     me->endpoint.configure = configure;
//     me->endpoint.post = post;
//     me->endpoint.obj = obj;
// }

void cusbd_add_configuration(struct cusbd *me,
                             struct cusbd_configuration *configuration)
{
    /* ECU library asserts if node is already within a tree. Necessary descriptor 
    contents (bNumConfigurations, etc) are not updated here since that complexity 
    is centralized to the cusbd_start() function. */
    ECU_ASSERT( (me && configuration) );
    ECU_ASSERT( (cusbd_valid(me)) );
    ECU_ASSERT( (cusbd_configuration_valid(configuration)) );
    ecu_ntnode_push_child_back(&me->base.ntnode, &configuration->base.ntnode);
}

void cusbd_add_manufacturer_string(struct cusbd *me,
                                   struct cusbd_string *string)
{
    /* ECU library asserts if node is already within a list. */
    ECU_ASSERT( (me && string) );
    ECU_ASSERT( (cusbd_valid(me)) );
    ECU_ASSERT( (cusbd_string_valid(string)) );
    ecu_dlist_push_back(&me->manufacturer_strings, &string->dnode);
}

void cusbd_add_product_string(struct cusbd *me,
                              struct cusbd_string *string)
{
    /* ECU library asserts if node is already within a list. */
    ECU_ASSERT( (me && string) );
    ECU_ASSERT( (cusbd_valid(me)) );
    ECU_ASSERT( (cusbd_string_valid(string)) );
    ecu_dlist_push_back(&me->product_strings, &string->dnode);
}

void cusbd_add_serial_number_string(struct cusbd *me,
                                    struct cusbd_string *string)
{
    /* ECU library asserts if node is already within a list. */
    ECU_ASSERT( (me && string) );
    ECU_ASSERT( (cusbd_valid(me)) );
    ECU_ASSERT( (cusbd_string_valid(string)) );
    ecu_dlist_push_back(&me->serial_number_strings, &string->dnode);
}

void cusbd_dispatch(struct cusbd *me, const void *event)
{
    ECU_ASSERT( (me && event) );
    ECU_ASSERT( (cusbd_vaild(me)) );
    ecu_hsm_dispatch(&me->base.hsm, event);
}

void cusbd_start(struct cusbd *me)
{
    (void)me;

    // // !! idea is to limit the complexity of assigning bConfigurationValue, etc to
    // // just this function.
    // ECU_ASSERT( (me) );
    // ECU_ASSERT( (device_descriptor_valid(&me->descriptor)) );

    // /* Set device descriptor's values that don't require iteration first. */
    // me->descriptor.iManufacturer = CUSBD_MANUFACTURER_STRING_ID;
    // me->descriptor.iProduct = CUSBD_PRODUCT_STRING_ID;
    // me->descriptor.iSerialNumber = CUSBD_SERIAL_NUMBER_STRING_ID;

    // /* Iterate over the descriptor tree and assign ID values.
    // Stored as size_t then downcasted so we can detect if values are too large. */
    // size_t bNumConfigurations = 0;
    // size_t wTotalLength = 0;
    // size_t bNumInterfaces = 0;
    // size_t bConfigurationValue = 0;
    // size_t bInterfaceNumber = 0;
    // size_t bAlternateSetting = 0;
    // size_t bNumEndpoints = 0;
    // size_t string_index = CUSBD_USER_STRING_ID_BEGIN;
    // struct cusbd_configuration *configuration = (struct cusbd_configuration *)0;
    // struct cusbd_interface *interface = (struct cusbd_interface *)0;
    // struct cusbd_alternate_interface *alternate_interface = (struct cusbd_alternate_interface *)0;
    // struct cusbd_endpoint *endpoint = (struct cusbd_endpoint *)0;
    // struct ecu_dlist_iterator configuration_iterator;
    // struct ecu_dlist_iterator interface_iterator;
    // struct ecu_dlist_iterator alternate_interface_iterator;
    // struct ecu_dlist_iterator endpoint_iterator;

    // ECU_DLIST_FOR_EACH(c, &configuration_iterator, &me->configurations)
    // {
    //     configuration = ECU_DNODE_GET_ENTRY(c, struct cusbd_configuration, dnode);
    //     ECU_ASSERT( (cusbd_configuration_valid(configuration)) );
    //     wTotalLength = sizeof(struct cusbd_configuration_descriptor);
    //     bNumConfigurations++;
    //     bConfigurationValue++; /* Starts at 1. */

    //     ECU_DLIST_FOR_EACH(i, &interface_iterator, &configuration->interfaces)
    //     {
    //         interface = ECU_DNODE_GET_ENTRY(i, struct cusbd_interface, dnode);
    //         ECU_ASSERT( (cusbd_interface_valid(interface)) );
    //         wTotalLength += sizeof(struct cusbd_interface_descriptor);
    //         bNumInterfaces++;

    //         ECU_DLIST_FOR_EACH(e, &endpoint_iterator, &interface->endpoints)
    //         {
    //             endpoint = ECU_DNODE_GET_ENTRY(e, struct cusbd_endpoint, dnode);
    //             ECU_ASSERT( (cusbd_endpoint_valid(endpoint)) );
    //             bNumEndpoints++;
    //         }

    //         ECU_DLIST_FOR_EACH(ai, &alternate_interface_iterator, &interface->alternate_interfaces)
    //         {
    //             alternate_interface = ECU_DNODE_GET_ENTRY(ai, struct cusbd_alternate_interface, dnode);
    //             ECU_ASSERT( (cusbd_alternate_interface_valid(alternate_interface)) );

    //             ECU_DLIST_FOR_EACH(e, &endpoint_iterator, &alternate_interface->endpoints)
    //             {
    //                 endpoint = ECU_DNODE_GET_ENTRY(e, struct cusbd_endpoint, dnode);
    //                 ECU_ASSERT( (cusbd_endpoint_valid(endpoint)) );

    //             }
    //         }


    //     }

    //     /* Set all config items here. */

    //     // wTotalLength = cusbd_configuration_size(configuration);
    //     // ECU_ASSERT( (wTotalLength <= UINT16_MAX) );
    //     // configuration->descriptor.wTotalLength = ECU_CPU_TO_LE16_RUNTIME((uint16_t)wTotalLength);
    // }

    // /* Device must have at least one configuration. */
    // ECU_ASSERT( (bNumConfigurations >= (size_t)1) );
    // me->descriptor.bNumConfigurations = bNumConfigurations;
}

// void cusbd_start()
// {
//     // assert no string descriptors added if string0 == CUSBD_STRINGS_UNUSED
//     // assert at least one configuration and one interface.
//     // loop through all descriptors. Assert they are valid(). Update
//         // all necessary fields. I.e. bNumConfigurations, wTotalLength, etc.
//         // add all string descriptors to device->strings list.
//         // assert all string descriptors are valid.
//         // Update iConfiguration, iInterface, etc accordingly.
// }

// void cusbd_configuration_add_interface()
// {
//     bool inserted = false;
//     uint8_t bInterfaceNumber = 1;   /* Starts at 1 since we are adding an interface. */
//     size_t bNumInterfaces = 0;      /* Use size_t so we can assert if more than UINT8_MAX number of interfaces. */
//     struct ecu_ntnode_child_iterator iter;
//     struct cusbd_visitor_w_total_length visitor;

//     /* Performing iteration is more reliable than simply incrementing bNumInterfaces. */
//     ECU_NTNODE_CHILD_FOR_EACH(c, &iter, &me->base.ntnode)
//     {
//         struct cusbd_descriptor *base_node = ECU_NTNODE_GET_ENTRY(c, struct cusbd_descriptor, ntnode);
//         if (cusbd_descriptor_type(base_node) == CUSBD_INTERFACE_BDESCRIPTORTYPE)
//         {
//             bNumInterfaces++;
//             ECU_ASSERT( (bNumInterfaces <= UINT8_MAX) );
//             struct cusbd_interface *interface_node = (struct cusbd_interface *)base_node;

//             if (!inserted)
//             {
//                 /* Find the first available bInterfaceNumber. */
//                 if (bInterfaceNumber < interface_node->descriptor.bInterfaceNumber)
//                 {
//                     ecu_ntnode_insert_before(&interface_node->base.ntnode, &interface->base.ntnode);
//                     inserted = true;
//                 }
//                 else
//                 {
//                     bInterfaceNumber++;
//                 }
//             }
//         }
//     }

//     if (!inserted)
//     {
//         ecu_ntnode_push_back(&me->base.ntnode, &interface->base.ntnode);
//     }

//     interface->descriptor.bInterfaceNumber = bInterfaceNumber;
//     me->descriptor.bNumInterfaces = (uint8_t)bNumInterfaces;
//     cusbd_visitor_w_total_length_ctor(&visitor);
//     cusbd_descriptor_caccept(&me->base, &visitor.base);
//     me->descriptor.wTotalLength = cusbd_visitor_w_total_length_value_le(&visitor);
// }


// void cusbd_interface_add_alternate_interface(struct cusbd_interface *me,
//                                              struct cusbd_alternate_interface *alternate_interface)
// {
//     uint16_t wTotalLength = 0;
//     struct ecu_ntnode *parent = (struct ecu_ntnode *)0;
//     struct cusbd_configuration *configuration = (struct cusbd_configuration *)0;
//     struct cusbd_descriptor *base = (struct cusbd_descriptor *)0;
//     struct cusbd_alternate_interface *alt_interface_node = (struct cusbd_alternate_interface *)0;
//     struct ecu_ntnode_child_iterator iterator;
//     ECU_ASSERT( (me && alternate_interface) );
//     ECU_ASSERT( (v_cusbd_descriptor_valid(CUSBD_DESCRIPTOR_CONST_BASE_CAST(me))) );
//     ECU_ASSERT( (v_cusbd_descriptor_valid(CUSBD_DESCRIPTOR_CONST_BASE_CAST(alternate_interface))) );

//     /* Update bInterfaceNumber. */
//     alternate_interface->descriptor.bInterfaceNumber = me->descriptor.bInterfaceNumber;

//     /* Update bAlternateSetting while inserting alternate interface into descriptor tree. 
//     ECU library asserts if node is already within a tree. */
//     alternate_interface->descriptor.bAlternateSetting = 1; /* Alternate settings start at 1. */
//     ECU_NTNODE_CHILD_FOR_EACH(n, &iterator, &me->base.ntnode)
//     {
//         base = ECU_NTNODE_GET_ENTRY(n, struct cusbd_descriptor, ntnode);

//         if (cusbd_descriptor_type(base) == CUSBD_DESCRIPTOR_TYPE_INTERFACE)
//         {
//             alt_interface_node = (struct cusbd_alternate_interface *)base;
            
//             if (alt_interface_node->descriptor.bAlternateSetting < alternate_interface->descriptor.bAlternateSetting)
//             {
//                 ecu_ntnode_insert_before(n, &alternate_interface->base.ntnode);
//                 break;
//             }
//             else
//             {
//                 alternate_interface->descriptor.bAlternateSetting++;
//             }
//         }
//     }

//     if (!ecu_ntnode_in_subtree(&alternate_interface->base.ntnode))
//     {
//         ecu_ntnode_push_back(&me->base.ntnode, &alternate_interface->base.ntnode);
//     }

//     /* If the interface is attached to a configuration, update wTotalLength. */
//     parent = ecu_ntnode_parent(&me->base.ntnode);
//     if (parent)
//     {
//         base = ECU_NTNODE_GET_ENTRY(&parent, struct cusbd_descriptor, ntnode);
//         ECU_ASSERT( (cusbd_descriptor_type(&base) == CUSBD_DESCRIPTOR_TYPE_CONFIGURATION) );
//         configuration = (struct cusbd_configuration *)base;
//         wTotalLength = cusbd_configuration_size(configuration);
//         configuration->descriptor.wTotalLength = ECU_CPU_TO_LE16_RUNTIME(wTotalLength);
//     }

//     /* iInterface is updated when device starts. */
// }

// void cusbd_interface_add_endpoint(struct cusbd_interface *me,
//                                   struct cusbd_endpoint *endpoint)
// {
//     const struct cusbd_descriptor *base = (const struct cusbd_descriptor *)0;
//     const struct cusbd_endpoint *e = (const struct cusbd_endpoint *)0;
//     struct ecu_ntnode_child_citerator citerator;
//     ECU_ASSERT( (me && endpoint) );
//     ECU_ASSERT( (v_cusbd_descriptor_valid(CUSBD_DESCRIPTOR_CONST_BASE_CAST(me))) );
//     ECU_ASSERT( (v_cusbd_descriptor_valid(CUSBD_DESCRIPTOR_CONST_BASE_CAST(endpoint))) );

//     /* Update bNumEndpoints. Also verify no endpoints are duplicated. I.e. cannot 
//     have multiple endpoint1 INs attached to the same interface descriptor. */
//     me->descriptor.bNumEndpoints = 0;
//     ECU_NTNODE_CONST_CHILD_FOR_EACH(n, &citerator, &me->base.ntnode)
//     {
//         base = ECU_NTNODE_GET_CONST_ENTRY(n, struct cusbd_descriptor, ntnode);

//         if (cusbd_descriptor_type(base) == CUSBD_DESCRIPTOR_TYPE_ENDPOINT)
//         {
//             e = (const struct cusb_endpoint *)base;
//             ECU_ASSERT( (e->descriptor.bEndpointAddress != endpoint->descriptor.bEndpointAddress) );
//             me->descriptor.bNumEndpoints++;
//         }
//     }

//     /* Add endpoint to descriptor tree. ECU library asserts if node already in tree. */
//     ecu_ntnode_push_front(&me->base.ntnode, &endpoint->base.ntnode);
// }

void cusbd_stop(struct cusbd *me)
{
    #pragma message("TODO:")
    (void)me;
}

bool cusbd_valid(const struct cusbd *me)
{
    ECU_ASSERT( (me) );
    return (cusbd_descriptor_valid(&me->base) &&
            cusbd_descriptor_type(&me->base) == CUSBD_BDESCRIPTORTYPE &&
            device_descriptor_valid(&me->descriptor) &&
            ecu_dlist_valid(&me->manufacturer_strings) &&
            ecu_dlist_valid(&me->product_strings) &&
            ecu_dlist_valid(&me->serial_number_strings));
}
