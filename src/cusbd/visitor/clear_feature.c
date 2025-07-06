/**
 * @file
 * @brief See @ref clear_feature.h.
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
#include "cusbd/visitor/clear_feature.h"

/* CUSB. */
#include "cusbd/cusbd.h"
#include "cusbd/configuration.h"
#include "cusbd/interface.h"
#include "cusbd/endpoint.h"

/* ECU. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_NAME("cusbd/visitor/clear_feature.c")

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/

/// @brief Override of @ref v_cusbd_visitor_visit_device().
static void o_visit_device(struct cusbd_visitor_clear_feature *me, struct cusbd *node);

/// @brief Override of @ref v_cusbd_visitor_visit_configuration().
static void o_visit_configuration(struct cusbd_visitor_clear_feature *me, struct cusbd_configuration *node);

/// @brief Override of @ref v_cusbd_visitor_visit_interface().
static void o_visit_interface(struct cusbd_visitor_clear_feature *me, struct cusbd_interface *node);

/// @brief Override of @ref v_cusbd_visitor_visit_alternate_interface().
static void o_visit_alternate_interface(struct cusbd_visitor_clear_feature *me, struct cusbd_alternate_interface *node);

/// @brief Override of @ref v_cusbd_visitor_visit_endpoint().
static void o_visit_endpoint(struct cusbd_visitor_clear_feature *me, struct cusbd_endpoint *node);

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
/*------------------------------------------------------------*/

static void o_visit_device(struct cusbd_visitor_clear_feature *me, struct cusbd *node)
{
    /* Do not assert valid() since that is centralized in the v_cusbd_descriptor_caccept() function. */
    ECU_RUNTIME_ASSERT( (me && node) );
    /* Implementation responsible for creating correct visitor. */
    ECU_RUNTIME_ASSERT( (cusbd_request_value(&me->request) == CUSBD_REQUEST_VALUE_CLEAR_FEATURE) );

#pragma message("TODO: Clean up. May put direction, type, w_value, etc checks first.")
    if (cusbd_request_recipient(&me->request) == CUSBD_REQUEST_RECIPIENT_DEVICE)
    {
        switch (cusbd_request_state(&me->request))
        {
            case CUSBD_REQUEST_STATE_DEFAULT_STATE:
            {
                /* CLEAR_FEATURE() not supported in default state. */
                cusbd_request_set_status(&me->request, CUSBD_REQUEST_STATUS_STALL);
                break;
            }

            case CUSBD_REQUEST_STATE_ADDRESS_STATE:
            {
                /* CLEAR_FEATURE() is OUT and standard request,
                only supported feature selector for device is remote wakeup, wLength always 0
                for CLEAR_FEATURE(), and wIndex should be 0 since not addressing interface or endpoint. */
                if (cusbd_request_direction(&me->request) != CUSBD_REQUEST_DIRECTION_OUT ||
                    cusbd_request_type(&me->request) != CUSBD_REQUEST_TYPE_STANDARD ||
                    cusbd_request_w_value(&me->request) != CUSBD_FEATURE_DEVICE_REMOTE_WAKEUP ||
                    cusbd_request_w_length(&me->request) != 0 ||
                    cusbd_request_w_index(&me->request) != 0)
                {
                    cusbd_request_set_status(&me->request, CUSBD_REQUEST_STATUS_STALL);
                }
                else
                {
                    node->remote_wakeup = false;
                    cusbd_request_set_status(&me->request, CUSBD_REQUEST_STATUS_ACK);
                }
                break;
            }

            case CUSBD_REQUEST_STATE_CONFIGURED_STATE:
            {
                /* CLEAR_FEATURE() is OUT and standard request,
                Only supported feature selector for device is remote wakeup, wLength always 0
                for CLEAR_FEATURE(), and wIndex should be 0 since not addressing interface or endpoint. */
                if (cusbd_request_direction(&me->request) != CUSBD_REQUEST_DIRECTION_OUT ||
                    cusbd_request_type(&me->request) != CUSBD_REQUEST_TYPE_STANDARD ||
                    cusbd_request_w_value(&me->request) != CUSBD_FEATURE_DEVICE_REMOTE_WAKEUP ||
                    cusbd_request_w_length(&me->request) != 0 ||
                    cusbd_request_w_index(&me->request) != 0)
                {
                    cusbd_request_set_status(&me->request, CUSBD_REQUEST_STATUS_STALL);
                }
                else
                {
                    node->remote_wakeup = false;
                    cusbd_request_set_status(&me->request, CUSBD_REQUEST_STATUS_ACK);
                }
                break;
            }

            default:
            {
                /* Unconstructed request. */
                ECU_RUNTIME_ASSERT( (false) );
                break;
            }
        }
    }
}

static void o_visit_configuration(struct cusbd_visitor_clear_feature *me, struct cusbd_configuration *node)
{
    /* CLEAR_FEATURE() does not apply to configuration descriptors. */
    (void)me;
    (void)node;
}

static void o_visit_interface(struct cusbd_visitor_clear_feature *me, struct cusbd_interface *node)
{
    /* Do not assert valid() since that is centralized in the v_cusbd_descriptor_accept() function. */
    ECU_RUNTIME_ASSERT( (me && node) );
    /* Implementation responsible for creating correct visitor. */
    ECU_RUNTIME_ASSERT( (cusbd_request_value(&me->request) == CUSBD_REQUEST_VALUE_CLEAR_FEATURE) );

    if (cusbd_request_recipient(&me->request) == CUSBD_REQUEST_RECIPIENT_INTERFACE &&
        cusbd_request_w_index(&me->request) == node->descriptor.bInterfaceNumber)
    {
        switch (cusbd_request_state(&me->request))
        {
            case CUSBD_REQUEST_STATE_DEFAULT_STATE:
            {
                /* CLEAR_FEATURE() not supported in default state. */
                cusbd_request_set_status(&me->request, CUSBD_REQUEST_STATUS_STALL);
                break;
            }

            case CUSBD_REQUEST_STATE_ADDRESS_STATE:
            {
                /* CLEAR_FEATURE() addressed to interfaces not supported in address state. */
                cusbd_request_set_status(&me->request, CUSBD_REQUEST_STATUS_STALL);
                break;
            }

            case CUSBD_REQUEST_STATE_CONFIGURED_STATE:
            {
                /* Interfaces currently have no feature selectors. */
                cusbd_request_set_status(&me->request, CUSBD_REQUEST_STATUS_STALL);
                break;
            }

            default:
            {
                /* Unconstructed request. */
                ECU_RUNTIME_ASSERT( (false) );
                break;
            }
        }
    }
}

static void o_visit_alternate_interface(struct cusbd_visitor_clear_feature *me, struct cusbd_alternate_interface *node)
{
    /* CLEAR_FEATURE() does not apply to alternate interfaces. */
    (void)me;
    (void)node;
}

static void o_visit_endpoint(struct cusbd_visitor_clear_feature *me, struct cusbd_endpoint *node)
{
    /* Do not assert valid() since that is centralized in the v_cusbd_descriptor_accept() function. */
    ECU_RUNTIME_ASSERT( (me && node) );
    /* Implementation responsible for creating correct visitor. */
    ECU_RUNTIME_ASSERT( (cusbd_request_value(&me->request) == CUSBD_REQUEST_VALUE_CLEAR_FEATURE) );
#pragma message("TODO: Clean up. May put direction, type, w_value, etc checks first.")
    if (cusbd_request_recipient(&me->request) == CUSBD_REQUEST_RECIPIENT_ENDPOINT &&
        cusbd_request_w_index(&me->request) == cusbd_endpoint_number(node))
    {
        switch (cusbd_request_state(&me->request))
        {
            case CUSBD_REQUEST_STATE_DEFAULT_STATE:
            {
                /* CLEAR_FEATURE() not supported in default state. */
                cusbd_request_set_status(&me->request, CUSBD_REQUEST_STATUS_STALL);
                break;
            }

            case CUSBD_REQUEST_STATE_ADDRESS_STATE:
            {
                /* CLEAR_FEATURE() can only be addressed to endpoint0 in address state.
                Do not support halting of ep0 since datasheet encourages not to. */
                cusbd_request_set_status(&me->request, CUSBD_REQUEST_STATUS_STALL);
                break;
            }

            case CUSBD_REQUEST_STATE_CONFIGURED_STATE:
            {
                if (cusbd_request_direction(&me->request) != CUSBD_REQUEST_DIRECTION_OUT ||
                    cusbd_request_type(&me->request) != CUSBD_REQUEST_TYPE_STANDARD ||
                    cusbd_request_w_value(&me->request) != CUSBD_FEATURE_ENDPOINT_HALT ||
                    cusbd_request_w_index(&me->request) == 0 ||
                    cusbd_request_w_length(&me->request) != 0)
                {
                    /* CLEAR_FEATURE() is OUT and standard request,
                    halt is only feature supported by endpoints, wIndex cannot be 0 since currently 
                    not supporting halt on ep0, and wLength always 0 for CLEAR_FEATURE(). */
                    cusbd_request_set_status(&me->request, CUSBD_REQUEST_STATUS_STALL);
                }
                else if (cusbd_endpoint_transfer_type(node) == CUSBD_ENDPOINT_TRANSFER_TYPE_BULK ||
                         cusbd_endpoint_transfer_type(node) == CUSBD_ENDPOINT_TRANSFER_TYPE_INTERRUPT)
                {
                    /* Currently halting is only supported for bulk and interrupt endpoints since it is required. */
                #pragma message("TODO: From datasheet..For endpoints using data toggle, a ClearFeature(ENDPOINT_HALT) \
                    request always results in the data toggle being reinitialized to DATA0.")
                    node->halt = false;
                    cusbd_request_set_status(&me->request, CUSBD_REQUEST_STATUS_ACK);
                }
                else
                {
                    /* Currently halting is only supported for bulk and interrupt endpoints since it is required. */
                    cusbd_request_set_status(&me->request, CUSBD_REQUEST_STATUS_STALL);
                }
                break;
            }

            default:
            {
                /* Unconstructed request. */
                ECU_RUNTIME_ASSERT( (false) );
                break;
            }
        }
    }
}

/*------------------------------------------------------------*/
/*---------------------- STATIC ASSERTS ----------------------*/
/*------------------------------------------------------------*/

ECU_STATIC_ASSERT( (CUSBD_VISITOR_IS_BASEOF(base, struct cusbd_visitor_clear_feature)),
                    "cusbd_visitor_clear_feature must inherit cusbd_visitor." );

/*------------------------------------------------------------*/
/*------------------- VISITOR MEMBER FUNCTIONS ---------------*/
/*------------------------------------------------------------*/

void cusbd_visitor_clear_feature_ctor(struct cusbd_visitor_clear_feature *me,
                                      const struct cusbd_setup_packet_rx_event *e,
                                      enum cusbd_request_state state)
{
    ECU_RUNTIME_ASSERT( (me && e) );
    ECU_RUNTIME_ASSERT( (state > CUSBD_REQUEST_STATE_RESERVED && state < CUSBD_REQUEST_STATE_COUNT) );
    
    static const struct cusbd_visitor_vtable vtable = CUSBD_VISITOR_VTABLE_CTOR(
        &o_visit_device,
        &o_visit_configuration,
        &o_visit_interface,
        &o_visit_alternate_interface,
        &o_visit_endpoint
    );

    cusbd_visitor_ctor(&me->base);
    cusbd_request_ctor(&me->request, e, state);
    me->base.vptr = &vtable; /* MUST be AFTER cusbd_visitor_ctor(). */
}
