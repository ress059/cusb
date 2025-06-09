/**
 * @file
 * @brief Object representing an interface descriptor.
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
#include "cusbd/configuration.h"
#include "cusbd/interface.h"

/* STDLib. */
#include <string.h> /* memcpy. */

/* CUSB. */
#include "cusbd/request.h"

/* ECU. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_NAME("cusbd/interface.c")

/*------------------------------------------------------------*/
/*----------- STATIC FUNCTION DECLARATIONS - COMMON ----------*/
/*------------------------------------------------------------*/

/**
 * @brief Returns true if the supplied interface descriptor contains
 * valid data and was properly constructed via @ref CUSBD_INTERFACE_DESCRIPTOR_CTOR(). 
 * False otherwise.
 * 
 * @param descriptor Descriptor to check.
 */
static bool interface_descriptor_valid(const struct cusbd_interface_descriptor *descriptor);

/*------------------------------------------------------------*/
/*---------- STATIC FUNCTION DECLARATIONS - INTERFACE --------*/
/*------------------------------------------------------------*/

/**
 * @brief Override for @ref v_cusbd_descriptor_valid(). 
 * See @ref v_cusbd_descriptor_valid().
 */
static bool o_interface_valid(const struct cusbd_interface *me);

/**
 * @brief Override for @ref v_cusbd_descriptor_wTotalLength(). 
 * See @ref v_cusbd_descriptor_wTotalLength().
 */
static uint16_t o_interface_wTotalLength(const struct cusbd_interface *me);

/**
 * @brief Override for @ref v_cusbd_descriptor_default_state_in(). 
 * See @ref v_cusbd_descriptor_default_state_in().
 */
static enum cusbd_request_status o_interface_default_state_in(struct cusbd_interface *me,
                                                              const struct cusbd_request *request,
                                                              void *buf,
                                                              size_t len);

/**
 * @brief Override for @ref v_cusbd_descriptor_default_state_out(). 
 * See @ref v_cusbd_descriptor_default_state_out().
 */
static enum cusbd_request_status o_interface_default_state_out(struct cusbd_interface *me,
                                                               const struct cusbd_request *request);

/**
 * @brief Override for @ref v_cusbd_descriptor_address_state_in(). 
 * See @ref v_cusbd_descriptor_address_state_in().
 */
static enum cusbd_request_status o_interface_address_state_in(struct cusbd_interface *me,
                                                              const struct cusbd_request *request,
                                                              void *buf,
                                                              size_t len);

/**
 * @brief Override for @ref v_cusbd_descriptor_address_state_out(). 
 * See @ref v_cusbd_descriptor_address_state_out().
 */
static enum cusbd_request_status o_interface_address_state_out(struct cusbd_interface *me,
                                                               const struct cusbd_request *request);

/**
 * @brief Override for @ref v_cusbd_descriptor_configured_state_in(). 
 * See @ref v_cusbd_descriptor_configured_state_in().
 */
static enum cusbd_request_status o_interface_configured_state_in(struct cusbd_interface *me,
                                                                 const struct cusbd_request *request,
                                                                 void *buf,
                                                                 size_t len);

/**
 * @brief Override for @ref v_cusbd_descriptor_configured_state_out(). 
 * See @ref v_cusbd_descriptor_configured_state_out().
 */
static enum cusbd_request_status o_interface_configured_state_out(struct cusbd_interface *me,
                                                                  const struct cusbd_request *request);

/*------------------------------------------------------------*/
/*---- STATIC FUNCTION DECLARATIONS - ALTERNATE INTERFACE ----*/
/*------------------------------------------------------------*/

/**
 * @brief Override for @ref v_cusbd_descriptor_valid(). 
 * See @ref v_cusbd_descriptor_valid().
 */
static bool o_alternate_interface_valid(const struct cusbd_alternate_interface *me);

/**
 * @brief Override for @ref v_cusbd_descriptor_wTotalLength(). 
 * See @ref v_cusbd_descriptor_wTotalLength().
 */
static uint16_t o_alternate_interface_wTotalLength(const struct cusbd_alternate_interface *me);

/**
 * @brief Override for @ref v_cusbd_descriptor_default_state_in(). 
 * See @ref v_cusbd_descriptor_default_state_in().
 */
static enum cusbd_request_status o_alternate_interface_default_state_in(struct cusbd_alternate_interface *me,
                                                                        const struct cusbd_request *request,
                                                                        void *buf,
                                                                        size_t len);

/**
 * @brief Override for @ref v_cusbd_descriptor_default_state_out(). 
 * See @ref v_cusbd_descriptor_default_state_out().
 */
static enum cusbd_request_status o_alternate_interface_default_state_out(struct cusbd_alternate_interface *me,
                                                                         const struct cusbd_request *request);

/**
 * @brief Override for @ref v_cusbd_descriptor_address_state_in(). 
 * See @ref v_cusbd_descriptor_address_state_in().
 */
static enum cusbd_request_status o_alternate_interface_address_state_in(struct cusbd_alternate_interface *me,
                                                                        const struct cusbd_request *request,
                                                                        void *buf,
                                                                        size_t len);

/**
 * @brief Override for @ref v_cusbd_descriptor_address_state_out(). 
 * See @ref v_cusbd_descriptor_address_state_out().
 */
static enum cusbd_request_status o_alternate_interface_address_state_out(struct cusbd_alternate_interface *me,
                                                                         const struct cusbd_request *request);

/**
 * @brief Override for @ref v_cusbd_descriptor_configured_state_in(). 
 * See @ref v_cusbd_descriptor_configured_state_in().
 */
static enum cusbd_request_status o_alternate_interface_configured_state_in(struct cusbd_alternate_interface *me,
                                                                           const struct cusbd_request *request,
                                                                           void *buf,
                                                                           size_t len);

/**
 * @brief Override for @ref v_cusbd_descriptor_configured_state_out(). 
 * See @ref v_cusbd_descriptor_configured_state_out().
 */
static enum cusbd_request_status o_alternate_interface_configured_state_out(struct cusbd_alternate_interface *me,
                                                                            const struct cusbd_request *request);

/*------------------------------------------------------------*/
/*----------- STATIC FUNCTION DEFINITIONS - COMMON -----------*/
/*------------------------------------------------------------*/

static bool interface_descriptor_valid(const struct cusbd_interface_descriptor *descriptor)
{
    bool status = false;
    ECU_RUNTIME_ASSERT( (descriptor) );

#pragma message("TODO: Figure out bInterfaceClass, bInterfaceSubClass, and bInterfaceProtocol.")
    /* Do not assert bInterfaceNumber, bAlternateSetting, bNumEndpoints, and 
    iInterface since these are automatically updated when descriptors are added
    to the device. */
    if (descriptor->bLength == sizeof(struct cusbd_interface_descriptor) &&
        descriptor->bDescriptorType == (uint8_t)CUSBD_DESCRIPTOR_TYPE_INTERFACE)
    {
        status = true;
    }

    return status;
}

/*------------------------------------------------------------*/
/*---------- STATIC FUNCTION DEFINITIONS - INTERFACE ---------*/
/*------------------------------------------------------------*/

static bool o_interface_valid(const struct cusbd_interface *me)
{
    bool status = false;
    ECU_RUNTIME_ASSERT( (me) );

    /* Alternate setting should always be 0 for normal CUSBD interface descriptors. */
    if ((interface_descriptor_valid(&me->descriptor)) && 
        (me->descriptor.bAlternateSetting == 0))
    {
        status = true;
    }

    return status;
}

static uint16_t o_interface_wTotalLength(const struct cusbd_interface *me)
{
    static const uint16_t len = (uint16_t)(sizeof(struct cusbd_interface_descriptor));
    ECU_RUNTIME_ASSERT( (me) );
    return len;
}

static enum cusbd_request_status o_interface_default_state_in(struct cusbd_interface *me,
                                                              const struct cusbd_request *request,
                                                              void *buf,
                                                              size_t len)
{
    enum cusbd_request_status status = CUSBD_REQUEST_STATUS_IGNORED;
    ECU_RUNTIME_ASSERT( (me && request && buf) );
    ECU_RUNTIME_ASSERT( (cusbd_request_direction(request) == CUSBD_REQUEST_DIRECTION_DEVICE_TO_HOST) );
    (void)len;

    if ((cusbd_request_recipient(request) == CUSBD_REQUEST_RECIPIENT_INTERFACE) &&
        (cusbd_request_type(request) == CUSBD_REQUEST_TYPE_STANDARD))
    {
        switch (cusbd_request_value(request))
        {
            case CUSBD_REQUEST_VALUE_GET_INTERFACE:
            {
                /* Invalid request since in default state. */
                status = CUSBD_REQUEST_STATUS_STALL;
                break;
            }

            case CUSBD_REQUEST_VALUE_GET_STATUS:
            {
                /* Invalid request since in default state. */
                status = CUSBD_REQUEST_STATUS_STALL;
                break;
            }

            default:
            {
                /* Ignore all other requests. */
                break;
            }
        }
    }

    return status;
}

static enum cusbd_request_status o_interface_default_state_out(struct cusbd_interface *me,
                                                               const struct cusbd_request *request)
{
    enum cusbd_request_status status = CUSBD_REQUEST_STATUS_IGNORED;
    ECU_RUNTIME_ASSERT( (me && request) );
    ECU_RUNTIME_ASSERT( (cusbd_request_direction(request) == CUSBD_REQUEST_DIRECTION_HOST_TO_DEVICE) );

    if ((cusbd_request_recipient(request) == CUSBD_REQUEST_RECIPIENT_INTERFACE) &&
        (cusbd_request_type(request) == CUSBD_REQUEST_TYPE_STANDARD))
    {
        switch (cusbd_request_value(request))
        {
            case CUSBD_REQUEST_VALUE_CLEAR_FEATURE:
            {
                /* Invalid request since in default state. */
                status = CUSBD_REQUEST_STATUS_STALL;
                break;
            }

            case CUSBD_REQUEST_VALUE_SET_FEATURE:
            {
                /* Invalid request since in default state. */
                status = CUSBD_REQUEST_STATUS_STALL;
                break;
            }

            case CUSBD_REQUEST_VALUE_SET_INTERFACE:
            {
                /* Invalid request since in default state. */
                status = CUSBD_REQUEST_STATUS_STALL;
                break;
            }

            default:
            {
                /* Ignore all other requests. */
                break;
            }
        }
    }

    return status;
}

static enum cusbd_request_status o_interface_address_state_in(struct cusbd_interface *me,
                                                              const struct cusbd_request *request,
                                                              void *buf,
                                                              size_t len)
{
    enum cusbd_request_status status = CUSBD_REQUEST_STATUS_IGNORED;
    ECU_RUNTIME_ASSERT( (me && request && buf) );
    ECU_RUNTIME_ASSERT( (cusbd_request_direction(request) == CUSBD_REQUEST_DIRECTION_DEVICE_TO_HOST) );
    (void)len;

    if ((cusbd_request_recipient(request) == CUSBD_REQUEST_RECIPIENT_INTERFACE) &&
        (cusbd_request_type(request) == CUSBD_REQUEST_TYPE_STANDARD))
    {
        switch (cusbd_request_value(request))
        {
            case CUSBD_REQUEST_VALUE_GET_INTERFACE:
            {
                /* Invalid request since in address state. */
                status = CUSBD_REQUEST_STATUS_STALL;
                break;
            }

            case CUSBD_REQUEST_VALUE_GET_STATUS:
            {
                /* Invalid request since in address state. */
                status = CUSBD_REQUEST_STATUS_STALL;
                break;
            }

            default:
            {
                /* Ignore all other requests. */
                break;
            }
        }
    }

    return status;
}

static enum cusbd_request_status o_interface_address_state_out(struct cusbd_interface *me,
                                                               const struct cusbd_request *request)
{
    enum cusbd_request_status status = CUSBD_REQUEST_STATUS_IGNORED;
    ECU_RUNTIME_ASSERT( (me && request) );
    ECU_RUNTIME_ASSERT( (cusbd_request_direction(request) == CUSBD_REQUEST_DIRECTION_HOST_TO_DEVICE) );

    if ((cusbd_request_recipient(request) == CUSBD_REQUEST_RECIPIENT_INTERFACE) &&
        (cusbd_request_type(request) == CUSBD_REQUEST_TYPE_STANDARD))
    {
        switch (cusbd_request_value(request))
        {
            case CUSBD_REQUEST_VALUE_CLEAR_FEATURE:
            {
                /* Invalid request since in address state. */
                status = CUSBD_REQUEST_STATUS_STALL;
                break;
            }

            case CUSBD_REQUEST_VALUE_SET_FEATURE:
            {
                /* Invalid request since in address state. */
                status = CUSBD_REQUEST_STATUS_STALL;
                break;
            }

            case CUSBD_REQUEST_VALUE_SET_INTERFACE:
            {
                /* Invalid request since in address state. */
                status = CUSBD_REQUEST_STATUS_STALL;
                break;
            }

            default:
            {
                /* Ignore all other requests. */
                break;
            }
        }
    }

    return status;
}

static enum cusbd_request_status o_interface_configured_state_in(struct cusbd_interface *me,
                                                                 const struct cusbd_request *request,
                                                                 void *buf,
                                                                 size_t len)
{
    static const uint16_t INTERFACE_GET_STATUS = 0; /* A GET_STATUS() request to an interface always returns 0. */
    enum cusbd_request_status status = CUSBD_REQUEST_STATUS_IGNORED;
    ECU_RUNTIME_ASSERT( (me && request && buf) );
    ECU_RUNTIME_ASSERT( (cusbd_request_direction(request) == CUSBD_REQUEST_DIRECTION_DEVICE_TO_HOST) );
    uint16_t wValue = 0;
    uint16_t wIndex = 0;
    uint16_t wLength = 0;

    if ((cusbd_request_recipient(request) == CUSBD_REQUEST_RECIPIENT_INTERFACE) &&
        (cusbd_request_type(request) == CUSBD_REQUEST_TYPE_STANDARD))
    {
        wValue = ECU_LE16_TO_CPU_RUNTIME(request->wValue);
        wIndex = ECU_LE16_TO_CPU_RUNTIME(request->wIndex);
        wLength = ECU_LE16_TO_CPU_RUNTIME(request->wLength);

        switch (cusbd_request_value(request))
        {
            case CUSBD_REQUEST_VALUE_GET_INTERFACE:
            {
                if (wValue == 0 && wLength == 1)
                {
                    if (wIndex == me->descriptor.bInterfaceNumber)
                    {
                        ECU_STATIC_ASSERT( (sizeof(me->alternate_setting) == (size_t)1), 
                                            "alternate setting must be 1 byte unless USB spec has changed." );
                        ECU_RUNTIME_ASSERT( (len >= sizeof(me->alternate_setting)) );
                        memcpy(buf, &me->alternate_setting, sizeof(me->alternate_setting));
                        status = CUSBD_REQUEST_STATUS_ACK;
                    }
                }
                else
                {
                    /* Setup packet in invalid format. */
                    status = CUSBD_REQUEST_STATUS_STALL;
                }
                break;
            }

            case CUSBD_REQUEST_VALUE_GET_STATUS:
            {
                if (wValue == 0 && wLength == 2)
                {
                    if (wIndex == me->descriptor.bInterfaceNumber)
                    {
                        ECU_STATIC_ASSERT( (sizeof(INTERFACE_GET_STATUS) == (size_t)2),
                                            "Data must be 2 bytes unless USB spec has changed." );
                        ECU_RUNTIME_ASSERT( (len >= sizeof(INTERFACE_GET_STATUS)) );
                        memcpy(buf, INTERFACE_GET_STATUS, sizeof(INTERFACE_GET_STATUS));
                        status = CUSBD_REQUEST_STATUS_ACK;
                    }
                }
                else
                {
                    /* Setup packet in invalid format. */
                    status = CUSBD_REQUEST_STATUS_STALL;
                }
                break;
            }

            default:
            {
                /* Ignore all other requests. */
                break;
            }
        }
    }

    return status;
}

static enum cusbd_request_status o_interface_configured_state_out(struct cusbd_interface *me,
                                                                  const struct cusbd_request *request)
{
    enum cusbd_request_status status = CUSBD_REQUEST_STATUS_IGNORED;
    uint16_t wValue = 0;
    uint16_t wIndex = 0;
    uint16_t wLength = 0;
    ECU_RUNTIME_ASSERT( (me && request) );
    ECU_RUNTIME_ASSERT( (cusbd_request_direction(request) == CUSBD_REQUEST_DIRECTION_HOST_TO_DEVICE) );

    if ((cusbd_request_recipient(request) == CUSBD_REQUEST_RECIPIENT_INTERFACE) &&
        (cusbd_request_type(request) == CUSBD_REQUEST_TYPE_STANDARD))
    {
        wValue = ECU_LE16_TO_CPU_RUNTIME(request->wValue);
        wIndex = ECU_LE16_TO_CPU_RUNTIME(request->wIndex);
        wLength = ECU_LE16_TO_CPU_RUNTIME(request->wLength);

        switch (cusbd_request_value(request))
        {
            case CUSBD_REQUEST_VALUE_CLEAR_FEATURE:
            {
                /* No features exist for standard interfaces so return STALL. */
                status = CUSBD_REQUEST_STATUS_STALL;
                break;
            }

            case CUSBD_REQUEST_VALUE_SET_FEATURE:
            {
                /* No features exist for standard interfaces so return STALL. */
                status = CUSBD_REQUEST_STATUS_STALL;
                break;
            }

            case CUSBD_REQUEST_VALUE_SET_INTERFACE:
            {
                if (wLength == 0)
                {
                    if (wIndex == me->descriptor.bInterfaceNumber)
                    {
                        status = CUSBD_REQUEST_STATUS_STALL; /* STALL if alt setting not found. */
                        struct ecu_ntnode_child_citerator citerator;

                        ECU_NTNODE_CONST_CHILD_FOR_EACH(n, &citerator, &me->base.ntnode)
                        {
                            struct cusbd_descriptor *base = ECU_NTNODE_GET_CONST_ENTRY(n, struct cusbd_descriptor, ntnode);

                            if (cusbd_descriptor_type(base) == CUSBD_DESCRIPTOR_TYPE_INTERFACE)
                            {
                                const struct cusbd_alternate_interface *alternate_interface = (const struct cusbd_alternate_interface *)base;
                                ECU_RUNTIME_ASSERT( (me->descriptor.bInterfaceNumber == alternate_interface->descriptor.bInterfaceNumber) );

                                if (wValue == alternate_interface->descriptor.bAlternateSetting)
                                {
                                    me->alternate_setting = (uint8_t)wValue;
                                    status = CUSBD_REQUEST_STATUS_ACK;
                                    break;
                                }
                            }
                        }
                    }
                }
                else
                {
                    /* Setup packet in invalid format. */
                    status = CUSBD_REQUEST_STATUS_STALL;
                }
                break;
            }

            default:
            {
                /* Ignore all other requests. */
                break;
            }
        }
    }

    return status;
}

/*------------------------------------------------------------*/
/*----- STATIC FUNCTION DEFINITIONS - ALTERNATE INTERFACE ----*/
/*------------------------------------------------------------*/

static bool o_alternate_interface_valid(const struct cusbd_alternate_interface *me)
{
    bool status = false;
    ECU_RUNTIME_ASSERT( (me) );
    return (interface_descriptor_valid(&me->descriptor));
}

static uint16_t o_interface_wTotalLength(const struct cusbd_interface *me)
{
    static const uint16_t len = (uint16_t)(sizeof(struct cusbd_interface_descriptor));
    ECU_RUNTIME_ASSERT( (me) );
    return len;
}

static enum cusbd_request_status o_alternate_interface_default_state_in(struct cusbd_alternate_interface *me,
                                                                        const struct cusbd_request *request,
                                                                        void *buf,
                                                                        size_t len)
{
    ECU_RUNTIME_ASSERT( (me && request && buf) );
    ECU_RUNTIME_ASSERT( (cusbd_request_direction(request) == CUSBD_REQUEST_DIRECTION_DEVICE_TO_HOST) );
    (void)len;
    return CUSBD_REQUEST_STATUS_IGNORED;
}

static enum cusbd_request_status o_alternate_interface_default_state_out(struct cusbd_alternate_interface *me,
                                                                         const struct cusbd_request *request)
{
    ECU_RUNTIME_ASSERT( (me && request) );
    ECU_RUNTIME_ASSERT( (cusbd_request_direction(request) == CUSBD_REQUEST_DIRECTION_HOST_TO_DEVICE) );
    return CUSBD_REQUEST_STATUS_IGNORED;
}

static enum cusbd_request_status o_alternate_interface_address_state_in(struct cusbd_alternate_interface *me,
                                                                        const struct cusbd_request *request,
                                                                        void *buf,
                                                                        size_t len)
{
    ECU_RUNTIME_ASSERT( (me && request && buf) );
    ECU_RUNTIME_ASSERT( (cusbd_request_direction(request) == CUSBD_REQUEST_DIRECTION_DEVICE_TO_HOST) );
    (void)len;
    return CUSBD_REQUEST_STATUS_IGNORED;
}

static enum cusbd_request_status o_alternate_interface_address_state_out(struct cusbd_alternate_interface *me,
                                                                         const struct cusbd_request *request)
{
    ECU_RUNTIME_ASSERT( (me && request) );
    ECU_RUNTIME_ASSERT( (cusbd_request_direction(request) == CUSBD_REQUEST_DIRECTION_HOST_TO_DEVICE) );
    return CUSBD_REQUEST_STATUS_IGNORED;
}

static enum cusbd_request_status o_alternate_interface_configured_state_in(struct cusbd_alternate_interface *me,
                                                                           const struct cusbd_request *request,
                                                                           void *buf,
                                                                           size_t len)
{
    ECU_RUNTIME_ASSERT( (me && request && buf) );
    ECU_RUNTIME_ASSERT( (cusbd_request_direction(request) == CUSBD_REQUEST_DIRECTION_DEVICE_TO_HOST) );
    (void)len;
    return CUSBD_REQUEST_STATUS_IGNORED;
}

static enum cusbd_request_status o_alternate_interface_configured_state_out(struct cusbd_alternate_interface *me,
                                                                            const struct cusbd_request *request)
{
    ECU_RUNTIME_ASSERT( (me && request) );
    ECU_RUNTIME_ASSERT( (cusbd_request_direction(request) == CUSBD_REQUEST_DIRECTION_HOST_TO_DEVICE) );
    return CUSBD_REQUEST_STATUS_IGNORED;
}

/*------------------------------------------------------------*/
/*---------------------- STATIC ASSERTS ----------------------*/
/*------------------------------------------------------------*/

ECU_STATIC_ASSERT( (sizeof(struct cusbd_interface_descriptor) == (size_t)9),
                    "Interface descriptor is 9 bytes." );

ECU_STATIC_ASSERT( (CUSBD_DESCRIPTOR_IS_BASEOF(base, struct cusbd_interface)),
                    "cusbd_interface must inherit cusbd_descriptor." );

ECU_STATIC_ASSERT( (CUSBD_DESCRIPTOR_IS_BASEOF(base, struct cusbd_alternate_interface)),
                    "cusbd_alternate_interface must inherit cusbd_descriptor." );

/*------------------------------------------------------------*/
/*-------------- CUSBD INTERFACE MEMBER FUNCTIONS ------------*/
/*------------------------------------------------------------*/

void cusbd_interface_ctor(struct cusbd_interface *me,
                          const struct cusbd_interface_descriptor *descriptor)
{
    static const struct cusbd_descriptor_vtable vtable = CUSBD_DESCRIPTOR_VTABLE_CTOR(
        &o_interface_valid, &o_interface_wTotalLength, &o_interface_default_state_in, 
        &o_interface_default_state_out, &o_interface_address_state_in, &o_interface_address_state_out,
        &o_interface_configured_state_in, &o_interface_configured_state_out
    );

    ECU_RUNTIME_ASSERT( (me && descriptor) );
    ECU_RUNTIME_ASSERT( (interface_descriptor_valid(descriptor)) );

    cusbd_descriptor_ctor(CUSBD_DESCRIPTOR_BASE_CAST(me), CUSBD_DESCRIPTOR_TYPE_INTERFACE);
    me->base.vptr = &vtable; /* MUST be AFTER cusbd_descriptor_ctor(). */
    memcpy(&me->descriptor, descriptor, sizeof(struct cusbd_interface_descriptor));
    me->alternate_setting = 0;
    ecu_dlist_ctor(&me->strings);
}

void cusbd_interface_add_alternate_interface(struct cusbd_interface *me,
                                             struct cusbd_alternate_interface *alternate_interface)
{
    uint16_t wTotalLength = 0;
    struct ecu_ntnode *parent = (struct ecu_ntnode *)0;
    struct cusbd_configuration *configuration = (struct cusbd_configuration *)0;
    struct cusbd_descriptor *base = (struct cusbd_descriptor *)0;
    struct cusbd_alternate_interface *alt_interface_node = (struct cusbd_alternate_interface *)0;
    struct ecu_ntnode_child_iterator iterator;
    ECU_RUNTIME_ASSERT( (me && alternate_interface) );
    ECU_RUNTIME_ASSERT( (v_cusbd_descriptor_valid(CUSBD_DESCRIPTOR_CONST_BASE_CAST(me))) );
    ECU_RUNTIME_ASSERT( (v_cusbd_descriptor_valid(CUSBD_DESCRIPTOR_CONST_BASE_CAST(alternate_interface))) );

    /* Update bInterfaceNumber. */
    alternate_interface->descriptor.bInterfaceNumber = me->descriptor.bInterfaceNumber;

    /* Update bAlternateSetting while inserting alternate interface into descriptor tree. 
    ECU library asserts if node is already within a tree. */
    alternate_interface->descriptor.bAlternateSetting = 1; /* Alternate settings start at 1. */
    ECU_NTNODE_CHILD_FOR_EACH(n, &iterator, &me->base.ntnode)
    {
        base = ECU_NTNODE_GET_ENTRY(n, struct cusbd_descriptor, ntnode);

        if (cusbd_descriptor_type(base) == CUSBD_DESCRIPTOR_TYPE_INTERFACE)
        {
            alt_interface_node = (struct cusbd_alternate_interface *)base;
            
            if (alt_interface_node->descriptor.bAlternateSetting < alternate_interface->descriptor.bAlternateSetting)
            {
                ecu_ntnode_insert_before(n, &alternate_interface->base.ntnode);
                break;
            }
            else
            {
                alternate_interface->descriptor.bAlternateSetting++;
            }
        }
    }

    if (!ecu_ntnode_in_subtree(&alternate_interface->base.ntnode))
    {
        ecu_ntnode_push_back(&me->base.ntnode, &alternate_interface->base.ntnode);
    }

    /* If the interface is attached to a configuration, update wTotalLength. */
    parent = ecu_ntnode_parent(&me->base.ntnode);
    if (parent)
    {
        base = ECU_NTNODE_GET_ENTRY(&parent, struct cusbd_descriptor, ntnode);
        ECU_RUNTIME_ASSERT( (cusbd_descriptor_type(&base) == CUSBD_DESCRIPTOR_TYPE_CONFIGURATION) );
        configuration = (struct cusbd_configuration *)base;
        wTotalLength = cusbd_configuration_size(configuration);
        configuration->descriptor.wTotalLength = ECU_CPU_TO_LE16_RUNTIME(wTotalLength);
    }

    /* iInterface is updated when device starts. */
}

void cusbd_interface_add_endpoint(struct cusbd_interface *me,
                                  struct cusbd_endpoint *endpoint)
{
    const struct cusbd_descriptor *base = (const struct cusbd_descriptor *)0;
    const struct cusbd_endpoint *e = (const struct cusbd_endpoint *)0;
    struct ecu_ntnode_child_citerator citerator;
    ECU_RUNTIME_ASSERT( (me && endpoint) );
    ECU_RUNTIME_ASSERT( (v_cusbd_descriptor_valid(CUSBD_DESCRIPTOR_CONST_BASE_CAST(me))) );
    ECU_RUNTIME_ASSERT( (v_cusbd_descriptor_valid(CUSBD_DESCRIPTOR_CONST_BASE_CAST(endpoint))) );

    /* Update bNumEndpoints. Also verify no endpoints are duplicated. I.e. cannot 
    have multiple endpoint1 INs attached to the same interface descriptor. */
    me->descriptor.bNumEndpoints = 0;
    ECU_NTNODE_CONST_CHILD_FOR_EACH(n, &citerator, &me->base.ntnode)
    {
        base = ECU_NTNODE_GET_CONST_ENTRY(n, struct cusbd_descriptor, ntnode);

        if (cusbd_descriptor_type(base) == CUSBD_DESCRIPTOR_TYPE_ENDPOINT)
        {
            e = (const struct cusb_endpoint *)base;
            ECU_RUNTIME_ASSERT( (e->descriptor.bEndpointAddress != endpoint->descriptor.bEndpointAddress) );
            me->descriptor.bNumEndpoints++;
        }
    }

    /* Add endpoint to descriptor tree. ECU library asserts if node already in tree. */
    ecu_ntnode_push_front(&me->base.ntnode, &endpoint->base.ntnode);
}

void cusbd_interface_add_string(struct cusbd_interface *me,
                                struct cusbd_string *string)
{
    /* Add string to list. ECU library asserts if node is already within a list. */
    ECU_RUNTIME_ASSERT( (me && string) );
    ECU_RUNTIME_ASSERT( (v_cusbd_descriptor_valid(CUSBD_DESCRIPTOR_CONST_BASE_CAST(me))) );
    ECU_RUNTIME_ASSERT( (cusbd_string_valid(string)) );
    ecu_dlist_push_back(&me->strings, &string->dnode);
}

/*------------------------------------------------------------*/
/*--------- CUSBD ALTERNATE INTERFACE MEMBER FUNCTIONS -------*/
/*------------------------------------------------------------*/

void cusbd_alternate_interface_ctor(struct cusbd_alternate_interface *me,
                                    const struct cusbd_interface_descriptor *descriptor)
{
    static const struct cusbd_descriptor_vtable vtable = CUSBD_DESCRIPTOR_VTABLE_CTOR(
        &o_alternate_interface_valid, &o_alternate_interface_wTotalLength, &o_alternate_interface_default_state_in, 
        &o_alternate_interface_default_state_out, &o_alternate_interface_address_state_in, &o_alternate_interface_address_state_out,
        &o_alternate_interface_configured_state_in, &o_alternate_interface_configured_state_out
    );

    ECU_RUNTIME_ASSERT( (me && descriptor) );
    ECU_RUNTIME_ASSERT( (interface_descriptor_valid(descriptor)) );

    cusbd_descriptor_ctor(CUSBD_DESCRIPTOR_BASE_CAST(me), CUSBD_DESCRIPTOR_TYPE_INTERFACE);
    me->base.vptr = &vtable; /* MUST be AFTER cusbd_descriptor_ctor(). */
    memcpy(&me->descriptor, descriptor, sizeof(struct cusbd_interface_descriptor));
    ecu_dlist_ctor(&me->strings);
}

void cusbd_alternate_interface_add_endpoint(struct cusbd_alternate_interface *me,
                                            struct cusbd_endpoint *endpoint)
{
    const struct cusbd_descriptor *base = (const struct cusbd_descriptor *)0;
    const struct cusbd_endpoint *e = (const struct cusbd_endpoint *)0;
    struct ecu_ntnode_child_citerator citerator;
    ECU_RUNTIME_ASSERT( (me && endpoint) );
    ECU_RUNTIME_ASSERT( (v_cusbd_descriptor_valid(CUSBD_DESCRIPTOR_CONST_BASE_CAST(me))) );
    ECU_RUNTIME_ASSERT( (v_cusbd_descriptor_valid(CUSBD_DESCRIPTOR_CONST_BASE_CAST(endpoint))) );

    /* Update bNumEndpoints. Also verify no endpoints are duplicated. I.e. cannot 
    have multiple endpoint1 INs attached to the same interface descriptor. */
    me->descriptor.bNumEndpoints = 0;
    ECU_NTNODE_CONST_CHILD_FOR_EACH(n, &citerator, &me->base.ntnode)
    {
        base = ECU_NTNODE_GET_CONST_ENTRY(n, struct cusbd_descriptor, ntnode);

        if (cusbd_descriptor_type(base) == CUSBD_DESCRIPTOR_TYPE_ENDPOINT)
        {
            e = (const struct cusb_endpoint *)base;
            ECU_RUNTIME_ASSERT( (e->descriptor.bEndpointAddress != endpoint->descriptor.bEndpointAddress) );
            me->descriptor.bNumEndpoints++;
        }
    }

    /* Add endpoint to descriptor tree. ECU library asserts if node already in tree. */
    ecu_ntnode_push_front(&me->base.ntnode, &endpoint->base.ntnode);
}

void cusbd_alternate_interface_add_string(struct cusbd_alternate_interface *me,
                                          struct cusbd_string *string)
{
    /* Add string to list. ECU library asserts if node is already within a list. */
    ECU_RUNTIME_ASSERT( (me && string) );
    ECU_RUNTIME_ASSERT( (v_cusbd_descriptor_valid(CUSBD_DESCRIPTOR_CONST_BASE_CAST(me))) );
    ECU_RUNTIME_ASSERT( (cusbd_string_valid(string)) );
    ecu_dlist_push_back(&me->strings, &string->dnode);
}
