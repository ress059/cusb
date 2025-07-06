/**
 * @file
 * @brief See @ref w_total_length.h.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-06-28
 * @copyright Copyright (c) 2025
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* Translation unit. */
#include "cusbd/visitor/w_total_length.h"

/* CUSB. */
#include "cusbd/configuration.h"
#include "cusbd/interface.h"
#include "cusbd/endpoint.h"

/* ECU. */
#include "ecu/asserter.h"
#include "ecu/endian.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_NAME("cusbd/visitor/w_total_length.c")

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/

/// @brief Override of @ref v_cusbd_cvisitor_visit_device().
static void o_visit_device(struct cusbd_visitor_w_total_length *me, const struct cusbd *node);

/// @brief Override of @ref v_cusbd_cvisitor_visit_configuration().
static void o_visit_configuration(struct cusbd_visitor_w_total_length *me, const struct cusbd_configuration *node);

/// @brief Override of @ref v_cusbd_cvisitor_visit_interface().
static void o_visit_interface(struct cusbd_visitor_w_total_length *me, const struct cusbd_interface *node);

/// @brief Override of @ref v_cusbd_cvisitor_visit_alternate_interface().
static void o_visit_alternate_interface(struct cusbd_visitor_w_total_length *me, const struct cusbd_alternate_interface *node);

/// @brief Override of @ref v_cusbd_cvisitor_visit_endpoint().
static void o_visit_endpoint(struct cusbd_visitor_w_total_length *me, const struct cusbd_endpoint *node);

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
/*------------------------------------------------------------*/

static void o_visit_device(struct cusbd_visitor_w_total_length *me, const struct cusbd *node)
{
    /* Device descriptor not apart of wTotalLength calculation. 
    Do not assert valid() since that is centralized in the v_cusbd_descriptor_caccept() function. */
    (void)me;
    (void)node;
}

static void o_visit_configuration(struct cusbd_visitor_w_total_length *me, const struct cusbd_configuration *node)
{
    /* Do not assert valid() since that is centralized in the v_cusbd_descriptor_caccept() function. */
    ECU_RUNTIME_ASSERT( (me && node) );
    ECU_RUNTIME_ASSERT( (me->wTotalLength <= UINT16_MAX) );
    /* Do not reset cound by doing me->wTotalLength = ... since we do not know the order of iteration. */
    me->wTotalLength += sizeof(struct cusbd_configuration_descriptor);
    ECU_RUNTIME_ASSERT( (me->wTotalLength <= UINT16_MAX) );
}

static void o_visit_interface(struct cusbd_visitor_w_total_length *me, const struct cusbd_interface *node)
{
    /* Do not assert valid() since that is centralized in the v_cusbd_descriptor_caccept() function. */
    ECU_RUNTIME_ASSERT( (me && node) );
    ECU_RUNTIME_ASSERT( (me->wTotalLength <= UINT16_MAX) );
    me->wTotalLength += sizeof(struct cusbd_interface_descriptor);
    ECU_RUNTIME_ASSERT( (me->wTotalLength <= UINT16_MAX) );
}

static void o_visit_alternate_interface(struct cusbd_visitor_w_total_length *me, const struct cusbd_alternate_interface *node)
{
    /* Do not assert valid() since that is centralized in the v_cusbd_descriptor_caccept() function. */
    ECU_RUNTIME_ASSERT( (me && node) );
    ECU_RUNTIME_ASSERT( (me->wTotalLength <= UINT16_MAX) );
    me->wTotalLength += sizeof(struct cusbd_interface_descriptor);
    ECU_RUNTIME_ASSERT( (me->wTotalLength <= UINT16_MAX) );
}

static void o_visit_endpoint(struct cusbd_visitor_w_total_length *me, const struct cusbd_endpoint *node)
{
    /* Do not assert valid() since that is centralized in the v_cusbd_descriptor_caccept() function. */
    ECU_RUNTIME_ASSERT( (me && node) );
    ECU_RUNTIME_ASSERT( (me->wTotalLength <= UINT16_MAX) );
    me->wTotalLength += sizeof(struct cusbd_endpoint_descriptor);
    ECU_RUNTIME_ASSERT( (me->wTotalLength <= UINT16_MAX) );
}

/*------------------------------------------------------------*/
/*---------------------- STATIC ASSERTS ----------------------*/
/*------------------------------------------------------------*/

ECU_STATIC_ASSERT( (CUSBD_VISITOR_IS_BASEOF(base, struct cusbd_visitor_w_total_length)),
                    "cusbd_visitor_w_total_length must inherit cusbd_cvisitor." );

/*------------------------------------------------------------*/
/*------------------- VISITOR MEMBER FUNCTIONS ---------------*/
/*------------------------------------------------------------*/

void cusbd_visitor_w_total_length_ctor(struct cusbd_visitor_w_total_length *me)
{
    ECU_RUNTIME_ASSERT( (me) );

    static const struct cusbd_cvisitor_vtable vtable = CUSBD_CVISITOR_VTABLE_CTOR(
        &o_visit_device,
        &o_visit_configuration,
        &o_visit_interface,
        &o_visit_alternate_interface,
        &o_visit_endpoint
    );

    cusbd_cvisitor_ctor(&me->base);
    me->base.vptr = &vtable; /* MUST be AFTER cusbd_cvisitor_ctor(). */
    me->wTotalLength = 0;
}

uint16_t cusbd_visitor_w_total_length_value(const struct cusbd_visitor_w_total_length *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (me->wTotalLength <= UINT16_MAX) );
    return ((uint16_t)me->wTotalLength);
}

uint16_t cusbd_visitor_w_total_length_value_le(const struct cusbd_visitor_w_total_length *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (me->wTotalLength <= UINT16_MAX) );
    return (ECU_CPU_TO_LE16_RUNTIME((uint16_t)me->wTotalLength));
}
