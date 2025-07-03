/**
 * @file
 * @brief CUSBD descriptor base class. Used to provide a common
 * interface between many different types of descriptors. Should 
 * not be used directly by end user.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-05-17
 * @copyright Copyright (c) 2025
 */

#ifndef CUSBD_DESCRIPTOR_H_
#define CUSBD_DESCRIPTOR_H_

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ECU. */
#include "ecu/ntree.h"

/*------------------------------------------------------------*/
/*---------------------- DEFINES AND MACROS ------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Verifies, at compile-time, the descriptor
 * correctly inherits @ref cusbd_descriptor base class. 
 * Returns true if correctly inherited. False otherwise.
 * 
 * @param base_ Name of @ref cusbd_descriptor <b>member</b> 
 * within derived descriptor's type.
 * @param derived_ Derived descriptor type to check.
 */
#define CUSBD_DESCRIPTOR_IS_BASEOF(base_, derived_) \
    ((bool)(offsetof(derived_, base_) == (size_t)0))

/**
 * @brief Upcasts derived descriptor pointer into @ref cusbd_descriptor
 * base class pointer. This macro encapsulates the cast
 * and allows derived descriptors to be passed into base 
 * class functions defined in this module.
 * 
 * @param me_ Pointer to derived descriptor. This must inherit
 * @ref cusbd_descriptor base class. This must be pointer to
 * non-const.
 */
#define CUSBD_DESCRIPTOR_BASE_CAST(x_) \
    ((struct cusbd_descriptor *)(x_))

/**
 * @brief Const-qualified version of @ref CUSBD_DESCRIPTOR_BASE_CAST().
 * Upcasts derived descriptor pointer into @ref cusbd_descriptor
 * base class pointer. This macro encapsulates the cast
 * and allows derived descriptors to be passed into base 
 * class functions defined in this module.
 * 
 * @param me_ Pointer to derived descriptor. This must inherit
 * @ref cusbd_descriptor base class.
 */
#define CUSBD_DESCRIPTOR_CONST_BASE_CAST(x_) \
    ((const struct cusbd_descriptor *)(x_))

/**
 * @brief Creates a virtual table at compile-time. Supplied
 * functions taking in a derived descriptor pointer are upcasted
 * to functions taking in an @ref cusbd_descriptor base class
 * pointer. The results of these casts are assigned to
 * members of @ref cusbd_descriptor_vtable. This macro encapsulates
 * all casts and member initializations of @ref cusbd_descriptor_vtable.
 * Example usage:
 * @code{.c}
 * static const struct cusbd_descriptor_vtable vtable = CUSBD_DESCRIPTOR_VTABLE_CTOR(
 *      &accept, ...
 * );
 * @endcode
 * 
 * @param accept_ See @ref v_cusbd_descriptor_accept().
 * @param caccept_ See @ref v_cusbd_descriptor_caccept().
 * @param valid_ See @ref v_cusbd_descriptor_valid().
 */
#define CUSBD_DESCRIPTOR_VTABLE_CTOR(accept_, caccept_, valid_)                                     \
    {                                                                                               \
        .accept = (void(*)(struct cusbd_descriptor *, struct cusbd_visitor *))(accept_),            \
        .caccept = (void(*)(const struct cusbd_descriptor *, struct cusbd_cvisitor *))(caccept_),   \
        .valid = (bool(*)(const struct cusbd_descriptor *))(valid_)                                 \
    }

/*------------------------------------------------------------*/
/*----------------- CUSBD DESCRIPTOR BASE CLASS --------------*/
/*------------------------------------------------------------*/

/* Forward declare for vtable. */
struct cusbd_descriptor;
struct cusbd_visitor;
struct cusbd_cvisitor;

/**
 * @brief Virtual table for @ref cusbd_descriptor base
 * class.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_descriptor_vtable
{
    /// @brief See @ref v_cusbd_descriptor_accept().
    void (*accept)(struct cusbd_descriptor *me, struct cusbd_visitor *visitor);

    /// @brief See @ref v_cusbd_descriptor_caccept().
    void (*caccept)(const struct cusbd_descriptor *me, struct cusbd_cvisitor *visitor);

    /// @brief See @ref v_cusbd_descriptor_valid().
    bool (*valid)(const struct cusbd_descriptor *me);
};

/**
 * @brief CUSBD descriptor base class. Used to provide
 * a common interface between many types of descriptors.
 * All concrete descriptors (device, configuration, 
 * interface, etc.) inherit this base class.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_descriptor
{
    /// @brief Virtual functions.
    const struct cusbd_descriptor_vtable *vptr;

    /// @brief N-ary tree node. All descriptors besides strings
    /// represented as nodes in a tree.
    struct ecu_ntnode ntnode;
};

/*------------------------------------------------------------*/
/*------- CUSBD DESCRIPTOR BASE CLASS MEMBER FUNCTIONS -------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name CUSBD Base Descriptor Constructors
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @brief Descriptor base class constructor.
 * 
 * @warning This cannot be called on an active 
 * descriptor. Doing so is undefined behavior.
 * 
 * @param me Descriptor to construct.
 * @param bDescriptorType Value of bDescriptorType.
 */
extern void cusbd_descriptor_ctor(struct cusbd_descriptor *me, uint8_t bDescriptorType);
/**@}*/

/**
 * @name CUSBD Base Descriptor Member Functions
 */
/**@{*/
/**
 * @pre @p me is a concrete descriptor that has been constructed 
 * and inherits @ref cusbd_descriptor.
 * @pre @p visitor is a concrete visitor that has been constructed 
 * and inherits @ref cusbd_visitor.
 * @brief Accepts concrete visitor on @p me, and on all descriptors
 * in @p me's subtree.
 * 
 * @param me Concrete descriptor that visitor algorithm runs on.
 * @param visitor Concrete visitor containing specific algorithm
 * to run.
 */
extern void cusbd_descriptor_accept(struct cusbd_descriptor *me, struct cusbd_visitor *visitor);

/**
 * @pre @p me is a concrete descriptor that has been constructed 
 * and inherits @ref cusbd_descriptor.
 * @pre @p visitor is a concrete visitor that has been constructed 
 * and inherits @ref cusbd_cvisitor.
 * @brief Const-qualified version of @ref cusbd_descriptor_accept().
 * Accepts concrete visitor on @p me, and on all descriptors
 * in @p me's subtree.
 * 
 * @param me Concrete descriptor that visitor algorithm runs on.
 * @param visitor Concrete visitor containing specific algorithm
 * to run.
 */
extern void cusbd_descriptor_caccept(const struct cusbd_descriptor *me, struct cusbd_cvisitor *visitor);

/**
 * @pre @p me previously constructed via @ref cusbd_descriptor_ctor().
 * @brief Returns descriptor's bDescriptorType.
 * 
 * @param me Descriptor to check.
 */
extern uint8_t cusbd_descriptor_type(const struct cusbd_descriptor *me);

/**
 * @pre @p me previously constructed via @ref cusbd_descriptor_ctor().
 * @brief Returns true if the supplied descriptor contains
 * valid data and was properly constructed via @ref cusbd_descriptor_ctor(). 
 * False otherwise.
 * 
 * @param me Descriptor to check.
 */
extern bool cusbd_descriptor_valid(const struct cusbd_descriptor *me);

/**
 * @pre @p me is a derived class that has been constructed 
 * and inherits @ref cusbd_descriptor.
 * @pre @p visitor is a concrete visitor that has been 
 * constructed and inherits @ref cusbd_visitor.
 * @brief Accepts concrete visitor on a single descriptor, @p me.
 * 
 * @warning Virtual call. Must be overridden.
 * 
 * @param me Concrete descriptor that visitor algorithm runs on.
 * @param visitor Concrete visitor containing specific algorithm
 * to run.
 */
extern void v_cusbd_descriptor_accept(struct cusbd_descriptor *me, struct cusbd_visitor *visitor);

/**
 * @pre @p me is a concrete descriptor that has been constructed 
 * and inherits @ref cusbd_descriptor.
 * @pre @p visitor is a concrete visitor that has been constructed 
 * and inherits @ref cusbd_cvisitor.
 * @brief Const-qualified version of @ref v_cusbd_descriptor_accept(). 
 * Accepts concrete visitor on a single descriptor, @p me.
 * 
 * @warning Virtual call. Must be overridden.
 * 
 * @param me Concrete descriptor that visitor algorithm runs on.
 * @param visitor Concrete visitor containing specific algorithm
 * to run.
 */
extern void v_cusbd_descriptor_caccept(const struct cusbd_descriptor *me, struct cusbd_cvisitor *visitor);

/**
 * @pre @p me is a concrete descriptor that has been constructed 
 * and inherits @ref cusbd_descriptor.
 * @brief Returns true if the supplied descriptor contains
 * valid data and was properly constructed. False otherwise.
 * 
 * @warning Virtual call. Must be overridden.
 * 
 * @param me Concrete descriptor to check.
 */
extern bool v_cusbd_descriptor_valid(const struct cusbd_descriptor *me);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* CUSBD_DESCRIPTOR_H_ */
