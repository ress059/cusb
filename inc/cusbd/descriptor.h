/**
 * @file
 * @brief CUSBD descriptor base class. Used to provide a common
 * interface between many different types of descriptors. Also
 * contains common definitions that apply to all USB devices.
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

/* ECU. */
#include "ecu/object_id.h"
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
 * @brief Upcasts derived descriptor pointer into @ref cusbd_descriptor
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
 *      &valid_func, &wTotalLengthfunc, &process_std_request_func
 * );
 * @endcode
 * 
 * @param valid_ See @ref cusbd_descriptor_vvalid().
 * @param wTotalLength_ See @ref cusbd_descriptor_vwTotalLength().
 * @param process_std_request_ See @ref cusbd_descriptor_vprocess_std_request().
 */
#define CUSBD_DESCRIPTOR_VTABLE_CTOR(valid_, wTotalLength_, process_std_request_)       \
    {                                                                                   \
        .valid = (bool (*)(const struct cusbd_descriptor *))(valid_),                   \
        .wTotalLength = (size_t (*)(const struct cusbd_descriptor *))(wTotalLength_),   \
        .process_std_request = (bool (*)(struct cusbd_descriptor *,                     \
                                         const struct cusbd_std_request *,              \
                                         enum cusbd_state,                              \
                                         void *buf,                                     \
                                         size_t len))(process_std_request_)             \
    }

/*------------------------------------------------------------*/
/*------------------------ COMMON TYPES ----------------------*/
/*------------------------------------------------------------*/

/**
 * @brief USB device states, adapted for use in
 * modeled USB state machine. This value is used
 * when certain actions depend on which state
 * the device is in. I.e. standard requests.
 */
enum cusbd_state
{
    CUSBD_IDLE_STATE,           /**<! USB device idle. Connection between host not yet established. */
    CUSBD_DEFAULT_STATE,        /**<! USB device is in default state. */
    CUSBD_ADDRESS_STATE,        /**<! USB device is in address state. */
    CUSBD_CONFIGURED_STATE,     /**<! USB device is in configured state. */
    CUSBD_SUSPENDED_STATE,      /**<! USB device is in suspended state. */
    /************************/
    CUSBD_STATE_COUNT           /**<! Total number of states. */
};

/**
 * @brief Values of bDescriptorType in standard descriptors
 * common across all USB devices.
 */
enum cusbd_descriptor_type
{
    CUSBD_DEVICE_DESCRIPTOR_TYPE = 0x01,
    CUSBD_CONFIGURATION_DESCRIPTOR_TYPE = 0x02,
    CUSBD_STRING_DESCRIPTOR_TYPE = 0x03,
    CUSBD_INTERFACE_DESCRIPTOR_TYPE = 0x04,
    CUSBD_ENDPOINT_DESCRIPTOR_TYPE = 0x05,
    CUSBD_DEVICE_QUALIFIER_DESCRIPTOR_TYPE = 0x06,
    CUSBD_OTHER_SPEED_CONFIGURATION_DESRIPTOR_TYPE = 0x07,
    CUSBD_INTERFACE_POWER_DESCRIPTOR_TYPE = 0x08
};

/*------------------------------------------------------------*/
/*----------------- CUSBD DESCRIPTOR BASE CLASS --------------*/
/*------------------------------------------------------------*/

/* Forward declare for vtable. */
struct cusbd_descriptor;

/**
 * @brief Virtual table for @ref cusbd_descriptor base
 * class.
 */
struct cusbd_descriptor_vtable
{
    /// @brief See @ref cusbd_descriptor_vvalid().
    bool (*const valid)(const struct cusbd_descriptor *me);

    /// @brief See @ref cusbd_descriptor_vwTotalLength().
    size_t (*const wTotalLength)(const struct cusbd_descriptor *me);

    /// @brief See @ref cusbd_descriptor_vprocess_std_request().
    bool (*const process_std_request)(struct cusbd_descriptor *me, 
                                      const struct cusbd_std_request *request,
                                      enum cusbd_state state, 
                                      void *buf, 
                                      size_t len);
};

/**
 * @brief CUSBD descriptor base class. Used to provide
 * a common interface between many types of descriptors.
 * All concrete descriptors (device, configuration, 
 * interface, etc.) inherit this base class.
 */
struct cusbd_descriptor
{
    /// @brief N-ary tree node. All descriptors besides strings
    /// represented as nodes in a tree.
    struct ecu_ntnode ntnode;

    /// @brief Virtual functions.
    const struct cusbd_descriptor_vtable *vptr;
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
extern void cusbd_descriptor_ctor(struct cusbd_descriptor *me, ecu_object_id bDescriptorType);
/**@}*/

/**
 * @name CUSBD Base Descriptor Member Functions
 */
/**@{*/
/**
 * @pre @p me previously constructed via @ref cusbd_descriptor_ctor().
 * @brief Returns bDescriptorType assigned in @ref cusbd_descriptor_ctor().
 * 
 * @param me Descriptor to check.
 */
extern ecu_object_id cusbd_descriptor_type(const struct cusbd_descriptor *me);

/**
 * @pre @p me previously constructed via @ref cusbd_descriptor_ctor().
 * @brief Returns true if the supplied descriptor contains valid data 
 * and was properly constructed. False otherwise.
 * 
 * @warning Virtual call.
 * 
 * @param me Descriptor to check.
 */
extern bool cusbd_descriptor_vvalid(const struct cusbd_descriptor *me);

/**
 * @pre @p me previously constructed via @ref cusbd_descriptor_ctor().
 * @brief Returns descriptor's size to update configuration
 * descriptor's wTotalLength field. Returns 0 if descriptor
 * not relevant to wTotalLength.
 * 
 * @warning Virtual call.
 * 
 * @param me Descriptor to check.
 */
extern size_t cusbd_descriptor_vwTotalLength(const struct cusbd_descriptor *me);

/**
 * @pre @p me previously constructed via @ref cusbd_descriptor_ctor().
 * @brief Processes the supplied standard USB request. I.e. GET_DESCRIPTOR(),
 * GET_CONFIGURATION(), etc.
 * 
 * @param me Descriptor to process request.
 * @param request The standard request. Contents will always be in
 * little endian, not native endianness.
 * @param state Current state the USB device is in. This allows
 * requests to be processed differently according to state.
 * @param buf If data needs to be sent, copy it into this buffer.
 * @param len Number of bytes available in @p buf.
 * @return True if the request was processed. False if the request
 * is ignored due to not being relevant. I.e. GET_INTERFACE() called
 * on configuration descriptor.
 */
extern bool cusbd_descriptor_vprocess_std_request(struct cusbd_descriptor *me, 
                                                  const struct cusbd_std_request *request,
                                                  enum cusbd_state state, 
                                                  void *buf, 
                                                  size_t len);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* CUSBD_DESCRIPTOR_H_ */
