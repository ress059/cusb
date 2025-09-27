/**
 * @file
 * @brief See @ref interface.h.
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
#include "cusbd/interface.h"

/* STDLib. */
#include <string.h> /* memset. */

/* CUSB. */
#include "cusbd/descriptor_private.h"
#include "cusbd/setup_packet.h"

/* ECU. */
#include "ecu/asserter.h"
#include "ecu/utils.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_FILE("cusbd/interface.c")

/*------------------------------------------------------------*/
/*--------------------------- DEFINES ------------------------*/
/*------------------------------------------------------------*/

/// @brief bDescriptorType value of interface and alternate interface descriptors.
#define BDESCRIPTORTYPE_INTERFACE ((uint8_t)0x04)

/**@{*/
/// @brief Current class codes defined by USB.
#define BINTERFACECLASS_AUDIO ((uint8_t)0x01)
#define BINTERFACECLASS_CDC_CONTROL ((uint8_t)0x02)
#define BINTERFACECLASS_HID ((uint8_t)0x03)
#define BINTERFACECLASS_PHYSICAL ((uint8_t)0x05)
#define BINTERFACECLASS_IMAGE ((uint8_t)0x06)
#define BINTERFACECLASS_PRINTER ((uint8_t)0x07)
#define BINTERFACECLASS_MASS_STORAGE ((uint8_t)0x08)
#define BINTERFACECLASS_CDC_DATA ((uint8_t)0x0A)
#define BINTERFACECLASS_SMART_CARD ((uint8_t)0x0B)
#define BINTERFACECLASS_CONTENT_SECURITY ((uint8_t)0x0D)
#define BINTERFACECLASS_VIDEO ((uint8_t)0x0E)
#define BINTERFACECLASS_PERSONAL_HEALTHCARE ((uint8_t)0x0F)
#define BINTERFACECLASS_AUDIO_AND_VIDEO ((uint8_t)0x10)
#define BINTERFACECLASS_USBC_BRIDGE ((uint8_t)0x12)
#define BINTERFACECLASS_BULK_DISPLAY ((uint8_t)0x13)
#define BINTERFACECLASS_MCTP ((uint8_t)0x14)
#define BINTERFACECLASS_I3C ((uint8_t)0x3C)
#define BINTERFACECLASS_DIAGNOSTIC_DEVICE ((uint8_t)0xDC)
#define BINTERFACECLASS_WIRELESS_CONTROLLER ((uint8_t)0xE0)
#define BINTERFACECLASS_MISC ((uint8_t)0xEF)
#define BINTERFACECLASS_APP_SPECIFIC ((uint8_t)0xFE)
#define BINTERFACECLASS_VENDOR_SPECIFIC ((uint8_t)0xFF)
/**@}*/

/**@{*/
/// @brief bRequest values relevant to standard interface and alternate
/// interface descriptors. This is in the setup packet received in the
/// status stage of the control transfer.
#define BREQUEST_GET_STATUS ((uint8_t)0)
#define BREQUEST_CLEAR_FEATURE ((uint8_t)1)
#define BREQUEST_SET_FEATURE ((uint8_t)3)
#define BREQUEST_GET_INTERFACE ((uint8_t)10)
#define BREQUEST_SET_INTERFACE ((uint8_t)11)
/**@}*/

/*------------------------------------------------------------*/
/*----------- STATIC FUNCTION DECLARATIONS - COMMON ----------*/
/*------------------------------------------------------------*/

/**
 * @brief Returns true if the interface's class code is
 * one of the valid ones listed in https://www.usb.org/defined-class-codes.
 * False otherwise.
 */
static bool b_interface_class_valid(uint8_t bInterfaceClass);

/*------------------------------------------------------------*/
/*---------- STATIC FUNCTION DECLARATIONS - INTERFACE --------*/
/*------------------------------------------------------------*/

/**
 * @brief Returns true if the supplied interface descriptor contains
 * valid data. False otherwise.
 * 
 * @param descriptor Descriptor to check.
 */
static bool interface_descriptor_valid(const struct cusbd_interface_descriptor *descriptor);

/**@{*/
/// @brief Overrides for descriptors virtual table. See @ref cusbd_descriptor_vtable
static size_t interface_v_process_setup_packet_in(const struct cusbd_interface *me, 
                                                  const struct cusbd_setup_packet *packet, 
                                                  enum cusbd_descriptor_state state,
                                                  void *buffer, 
                                                  size_t buffer_len);
static bool interface_v_process_setup_packet_out(struct cusbd_interface *me, 
                                                 const struct cusbd_setup_packet *packet,
                                                 enum cusbd_descriptor_state state);
static void interface_v_string_index_set(struct cusbd_interface *me, size_t index);
static size_t interface_v_w_total_length(const struct cusbd_interface *me);
/**@}*/

/*------------------------------------------------------------*/
/*---- STATIC FUNCTION DECLARATIONS - ALTERNATE INTERFACE ----*/
/*------------------------------------------------------------*/

/**
 * @brief Returns true if the supplied interface descriptor contains
 * valid data. False otherwise.
 * 
 * @param descriptor Descriptor to check.
 */
static bool alt_interface_descriptor_valid(const struct cusbd_interface_descriptor *descriptor);

/**@{*/
/// @brief Overrides for descriptors virtual table. See @ref cusbd_descriptor_vtable
static void alt_interface_v_string_index_set(struct cusbd_alt_interface *me, size_t index);
static size_t alt_interface_v_w_total_length(const struct cusbd_alt_interface *me);
/**@}*/

/*------------------------------------------------------------*/
/*----------- STATIC FUNCTION DEFINITIONS - COMMON -----------*/
/*------------------------------------------------------------*/

static bool b_interface_class_valid(uint8_t bInterfaceClass)
{
    bool status = false;

    switch (bInterfaceClass)
    {
        /* Intentional fallthroughs. */
        case BINTERFACECLASS_AUDIO:
        case BINTERFACECLASS_CDC_CONTROL:
        case BINTERFACECLASS_HID:
        case BINTERFACECLASS_PHYSICAL:
        case BINTERFACECLASS_IMAGE:
        case BINTERFACECLASS_PRINTER:
        case BINTERFACECLASS_MASS_STORAGE:
        case BINTERFACECLASS_CDC_DATA:
        case BINTERFACECLASS_SMART_CARD:
        case BINTERFACECLASS_CONTENT_SECURITY:
        case BINTERFACECLASS_VIDEO:
        case BINTERFACECLASS_PERSONAL_HEALTHCARE:
        case BINTERFACECLASS_AUDIO_AND_VIDEO:
        case BINTERFACECLASS_USBC_BRIDGE:
        case BINTERFACECLASS_BULK_DISPLAY:
        case BINTERFACECLASS_MCTP:
        case BINTERFACECLASS_I3C:
        case BINTERFACECLASS_DIAGNOSTIC_DEVICE:
        case BINTERFACECLASS_WIRELESS_CONTROLLER:
        case BINTERFACECLASS_MISC:
        case BINTERFACECLASS_APP_SPECIFIC:
        case BINTERFACECLASS_VENDOR_SPECIFIC:
        {
            status = true;
            break;
        }

        default:
        {
            status = false;
            break;
        }
    }
    
    return status;
}

/*------------------------------------------------------------*/
/*---------- STATIC FUNCTION DEFINITIONS - INTERFACE ---------*/
/*------------------------------------------------------------*/

static bool interface_descriptor_valid(const struct cusbd_interface_descriptor *descriptor)
{
    bool status = false;
    ECU_ASSERT( (descriptor) );

    /* Do not assert bInterfaceNumber, bNumEndpoints, bInterfaceSubClass,
    bInterfaceProtocol, and iInterface. These are either automatically updated 
    by the library or assigned by user (and are the user's responsibility to 
    ensure proper values). */
    if (descriptor->bLength == sizeof(*descriptor) &&
        descriptor->bDescriptorType == BDESCRIPTORTYPE_INTERFACE &&
        descriptor->bAlternateSetting == 0 &&
        b_interface_class_valid(descriptor->bInterfaceClass))
    {
        status = true;
    }

    return status;
}

static size_t interface_v_process_setup_packet_in(const struct cusbd_interface *me, 
                                                  const struct cusbd_setup_packet *packet, 
                                                  enum cusbd_descriptor_state state,
                                                  void *buffer, 
                                                  size_t buffer_len)
{
    ECU_ASSERT( (me && packet && buffer) );
    ECU_ASSERT( (cusbd_interface_valid(me)) );
    ECU_ASSERT( (cusbd_setup_packet_direction(packet) == CUSBD_SETUP_PACKET_DIRECTION_IN) );
    ECU_ASSERT( (state < CUSBD_DESCRIPTOR_STATE_COUNT) );
    ECU_ASSERT( (buffer_len > 0) );
    size_t num_bytes_written = 0;

    if ((cusbd_setup_packet_recipient(packet) == CUSBD_SETUP_PACKET_RECIPIENT_INTERFACE) &&
        (cusbd_setup_packet_type(packet) == CUSBD_SETUP_PACKET_TYPE_STANDARD))
    {
        /* Packet addressed to standard interface. */
        switch (cusbd_setup_packet_b_request(packet))
        {
            case BREQUEST_GET_INTERFACE:
            {
                if (state == CUSBD_DESCRIPTOR_STATE_CONFIGURED)
                {
                    /* Request is only valid when in configured state. */
                    uint16_t wIndex = cusbd_setup_packet_w_index(packet);

                    if ((uint16_t)me->descriptor.bInterfaceNumber == wIndex)
                    {
                        /* Packet is for this interface. */
                        uint16_t wValue = cusbd_setup_packet_w_value(packet);
                        uint16_t wLength = cusbd_setup_packet_w_length(packet);

                        if ((wValue == 0) && (wLength == 1))
                        {
                            /* Rest of packet is valid. Send alternate interface number if it exists. */
                            if (!me->alt_interface)
                            {
                                /* Currently using default interface so no active alternate interface. */
                                uint8_t alternate_setting = 0;
                                ECU_ASSERT( (buffer_len >= sizeof(alternate_setting)) );
                                memcpy(buffer, &alternate_setting, sizeof(alternate_setting));
                                num_bytes_written = sizeof(alternate_setting);
                            }
                            else
                            {
                                /* An alternate interface is currently active. */
                                struct ecu_ntnode_child_citerator citerator;

                                ECU_NTNODE_CONST_CHILD_FOR_EACH(cntnode, &citerator, cusbd_descriptor_cntnode(CUSBD_DESCRIPTOR_CONST_BASE_CAST(me)))
                                {
                                    /* Verify alternate interface exists before sending its number. */
                                    const struct cusbd_descriptor *base = CUSBD_DESCRIPTOR_GET_CONST_ENTRY(cntnode); /* MUST convert to base class before comparison. */

                                    if ((const void *)me->alt_interface == (const void *)base)
                                    {
                                        /* Alternate interface exists. Send its number. */
                                        uint8_t alternate_setting = me->alt_interface->descriptor.bAlternateSetting;
                                        ECU_STATIC_ASSERT( (sizeof(alternate_setting) == sizeof(uint8_t)), "Value must be uint8_t.");
                                        ECU_ASSERT( (buffer_len >= sizeof(alternate_setting)) );
                                        memcpy(buffer, &alternate_setting, sizeof(alternate_setting));
                                        num_bytes_written = sizeof(alternate_setting);
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                break;
            }

            case BREQUEST_GET_STATUS:
            {
                if (state == CUSBD_DESCRIPTOR_STATE_CONFIGURED)
                {
                    /* Request is only valid when in configured state. */
                    uint16_t wIndex = cusbd_setup_packet_w_index(packet);

                    if ((uint16_t)me->descriptor.bInterfaceNumber == wIndex)
                    {
                        /* Packet is for this interface. */
                        uint16_t wValue = cusbd_setup_packet_w_value(packet);
                        uint16_t wLength = cusbd_setup_packet_w_length(packet);

                        if ((wValue == 0) && (wLength == 2))
                        {
                            /* Rest of packet is valid. Send status which is just 0 for interfaces. */
                            uint16_t status = 0;
                            ECU_STATIC_ASSERT( (sizeof(status) == sizeof(uint16_t)), "Value must be uint16_t.");
                            ECU_ASSERT( (buffer_len >= sizeof(status)) );
                            memcpy(buffer, &status, sizeof(status));
                            num_bytes_written = sizeof(status);
                        }
                    }
                }
                break;
            }
            
            default:
            {
                /* Unsupported request. */
                break;
            }
        }
    }

    return num_bytes_written;
}

static bool interface_v_process_setup_packet_out(struct cusbd_interface *me, 
                                                 const struct cusbd_setup_packet *packet,
                                                 enum cusbd_descriptor_state state)
{
    ECU_ASSERT( (me && packet) );
    ECU_ASSERT( (cusbd_interface_valid(me)) );
    ECU_ASSERT( (cusbd_setup_packet_direction(packet) == CUSBD_SETUP_PACKET_DIRECTION_OUT) );
    ECU_ASSERT( (state < CUSBD_DESCRIPTOR_STATE_COUNT) );
    bool processed = false;

    /* Packet addressed to standard interfaces? */
    if ((cusbd_setup_packet_recipient(packet) == CUSBD_SETUP_PACKET_RECIPIENT_INTERFACE) &&
        (cusbd_setup_packet_type(packet) == CUSBD_SETUP_PACKET_TYPE_STANDARD))
    {
        switch (cusbd_setup_packet_b_request(packet))
        {
            case BREQUEST_CLEAR_FEATURE:
            {
                /* Request not supported since no feature selectors for interfaces. */
                break;
            }

            case BREQUEST_SET_FEATURE:
            {
                /* Request not supported since no feature selectors for interfaces. */
                break;
            }

            case BREQUEST_SET_INTERFACE:
            {
                if (state == CUSBD_DESCRIPTOR_STATE_CONFIGURED)
                {
                    /* Request is only valid when in configured state. */
                    uint16_t wIndex = cusbd_setup_packet_w_index(packet);

                    if ((uint16_t)me->descriptor.bInterfaceNumber == wIndex)
                    {
                        /* Packet is for this interface. */
                        uint16_t wLength = cusbd_setup_packet_w_length(packet);

                        if (wLength == 0)
                        {
                            /* Rest of packet is valid. Verify the requested alternate interface exists. */
                            uint16_t wValue = cusbd_setup_packet_w_value(packet);
                            struct ecu_ntnode_child_iterator iterator;

                            if (wValue == 0)
                            {
                                /* Host wants device to use default interface descriptor. */
                                if (me->alt_interface)
                                {
                                    /* Switching from alternate interface to default interface. */
                                    ECU_NTNODE_CHILD_FOR_EACH(ntnode, &iterator, cusbd_descriptor_ntnode(CUSBD_DESCRIPTOR_BASE_CAST(me->alt_interface)))
                                    {
                                        struct cusbd_descriptor *base = CUSBD_DESCRIPTOR_GET_ENTRY(ntnode);

                                        if (cusbd_descriptor_id(base) == CUSBD_DESCRIPTOR_ID_ENDPOINT)
                                        {
                                            /* Unhalt and deconfigure alternate interface's endpoints. */
                                            struct cusbd_endpoint *endpoint = (struct cusbd_endpoint *)base;

                                            if (cusbd_endpoint_halted(endpoint))
                                            {
                                                cusbd_endpoint_unhalt(endpoint);
                                            }

                                            cusbd_endpoint_deconfigure(endpoint);
                                        }
                                    }

                                    ECU_NTNODE_CHILD_FOR_EACH(ntnode, &iterator, cusbd_descriptor_ntnode(CUSBD_DESCRIPTOR_BASE_CAST(me)))
                                    {
                                        struct cusbd_descriptor *base = CUSBD_DESCRIPTOR_GET_ENTRY(ntnode);

                                        if (cusbd_descriptor_id(base) == CUSBD_DESCRIPTOR_ID_ENDPOINT)
                                        {
                                            /* Unhalt and configure default interface's endpoints. */
                                            struct cusbd_endpoint *endpoint = (struct cusbd_endpoint *)base;
                                            
                                            if (cusbd_endpoint_halted(endpoint))
                                            {
                                                cusbd_endpoint_unhalt(endpoint);
                                            }

                                            cusbd_endpoint_configure(endpoint);
                                        }
                                    }

                                    me->alt_interface = (struct cusbd_alt_interface *)0; /* Finally reset alternate interface value. */
                                    processed = true;
                                }
                                else
                                {
                                    /* Request interface equals one that is currently active so do not have to do anything. */
                                    processed = true;
                                }
                            }
                            else
                            {
                                /* Host wants device to use an alternate interface. */
                                bool alt_interface_exists = false;
                                struct cusbd_alt_interface *requested_alt_interface = (struct cusbd_alt_interface *)0;

                                ECU_NTNODE_CHILD_FOR_EACH(ntnode, &iterator, cusbd_descriptor_ntnode(CUSBD_DESCRIPTOR_BASE_CAST(me)))
                                {
                                    /* Verify alternate interface exists. Once found, record it. */
                                    struct cusbd_descriptor *base = CUSBD_DESCRIPTOR_GET_ENTRY(ntnode);

                                    if (cusbd_descriptor_id(base) == CUSBD_DESCRIPTOR_ID_ALT_INTERFACE)
                                    {
                                        struct cusbd_alt_interface *alt_interface = (struct cusbd_alt_interface *)base;

                                        if ((uint16_t)alt_interface->descriptor.bAlternateSetting == wValue)
                                        {
                                            alt_interface_exists = true;
                                            requested_alt_interface = alt_interface;
                                            break;
                                        }
                                    }
                                }

                                if (alt_interface_exists && requested_alt_interface)
                                {
                                    if (!me->alt_interface)
                                    {
                                        /* Switching from default interface to alternate interface. */
                                        ECU_NTNODE_CHILD_FOR_EACH(ntnode, &iterator, cusbd_descriptor_ntnode(CUSBD_DESCRIPTOR_BASE_CAST(me)))
                                        {
                                            struct cusbd_descriptor *base = CUSBD_DESCRIPTOR_GET_ENTRY(ntnode);

                                            if (cusbd_descriptor_id(base) == CUSBD_DESCRIPTOR_ID_ENDPOINT)
                                            {
                                                /* Unhalt and deconfigure default interface's endpoints. */
                                                struct cusbd_endpoint *endpoint = (struct cusbd_endpoint *)base;

                                                if (cusbd_endpoint_halted(endpoint))
                                                {
                                                    cusbd_endpoint_unhalt(endpoint);
                                                }

                                                cusbd_endpoint_deconfigure(endpoint);
                                            }
                                        }

                                        ECU_NTNODE_CHILD_FOR_EACH(ntnode, &iterator, cusbd_descriptor_ntnode(CUSBD_DESCRIPTOR_BASE_CAST(requested_alt_interface)))
                                        {
                                            struct cusbd_descriptor *base = CUSBD_DESCRIPTOR_GET_ENTRY(ntnode);

                                            if (cusbd_descriptor_id(base) == CUSBD_DESCRIPTOR_ID_ENDPOINT)
                                            {
                                                /* Unhalt and configure alternate interface's endpoints. */
                                                struct cusbd_endpoint *endpoint = (struct cusbd_endpoint *)base;

                                                if (cusbd_endpoint_halted(endpoint))
                                                {
                                                    cusbd_endpoint_unhalt(endpoint);
                                                }

                                                cusbd_endpoint_configure(endpoint);
                                            }
                                        }

                                        me->alt_interface = requested_alt_interface;
                                        processed = true;
                                    }
                                    else if (me->alt_interface != requested_alt_interface)
                                    {
                                        /* Switching from one alternate interface to another alternate interface. */
                                        ECU_NTNODE_CHILD_FOR_EACH(ntnode, &iterator, cusbd_descriptor_ntnode(CUSBD_DESCRIPTOR_BASE_CAST(me->alt_interface)))
                                        {
                                            struct cusbd_descriptor *base = CUSBD_DESCRIPTOR_GET_ENTRY(ntnode);

                                            if (cusbd_descriptor_id(base) == CUSBD_DESCRIPTOR_ID_ENDPOINT)
                                            {
                                                /* Unhalt and deconfigure old alternate interface's endpoints. */
                                                struct cusbd_endpoint *endpoint = (struct cusbd_endpoint *)base;

                                                if (cusbd_endpoint_halted(endpoint))
                                                {
                                                    cusbd_endpoint_unhalt(endpoint);
                                                }

                                                cusbd_endpoint_deconfigure(endpoint);
                                            }
                                        }

                                        ECU_NTNODE_CHILD_FOR_EACH(ntnode, &iterator, cusbd_descriptor_ntnode(CUSBD_DESCRIPTOR_BASE_CAST(requested_alt_interface)))
                                        {
                                            struct cusbd_descriptor *base = CUSBD_DESCRIPTOR_GET_ENTRY(ntnode);

                                            if (cusbd_descriptor_id(base) == CUSBD_DESCRIPTOR_ID_ENDPOINT)
                                            {
                                                /* Unhalt and configure new alternate interface's endpoints. */
                                                struct cusbd_endpoint *endpoint = (struct cusbd_endpoint *)base;

                                                if (cusbd_endpoint_halted(endpoint))
                                                {
                                                    cusbd_endpoint_unhalt(endpoint);
                                                }

                                                cusbd_endpoint_configure(endpoint);
                                            }
                                        }

                                        me->alt_interface = requested_alt_interface;
                                        processed = true;
                                    }
                                    else
                                    {
                                        /* Requested alternate interface equals one that is currently active so do not have to do anything. */
                                        processed = true;
                                    }
                                }
                            }
                        }
                    }
                }
                break;
            }

            default:
            {
                /* Unsupported request. */
                break;
            }
        }
    }

    return processed;
}

static void interface_v_string_index_set(struct cusbd_interface *me, size_t index)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (cusbd_interface_valid(me)) );
    ECU_ASSERT( (index > 0 && index <= UINT8_MAX) ); /* index 0 only for string descriptor zero. Index fields are uint8_t. */
    struct ecu_ntnode_child_iterator iterator;

    if (!ecu_dlist_empty(&me->strings))
    {
        /* Assign and increment index if this descriptor uses string descriptors. */
        me->descriptor.iInterface = (uint8_t)index;
        index++;
    }
    else
    {
        /* Otherwise 0 signifies no string descriptors used. */
        me->descriptor.iInterface = 0;
    }

    ECU_NTNODE_CHILD_FOR_EACH(ntnode, &iterator, cusbd_descriptor_cntnode(CUSBD_DESCRIPTOR_BASE_CAST(me)))
    {
        /* Composite. */
        cusbd_descriptor_v_string_index_set(CUSBD_DESCRIPTOR_GET_ENTRY(ntnode), index);
    }
}

static size_t interface_v_w_total_length(const struct cusbd_interface *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (cusbd_interface_valid(me)) );
    size_t len = sizeof(me->descriptor);
    struct ecu_ntnode_child_citerator citerator;

    ECU_NTNODE_CONST_CHILD_FOR_EACH(cntnode, &citerator, cusbd_descriptor_cntnode(CUSBD_DESCRIPTOR_CONST_BASE_CAST(me)))
    {
        /* Composite. */
        len += cusbd_descriptor_v_w_total_length(CUSBD_DESCRIPTOR_GET_CONST_ENTRY(cntnode));
    }

    return len;
}

/*------------------------------------------------------------*/
/*----- STATIC FUNCTION DEFINITIONS - ALTERNATE INTERFACE ----*/
/*------------------------------------------------------------*/

static bool alt_interface_descriptor_valid(const struct cusbd_interface_descriptor *descriptor)
{
    bool status = false;
    ECU_ASSERT( (descriptor) );

    /* Do not assert bInterfaceNumber, bNumEndpoints, bInterfaceSubClass,
    bInterfaceProtocol, and iInterface. These are either automatically updated 
    by the library or assigned by user (and are the user's responsibility to 
    ensure proper values). */
    if (descriptor->bLength == sizeof(*descriptor) &&
        descriptor->bDescriptorType == BDESCRIPTORTYPE_INTERFACE &&
        descriptor->bAlternateSetting > 0 &&
        b_interface_class_valid(descriptor->bInterfaceClass))
    {
        status = true;
    }

    return status;
}

static void alt_interface_v_string_index_set(struct cusbd_alt_interface *me, size_t index)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (cusbd_alt_interface_valid(me)) );
    ECU_ASSERT( (index > 0 && index <= UINT8_MAX) ); /* index 0 only for string descriptor zero. Index fields are uint8_t. */
    struct ecu_ntnode_child_iterator iterator;

    if (!ecu_dlist_empty(&me->strings))
    {
        /* Assign and increment index if this descriptor uses string descriptors. */
        me->descriptor.iInterface = (uint8_t)index;
        index++;
    }
    else
    {
        /* Otherwise 0 signifies no string descriptors used. */
        me->descriptor.iInterface = 0;
    }

    ECU_NTNODE_CHILD_FOR_EACH(ntnode, &iterator, cusbd_descriptor_cntnode(CUSBD_DESCRIPTOR_BASE_CAST(me)))
    {
        /* Composite. */
        cusbd_descriptor_v_string_index_set(CUSBD_DESCRIPTOR_GET_ENTRY(ntnode), index);
    }
}

static size_t alt_interface_v_w_total_length(const struct cusbd_alt_interface *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (cusbd_interface_valid(me)) );
    size_t len = sizeof(me->descriptor);
    struct ecu_ntnode_child_citerator citerator;

    ECU_NTNODE_CONST_CHILD_FOR_EACH(cntnode, &citerator, cusbd_descriptor_cntnode(CUSBD_DESCRIPTOR_CONST_BASE_CAST(me)))
    {
        /* Composite. */
        len += cusbd_descriptor_v_w_total_length(CUSBD_DESCRIPTOR_GET_CONST_ENTRY(cntnode));
    }

    return len;
}

/*------------------------------------------------------------*/
/*---------------------- STATIC ASSERTS ----------------------*/
/*------------------------------------------------------------*/

ECU_STATIC_ASSERT( (sizeof(struct cusbd_interface_descriptor) == (size_t)9),
                    "Interface descriptor is 9 bytes." );

ECU_STATIC_ASSERT( (ECU_FIELD_SIZEOF(struct cusbd_interface, descriptor) == (size_t)9),
                    "Interface descriptor is 9 bytes." );

ECU_STATIC_ASSERT( (CUSBD_DESCRIPTOR_IS_BASE_OF(base, struct cusbd_interface)),
                    "Interface descriptor must inherit cusbd_descriptor." );

/*------------------------------------------------------------*/
/*-------------- CUSBD INTERFACE MEMBER FUNCTIONS ------------*/
/*------------------------------------------------------------*/

void cusbd_interface_ctor(struct cusbd_interface *me, 
                          uint8_t bInterfaceNumber,
                          uint8_t bInterfaceClass,
                          uint8_t bInterfaceSubClass,
                          uint8_t bInterfaceProtocol)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (b_interface_class_valid(bInterfaceClass)) );

    static const struct cusbd_descriptor_vtable vtable = CUSBD_DESCRIPTOR_VTABLE_CTOR(
        &interface_v_process_setup_packet_in, &interface_v_process_setup_packet_out, &interface_v_string_index_set, &interface_v_w_total_length
    );

    /* Base class. */
    cusbd_descriptor_ctor(CUSBD_DESCRIPTOR_BASE_CAST(me), CUSBD_DESCRIPTOR_ID_INTERFACE, &me->descriptor, sizeof(me->descriptor));
    me->base.vptr = &vtable;

    /* Descriptor data. */
    memset(&me->descriptor, 0, sizeof(me->descriptor));
    me->descriptor.bLength = sizeof(me->descriptor);
    me->descriptor.bDescriptorType = BDESCRIPTORTYPE_INTERFACE;
    me->descriptor.bInterfaceClass = bInterfaceClass;
    me->descriptor.bInterfaceSubClass = bInterfaceSubClass;
    me->descriptor.bInterfaceProtocol = bInterfaceProtocol;
    
    /* Rest of object. */
    me->alt_interface = (const struct cusbd_alt_interface *)0;
    ecu_dlist_ctor(&me->strings);
}

void cusbd_interface_add_alt_interface(struct cusbd_interface *me,
                                       struct cusbd_alt_interface *alt_interface)
{
    /* ECU library asserts if node is already within a tree. */
    ECU_ASSERT( (me && alt_interface) );
    ECU_ASSERT( (cusbd_interface_valid(me)) );
    ECU_ASSERT( (cusbd_alt_interface_valid(alt_interface)) );

    struct ecu_ntnode *interface_ntnode = cusbd_descriptor_ntnode(CUSBD_DESCRIPTOR_BASE_CAST(me));
    struct ecu_ntnode *alt_interface_ntnode = cusbd_descriptor_ntnode(CUSBD_DESCRIPTOR_BASE_CAST(alt_interface));
    ecu_ntnode_push_child_back(interface_ntnode, alt_interface_ntnode); /* Push back so GET_DESCRIPTOR(CONFIGURATION) can be accomplished with preorder iteration. */
    alt_interface->descriptor.bInterfaceNumber = me->descriptor.bInterfaceNumber;
}

void cusbd_interface_add_endpoint(struct cusbd_interface *me,
                                  struct cusbd_endpoint *endpoint)
{
    /* ECU library asserts if node is already within a tree. */
    ECU_ASSERT( (me && endpoint) );
    ECU_ASSERT( (cusbd_interface_valid(me)) );
    ECU_ASSERT( (cusbd_endpoint_valid(endpoint)) );

    struct ecu_ntnode *interface_ntnode = cusbd_descriptor_ntnode(CUSBD_DESCRIPTOR_BASE_CAST(me));
    struct ecu_ntnode *endpoint_ntnode = cusbd_descriptor_ntnode(CUSBD_DESCRIPTOR_BASE_CAST(endpoint));
    ecu_ntnode_push_child_front(interface_ntnode, endpoint_ntnode); /* Push front so GET_DESCRIPTOR(CONFIGURATION) can be accomplished with preorder iteration. */
    me->descriptor.bNumEndpoints++;
}

void cusbd_interface_add_string(struct cusbd_interface *me,
                                struct cusbd_string *string)
{
    /* ECU library asserts if node is already within a list. */
    ECU_ASSERT( (me && string) );
    ECU_ASSERT( (cusbd_interface_valid(me)) );
    ECU_ASSERT( (cusbd_string_valid(string)) );
    ecu_dlist_push_back(&me->strings, cusbd_string_dnode(string));
}

bool cusbd_interface_valid(const struct cusbd_interface *me)
{
    ECU_ASSERT( (me) );
    bool status = false;

    if ((cusbd_descriptor_valid(CUSBD_DESCRIPTOR_BASE_CAST(me))) &&
        (cusbd_descriptor_id(CUSBD_DESCRIPTOR_BASE_CAST(me)) == CUSBD_DESCRIPTOR_ID_INTERFACE) &&
        (interface_descriptor_valid(&me->descriptor)) &&
        (ecu_dlist_valid(&me->strings)))
    {
        status = true;
    }

    return status;
}

/*------------------------------------------------------------*/
/*--------- CUSBD ALTERNATE INTERFACE MEMBER FUNCTIONS -------*/
/*------------------------------------------------------------*/

void cusbd_alt_interface_ctor(struct cusbd_alt_interface *me, 
                              uint8_t bAlternateSetting,
                              uint8_t bInterfaceClass,
                              uint8_t bInterfaceSubClass,
                              uint8_t bInterfaceProtocol)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (bAlternateSetting > 0) );
    ECU_ASSERT( (b_interface_class_valid(bInterfaceClass)) );

    static const struct cusbd_descriptor_vtable vtable = CUSBD_DESCRIPTOR_VTABLE_CTOR(
        &CUSBD_DESCRIPTOR_V_PROCESS_SETUP_PACKET_IN_UNUSED,
        &CUSBD_DESCRIPTOR_V_PROCESS_SETUP_PACKET_OUT_UNUSED,
        &alt_interface_v_string_index_set, 
        &alt_interface_v_w_total_length
    );

    /* Base class. */
    cusbd_descriptor_ctor(CUSBD_DESCRIPTOR_BASE_CAST(me), CUSBD_DESCRIPTOR_ID_ALT_INTERFACE, &me->descriptor, sizeof(me->descriptor));
    me->base.vptr = &vtable;

    /* Descriptor data. */
    memset(&me->descriptor, 0, sizeof(me->descriptor));
    me->descriptor.bLength = sizeof(me->descriptor);
    me->descriptor.bDescriptorType = BDESCRIPTORTYPE_INTERFACE;
    me->descriptor.bAlternateSetting = bAlternateSetting;
    me->descriptor.bInterfaceClass = bInterfaceClass;
    me->descriptor.bInterfaceSubClass = bInterfaceSubClass;
    me->descriptor.bInterfaceProtocol = bInterfaceProtocol;
    
    /* Rest of object. */
    ecu_dlist_ctor(&me->strings);
}

void cusbd_alt_interface_add_endpoint(struct cusbd_alt_interface *me,
                                      struct cusbd_endpoint *endpoint)
{
    /* ECU library asserts if node is already within a tree. */
    ECU_ASSERT( (me && endpoint) );
    ECU_ASSERT( (cusbd_alt_interface_valid(me)) );
    ECU_ASSERT( (cusbd_endpoint_valid(endpoint)) );

    struct ecu_ntnode *alt_interface_ntnode = cusbd_descriptor_ntnode(CUSBD_DESCRIPTOR_BASE_CAST(me));
    struct ecu_ntnode *endpoint_ntnode = cusbd_descriptor_ntnode(CUSBD_DESCRIPTOR_BASE_CAST(endpoint));
    ecu_ntnode_push_child_front(alt_interface_ntnode, endpoint_ntnode); /* Push front so GET_DESCRIPTOR(CONFIGURATION) can be accomplished with preorder iteration. */
    me->descriptor.bNumEndpoints++;
}

void cusbd_alt_interface_add_string(struct cusbd_alt_interface *me,
                                    struct cusbd_string *string)
{
    /* ECU library asserts if node is already within a list. */
    ECU_ASSERT( (me && string) );
    ECU_ASSERT( (cusbd_alt_interface_valid(me)) );
    ECU_ASSERT( (cusbd_string_valid(string)) );
    ecu_dlist_push_back(&me->strings, cusbd_string_dnode(string));
}

bool cusbd_alt_interface_valid(const struct cusbd_alt_interface *me)
{
    ECU_ASSERT( (me) );
    bool status = false;

    if ((cusbd_descriptor_valid(CUSBD_DESCRIPTOR_BASE_CAST(me))) &&
        (cusbd_descriptor_id(CUSBD_DESCRIPTOR_BASE_CAST(me)) == CUSBD_DESCRIPTOR_ID_ALT_INTERFACE) &&
        (alt_interface_descriptor_valid(&me->descriptor)) &&
        (ecu_dlist_valid(&me->strings)))
    {
        status = true;
    }

    return status;
}
