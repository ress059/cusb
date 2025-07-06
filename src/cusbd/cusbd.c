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
#include "cusbd/visitor/visitor.h"
#include "cusbd/visitor/clear_feature.h"
#include "cusbd/visitor/get_configuration.h"

/* ECU. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_NAME("cusbd/cusbd.c")

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Returns true if supplied descriptor was properly
 * constructed via @ref CUSBD_DEVICE_DESCRIPTOR_CTOR().
 * False otherwise.
 * 
 * @param descriptor Descriptor to check.
 */
static bool device_descriptor_valid(const struct cusbd_device_descriptor *descriptor);

/**
 * @brief Override of @ref v_cusbd_descriptor_accept().
 */
static void o_accept(struct cusbd *me, struct cusbd_visitor *visitor);

/**
 * @brief Override of @ref v_cusbd_descriptor_caccept().
 */
static void o_caccept(const struct cusbd *me, struct cusbd_cvisitor *visitor);

/*------------------------------------------------------------*/
/*--- STATIC FUNCTION DECLARATIONS - STATE MACHINE HELPERS ---*/
/*------------------------------------------------------------*/

/**
 * @brief Returns true when the current descriptor in the iteration
 * has processed the setup packet. Otherwise returns false. Callback
 * function passed to @ref cusbd_descriptor_accept_before() and 
 * @ref cusbd_descriptor_caccept_before(). Do not call directly.
 * 
 * @param d Unused.
 * @param obj @ref cusbd_request object.
 */
bool setup_packet_processed(const struct cusbd_descriptor *d, void *obj);

/**
 * @brief Processes setup packet received in control transfer 
 * when device is in default state. If request is valid in
 * default state, it is processed by creating the proper visitor 
 * and accepting it across the descriptor tree. Architected such 
 * that support for new requests should only require additions 
 * to this function.
 * 
 * @warning Can change device's HSM state.
 * 
 * @param me Device that received setup packet.
 * @param event Setup packet RX event.
 */
static void process_setup_packet_default_state(struct cusbd *me, 
                                               const struct cusbd_setup_packet_rx_event *event);

/**
 * @brief Same as @ref process_setup_packet_default_state(), 
 * however this function processes the packet while the device
 * is in the address state.
 */
static void process_setup_packet_address_state(struct cusbd *me,
                                               const struct cusbd_setup_packet_rx_event *event);

/**
 * @brief Same as @ref process_setup_packet_default_state(), 
 * however this function processes the packet while the device
 * is in the configured state.
 */
static void process_setup_packet_configured_state(struct cusbd *me,
                                                  const struct cusbd_setup_packet_rx_event *event);

/*------------------------------------------------------------*/
/*------------ STATIC FUNCTION DECLARATIONS - STATES ---------*/
/*------------------------------------------------------------*/

/// @brief State in device hierarchical state machine. Runs when
/// event dispatched to this state. See file description.
static bool TOP_STATE_HANDLER(struct cusbd *me, const void *event);

// /// @brief State in device hierarchical state machine. Runs when
// /// hsm first enters this state. See file description.
// static void ON_STATE_ENTRY(struct cusbd *me);

/// @brief State in device hierarchical state machine. Runs when
/// event dispatched to this state. See file description.
static bool ON_STATE_HANDLER(struct cusbd *me, const void *event);

/// @brief State in device hierarchical state machine. Runs when
/// event dispatched to this state. See file description.
static bool OFF_STATE_HANDLER(struct cusbd *me, const void *event);

// /// @brief State in device hierarchical state machine. Runs when
// /// hsm first enters this state. See file description.
// static void DEFAULT_STATE_ENTRY(struct cusbd *me);

/// @brief State in device hierarchical state machine. Runs when
/// event dispatched to this state. See file description.
static bool DEFAULT_STATE_HANDLER(struct cusbd *me, const void *event);

// /// @brief State in device hierarchical state machine. Runs when
// /// hsm first enters this state. See file description.
// static void ADDRESS_STATE_ENTRY(struct cusbd *me);

/// @brief State in device hierarchical state machine. Runs when
/// event dispatched to this state. See file description.
static bool ADDRESS_STATE_HANDLER(struct cusbd *me, const void *event);

// /// @brief State in device hierarchical state machine. Runs when
// /// hsm first enters this state. See file description.
// static void CONFIGURED_STATE_ENTRY(struct cusbd *me);

/// @brief State in device hierarchical state machine. Runs when
/// event dispatched to this state. See file description.
static bool CONFIGURED_STATE_HANDLER(struct cusbd *me, const void *event);

/*------------------------------------------------------------*/
/*-------------------- STATIC VARIABLES ----------------------*/
/*------------------------------------------------------------*/

static const struct ecu_hsm_state TOP_STATE = ECU_HSM_STATE_CTOR(
    ECU_HSM_STATE_ENTRY_UNUSED, ECU_HSM_STATE_EXIT_UNUSED, &TOP_STATE_HANDLER, ECU_HSM_STATE_NO_PARENT
);

static const struct ecu_hsm_state ON_STATE = ECU_HSM_STATE_CTOR(
    ECU_HSM_STATE_ENTRY_UNUSED, ECU_HSM_STATE_EXIT_UNUSED, &ON_STATE_HANDLER, &TOP_STATE
);

static const struct ecu_hsm_state OFF_STATE = ECU_HSM_STATE_CTOR(
    ECU_HSM_STATE_ENTRY_UNUSED, ECU_HSM_STATE_EXIT_UNUSED, &OFF_STATE_HANDLER, &TOP_STATE
);

static const struct ecu_hsm_state DEFAULT_STATE = ECU_HSM_STATE_CTOR(
    ECU_HSM_STATE_ENTRY_UNUSED, ECU_HSM_STATE_EXIT_UNUSED, &DEFAULT_STATE_HANDLER, &ON_STATE
);

static const struct ecu_hsm_state ADDRESS_STATE = ECU_HSM_STATE_CTOR(
    ECU_HSM_STATE_ENTRY_UNUSED, ECU_HSM_STATE_EXIT_UNUSED, &ADDRESS_STATE_HANDLER, &ON_STATE
);

static const struct ecu_hsm_state CONFIGURED_STATE = ECU_HSM_STATE_CTOR(
    ECU_HSM_STATE_ENTRY_UNUSED, ECU_HSM_STATE_EXIT_UNUSED, &CONFIGURED_STATE_HANDLER, &ON_STATE
);

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
/*------------------------------------------------------------*/

static bool device_descriptor_valid(const struct cusbd_device_descriptor *descriptor)
{
    bool status = false;
    ECU_RUNTIME_ASSERT( (descriptor) );

#pragma message("TODO: Figure out bcdUSB, bDeviceClass, bDeviceSubClass, bDeviceProtocol, "\
        "and valid bcdUSB and bMaxPacketSize0 combinations.")
    /* Do not assert idVendor, idProduct, and bcdDevice since these are user-specific. 
    Do not assert iManufacturer, iProduct, iSerialNumber, and bNumConfigurations 
    since these are automatically updated when descriptors are added to the device. */
    if ((descriptor->bLength == sizeof(struct cusbd_device_descriptor)) &&
        (descriptor->bDescriptorType == CUSBD_BDESCRIPTORTYPE) &&
        (descriptor->bMaxPacketSize0 == (uint8_t)8 || descriptor->bMaxPacketSize0 == (uint8_t)16 ||
         descriptor->bMaxPacketSize0 == (uint8_t)32 || descriptor->bMaxPacketSize0 == (uint8_t)64))
    {
        status = true;
    }

    return status;
}

static void o_accept(struct cusbd *me, struct cusbd_visitor *visitor)
{
    /* Do not assert valid() since that is centralized in the v_cusbd_descriptor_accept() function. */
    ECU_RUNTIME_ASSERT( (me && visitor) );
    v_cusbd_visitor_visit_device(visitor, me);
}

static void o_caccept(const struct cusbd *me, struct cusbd_cvisitor *visitor)
{
    /* Do not assert valid() since that is centralized in the v_cusbd_descriptor_caccept() function. */
    ECU_RUNTIME_ASSERT( (me && visitor) );
    v_cusbd_cvisitor_visit_device(visitor, me);
}

/*------------------------------------------------------------*/
/*--- STATIC FUNCTION DEFINITIONS - STATE MACHINE HELPERS ----*/
/*------------------------------------------------------------*/

bool setup_packet_processed(const struct cusbd_descriptor *d, void *obj)
{
    (void)d;
    ECU_RUNTIME_ASSERT( (obj) );
    struct cusbd_request *request = (struct cusbd_request *)obj;
    return (cusbd_request_done(request));
}

static void process_setup_packet_default_state(struct cusbd *me, 
                                               const struct cusbd_setup_packet_rx_event *event)
{
    ECU_RUNTIME_ASSERT( (me && event) );
    /* Use variable to avoid passing wrong state. */
    static const enum cusbd_request_state STATE = CUSBD_REQUEST_STATE_DEFAULT_STATE;

    enum cusbd_request_status status = CUSBD_REQUEST_STATUS_UNPROCESSED;
    struct cusbd_request request;
    cusbd_request_ctor(&request, event, CUSBD_REQUEST_STATE_DEFAULT_STATE);

#pragma message("TODO")
    switch (cusbd_request_value(&request))
    {
        case CUSBD_REQUEST_VALUE_GET_DESCRIPTOR:
        {
            break;
        }

        case CUSBD_REQUEST_VALUE_SET_ADDRESS:
        {
            break;
        }

        case CUSBD_REQUEST_VALUE_SET_FEATURE:
        {
            break;
        }

        /* !! FUTURE SUPPORTED REQUESTS ADDED HERE. ONLY ADD 
        REQUESTS THAT ARE VALID IN THE DEFAULT STATE !! */

        default:
        {
            /* Any unsupported requests. */
            status = CUSBD_REQUEST_STATUS_STALL;
            break;
        }
    }

#pragma message("TODO: Need to know how to ACK, NAK, and STALL.")

    if (status == CUSBD_REQUEST_STATUS_ACK)
    {
        // ACK.
    }
    else if (status == CUSBD_REQUEST_STATUS_NAK)
    {
        // NAK
    }
    else
    {
        // STALL if STALL or UNPROCESSED.
    }
}

static void process_setup_packet_address_state(struct cusbd *me,
                                               const struct cusbd_setup_packet_rx_event *event)
{
    ECU_RUNTIME_ASSERT( (me && event) );
    /* Use variable to avoid passing wrong state. */
    static const enum cusbd_request_state STATE = CUSBD_REQUEST_STATE_ADDRESS_STATE;

    enum cusbd_request_status status = CUSBD_REQUEST_STATUS_UNPROCESSED;
    struct cusbd_request request;
    cusbd_request_ctor(&request, event, STATE);

    #pragma message("TODO")

    switch (cusbd_request_value(&request))
    {
        case CUSBD_REQUEST_VALUE_CLEAR_FEATURE:
        {
            struct cusbd_visitor_clear_feature clear_feature;
            cusbd_visitor_clear_feature_ctor(&clear_feature, event, STATE);
            (void)cusbd_descriptor_accept_before(&me->base, &clear_feature.base, &setup_packet_processed, &clear_feature.request);
            status = cusbd_request_status(&clear_feature.request);
            break;
        }

        case CUSBD_REQUEST_VALUE_GET_CONFIGURATION:
        {
            struct cusbd_visitor_get_configuration get_configuration;
            cusbd_visitor_get_configuration_ctor(&get_configuration, event, STATE);
            v_cusbd_descriptor_caccept(&me->base, &get_configuration.base); /* Can get away with only running on device. */
            status = cusbd_request_status(&get_configuration.request);
            break;
        }

        case CUSBD_REQUEST_VALUE_GET_DESCRIPTOR:
        {
            break;
        }

        case CUSBD_REQUEST_VALUE_GET_STATUS:
        {
            break;
        }

        case CUSBD_REQUEST_VALUE_SET_ADDRESS:
        {
            if (address == 0)
            {
                ecu_hsm_change_state(me, &DEFAULT_STATE);
            }
            break;
        }

        case CUSBD_REQUEST_VALUE_SET_CONFIGURATION:
        {
            if (config > 0)
            {
                ecu_hsm_change_state(me, &CONFIGURED_STATE);
            }
            break;
        }

        case CUSBD_REQUEST_VALUE_SET_DESCRIPTOR:
        {
            break;
        }

        case CUSBD_REQUEST_VALUE_SET_FEATURE:
        {
            break;
        }

        /* !! FUTURE SUPPORTED REQUESTS ADDED HERE. ONLY ADD 
        REQUESTS THAT ARE VALID IN THE ADDRESS STATE !! */

        default:
        {
            /* Any unsupported requests. */
            status = CUSBD_REQUEST_STATUS_STALL;
            break;
        }
    }

#pragma message("TODO: Need to know how to ACK, NAK, and STALL.")

    if (status == CUSBD_REQUEST_STATUS_ACK)
    {
        // ACK.
    }
    else if (status == CUSBD_REQUEST_STATUS_NAK)
    {
        // NAK
    }
    else
    {
        // STALL if STALL or UNPROCESSED.
    }
}

static void process_setup_packet_configured_state(struct cusbd *me,
                                                  const struct cusbd_setup_packet_rx_event *event)
{
    ECU_RUNTIME_ASSERT( (me && event) );
    /* Use variable to avoid passing wrong state. */
    static const enum cusbd_request_state STATE = CUSBD_REQUEST_STATE_CONFIGURED_STATE;

    enum cusbd_request_status status = CUSBD_REQUEST_STATUS_UNPROCESSED;
    struct cusbd_request request;
    cusbd_request_ctor(&request, event, STATE);

    #pragma message("TODO")

    switch (cusbd_request_value(&request))
    {
        case CUSBD_REQUEST_VALUE_CLEAR_FEATURE:
        {
            struct cusbd_visitor_clear_feature clear_feature;
            cusbd_visitor_clear_feature_ctor(&clear_feature, event, STATE);
            (void)cusbd_descriptor_accept_before(&me->base, &clear_feature.base, &setup_packet_processed, &clear_feature.request);
            status = cusbd_request_status(&clear_feature.request);
            break;
        }

        case CUSBD_REQUEST_VALUE_GET_CONFIGURATION:
        {
            struct cusbd_visitor_get_configuration get_configuration;
            cusbd_visitor_get_configuration_ctor(&get_configuration, event, STATE);
            v_cusbd_descriptor_caccept(&me->base, &get_configuration.base); /* Can get away with only running on device. */
            status = cusbd_request_status(&get_configuration.request);
            break;
        }

        case CUSBD_REQUEST_VALUE_GET_DESCRIPTOR:
        {
            break;
        }

        case CUSBD_REQUEST_VALUE_GET_INTERFACE:
        {
            break;
        }

        case CUSBD_REQUEST_VALUE_GET_STATUS:
        {
            break;
        }

        case CUSBD_REQUEST_VALUE_SET_CONFIGURATION:
        {
            if (config == 0)
            {
                ecu_hsm_change_state(me, &ADDRESS_STATE);
            }
            break;
        }

        case CUSBD_REQUEST_VALUE_SET_DESCRIPTOR:
        {
            break;
        }

        case CUSBD_REQUEST_VALUE_SET_FEATURE:
        {
            break;
        }

        case CUSBD_REQUEST_VALUE_SET_INTERFACE:
        {
            break;
        }

        case CUSBD_REQUEST_VALUE_SYNCH_FRAME:
        {
            break;
        }

        /* !! FUTURE SUPPORTED REQUESTS ADDED HERE. ONLY ADD 
        REQUESTS THAT ARE VALID IN THE CONFIGURED STATE !! */

        default:
        {
            /* Any unsupported requests. */
            status = CUSBD_REQUEST_STATUS_STALL;
            break;
        }
    }

#pragma message("TODO: Need to know how to ACK, NAK, and STALL.")

    if (status == CUSBD_REQUEST_STATUS_ACK)
    {
        // ACK.
    }
    else if (status == CUSBD_REQUEST_STATUS_NAK)
    {
        // NAK
    }
    else
    {
        // STALL if STALL or UNPROCESSED.
    }
}                

/*------------------------------------------------------------*/
/*------------- STATIC FUNCTION DEFINITIONS - STATES ---------*/
/*------------------------------------------------------------*/

static bool TOP_STATE_HANDLER(struct cusbd *me, const void *event)
{
    /* Ignore all other events. */
    (void)me;
    (void)event;
    return true;
}

static bool ON_STATE_HANDLER(struct cusbd *me, const void *event)
{
#pragma message("TODO: Placeholder for now.")
    (void)me;
    (void)event;
    return true;
}

static bool OFF_STATE_HANDLER(struct cusbd *me, const void *event)
{
#pragma message("TODO: Placeholder for now.")
    (void)me;
    (void)event;
    return true;
}

static bool DEFAULT_STATE_HANDLER(struct cusbd *me, const void *event)
{
    ECU_RUNTIME_ASSERT( (me && event) );
    ECU_RUNTIME_ASSERT( (cusbd_valid(me)) );
    bool handled = true;

    switch (cusbd_event_id((const struct cusbd_event *)event))
    {
        case CUSBD_EVENT_ID_SETUP_PACKET_RX:
        {
            process_setup_packet_default_state(me, (const struct cusbd_setup_packet_rx_event *)event);
            break;
        }

        default:
        {
            /* Propagate events up the HSM hierarchy if event unhandled. */
            handled = false;
            break;
        }
    }

    return handled;
}

static bool ADDRESS_STATE_HANDLER(struct cusbd *me, const void *event)
{
    ECU_RUNTIME_ASSERT( (me && event) );
    ECU_RUNTIME_ASSERT( (cusbd_valid(me)) );
    bool handled = true;

    switch (cusbd_event_id((const struct cusbd_event *)event))
    {
        case CUSBD_EVENT_ID_SETUP_PACKET_RX:
        {
            process_setup_packet_address_state(me, (const struct cusbd_setup_packet_rx_event *)event);
            break;
        }

        default:
        {
            /* Propagate events up the HSM hierarchy if event unhandled. */
            handled = false;
            break;
        }
    }

    return handled;
}

static bool CONFIGURED_STATE_HANDLER(struct cusbd *me, const void *event)
{
    ECU_RUNTIME_ASSERT( (me && event) );
    ECU_RUNTIME_ASSERT( (cusbd_valid(me)) );
    bool handled = true;

    switch (cusbd_event_id((const struct cusbd_event *)event))
    {
        case CUSBD_EVENT_ID_SETUP_PACKET_RX:
        {
            process_setup_packet_configured_state(me, (const struct cusbd_setup_packet_rx_event *)event);
            break;
        }

        default:
        {
            /* Propagate events up the HSM hierarchy if event unhandled. */
            handled = false;
            break;
        }
    }

    return handled;
}

/*------------------------------------------------------------*/
/*---------------------- STATIC ASSERTS ----------------------*/
/*------------------------------------------------------------*/

ECU_STATIC_ASSERT( (sizeof(struct cusbd_device_descriptor) == (size_t)18),
                    "Device descriptor is 18 bytes." );

ECU_STATIC_ASSERT( (CUSBD_DESCRIPTOR_IS_BASEOF(base, struct cusbd)),
                    "cusbd must inherit cusbd_descriptor." );

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
//     ECU_RUNTIME_ASSERT( (me && descriptor && configure && post) );
//     ECU_RUNTIME_ASSERT( (device_descriptor_valid(descriptor)) );
//     if (string0 != CUSBD_STRING_ZERO_UNUSED)
//     {
//         ECU_RUNTIME_ASSERT( (cusbd_string_zero_valid(string0)) );
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
    ECU_RUNTIME_ASSERT( (me && configuration) );
    ECU_RUNTIME_ASSERT( (cusbd_valid(me)) );
    ECU_RUNTIME_ASSERT( (cusbd_configuration_valid(configuration)) );
    ecu_ntnode_push_back(&me->base.ntnode, &configuration->base.ntnode);
}

void cusbd_add_manufacturer_string(struct cusbd *me,
                                   struct cusbd_string *string)
{
    /* ECU library asserts if node is already within a list. */
    ECU_RUNTIME_ASSERT( (me && string) );
    ECU_RUNTIME_ASSERT( (cusbd_valid(me)) );
    ECU_RUNTIME_ASSERT( (cusbd_string_valid(string)) );
    ecu_dlist_push_back(&me->manufacturer_strings, &string->dnode);
}

void cusbd_add_product_string(struct cusbd *me,
                              struct cusbd_string *string)
{
    /* ECU library asserts if node is already within a list. */
    ECU_RUNTIME_ASSERT( (me && string) );
    ECU_RUNTIME_ASSERT( (cusbd_valid(me)) );
    ECU_RUNTIME_ASSERT( (cusbd_string_valid(string)) );
    ecu_dlist_push_back(&me->product_strings, &string->dnode);
}

void cusbd_add_serial_number_string(struct cusbd *me,
                                    struct cusbd_string *string)
{
    /* ECU library asserts if node is already within a list. */
    ECU_RUNTIME_ASSERT( (me && string) );
    ECU_RUNTIME_ASSERT( (cusbd_valid(me)) );
    ECU_RUNTIME_ASSERT( (cusbd_string_valid(string)) );
    ecu_dlist_push_back(&me->serial_number_strings, &string->dnode);
}

void cusbd_dispatch(struct cusbd *me, const void *event)
{
    ECU_RUNTIME_ASSERT( (me && event) );
    ECU_RUNTIME_ASSERT( (cusbd_vaild(me)) );
    ecu_hsm_dispatch(&me->base.hsm, event);
}

void cusbd_start(struct cusbd *me)
{
    (void)me;

    // // !! idea is to limit the complexity of assigning bConfigurationValue, etc to
    // // just this function.
    // ECU_RUNTIME_ASSERT( (me) );
    // ECU_RUNTIME_ASSERT( (device_descriptor_valid(&me->descriptor)) );

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
    //     ECU_RUNTIME_ASSERT( (cusbd_configuration_valid(configuration)) );
    //     wTotalLength = sizeof(struct cusbd_configuration_descriptor);
    //     bNumConfigurations++;
    //     bConfigurationValue++; /* Starts at 1. */

    //     ECU_DLIST_FOR_EACH(i, &interface_iterator, &configuration->interfaces)
    //     {
    //         interface = ECU_DNODE_GET_ENTRY(i, struct cusbd_interface, dnode);
    //         ECU_RUNTIME_ASSERT( (cusbd_interface_valid(interface)) );
    //         wTotalLength += sizeof(struct cusbd_interface_descriptor);
    //         bNumInterfaces++;

    //         ECU_DLIST_FOR_EACH(e, &endpoint_iterator, &interface->endpoints)
    //         {
    //             endpoint = ECU_DNODE_GET_ENTRY(e, struct cusbd_endpoint, dnode);
    //             ECU_RUNTIME_ASSERT( (cusbd_endpoint_valid(endpoint)) );
    //             bNumEndpoints++;
    //         }

    //         ECU_DLIST_FOR_EACH(ai, &alternate_interface_iterator, &interface->alternate_interfaces)
    //         {
    //             alternate_interface = ECU_DNODE_GET_ENTRY(ai, struct cusbd_alternate_interface, dnode);
    //             ECU_RUNTIME_ASSERT( (cusbd_alternate_interface_valid(alternate_interface)) );

    //             ECU_DLIST_FOR_EACH(e, &endpoint_iterator, &alternate_interface->endpoints)
    //             {
    //                 endpoint = ECU_DNODE_GET_ENTRY(e, struct cusbd_endpoint, dnode);
    //                 ECU_RUNTIME_ASSERT( (cusbd_endpoint_valid(endpoint)) );

    //             }
    //         }


    //     }

    //     /* Set all config items here. */

    //     // wTotalLength = cusbd_configuration_size(configuration);
    //     // ECU_RUNTIME_ASSERT( (wTotalLength <= UINT16_MAX) );
    //     // configuration->descriptor.wTotalLength = ECU_CPU_TO_LE16_RUNTIME((uint16_t)wTotalLength);
    // }

    // /* Device must have at least one configuration. */
    // ECU_RUNTIME_ASSERT( (bNumConfigurations >= (size_t)1) );
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
//             ECU_RUNTIME_ASSERT( (bNumInterfaces <= UINT8_MAX) );
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
//     ECU_RUNTIME_ASSERT( (me && alternate_interface) );
//     ECU_RUNTIME_ASSERT( (v_cusbd_descriptor_valid(CUSBD_DESCRIPTOR_CONST_BASE_CAST(me))) );
//     ECU_RUNTIME_ASSERT( (v_cusbd_descriptor_valid(CUSBD_DESCRIPTOR_CONST_BASE_CAST(alternate_interface))) );

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
//         ECU_RUNTIME_ASSERT( (cusbd_descriptor_type(&base) == CUSBD_DESCRIPTOR_TYPE_CONFIGURATION) );
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
//     ECU_RUNTIME_ASSERT( (me && endpoint) );
//     ECU_RUNTIME_ASSERT( (v_cusbd_descriptor_valid(CUSBD_DESCRIPTOR_CONST_BASE_CAST(me))) );
//     ECU_RUNTIME_ASSERT( (v_cusbd_descriptor_valid(CUSBD_DESCRIPTOR_CONST_BASE_CAST(endpoint))) );

//     /* Update bNumEndpoints. Also verify no endpoints are duplicated. I.e. cannot 
//     have multiple endpoint1 INs attached to the same interface descriptor. */
//     me->descriptor.bNumEndpoints = 0;
//     ECU_NTNODE_CONST_CHILD_FOR_EACH(n, &citerator, &me->base.ntnode)
//     {
//         base = ECU_NTNODE_GET_CONST_ENTRY(n, struct cusbd_descriptor, ntnode);

//         if (cusbd_descriptor_type(base) == CUSBD_DESCRIPTOR_TYPE_ENDPOINT)
//         {
//             e = (const struct cusb_endpoint *)base;
//             ECU_RUNTIME_ASSERT( (e->descriptor.bEndpointAddress != endpoint->descriptor.bEndpointAddress) );
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
    ECU_RUNTIME_ASSERT( (me) );
    return (cusbd_descriptor_valid(&me->base) &&
            cusbd_descriptor_type(&me->base) == CUSBD_BDESCRIPTORTYPE &&
            device_descriptor_valid(&me->descriptor) &&
            ecu_dlist_valid(&me->manufacturer_strings) &&
            ecu_dlist_valid(&me->product_strings) &&
            ecu_dlist_valid(&me->serial_number_strings));
}
