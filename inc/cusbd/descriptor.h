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
#include <stdint.h>

/* CUSB. */
#include "cusbd/request.h"

/* ECU. */
#include "ecu/attributes.h"
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
 *      &valid_func, &wTotalLength_func, ....
 * );
 * @endcode
 * 
 * @param valid_ See @ref v_cusbd_descriptor_valid().
 * @param wTotalLength_ See @ref v_cusbd_descriptor_wTotalLength().
 * @param default_state_in_ See @ref v_cusbd_descriptor_default_state_in().
 * @param default_state_out_ See @ref v_cusbd_descriptor_default_state_out().
 * @param address_state_in_ See @ref v_cusbd_descriptor_address_state_in().
 * @param address_state_out_ See @ref v_cusbd_descriptor_address_state_out().
 * @param configured_state_in_ See @ref v_cusbd_descriptor_configured_state_in().
 * @param configured_state_out_ See @ref v_cusbd_descriptor_configured_state_out().
 */
#define CUSBD_DESCRIPTOR_VTABLE_CTOR(valid_,                                                                            \
                                     wTotalLength_,                                                                     \
                                     default_state_in_,                                                                 \
                                     default_state_out_,                                                                \
                                     address_state_in_,                                                                 \
                                     address_state_out_,                                                                \
                                     configured_state_in_,                                                              \
                                     configured_state_out_)                                                             \
    {                                                                                                                   \
        .valid = (bool (*)(const struct cusbd_descriptor *))(valid_),                                                   \
        .wTotalLength = (uint16_t (*)(const struct cusbd_descriptor *))(wTotalLength_),                                 \
        .default_state_in = (enum cusbd_request_status (*)(struct cusbd_descriptor *,                                   \
                                                           const struct cusbd_request *,                                \
                                                           void *buf,                                                   \
                                                           size_t len))(default_state_in_),                             \
        .default_state_out = (enum cusbd_request_status (*)(struct cusbd_descriptor *,                                  \
                                                            const struct cusbd_request *))(default_state_out_),         \
        .address_state_in = (enum cusbd_request_status (*)(struct cusbd_descriptor *,                                   \
                                                           const struct cusbd_request *,                                \
                                                           void *buf,                                                   \
                                                           size_t len))(address_state_in_),                             \
        .address_state_out = (enum cusbd_request_status (*)(struct cusbd_descriptor *,                                  \
                                                            const struct cusbd_request *))(address_state_out_),         \
        .configured_state_in = (enum cusbd_request_status (*)(struct cusbd_descriptor *,                                \
                                                              const struct cusbd_request *,                             \
                                                              void *buf,                                                \
                                                              size_t len))(configured_state_in_),                       \
        .configured_state_out = (enum cusbd_request_status (*)(struct cusbd_descriptor *,                               \
                                                               const struct cusbd_request *))(configured_state_out_)    \
    }

/*------------------------------------------------------------*/
/*------------------------ COMMON TYPES ----------------------*/
/*------------------------------------------------------------*/

// /**
//  * @brief USB device states, adapted for use in
//  * modeled USB state machine. This value is used
//  * when certain actions depend on which state
//  * the device is in. I.e. standard requests.
//  */
// enum cusbd_state
// {
//     CUSBD_STATE_IDLE,           /**<! USB device idle. Connection between host not yet established. */
//     CUSBD_STATE_DEFAULT,        /**<! USB device is in default state. */
//     CUSBD_STATE_ADDRESS,        /**<! USB device is in address state. */
//     CUSBD_STATE_CONFIGURED,     /**<! USB device is in configured state. */
//     CUSBD_STATE_SUSPENDED,      /**<! USB device is in suspended state. */
//     /************************/
//     CUSBD_STATE_COUNT           /**<! Total number of states. */
// };

/**
 * @brief Values of bDescriptorType in standard descriptors
 * common across all USB devices.
 */
enum cusbd_descriptor_type
{
    CUSBD_DESCRIPTOR_TYPE_DEVICE = 0x01,
    CUSBD_DESCRIPTOR_TYPE_CONFIGURATION = 0x02,
    CUSBD_DESCRIPTOR_TYPE_STRING = 0x03,
    CUSBD_DESCRIPTOR_TYPE_INTERFACE = 0x04,
    CUSBD_DESCRIPTOR_TYPE_ENDPOINT = 0x05,
    CUSBD_DESCRIPTOR_TYPE_DEVICE_QUALIFIER = 0x06,
    CUSBD_DESCRIPTOR_TYPE_OTHER_SPEED_CONFIGURATION = 0x07,
    CUSBD_DESCRIPTOR_TYPE_INTERFACE_POWER = 0x08
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
    /// @brief See @ref v_cusbd_descriptor_valid().
    bool (*const valid)(const struct cusbd_descriptor *me);

    /// @brief See @ref v_cusbd_descriptor_wTotalLength().
    uint16_t (*const wTotalLength)(const struct cusbd_descriptor *me);

    /// @brief See @ref v_cusbd_descriptor_default_state_in().
    enum cusbd_request_status (*const default_state_in)(struct cusbd_descriptor *me,
                                                        const struct cusbd_request *request,
                                                        void *buf,
                                                        size_t len);

    /// @brief See @ref v_cusbd_descriptor_default_state_out().
    enum cusbd_request_status (*const default_state_out)(struct cusbd_descriptor *me,
                                                         const struct cusbd_request *request);

    /// @brief See @ref v_cusbd_descriptor_address_state_in().
    enum cusbd_request_status (*const address_state_in)(struct cusbd_descriptor *me,
                                                        const struct cusbd_request *request,
                                                        void *buf,
                                                        size_t len);

    /// @brief See @ref v_cusbd_descriptor_address_state_out().
    enum cusbd_request_status (*const address_state_out)(struct cusbd_descriptor *me,
                                                         const struct cusbd_request *request);

    /// @brief See @ref v_cusbd_descriptor_configured_state_in().
    enum cusbd_request_status (*const configured_state_in)(struct cusbd_descriptor *me,
                                                           const struct cusbd_request *request,
                                                           void *buf,
                                                           size_t len);

    /// @brief See @ref v_cusbd_descriptor_configured_state_out().
    enum cusbd_request_status (*const configured_state_out)(struct cusbd_descriptor *me,
                                                            const struct cusbd_request *request);
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
 * @pre @p me is a derived class that has been constructed and inherits @ref cusbd_descriptor.
 * @brief Returns true if the supplied descriptor contains valid data 
 * and was properly constructed. False otherwise.
 * 
 * @warning Virtual call.
 * 
 * @param me Descriptor to check.
 */
extern bool v_cusbd_descriptor_valid(const struct cusbd_descriptor *me);

/**
 * @pre @p me is a derived class that has been constructed and inherits @ref cusbd_descriptor.
 * @brief Returns descriptor's size to update configuration
 * descriptor's wTotalLength field. Returns 0 if descriptor
 * not relevant to wTotalLength. Value returned is always in
 * native endianness, not little endian.
 * 
 * @warning Virtual call.
 * 
 * @param me Descriptor to check.
 */
extern uint16_t v_cusbd_descriptor_wTotalLength(const struct cusbd_descriptor *me);

/**
 * @pre @p me is a derived class that has been constructed and 
 * inherits @ref cusbd_descriptor.
 * @brief Processes the supplied USB device IN request when the 
 * device is in the default state.
 * 
 * @warning Virtual call.
 * 
 * @param me Derived descriptor (device, configuration, interface, etc)
 * to process request.
 * @param request The request. Contents will always be in little 
 * endian, not native endianness.
 * @param buf Data that needs to be sent back to the host is copied
 * into this buffer.
 * @param len Number of bytes available in @p buf. This must always
 * be >= number of bytes of data sent back to host.
 * @return See @ref cusbd_request_status enumeration.
 */
extern enum cusbd_request_status v_cusbd_descriptor_default_state_in(struct cusbd_descriptor *me,
                                                                     const struct cusbd_request *request,
                                                                     void *buf,
                                                                     size_t len);

/**
 * @pre @p me is a derived class that has been constructed and 
 * inherits @ref cusbd_descriptor.
 * @brief Processes the supplied USB device OUT request when the 
 * device is in the default state.
 * 
 * @warning Virtual call.
 * 
 * @param me Derived descriptor (device, configuration, interface, etc)
 * to process request.
 * @param request The request. Contents will always be in little 
 * endian, not native endianness.
 * @return See @ref cusbd_request_status enumeration.
 */
extern enum cusbd_request_status v_cusbd_descriptor_default_state_out(struct cusbd_descriptor *me,
                                                                      const struct cusbd_request *request);

/**
 * @pre @p me is a derived class that has been constructed and 
 * inherits @ref cusbd_descriptor.
 * @brief Processes the supplied USB device IN request when the 
 * device is in the address state.
 * 
 * @warning Virtual call.
 * 
 * @param me Derived descriptor (device, configuration, interface, etc)
 * to process request.
 * @param request The request. Contents will always be in little 
 * endian, not native endianness.
 * @param buf Data that needs to be sent back to the host is copied
 * into this buffer.
 * @param len Number of bytes available in @p buf. This must always
 * be >= number of bytes of data sent back to host.
 * @return See @ref cusbd_request_status enumeration.
 */
extern enum cusbd_request_status v_cusbd_descriptor_address_state_in(struct cusbd_descriptor *me,
                                                                     const struct cusbd_request *request,
                                                                     void *buf,
                                                                     size_t len);

/**
 * @pre @p me is a derived class that has been constructed and 
 * inherits @ref cusbd_descriptor.
 * @brief Processes the supplied USB device OUT request when the 
 * device is in the address state.
 * 
 * @warning Virtual call.
 * 
 * @param me Derived descriptor (device, configuration, interface, etc)
 * to process request.
 * @param request The request. Contents will always be in little 
 * endian, not native endianness.
 * @return See @ref cusbd_request_status enumeration.
 */
extern enum cusbd_request_status v_cusbd_descriptor_address_state_out(struct cusbd_descriptor *me,
                                                                      const struct cusbd_request *request);

/**
 * @pre @p me is a derived class that has been constructed and 
 * inherits @ref cusbd_descriptor.
 * @brief Processes the supplied USB device IN request when the 
 * device is in the configured state.
 * 
 * @warning Virtual call.
 * 
 * @param me Derived descriptor (device, configuration, interface, etc)
 * to process request.
 * @param request The request. Contents will always be in little 
 * endian, not native endianness.
 * @param buf Data that needs to be sent back to the host is copied
 * into this buffer.
 * @param len Number of bytes available in @p buf. This must always
 * be >= number of bytes of data sent back to host.
 * @return See @ref cusbd_request_status enumeration.
 */
extern enum cusbd_request_status v_cusbd_descriptor_configured_state_in(struct cusbd_descriptor *me,
                                                                        const struct cusbd_request *request,
                                                                        void *buf,
                                                                        size_t len);

/**
 * @pre @p me is a derived class that has been constructed and 
 * inherits @ref cusbd_descriptor.
 * @brief Processes the supplied USB device OUT request when the 
 * device is in the configured state.
 * 
 * @warning Virtual call.
 * 
 * @param me Derived descriptor (device, configuration, interface, etc)
 * to process request.
 * @param request The request. Contents will always be in little 
 * endian, not native endianness.
 * @return See @ref cusbd_request_status enumeration.
 */
extern enum cusbd_request_status v_cusbd_descriptor_configured_state_out(struct cusbd_descriptor *me,
                                                                         const struct cusbd_request *request);


!!!!! TODO Thinking of having cusbd_descriptor_push_front(), cusbd_descriptor_push_back(), etc.
!!!!! User responsible for correctly populating bNumEndpoints, bNumInterfaces, wTotalLength etc.
!!!!! If this is the case descriptor objects can take in descriptors by (const *).
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* CUSBD_DESCRIPTOR_H_ */
