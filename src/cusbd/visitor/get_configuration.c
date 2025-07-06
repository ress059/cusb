/**
 * @file
 * @brief See @ref get_configuration.c.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-07-04
 * @copyright Copyright (c) 2025
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* Translation unit. */
#include "cusbd/visitor/get_configuration.h"

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

ECU_ASSERT_DEFINE_NAME("cusbd/visitor/get_configuration.c")

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/

/// @brief Override of @ref v_cusbd_cvisitor_visit_device().
static void o_visit_device(struct cusbd_visitor_get_configuration *me, const struct cusbd *node);

/// @brief Override of @ref v_cusbd_visitor_visit_configuration().
static void o_visit_configuration(struct cusbd_visitor_get_configuration *me, const struct cusbd_configuration *node);

/// @brief Override of @ref v_cusbd_cvisitor_visit_interface().
static void o_visit_interface(struct cusbd_visitor_get_configuration *me, const struct cusbd_interface *node);

/// @brief Override of @ref v_cusbd_cvisitor_visit_alternate_interface().
static void o_visit_alternate_interface(struct cusbd_visitor_get_configuration *me, const struct cusbd_alternate_interface *node);

/// @brief Override of @ref v_cusbd_cvisitor_visit_endpoint().
static void o_visit_endpoint(struct cusbd_visitor_get_configuration *me, const struct cusbd_endpoint *node);

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
/*------------------------------------------------------------*/

static void o_visit_device(struct cusbd_visitor_get_configuration *me, const struct cusbd *node)
{
    /* Do not assert valid() since that is centralized in the v_cusbd_descriptor_caccept() function. */
    ECU_RUNTIME_ASSERT( (me && node) );
    /* Implementation responsible for creating correct visitor. */
    ECU_RUNTIME_ASSERT( (cusbd_request_value(&me->request) == CUSBD_REQUEST_VALUE_GET_CONFIGURATION) );

    if (cusbd_request_direction(&me->request) != CUSBD_REQUEST_DIRECTION_IN ||
        cusbd_request_type(&me->request) != CUSBD_REQUEST_TYPE_STANDARD ||
        cusbd_request_recipient(&me->request) != CUSBD_REQUEST_RECIPIENT_DEVICE ||
        cusbd_request_w_value(&me->request) != 0 ||
        cusbd_request_w_index(&me->request) != 0 ||
        cusbd_request_w_length(&me->request) != 1)
    {
        /* GET_CONFIGURATION() is IN and standard request. wValue and wIndex
        always 0 for GET_CONFIGURATION(). wLength always 1 for GET_CONFIGURATION(). */
        cusbd_request_set_status(&me->request, CUSBD_REQUEST_STATUS_STALL);
    }
    else
    {
        switch (cusbd_request_state(&me->request))
        {
            case CUSBD_REQUEST_STATE_DEFAULT_STATE:
            {
                /* GET_CONFIGURATION() not supported in default state. */
                cusbd_request_set_status(&me->request, CUSBD_REQUEST_STATUS_STALL);
                break;
            }

            case CUSBD_REQUEST_STATE_ADDRESS_STATE:
            {
                /* Something has gone wrong if device's configuration is not 0 while in address state. */
                ECU_RUNTIME_ASSERT( (node->configuration_value == 0) );
                me->configuration_value = 0;
                cusbd_request_set_status(&me->request, CUSBD_REQUEST_STATUS_ACK);
                break;
            }

            case CUSBD_REQUEST_STATE_CONFIGURED_STATE:
            {
                /* Something has gone wrong if device's configuration == 0 while in configured state. */
                ECU_RUNTIME_ASSERT( (node->configuration_value != 0) );

                /* Verify the currently active configuration is in the descriptor tree. */
                struct ecu_ntnode_child_citerator citer;
                bool in_tree = false;

                ECU_NTNODE_CONST_CHILD_FOR_EACH(n, &citer, &node->base.ntnode)
                {
                    const struct cusbd_descriptor *descriptor = ECU_NTNODE_GET_CONST_ENTRY(n, struct cusbd_descriptor, ntnode);

                    if (cusbd_descriptor_type(descriptor) == CUSBD_CONFIGURATION_BDESCRIPTORTYPE)
                    {
                        const struct cusbd_configuration *configuration = (const struct cusbd_configuration *)descriptor;
                        if (configuration->descriptor.bConfigurationValue == node->configuration_value)
                        {
                            in_tree = true;
                            break;
                        }
                    }
                }

                /* Something has gone wrong if the currently active configuration is not in the descriptor tree. */
                ECU_RUNTIME_ASSERT( (in_tree) );

                /* OK. Send back active configuration. */
                me->configuration_value = node->configuration_value;
                cusbd_request_set_status(&me->request, CUSBD_REQUEST_STATUS_ACK);
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

static void o_visit_configuration(struct cusbd_visitor_get_configuration *me, const struct cusbd_configuration *node)
{
    /* GET_CONFIGURATION() does not apply to configuration descriptors. */
    (void)me;
    (void)node;
}

static void o_visit_interface(struct cusbd_visitor_get_configuration *me, const struct cusbd_interface *node)
{
    /* GET_CONFIGURATION() does not apply to interface descriptors. */
    (void)me;
    (void)node;
}

static void o_visit_alternate_interface(struct cusbd_visitor_get_configuration *me, const struct cusbd_alternate_interface *node)
{
    /* GET_CONFIGURATION() does not apply to alternate interfaces. */
    (void)me;
    (void)node;
}

static void o_visit_endpoint(struct cusbd_visitor_get_configuration *me, const struct cusbd_endpoint *node)
{
    /* GET_CONFIGURATION() does not apply to endpoint descriptors. */
    (void)me;
    (void)node;
}

/*------------------------------------------------------------*/
/*---------------------- STATIC ASSERTS ----------------------*/
/*------------------------------------------------------------*/

ECU_STATIC_ASSERT( (CUSBD_VISITOR_IS_BASEOF(base, struct cusbd_visitor_get_configuration)),
                    "cusbd_visitor_get_configuration must inherit cusbd_cvisitor." );

/*------------------------------------------------------------*/
/*------------------- VISITOR MEMBER FUNCTIONS ---------------*/
/*------------------------------------------------------------*/

void cusbd_visitor_get_configuration_ctor(struct cusbd_visitor_get_configuration *me,
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
    me->configuration_value = 0;
}
