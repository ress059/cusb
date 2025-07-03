/**
 * @file
 * @brief Base visitor class. Should not be used directly 
 * by end user.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-06-15
 * @copyright Copyright (c) 2025
 */

#ifndef CUSBD_VISITOR_H_
#define CUSBD_VISITOR_H_

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <stdbool.h>
#include <stddef.h>

/*------------------------------------------------------------*/
/*---------------------- DEFINES AND MACROS ------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Verifies, at compile-time, the concrete visitor
 * correctly inherits @ref cusbd_visitor base class. 
 * Returns true if correctly inherited. False otherwise.
 * 
 * @param base_ Name of @ref cusbd_visitor <b>member</b> 
 * within concrete visitor type.
 * @param derived_ Concrete visitor type to check.
 */
#define CUSBD_VISITOR_IS_BASEOF(base_, derived_) \
    ((bool)(offsetof(derived_, base_) == (size_t)0))

/**
 * @brief Upcasts concrete visitor pointer into
 * a @ref cusbd_visitor base class pointer. This macro 
 * encapsulates the cast. It allows the application to 
 * accept concrete visitors while using the base class
 * interface.
 * 
 * @param me_ Pointer to derived visitor. This must inherit
 * @ref cusbd_visitor base class and be a pointer to non-const.
 */
#define CUSBD_VISITOR_BASE_CAST(x_) \
    ((struct cusbd_visitor *)(x_))

/**
 * @brief Upcasts concrete visitor pointer into
 * a @ref cusbd_cvisitor base class pointer. This macro 
 * encapsulates the cast. It allows the application to 
 * accept concrete visitors while using the base class
 * interface.
 * 
 * @param me_ Pointer to derived visitor. This must inherit
 * @ref cusbd_cvisitor base class and be a pointer to non-const.
 */
#define CUSBD_CVISITOR_BASE_CAST(x_) \
    ((struct cusbd_cvisitor *)(x_))

/**
 * @brief Creates a virtual table at compile-time. Supplied
 * functions taking in a derived visitor pointer are upcasted
 * to functions taking in a @ref cusbd_visitor base class
 * pointer. The results of these casts are assigned to
 * members of @ref cusbd_visitor_vtable. This macro encapsulates
 * all casts and member initializations of @ref cusbd_visitor_vtable.
 * Example usage:
 * @code{.c}
 * static const struct cusbd_visitor_vtable vtable = CUSBD_VISITOR_VTABLE_CTOR(
 *      &visit_device, ....
 * );
 * @endcode
 * 
 * @param visit_device_ See @ref v_cusbd_visitor_visit_device().
 * @param visit_configuration_ See @ref v_cusbd_visitor_visit_configuration().
 * @param visit_interface_ See @ref v_cusbd_visitor_visit_interface().
 * @param visit_alternate_interface_ See @ref v_cusbd_visitor_visit_alternate_interface().
 * @param visit_endpoint_ See @ref v_cusbd_visitor_visit_endpoint().
 */
#define CUSBD_VISITOR_VTABLE_CTOR(visit_device_,                                                                                            \
                                  visit_configuration_,                                                                                     \
                                  visit_interface_,                                                                                         \
                                  visit_alternate_interface_,                                                                               \
                                  visit_endpoint_)                                                                                          \
    {                                                                                                                                       \
        .visit_device = (void (*)(struct cusbd_visitor *, struct cusbd *))(visit_device_),                                                  \
        .visit_configuration = (void (*)(struct cusbd_visitor *, struct cusbd_configuration *))(visit_configuration_),                      \
        .visit_interface = (void (*)(struct cusbd_visitor *, struct cusbd_interface *))(visit_interface_),                                  \
        .visit_alternate_interface = (void (*)(struct cusbd_visitor *, struct cusbd_alternate_interface *))(visit_alternate_interface_),    \
        .visit_endpoint = (void (*)(struct cusbd_visitor *, struct cusbd_endpoint *))(visit_endpoint_)                                      \
    }

/**
 * @brief Creates a virtual table at compile-time. Supplied
 * functions taking in a derived visitor pointer are upcasted
 * to functions taking in a @ref cusbd_cvisitor base class
 * pointer. The results of these casts are assigned to
 * members of @ref cusbd_cvisitor_vtable. This macro encapsulates
 * all casts and member initializations of @ref cusbd_cvisitor_vtable.
 * Example usage:
 * @code{.c}
 * static const struct cusbd_cvisitor_vtable vtable = CUSBD_CVISITOR_VTABLE_CTOR(
 *      &visit_device, ....
 * );
 * @endcode
 * 
 * @param visit_device_ See @ref v_cusbd_cvisitor_visit_device().
 * @param visit_configuration_ See @ref v_cusbd_cvisitor_visit_configuration().
 * @param visit_interface_ See @ref v_cusbd_cvisitor_visit_interface().
 * @param visit_alternate_interface_ See @ref v_cusbd_cvisitor_visit_alternate_interface().
 * @param visit_endpoint_ See @ref v_cusbd_cvisitor_visit_endpoint().
 */
#define CUSBD_CVISITOR_VTABLE_CTOR(visit_device_,                                                                                                   \
                                   visit_configuration_,                                                                                            \
                                   visit_interface_,                                                                                                \
                                   visit_alternate_interface_,                                                                                      \
                                   visit_endpoint_)                                                                                                 \
    {                                                                                                                                               \
        .visit_device = (void (*)(struct cusbd_cvisitor *, const struct cusbd *))(visit_device_),                                                   \
        .visit_configuration = (void (*)(struct cusbd_cvisitor *, const struct cusbd_configuration *))(visit_configuration_),                       \
        .visit_interface = (void (*)(struct cusbd_cvisitor *, const struct cusbd_interface *))(visit_interface_),                                   \
        .visit_alternate_interface = (void (*)(struct cusbd_cvisitor *, const struct cusbd_alternate_interface *))(visit_alternate_interface_),     \
        .visit_endpoint = (void (*)(struct cusbd_cvisitor *, const struct cusbd_endpoint *))(visit_endpoint_)                                       \
    }

/*------------------------------------------------------------*/
/*------------------------- VISITOR --------------------------*/
/*------------------------------------------------------------*/

/* Forward declarations for vtable. */
struct cusbd_visitor;
struct cusbd_cvisitor;
struct cusbd;
struct cusbd_configuration;
struct cusbd_interface;
struct cusbd_alternate_interface;
struct cusbd_endpoint;

/**
 * @brief Virtual table for @ref cusbd_visitor base
 * class. Support for new class-specific descriptors
 * would be added here.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_visitor_vtable
{
    /// @brief See @ref v_cusbd_visitor_visit_device().
    void (*visit_device)(struct cusbd_visitor *me, struct cusbd *node);

    /// @brief See @ref v_cusbd_visitor_visit_configuration().
    void (*visit_configuration)(struct cusbd_visitor *me, struct cusbd_configuration *node);

    /// @brief See @ref v_cusbd_visitor_visit_interface().
    void (*visit_interface)(struct cusbd_visitor *me, struct cusbd_interface *node);

    /// @brief See @ref v_cusbd_visitor_visit_alternate_interface().
    void (*visit_alternate_interface)(struct cusbd_visitor *me, struct cusbd_alternate_interface *node);

    /// @brief See @ref v_cusbd_visitor_visit_endpoint().
    void (*visit_endpoint)(struct cusbd_visitor *me, struct cusbd_endpoint *node);
};

/**
 * @brief Virtual table for @ref cusbd_cvisitor base
 * class. Support for new class-specific descriptors
 * would be added here.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_cvisitor_vtable
{
    /// @brief See @ref v_cusbd_cvisitor_visit_device().
    void (*visit_device)(struct cusbd_cvisitor *me, const struct cusbd *node);

    /// @brief See @ref v_cusbd_cvisitor_visit_configuration().
    void (*visit_configuration)(struct cusbd_cvisitor *me, const struct cusbd_configuration *node);

    /// @brief See @ref v_cusbd_cvisitor_visit_interface().
    void (*visit_interface)(struct cusbd_cvisitor *me, const struct cusbd_interface *node);

    /// @brief See @ref v_cusbd_cvisitor_visit_alternate_interface().
    void (*visit_alternate_interface)(struct cusbd_cvisitor *me, const struct cusbd_alternate_interface *node);

    /// @brief See @ref v_cusbd_cvisitor_visit_endpoint().
    void (*visit_endpoint)(struct cusbd_cvisitor *me, const struct cusbd_endpoint *node);
};

/**
 * @brief Base visitor class. Algorithms are separated out
 * in concrete visitor classes. Any items that would normally 
 * be function arguments can be contained within a concrete 
 * visitor class.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_visitor
{
    /// @brief Virtual functions.
    const struct cusbd_visitor_vtable *vptr;
};

/**
 * @brief Const-qualified version of @ref cusbd_visitor.
 * Base visitor class. Algorithms are separated out
 * in concrete visitor classes. Any items that would normally 
 * be function arguments can be contained within a concrete 
 * visitor class.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_cvisitor
{
    /// @brief Virtual functions.
    const struct cusbd_cvisitor_vtable *vptr;
};

/*------------------------------------------------------------*/
/*------------------- VISITOR MEMBER FUNCTIONS ---------------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Constructors
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @brief Visitor base class constructor.
 * 
 * @param me Visitor to construct.
 */
extern void cusbd_visitor_ctor(struct cusbd_visitor *me);

/**
 * @pre Memory already allocated for @p me.
 * @brief Visitor base class constructor.
 * 
 * @param me Visitor to construct.
 */
extern void cusbd_cvisitor_ctor(struct cusbd_cvisitor *me);
/**@}*/

/**
 * @name Member Functions
 */
/**@{*/
/**
 * @pre @p me is a concrete visitor that has been constructed.
 * @pre @p node previously constructed via @ref cusbd_ctor().
 * @brief Runs concrete visitor algorithm on supplied descriptor node.
 * 
 * @warning Virtual call. Must be overridden.
 * 
 * @param me Concrete visitor containing specific algorithm to run.
 * @param node Device descriptor node to run algorithm on.
 */
extern void v_cusbd_visitor_visit_device(struct cusbd_visitor *me, struct cusbd *node);

/**
 * @pre @p me is a concrete visitor that has been constructed.
 * @pre @p node previously constructed via @ref cusbd_ctor().
 * @brief Const-qualified version of @ref v_cusbd_visitor_visit_device().
 * Runs concrete visitor algorithm on supplied descriptor node.
 * 
 * @warning Virtual call. Must be overridden.
 * 
 * @param me Concrete visitor containing specific algorithm to run.
 * @param node Device descriptor node to run algorithm on.
 */
extern void v_cusbd_cvisitor_visit_device(struct cusbd_cvisitor *me, const struct cusbd *node);

/**
 * @pre @p me is a concrete visitor that has been constructed.
 * @pre @p node previously constructed via @ref cusbd_configuration_ctor().
 * @brief Runs concrete visitor algorithm on supplied configuration descriptor
 * node.
 * 
 * @warning Virtual call. Must be overridden.
 * 
 * @param me Concrete visitor containing specific algorithm to run.
 * @param node Configuration descriptor node to run algorithm on.
 */
extern void v_cusbd_visitor_visit_configuration(struct cusbd_visitor *me, struct cusbd_configuration *node);

/**
 * @pre @p me is a concrete visitor that has been constructed.
 * @pre @p node previously constructed via @ref cusbd_configuration_ctor().
 * @brief Const-qualified version of @ref v_cusbd_visitor_visit_configuration().
 * Runs concrete visitor algorithm on supplied configuration descriptor
 * node.
 * 
 * @warning Virtual call. Must be overridden.
 * 
 * @param me Concrete visitor containing specific algorithm to run.
 * @param node Configuration descriptor node to run algorithm on.
 */
extern void v_cusbd_cvisitor_visit_configuration(struct cusbd_cvisitor *me, const struct cusbd_configuration *node);

/**
 * @pre @p me is a concrete visitor that has been constructed.
 * @pre @p node previously constructed via @ref cusbd_interface_ctor().
 * @brief Runs concrete visitor algorithm on supplied interface descriptor
 * node.
 * 
 * @warning Virtual call. Must be overridden.
 * 
 * @param me Concrete visitor containing specific algorithm to run.
 * @param node Interface descriptor node to run algorithm on.
 */
extern void v_cusbd_visitor_visit_interface(struct cusbd_visitor *me, struct cusbd_interface *node);

/**
 * @pre @p me is a concrete visitor that has been constructed.
 * @pre @p node previously constructed via @ref cusbd_interface_ctor().
 * @brief Const-qualified version of @ref v_cusbd_visitor_visit_interface().
 * Runs concrete visitor algorithm on supplied interface descriptor
 * node.
 * 
 * @warning Virtual call. Must be overridden.
 * 
 * @param me Concrete visitor containing specific algorithm to run.
 * @param node Interface descriptor node to run algorithm on.
 */
extern void v_cusbd_cvisitor_visit_interface(struct cusbd_cvisitor *me, const struct cusbd_interface *node);

/**
 * @pre @p me is a concrete visitor that has been constructed.
 * @pre @p node previously constructed via @ref cusbd_alternate_interface_ctor().
 * @brief Runs concrete visitor algorithm on supplied alternate interface 
 * descriptor node.
 * 
 * @warning Virtual call. Must be overridden.
 * 
 * @param me Concrete visitor containing specific algorithm to run.
 * @param node Alternate interface descriptor node to run algorithm on.
 */
extern void v_cusbd_visitor_visit_alternate_interface(struct cusbd_visitor *me, struct cusbd_alternate_interface *node);

/**
 * @pre @p me is a concrete visitor that has been constructed.
 * @pre @p node previously constructed via @ref cusbd_alternate_interface_ctor().
 * @brief Const-qualified version of @ref v_cusbd_visitor_visit_alternate_interface().
 * Runs concrete visitor algorithm on supplied alternate interface 
 * descriptor node.
 * 
 * @warning Virtual call. Must be overridden.
 * 
 * @param me Concrete visitor containing specific algorithm to run.
 * @param node Alternate interface descriptor node to run algorithm on.
 */
extern void v_cusbd_cvisitor_visit_alternate_interface(struct cusbd_cvisitor *me, const struct cusbd_alternate_interface *node);

/**
 * @pre @p me is a concrete visitor that has been constructed.
 * @pre @p node previously constructed via @ref cusbd_endpoint_ctor().
 * @brief Runs concrete visitor algorithm on supplied endpoint descriptor
 * node.
 * 
 * @warning Virtual call. Must be overridden.
 * 
 * @param me Concrete visitor containing specific algorithm to run.
 * @param node Endpoint descriptor node to run algorithm on.
 */
extern void v_cusbd_visitor_visit_endpoint(struct cusbd_visitor *me, struct cusbd_endpoint *node);

/**
 * @pre @p me is a concrete visitor that has been constructed.
 * @pre @p node previously constructed via @ref cusbd_endpoint_ctor().
 * @brief Const-qualified version of @ref v_cusbd_visitor_visit_endpoint().
 * Runs concrete visitor algorithm on supplied endpoint descriptor
 * node.
 * 
 * @warning Virtual call. Must be overridden.
 * 
 * @param me Concrete visitor containing specific algorithm to run.
 * @param node Endpoint descriptor node to run algorithm on.
 */
extern void v_cusbd_cvisitor_visit_endpoint(struct cusbd_cvisitor *me, const struct cusbd_endpoint *node);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* CUSBD_VISITOR_H_ */
