/**
 * @file
 * @brief See @ref visitor.h.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-06-15
 * @copyright Copyright (c) 2025
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* Translation unit. */
#include "cusbd/visitor/visitor.h"

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

ECU_ASSERT_DEFINE_NAME("cusbd/visitor/visitor.c")

/*------------------------------------------------------------*/
/*------------------- VISITOR MEMBER FUNCTIONS ---------------*/
/*------------------------------------------------------------*/

void cusbd_visitor_ctor(struct cusbd_visitor *me)
{
    ECU_RUNTIME_ASSERT( (me) );

    /* Set pure virtual functions to NULL so NULL assertion fires if directly called. */
    static const struct cusbd_visitor_vtable vtable = {0};
    me->vptr = &vtable;
}

void cusbd_cvisitor_ctor(struct cusbd_cvisitor *me)
{
    ECU_RUNTIME_ASSERT( (me) );

    /* Set pure virtual functions to NULL so NULL assertion fires if directly called. */
    static const struct cusbd_cvisitor_vtable vtable = {0};
    me->vptr = &vtable;
}

void v_cusbd_visitor_visit_device(struct cusbd_visitor *me, struct cusbd *node)
{
    ECU_RUNTIME_ASSERT( (me && node) );
    ECU_RUNTIME_ASSERT( (me->vptr) );
    ECU_RUNTIME_ASSERT( (me->vptr->visit_device) );
    (*me->vptr->visit_device)(me, node);
}

void v_cusbd_cvisitor_visit_device(struct cusbd_cvisitor *me, const struct cusbd *node)
{
    ECU_RUNTIME_ASSERT( (me && node) );
    ECU_RUNTIME_ASSERT( (me->vptr) );
    ECU_RUNTIME_ASSERT( (me->vptr->visit_device) );
    (*me->vptr->visit_device)(me, node);
}

void v_cusbd_visitor_visit_configuration(struct cusbd_visitor *me, struct cusbd_configuration *node)
{
    ECU_RUNTIME_ASSERT( (me && node) );
    ECU_RUNTIME_ASSERT( (me->vptr) );
    ECU_RUNTIME_ASSERT( (me->vptr->visit_configuration) );
    (*me->vptr->visit_configuration)(me, node);
}

void v_cusbd_cvisitor_visit_configuration(struct cusbd_cvisitor *me, const struct cusbd_configuration *node)
{
    ECU_RUNTIME_ASSERT( (me && node) );
    ECU_RUNTIME_ASSERT( (me->vptr) );
    ECU_RUNTIME_ASSERT( (me->vptr->visit_configuration) );
    (*me->vptr->visit_configuration)(me, node);
}

void v_cusbd_visitor_visit_interface(struct cusbd_visitor *me, struct cusbd_interface *node)
{
    ECU_RUNTIME_ASSERT( (me && node) );
    ECU_RUNTIME_ASSERT( (me->vptr) );
    ECU_RUNTIME_ASSERT( (me->vptr->visit_interface) );
    (*me->vptr->visit_interface)(me, node);
}

void v_cusbd_cvisitor_visit_interface(struct cusbd_cvisitor *me, const struct cusbd_interface *node)
{
    ECU_RUNTIME_ASSERT( (me && node) );
    ECU_RUNTIME_ASSERT( (me->vptr) );
    ECU_RUNTIME_ASSERT( (me->vptr->visit_interface) );
    (*me->vptr->visit_interface)(me, node);
}

void v_cusbd_visitor_visit_alternate_interface(struct cusbd_visitor *me, struct cusbd_alternate_interface *node)
{
    ECU_RUNTIME_ASSERT( (me && node) );
    ECU_RUNTIME_ASSERT( (me->vptr) );
    ECU_RUNTIME_ASSERT( (me->vptr->visit_alternate_interface) );
    (*me->vptr->visit_alternate_interface)(me, node);
}

void v_cusbd_cvisitor_visit_alternate_interface(struct cusbd_cvisitor *me, const struct cusbd_alternate_interface *node)
{
    ECU_RUNTIME_ASSERT( (me && node) );
    ECU_RUNTIME_ASSERT( (me->vptr) );
    ECU_RUNTIME_ASSERT( (me->vptr->visit_alternate_interface) );
    (*me->vptr->visit_alternate_interface)(me, node);
}

void v_cusbd_visitor_visit_endpoint(struct cusbd_visitor *me, struct cusbd_endpoint *node)
{
    ECU_RUNTIME_ASSERT( (me && node) );
    ECU_RUNTIME_ASSERT( (me->vptr) );
    ECU_RUNTIME_ASSERT( (me->vptr->visit_endpoint) );
    (*me->vptr->visit_endpoint)(me, node);
}

void v_cusbd_cvisitor_visit_endpoint(struct cusbd_cvisitor *me, const struct cusbd_endpoint *node)
{
    ECU_RUNTIME_ASSERT( (me && node) );
    ECU_RUNTIME_ASSERT( (me->vptr) );
    ECU_RUNTIME_ASSERT( (me->vptr->visit_endpoint) );
    (*me->vptr->visit_endpoint)(me, node);
}
