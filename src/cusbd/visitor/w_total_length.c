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
    ECU_RUNTIME_ASSERT( (me && node) );
    ECU_RUNTIME_ASSERT( (me->wTotalLength <= UINT16_MAX) );
    /* Device descriptor not apart of wTotalLength calculation. */
}

static void o_visit_configuration(struct cusbd_visitor_w_total_length *me, const struct cusbd_configuration *node)
{
    ECU_RUNTIME_ASSERT( (me && node) );
    ECU_RUNTIME_ASSERT( (me->wTotalLength <= UINT16_MAX) );
    me->wTotalLength += sizeof(struct cusbd_configuration_descriptor);
    ECU_RUNTIME_ASSERT( (me->wTotalLength <= UINT16_MAX) );
}

static void o_visit_interface(struct cusbd_visitor_w_total_length *me, const struct cusbd_interface *node)
{
    ECU_RUNTIME_ASSERT( (me && node) );
    ECU_RUNTIME_ASSERT( (me->wTotalLength <= UINT16_MAX) );
    me->wTotalLength += sizeof(struct cusbd_interface_descriptor);
    ECU_RUNTIME_ASSERT( (me->wTotalLength <= UINT16_MAX) );
}

static void o_visit_alternate_interface(struct cusbd_visitor_w_total_length *me, const struct cusbd_alternate_interface *node)
{
    ECU_RUNTIME_ASSERT( (me && node) );
    ECU_RUNTIME_ASSERT( (me->wTotalLength <= UINT16_MAX) );
    me->wTotalLength += sizeof(struct cusbd_interface_descriptor);
    ECU_RUNTIME_ASSERT( (me->wTotalLength <= UINT16_MAX) );
}

static void o_visit_endpoint(struct cusbd_visitor_w_total_length *me, const struct cusbd_endpoint *node)
{
    ECU_RUNTIME_ASSERT( (me && node) );
    ECU_RUNTIME_ASSERT( (me->wTotalLength <= UINT16_MAX) );
    me->wTotalLength += sizeof(struct cusbd_endpoint_descriptor);
    ECU_RUNTIME_ASSERT( (me->wTotalLength <= UINT16_MAX) );
}

/*------------------------------------------------------------*/
/*---------------------- STATIC ASSERTS ----------------------*/
/*------------------------------------------------------------*/

ECU_STATIC_ASSERT( (CUSBD_VISITOR_IS_BASEOF(base, struct cusbd_visitor_w_total_length)),
                    "cusbd_visitor_w_total_length must inherit cusbd_visitor." );

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
    me->base.vptr = &vtable;
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
