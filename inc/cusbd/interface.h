/**
 * @file
 * @brief Object representing an interface descriptor.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-05-16
 * @copyright Copyright (c) 2025
 */

#ifndef CUSBD_INTERFACE_H_
#define CUSBD_INTERFACE_H_

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <stddef.h>
#include <stdint.h>

/* CUSBD. */
#include "cusbd/descriptor.h"
#include "cusbd/endpoint.h"
#include "cusbd/string.h"

/* ECU. */
#include "ecu/attributes.h"

/*------------------------------------------------------------*/
/*---------------------- DEFINES AND MACROS ------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Creates a @ref cusbd_interface_descriptor at
 * either compile-time or run-time. Example usage:
 * @code{.c}
 * static const struct cusbd_interface_descriptor interface = CUSBD_INTERFACE_DESCRIPTOR_CTOR(
 *      0, 0, 0
 * );
 * @endcode
 * 
 * @param bInterfaceClass_ This interface's class code. See USB spec.
 * @param bInterfaceSubClass_ This interface's subclass code. See USB spec.
 * @param bInterfaceProtocol_ This interface's protocol code. See USB spec.
 */
#define CUSBD_INTERFACE_DESCRIPTOR_CTOR(bInterfaceClass_,       \
                                        bInterfaceSubClass_,    \
                                        bInterfaceProtocol_)    \
    {                                                           \
        .bLength = sizeof(struct cusbd_interface_descriptor),   \
        .bDescriptorType = CUSBD_INTERFACE_DESCRIPTOR_TYPE,     \
        .bInterfaceNumber = 0,                                  \
        .bAlternateSetting = 0,                                 \
        .bNumEndpoints = 0,                                     \
        .bInterfaceClass = (bInterfaceClass_),                  \
        .bInterfaceSubClass = (bInterfaceSubClass_),            \
        .bInterfaceProtocol = (bInterfaceProtocol_),            \
        .iInterface = 0                                         \
    }

/*------------------------------------------------------------*/
/*----------------------- CUSBD INTERFACE --------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Data in a standard interface descriptor.
 * Alternate interfaces also use this same data.
 * Currently no multi-byte values but if USB spec ever
 * changes and adds multi-byte values, using this API
 * will ensure it is always in little endian format.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_interface_descriptor
{
    /// @brief Number of bytes of this descriptor.
    uint8_t bLength;

    /// @brief Descriptor type. Always 0x04 == Interface Descriptor.
    uint8_t bDescriptorType;

    /// @brief Unique ID used to identify all interface
    /// descriptors attached to a configuration descriptor.
    /// Starts at 0.
    uint8_t bInterfaceNumber;

    /// @brief Unique ID used to identify this interface's
    /// alternate settings. 0 if unused. I.e. if an interface
    /// had an alternate setting, the first interface would
    /// have bInterfaceNumber == 0 and bAlternateSetting == 0.
    /// The second interface (alternate interface) would have 
    /// bInterfaceNumber == 0 and bAlternateSettting == 1.
    uint8_t bAlternateSetting;

    /// @brief Number of endpoints attached to this interface.
    /// @warning This never includes endpoint0. I.e. if the 
    /// interface descriptor only uses endpoint0, this is 0.
    uint8_t bNumEndpoints;

    /// @brief This interface's class code. See USB spec. 
    uint8_t bInterfaceClass;

    /// @brief This interface's subclass code. See USB spec. 
    uint8_t bInterfaceSubClass;

    /// @brief This interface's protocol code. See USB spec. 
    uint8_t bInterfaceProtocol;

    /// @brief Index of string descriptor describing this interface.
    /// Strings are optional. Equals 0 if unused.
    uint8_t iInterface;
} ECU_ATTRIBUTE_PACKED;

/**
 * @brief Object representing a USB interface descriptor.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_interface
{
    /// @brief Inherit cusbd_descriptor base class.
    /// @warning MUST be first member.
    struct cusbd_descriptor base;

    /// @brief Descriptor data. A copy is stored so the API can
    /// automatically adjust bNumEndpoints, iInterface, etc as
    /// the device's descriptor tree is updated.
    /// @warning This struct is packed and will always be in 
    /// little endian.
    struct cusbd_interface_descriptor descriptor;

    /// @brief String descriptors attached to this interface descriptor. 
    /// Optional. Empty if unused.
    /// @warning If used, the device must have a string descriptor zero. 
    /// I.e. @ref cusbd.string0 must be populated.
    struct ecu_dlist strings;
};

/**
 * @brief Object representing a USB alternate interface descriptor.
 * Interface and alternate inteface descriptor data is the 
 * exactly the same, but CUSB represents these as two
 * separate objects for better organization since an 
 * alternate interface cannot have other alternate interfaces.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_alternate_interface
{
    /// @brief Inherit cusbd_descriptor base class.
    /// @warning MUST be first member.
    struct cusbd_descriptor base;

    /// @brief Descriptor data. A copy is stored so the API can
    /// automatically adjust bNumEndpoints, iInterface, etc as
    /// the device's descriptor tree is updated.
    /// @warning This struct is packed and will always be in 
    /// little endian.
    struct cusbd_interface_descriptor descriptor;

    /// @brief String descriptors attached to this alternate interface 
    /// descriptor. Optional. Empty if unused.
    /// @warning If used, the device must have a string descriptor zero. 
    /// I.e. @ref cusbd.string0 must be populated.
    struct ecu_dlist strings;
};

/*------------------------------------------------------------*/
/*--------------- CUSBD INTERFACE MEMBER FUNCTIONS -----------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name CUSBD Interface Constructors
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @pre @p descriptor previously constructed via @ref CUSBD_INTERFACE_DESCRIPTOR_CTOR().
 * @brief Interface descriptor constructor.
 * 
 * @warning This cannot be called on an active interface 
 * descriptor. Doing so is undefined behavior.
 * 
 * @param me Interface descriptor to construct.
 * @param descriptor The interface descriptor's data.
 */
extern void cusbd_interface_ctor(struct cusbd_interface *me,
                                 const struct cusbd_interface_descriptor *descriptor);
/**@}*/

/**
 * @name CUSBD Interface Member Functions
 */
/**@{*/
/**
 * @pre @p me previously constructed via @ref cusbd_interface_ctor().
 * @pre @p alternate_interface previously constructed via @ref cusbd_alternate_interface_ctor().
 * @brief Adds an alternate interface descriptor to the supplied interface 
 * descriptor.
 * 
 * @warning This must only be called on setup, before @ref cusbd_start() 
 * is called. Otherwise behavior is undefined.
 * 
 * @param me Interface descriptor to add to.
 * @param alternate_interface Alternate interface descriptor to add. 
 * This cannot already be within another interface descriptor.
 */
extern void cusbd_interface_add_alternate_interface(struct cusbd_interface *me,
                                                    struct cusbd_alternate_interface *alternate_interface);

/**
 * @pre @p me previously constructed via @ref cusbd_interface_ctor().
 * @pre @p endpoint previously constructed via @ref cusbd_endpoint_ctor().
 * @brief Adds an endpoint descriptor to the supplied interface descriptor.
 * 
 * @warning This must only be called on setup, before @ref cusbd_start() 
 * is called. Otherwise behavior is undefined.
 * 
 * @param me Interface descriptor to add to.
 * @param endpoint Endpoint descriptor to add. This cannot already be
 * within another interface or alternate interface descriptor.
 * @p endpoint's address (@ref cusbd_endpoint_descriptor.bEndpointAddress)
 * cannot be the same as any endpoint descriptors currently in @p me.
 * I.e. an interface cannot have multiple endpoint1 INs.
 */
extern void cusbd_interface_add_endpoint(struct cusbd_interface *me,
                                         struct cusbd_endpoint *endpoint);

/**
 * @pre @p me previously constructed via @ref cusbd_interface_ctor().
 * @pre @p string previously constructed via @ref cusbd_string_ctor().
 * @brief Adds a string descriptor to the supplied interface
 * descriptor.
 * 
 * @warning This must only be called on setup, before @ref cusbd_start() 
 * is called. Otherwise behavior is undefined.
 * @warning This can only be used if the USB device associated with
 * this descriptor has a string descriptor zero. I.e. a populated 
 * string0 was passed to @ref cusbd_ctor().
 * 
 * @param me Interface descriptor to add to.
 * @param string String descriptor to add. This cannot already be within
 * another descriptor.
 */
extern void cusbd_interface_add_string(struct cusbd_interface *me,
                                       struct cusbd_string *string);

/**
 * @pre @p me previously constructed via @ref cusbd_interface_ctor().
 * @brief Returns true if the supplied interface descriptor contains
 * valid data and was properly constructed via @ref cusbd_interface_ctor(). 
 * False otherwise.
 * 
 * @param me Interface descriptor to check.
 */
extern bool cusbd_interface_valid(const struct cusbd_interface *me);

/**
 * @pre @p me previously constructed via @ref cusbd_configuration_ctor().
 * @brief Returns number of bytes in the entire interface 
 * descriptor's subtree. I.e. sizeof(interface descriptor) + 
 * sizeof(all endpoint descriptors) + sizeof(all alternate interface descriptors) + ...
 * The size of the descriptor's data is used, NOT the size of the 
 * CUSBD objects. The return value of this function is meant to be 
 * used to update wTotalLength in the configuration descriptor.
 * 
 * @warning This value is returned in native endianness, not
 * little endian.
 * 
 * @param me Configuration descriptor to check.
 */
extern size_t cusbd_interface_size(const struct cusbd_interface *me);
/**@}*/

/*------------------------------------------------------------*/
/*--------- CUSBD ALTERNATE INTERFACE MEMBER FUNCTIONS -------*/
/*------------------------------------------------------------*/

/**
 * @name CUSBD Alternate Interface Constructors
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @pre @p descriptor previously constructed via @ref CUSBD_INTERFACE_DESCRIPTOR_CTOR().
 * @brief Alternate interface descriptor constructor.
 * 
 * @warning This cannot be called on an active alternate interface 
 * descriptor. Doing so is undefined behavior.
 * 
 * @param me Alternate interface descriptor to construct.
 * @param descriptor The alternate interface descriptor's data.
 */
extern void cusbd_alternate_interface_ctor(struct cusbd_alternate_interface *me,
                                           const struct cusbd_interface_descriptor *descriptor);
/**@}*/

/**
 * @name CUSBD Alternate Interface Member Functions
 */
/**@{*/
/**
 * @pre @p me previously constructed via @ref cusbd_alternate_interface_ctor().
 * @pre @p endpoint previously constructed via @ref cusbd_endpoint_ctor().
 * @brief Adds an endpoint descriptor to the supplied alternate 
 * interface descriptor.
 * 
 * @warning This must only be called on setup, before @ref cusbd_start() 
 * is called. Otherwise behavior is undefined.
 * 
 * @param me Alternate interface descriptor to add to.
 * @param endpoint Endpoint descriptor to add. This cannot already be
 * within another interface or alternate interface descriptor.
 * @p endpoint's address (@ref cusbd_endpoint_descriptor.bEndpointAddress)
 * cannot be the same as any endpoint descriptors currently in @p me.
 * I.e. an interface cannot have multiple endpoint1 INs.
 */
extern void cusbd_alternate_interface_add_endpoint(struct cusbd_alternate_interface *me,
                                                   struct cusbd_endpoint *endpoint);

/**
 * @pre @p me previously constructed via @ref cusbd_alternate_interface_ctor().
 * @pre @p string previously constructed via @ref cusbd_string_ctor().
 * @brief Adds a string descriptor to the supplied alternate
 * interface descriptor.
 * 
 * @warning This must only be called on setup, before @ref cusbd_start() 
 * is called. Otherwise behavior is undefined.
 * @warning This can only be used if the USB device associated with
 * this descriptor has a string descriptor zero. I.e. a populated 
 * string0 was passed to @ref cusbd_ctor().
 * 
 * @param me Alternate interface descriptor to add to.
 * @param string String descriptor to add. This cannot already be within
 * another descriptor.
 */
extern void cusbd_alternate_interface_add_string(struct cusbd_alternate_interface *me,
                                                 struct cusbd_string *string);

/**
 * @pre @p me previously constructed via @ref cusbd_alternate_interface_ctor().
 * @brief Returns true if the supplied alternate interface descriptor contains
 * valid data and was properly constructed via @ref cusbd_alternate_interface_ctor(). 
 * False otherwise.
 * 
 * @param me Alternate interface descriptor to check.
 */
extern bool cusbd_alternate_interface_valid(const struct cusbd_alternate_interface *me);

/**
 * @pre @p me previously constructed via @ref cusbd_configuration_ctor().
 * @brief Returns number of bytes of the supplied alternate interface
 * descriptor and all of its endpoints. The size of the descriptor's
 * data is used, NOT the size of the CUSBD objects. The return value 
 * of this function is meant to be used to update wTotalLength in 
 * the configuration descriptor.
 * 
 * @warning This value is returned in native endianness, not
 * little endian.
 * 
 * @param me Alternate interface descriptor to check.
 */
extern size_t cusbd_alternate_interface_size(const struct cusbd_alternate_interface *me);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* CUSBD_INTERFACE_H_ */
