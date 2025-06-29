/**
 * @file
 * @brief See @ref request.h.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-06-02
 * @copyright Copyright (c) 2025
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* Translation unit. */
#include "cusbd/request.h"

/* ECU. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_NAME("cusbd/request.c")

/*------------------------------------------------------------*/
/*----------------------- PUBLIC FUNCTIONS -------------------*/
/*------------------------------------------------------------*/

enum cusbd_request_direction cusbd_request_direction(const struct cusb_request *request)
{
    ECU_RUNTIME_ASSERT( (request) );
    enum cusbd_request_direction direction = CUSBD_REQUEST_DIRECTION_HOST_TO_DEVICE;

    if (request->bmRequestType & (1U << 7))
    {
        direction = CUSBD_REQUEST_DIRECTION_DEVICE_TO_HOST;
    }

    return direction;
}

enum cusbd_request_type cusbd_request_type(const struct cusb_request *request)
{
    ECU_RUNTIME_ASSERT( (request) );
    static const uint8_t BITMAP = ((1U << 6) | (1U << 5));

    /* Value can always be safely casted to enumeration since values constrained to 0b11. */
    return ((enum cusbd_request_type)((request->bmRequestType & BITMAP) >> 5U));
}

enum cusbd_request_recipient cusbd_request_recipient(const struct cusb_request *request)
{
    ECU_RUNTIME_ASSERT( (request) );
    static const uint8_t BITMAP = ((1U << 4) | (1U << 3) | (1U << 2) | (1U << 1) | (1U << 0));
    enum cusbd_request_recipient recipient = CUSBD_REQUEST_RECIPIENT_RESERVED;
    uint8_t data = request->bmRequestType & BITMAP;

    if (data < (uint8_t)CUSBD_REQUEST_RECIPIENT_RESERVED)
    {
        recipient = (enum cusbd_request_recipient)data;
    }

    return recipient;
}








// /*------------------------------------------------------------*/
// /*---------- STATIC FUNCTION DEFINITIONS - INTERFACE ---------*/
// /*------------------------------------------------------------*/

// static bool o_interface_valid(const struct cusbd_interface *me)
// {
//     bool status = false;
//     ECU_RUNTIME_ASSERT( (me) );

//     /* Alternate setting should always be 0 for normal CUSBD interface descriptors. */
//     if ((interface_descriptor_valid(&me->descriptor)) && 
//         (me->descriptor.bAlternateSetting == 0))
//     {
//         status = true;
//     }

//     return status;
// }

// static uint16_t o_interface_wTotalLength(const struct cusbd_interface *me)
// {
//     static const uint16_t len = (uint16_t)(sizeof(struct cusbd_interface_descriptor));
//     ECU_RUNTIME_ASSERT( (me) );
//     return len;
// }

// static enum cusbd_request_status o_interface_default_state_in(struct cusbd_interface *me,
//                                                               const struct cusbd_request *request,
//                                                               void *buf,
//                                                               size_t len)
// {
//     enum cusbd_request_status status = CUSBD_REQUEST_STATUS_IGNORED;
//     ECU_RUNTIME_ASSERT( (me && request && buf) );
//     ECU_RUNTIME_ASSERT( (cusbd_request_direction(request) == CUSBD_REQUEST_DIRECTION_DEVICE_TO_HOST) );
//     (void)len;

//     if ((cusbd_request_recipient(request) == CUSBD_REQUEST_RECIPIENT_INTERFACE) &&
//         (cusbd_request_type(request) == CUSBD_REQUEST_TYPE_STANDARD))
//     {
//         switch (cusbd_request_value(request))
//         {
//             case CUSBD_REQUEST_VALUE_GET_INTERFACE:
//             {
//                 /* Invalid request since in default state. */
//                 status = CUSBD_REQUEST_STATUS_STALL;
//                 break;
//             }

//             case CUSBD_REQUEST_VALUE_GET_STATUS:
//             {
//                 /* Invalid request since in default state. */
//                 status = CUSBD_REQUEST_STATUS_STALL;
//                 break;
//             }

//             default:
//             {
//                 /* Ignore all other requests. */
//                 break;
//             }
//         }
//     }

//     return status;
// }

// static enum cusbd_request_status o_interface_default_state_out(struct cusbd_interface *me,
//                                                                const struct cusbd_request *request)
// {
//     enum cusbd_request_status status = CUSBD_REQUEST_STATUS_IGNORED;
//     ECU_RUNTIME_ASSERT( (me && request) );
//     ECU_RUNTIME_ASSERT( (cusbd_request_direction(request) == CUSBD_REQUEST_DIRECTION_HOST_TO_DEVICE) );

//     if ((cusbd_request_recipient(request) == CUSBD_REQUEST_RECIPIENT_INTERFACE) &&
//         (cusbd_request_type(request) == CUSBD_REQUEST_TYPE_STANDARD))
//     {
//         switch (cusbd_request_value(request))
//         {
//             case CUSBD_REQUEST_VALUE_CLEAR_FEATURE:
//             {
//                 /* Invalid request since in default state. */
//                 status = CUSBD_REQUEST_STATUS_STALL;
//                 break;
//             }

//             case CUSBD_REQUEST_VALUE_SET_FEATURE:
//             {
//                 /* Invalid request since in default state. */
//                 status = CUSBD_REQUEST_STATUS_STALL;
//                 break;
//             }

//             case CUSBD_REQUEST_VALUE_SET_INTERFACE:
//             {
//                 /* Invalid request since in default state. */
//                 status = CUSBD_REQUEST_STATUS_STALL;
//                 break;
//             }

//             default:
//             {
//                 /* Ignore all other requests. */
//                 break;
//             }
//         }
//     }

//     return status;
// }

// static enum cusbd_request_status o_interface_address_state_in(struct cusbd_interface *me,
//                                                               const struct cusbd_request *request,
//                                                               void *buf,
//                                                               size_t len)
// {
//     enum cusbd_request_status status = CUSBD_REQUEST_STATUS_IGNORED;
//     ECU_RUNTIME_ASSERT( (me && request && buf) );
//     ECU_RUNTIME_ASSERT( (cusbd_request_direction(request) == CUSBD_REQUEST_DIRECTION_DEVICE_TO_HOST) );
//     (void)len;

//     if ((cusbd_request_recipient(request) == CUSBD_REQUEST_RECIPIENT_INTERFACE) &&
//         (cusbd_request_type(request) == CUSBD_REQUEST_TYPE_STANDARD))
//     {
//         switch (cusbd_request_value(request))
//         {
//             case CUSBD_REQUEST_VALUE_GET_INTERFACE:
//             {
//                 /* Invalid request since in address state. */
//                 status = CUSBD_REQUEST_STATUS_STALL;
//                 break;
//             }

//             case CUSBD_REQUEST_VALUE_GET_STATUS:
//             {
//                 /* Invalid request since in address state. */
//                 status = CUSBD_REQUEST_STATUS_STALL;
//                 break;
//             }

//             default:
//             {
//                 /* Ignore all other requests. */
//                 break;
//             }
//         }
//     }

//     return status;
// }

// static enum cusbd_request_status o_interface_address_state_out(struct cusbd_interface *me,
//                                                                const struct cusbd_request *request)
// {
//     enum cusbd_request_status status = CUSBD_REQUEST_STATUS_IGNORED;
//     ECU_RUNTIME_ASSERT( (me && request) );
//     ECU_RUNTIME_ASSERT( (cusbd_request_direction(request) == CUSBD_REQUEST_DIRECTION_HOST_TO_DEVICE) );

//     if ((cusbd_request_recipient(request) == CUSBD_REQUEST_RECIPIENT_INTERFACE) &&
//         (cusbd_request_type(request) == CUSBD_REQUEST_TYPE_STANDARD))
//     {
//         switch (cusbd_request_value(request))
//         {
//             case CUSBD_REQUEST_VALUE_CLEAR_FEATURE:
//             {
//                 /* Invalid request since in address state. */
//                 status = CUSBD_REQUEST_STATUS_STALL;
//                 break;
//             }

//             case CUSBD_REQUEST_VALUE_SET_FEATURE:
//             {
//                 /* Invalid request since in address state. */
//                 status = CUSBD_REQUEST_STATUS_STALL;
//                 break;
//             }

//             case CUSBD_REQUEST_VALUE_SET_INTERFACE:
//             {
//                 /* Invalid request since in address state. */
//                 status = CUSBD_REQUEST_STATUS_STALL;
//                 break;
//             }

//             default:
//             {
//                 /* Ignore all other requests. */
//                 break;
//             }
//         }
//     }

//     return status;
// }

// static enum cusbd_request_status o_interface_configured_state_in(struct cusbd_interface *me,
//                                                                  const struct cusbd_request *request,
//                                                                  void *buf,
//                                                                  size_t len)
// {
//     static const uint16_t INTERFACE_GET_STATUS = 0; /* A GET_STATUS() request to an interface always returns 0. */
//     enum cusbd_request_status status = CUSBD_REQUEST_STATUS_IGNORED;
//     ECU_RUNTIME_ASSERT( (me && request && buf) );
//     ECU_RUNTIME_ASSERT( (cusbd_request_direction(request) == CUSBD_REQUEST_DIRECTION_DEVICE_TO_HOST) );
//     uint16_t wValue = 0;
//     uint16_t wIndex = 0;
//     uint16_t wLength = 0;

//     if ((cusbd_request_recipient(request) == CUSBD_REQUEST_RECIPIENT_INTERFACE) &&
//         (cusbd_request_type(request) == CUSBD_REQUEST_TYPE_STANDARD))
//     {
//         wValue = ECU_LE16_TO_CPU_RUNTIME(request->wValue);
//         wIndex = ECU_LE16_TO_CPU_RUNTIME(request->wIndex);
//         wLength = ECU_LE16_TO_CPU_RUNTIME(request->wLength);

//         switch (cusbd_request_value(request))
//         {
//             case CUSBD_REQUEST_VALUE_GET_INTERFACE:
//             {
//                 if (wValue == 0 && wLength == 1)
//                 {
//                     if (wIndex == me->descriptor.bInterfaceNumber)
//                     {
//                         ECU_STATIC_ASSERT( (sizeof(me->alternate_setting) == (size_t)1), 
//                                             "alternate setting must be 1 byte unless USB spec has changed." );
//                         ECU_RUNTIME_ASSERT( (len >= sizeof(me->alternate_setting)) );
//                         memcpy(buf, &me->alternate_setting, sizeof(me->alternate_setting));
//                         status = CUSBD_REQUEST_STATUS_ACK;
//                     }
//                 }
//                 else
//                 {
//                     /* Setup packet in invalid format. */
//                     status = CUSBD_REQUEST_STATUS_STALL;
//                 }
//                 break;
//             }

//             case CUSBD_REQUEST_VALUE_GET_STATUS:
//             {
//                 if (wValue == 0 && wLength == 2)
//                 {
//                     if (wIndex == me->descriptor.bInterfaceNumber)
//                     {
//                         ECU_STATIC_ASSERT( (sizeof(INTERFACE_GET_STATUS) == (size_t)2),
//                                             "Data must be 2 bytes unless USB spec has changed." );
//                         ECU_RUNTIME_ASSERT( (len >= sizeof(INTERFACE_GET_STATUS)) );
//                         memcpy(buf, INTERFACE_GET_STATUS, sizeof(INTERFACE_GET_STATUS));
//                         status = CUSBD_REQUEST_STATUS_ACK;
//                     }
//                 }
//                 else
//                 {
//                     /* Setup packet in invalid format. */
//                     status = CUSBD_REQUEST_STATUS_STALL;
//                 }
//                 break;
//             }

//             default:
//             {
//                 /* Ignore all other requests. */
//                 break;
//             }
//         }
//     }

//     return status;
// }

// static enum cusbd_request_status o_interface_configured_state_out(struct cusbd_interface *me,
//                                                                   const struct cusbd_request *request)
// {
//     enum cusbd_request_status status = CUSBD_REQUEST_STATUS_IGNORED;
//     uint16_t wValue = 0;
//     uint16_t wIndex = 0;
//     uint16_t wLength = 0;
//     ECU_RUNTIME_ASSERT( (me && request) );
//     ECU_RUNTIME_ASSERT( (cusbd_request_direction(request) == CUSBD_REQUEST_DIRECTION_HOST_TO_DEVICE) );

//     if ((cusbd_request_recipient(request) == CUSBD_REQUEST_RECIPIENT_INTERFACE) &&
//         (cusbd_request_type(request) == CUSBD_REQUEST_TYPE_STANDARD))
//     {
//         wValue = ECU_LE16_TO_CPU_RUNTIME(request->wValue);
//         wIndex = ECU_LE16_TO_CPU_RUNTIME(request->wIndex);
//         wLength = ECU_LE16_TO_CPU_RUNTIME(request->wLength);

//         switch (cusbd_request_value(request))
//         {
//             case CUSBD_REQUEST_VALUE_CLEAR_FEATURE:
//             {
//                 /* No features exist for standard interfaces so return STALL. */
//                 status = CUSBD_REQUEST_STATUS_STALL;
//                 break;
//             }

//             case CUSBD_REQUEST_VALUE_SET_FEATURE:
//             {
//                 /* No features exist for standard interfaces so return STALL. */
//                 status = CUSBD_REQUEST_STATUS_STALL;
//                 break;
//             }

//             case CUSBD_REQUEST_VALUE_SET_INTERFACE:
//             {
//                 if (wLength == 0)
//                 {
//                     if (wIndex == me->descriptor.bInterfaceNumber)
//                     {
//                         status = CUSBD_REQUEST_STATUS_STALL; /* STALL if alt setting not found. */
//                         struct ecu_ntnode_child_citerator citerator;

//                         ECU_NTNODE_CONST_CHILD_FOR_EACH(n, &citerator, &me->base.ntnode)
//                         {
//                             struct cusbd_descriptor *base = ECU_NTNODE_GET_CONST_ENTRY(n, struct cusbd_descriptor, ntnode);

//                             if (cusbd_descriptor_type(base) == CUSBD_DESCRIPTOR_TYPE_INTERFACE)
//                             {
//                                 const struct cusbd_alternate_interface *alternate_interface = (const struct cusbd_alternate_interface *)base;
//                                 ECU_RUNTIME_ASSERT( (me->descriptor.bInterfaceNumber == alternate_interface->descriptor.bInterfaceNumber) );

//                                 if (wValue == alternate_interface->descriptor.bAlternateSetting)
//                                 {
//                                     me->alternate_setting = (uint8_t)wValue;
//                                     status = CUSBD_REQUEST_STATUS_ACK;
//                                     break;
//                                 }
//                             }
//                         }
//                     }
//                 }
//                 else
//                 {
//                     /* Setup packet in invalid format. */
//                     status = CUSBD_REQUEST_STATUS_STALL;
//                 }
//                 break;
//             }

//             default:
//             {
//                 /* Ignore all other requests. */
//                 break;
//             }
//         }
//     }

//     return status;
// }

// /*------------------------------------------------------------*/
// /*----- STATIC FUNCTION DEFINITIONS - ALTERNATE INTERFACE ----*/
// /*------------------------------------------------------------*/

// static bool o_alternate_interface_valid(const struct cusbd_alternate_interface *me)
// {
//     bool status = false;
//     ECU_RUNTIME_ASSERT( (me) );
//     return (interface_descriptor_valid(&me->descriptor));
// }

// static uint16_t o_interface_wTotalLength(const struct cusbd_interface *me)
// {
//     static const uint16_t len = (uint16_t)(sizeof(struct cusbd_interface_descriptor));
//     ECU_RUNTIME_ASSERT( (me) );
//     return len;
// }

// static enum cusbd_request_status o_alternate_interface_default_state_in(struct cusbd_alternate_interface *me,
//                                                                         const struct cusbd_request *request,
//                                                                         void *buf,
//                                                                         size_t len)
// {
//     ECU_RUNTIME_ASSERT( (me && request && buf) );
//     ECU_RUNTIME_ASSERT( (cusbd_request_direction(request) == CUSBD_REQUEST_DIRECTION_DEVICE_TO_HOST) );
//     (void)len;
//     return CUSBD_REQUEST_STATUS_IGNORED;
// }

// static enum cusbd_request_status o_alternate_interface_default_state_out(struct cusbd_alternate_interface *me,
//                                                                          const struct cusbd_request *request)
// {
//     ECU_RUNTIME_ASSERT( (me && request) );
//     ECU_RUNTIME_ASSERT( (cusbd_request_direction(request) == CUSBD_REQUEST_DIRECTION_HOST_TO_DEVICE) );
//     return CUSBD_REQUEST_STATUS_IGNORED;
// }

// static enum cusbd_request_status o_alternate_interface_address_state_in(struct cusbd_alternate_interface *me,
//                                                                         const struct cusbd_request *request,
//                                                                         void *buf,
//                                                                         size_t len)
// {
//     ECU_RUNTIME_ASSERT( (me && request && buf) );
//     ECU_RUNTIME_ASSERT( (cusbd_request_direction(request) == CUSBD_REQUEST_DIRECTION_DEVICE_TO_HOST) );
//     (void)len;
//     return CUSBD_REQUEST_STATUS_IGNORED;
// }

// static enum cusbd_request_status o_alternate_interface_address_state_out(struct cusbd_alternate_interface *me,
//                                                                          const struct cusbd_request *request)
// {
//     ECU_RUNTIME_ASSERT( (me && request) );
//     ECU_RUNTIME_ASSERT( (cusbd_request_direction(request) == CUSBD_REQUEST_DIRECTION_HOST_TO_DEVICE) );
//     return CUSBD_REQUEST_STATUS_IGNORED;
// }

// static enum cusbd_request_status o_alternate_interface_configured_state_in(struct cusbd_alternate_interface *me,
//                                                                            const struct cusbd_request *request,
//                                                                            void *buf,
//                                                                            size_t len)
// {
//     ECU_RUNTIME_ASSERT( (me && request && buf) );
//     ECU_RUNTIME_ASSERT( (cusbd_request_direction(request) == CUSBD_REQUEST_DIRECTION_DEVICE_TO_HOST) );
//     (void)len;
//     return CUSBD_REQUEST_STATUS_IGNORED;
// }

// static enum cusbd_request_status o_alternate_interface_configured_state_out(struct cusbd_alternate_interface *me,
//                                                                             const struct cusbd_request *request)
// {
//     ECU_RUNTIME_ASSERT( (me && request) );
//     ECU_RUNTIME_ASSERT( (cusbd_request_direction(request) == CUSBD_REQUEST_DIRECTION_HOST_TO_DEVICE) );
//     return CUSBD_REQUEST_STATUS_IGNORED;
// }
