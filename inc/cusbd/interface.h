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
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* CUSB. */
#include "cusbd/descriptor.h"
#include "cusbd/endpoint.h"
#include "cusbd/string.h"

/* ECU. */
#include "ecu/attributes.h"
#include "ecu/dlist.h"
#include "ecu/ntnode.h"

/*------------------------------------------------------------*/
/*----------------------- CUSBD INTERFACE --------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Data in a standard interface descriptor.
 * Alternate interfaces also use this same data.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_interface_descriptor
{
    /// @brief Number of bytes of this descriptor.
    uint8_t bLength;

    /// @brief Descriptor type. Always 0x04.
    uint8_t bDescriptorType;

    /// @brief Unique ID used to identify all interface
    /// descriptors attached to a configuration descriptor.
    /// Starts at 0. Assigned by user when interface constructed.
    uint8_t bInterfaceNumber;

    /// @brief Unique ID used to identify this interface's
    /// alternate settings. 0 if unused. I.e. if an interface
    /// had an alternate setting, the first interface would
    /// have bInterfaceNumber == 0 and bAlternateSetting == 0.
    /// The second interface (alternate interface) would have 
    /// bInterfaceNumber == 0 and bAlternateSettting == 1.
    /// Assigned by user when interface constructed.
    uint8_t bAlternateSetting;

    /// @brief Number of endpoints attached to this interface.
    /// Updated by library in add_endpoint() functions.
    /// @warning This never includes endpoint0. I.e. if the 
    /// interface descriptor only uses endpoint0, this is 0.
    uint8_t bNumEndpoints;

    /// @brief This interface's class code. See USB spec.
    /// Assigned by user when interface constructed.
    uint8_t bInterfaceClass;

    /// @brief This interface's subclass code. See USB spec.
    /// Assigned by user when interface constructed.
    uint8_t bInterfaceSubClass;

    /// @brief This interface's protocol code. See USB spec. 
    /// Assigned by user when interface constructed.
    uint8_t bInterfaceProtocol;

    /// @brief Index of string descriptor describing this interface.
    /// Strings are optional. Equals 0 if unused. Assigned by library
    /// when USB device starts up.
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
    /// @brief Inherit base descriptor class
    /// @warning MUST be first member.
    struct cusbd_descriptor base;

    /// @brief Descriptor data. A copy is stored so the API can
    /// automatically adjust it as the device is updated.
    /// @warning This struct is packed and will always be in 
    /// little endian.
    struct cusbd_interface_descriptor descriptor;

    /// @brief Currently active alternate interface (bAlternateSetting).
    /// NULL if no alternate interface is active.
    struct cusbd_alt_interface *alt_interface;

    /// @brief String descriptors attached to this interface descriptor. 
    /// Optional. Empty if unused.
    /// @warning If used, the device must use a string descriptor zero. 
    struct ecu_dlist strings;
};

/**
 * @brief Object representing a USB alternate interface descriptor.
 * Interface and alternate inteface descriptor data is the 
 * exactly the same, but this library represents these as two
 * separate objects for better organization since an 
 * alternate interface cannot have other alternate interfaces.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_alt_interface
{
    /// @brief Inherit base descriptor class
    /// @warning MUST be first member.
    struct cusbd_descriptor base;

    /// @brief Descriptor data. A copy is stored so the API can
    /// automatically adjust it as the device is updated.
    /// @warning This struct is packed and will always be in 
    /// little endian.
    struct cusbd_interface_descriptor descriptor;

    /// @brief String descriptors attached to this interface descriptor. 
    /// Optional. Empty if unused.
    /// @warning If used, the device must use a string descriptor zero. 
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
 * @brief Interface descriptor constructor.
 * 
 * @warning This cannot be called on an active interface 
 * descriptor. Doing so is undefined behavior.
 * 
 * @param me Interface descriptor to construct.
 * @param bInterfaceNumber bInterfaceNumber field in interface
 * descriptor. This starts at 0. It is the user's responsibility to 
 * set this to a valid value - it will not be checked by the library. 
 * See USB spec. 
 * @param bInterfaceClass bInterfaceClass field in the interface
 * descriptor. See USB spec and https://www.usb.org/defined-class-codes.
 * @param bInterfaceSubClass bInterfaceSubClass field in the interface
 * It's value depends on the class specified in @p bInterfaceClass.
 * See USB spec and class's specification for list of acceptable values.
 * @param bInterfaceProtocol bInterfaceProtocol field in the interface
 * It's value depends on the class specified in @p bInterfaceClass.
 * See USB spec and class's specification for list of acceptable values.
 */
extern void cusbd_interface_ctor(struct cusbd_interface *me, 
                                 uint8_t bInterfaceNumber,
                                 uint8_t bInterfaceClass,
                                 uint8_t bInterfaceSubClass,
                                 uint8_t bInterfaceProtocol);
/**@}*/

/**
 * @name CUSBD Interface Member Functions
 */
/**@{*/
/**
 * @pre @p me constructed via @ref cusbd_interface_ctor().
 * @pre @p alt_interface previously constructed via @ref cusbd_alt_interface_ctor().
 * @brief Adds an alternate interface descriptor to the supplied interface 
 * descriptor.
 * 
 * @warning This must only be called on setup, before @ref cusbd_start() 
 * is called. Otherwise behavior is undefined.
 * 
 * @param me Interface descriptor to add to.
 * @param alt_interface Alternate interface descriptor to add. 
 * This cannot already be within another interface descriptor.
 */
extern void cusbd_interface_add_alt_interface(struct cusbd_interface *me,
                                              struct cusbd_alt_interface *alt_interface);

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
 * within another interface or alternate interface descriptor. The
 * endpoint's address (bEndpointAddress) cannot be the same as any 
 * endpoint descriptors currently attached to the supplied
 * interface descriptor. I.e. the interface descriptor cannot have 
 * multiple endpoint1 INs. This is the user's responsibility.
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
 * this descriptor has a string descriptor zero.
 * 
 * @param me Interface descriptor to add to.
 * @param string String descriptor to add. This cannot already be within
 * another descriptor.
 */
extern void cusbd_interface_add_string(struct cusbd_interface *me,
                                       struct cusbd_string *string);

/**
 * @brief Returns true if the supplied interface descriptor contains
 * valid data and was properly constructed via @ref cusbd_interface_ctor(). 
 * False otherwise.
 * 
 * @param me Interface descriptor to check.
 */
extern bool cusbd_interface_valid(const struct cusbd_interface *me);
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
 * @brief Alternate interface descriptor constructor.
 * 
 * @warning This cannot be called on an active alternate
 * interface descriptor. Doing so is undefined behavior.
 * 
 * @param me Alternate interface descriptor to construct.
 * @param bAlternateSetting bAlternateSetting field in the interface
 * descriptor. This must be greater than 0 since this is an alternate
 * interface. It is the user's responsibility to set this to a valid 
 * value - the library only verifies this is not 0. See USB spec. 
 * @param bInterfaceClass bInterfaceClass field in the interface
 * descriptor. See USB spec and https://www.usb.org/defined-class-codes.
 * @param bInterfaceSubClass bInterfaceSubClass field in the interface
 * It's value depends on the class specified in @p bInterfaceClass.
 * See USB spec and class's specification for list of acceptable values.
 * @param bInterfaceProtocol bInterfaceProtocol field in the interface
 * It's value depends on the class specified in @p bInterfaceClass.
 * See USB spec and class's specification for list of acceptable values.
 */
extern void cusbd_alt_interface_ctor(struct cusbd_alt_interface *me, 
                                     uint8_t bAlternateSetting,
                                     uint8_t bInterfaceClass,
                                     uint8_t bInterfaceSubClass,
                                     uint8_t bInterfaceProtocol);
/**@}*/

/**
 * @name CUSBD Alternate Interface Member Functions
 */
/**@{*/
/**
 * @pre @p me previously constructed via @ref cusbd_alt_interface_ctor().
 * @pre @p endpoint previously constructed via @ref cusbd_endpoint_ctor().
 * @brief Adds an endpoint descriptor to the supplied alternate interface 
 * descriptor.
 * 
 * @warning This must only be called on setup, before @ref cusbd_start() 
 * is called. Otherwise behavior is undefined.
 * 
 * @param me Alternate interface descriptor to add to.
 * @param endpoint Endpoint descriptor to add. This cannot already be
 * within another interface or alternate interface descriptor. The
 * endpoint's address (bEndpointAddress) cannot be the same as any 
 * endpoint descriptors currently attached to the supplied alternate
 * interface descriptor. I.e. the alternate interface descriptor cannot 
 * have multiple endpoint1 INs. This is the user's responsibility.
 */
extern void cusbd_alt_interface_add_endpoint(struct cusbd_alt_interface *me,
                                             struct cusbd_endpoint *endpoint);

/**
 * @pre @p me previously constructed via @ref cusbd_alt_interface_ctor().
 * @pre @p string previously constructed via @ref cusbd_string_ctor().
 * @brief Adds a string descriptor to the supplied alternate interface
 * descriptor.
 * 
 * @warning This must only be called on setup, before @ref cusbd_start() 
 * is called. Otherwise behavior is undefined.
 * @warning This can only be used if the USB device associated with
 * this descriptor has a string descriptor zero.
 * 
 * @param me Alternate interface descriptor to add to.
 * @param string String descriptor to add. This cannot already be within
 * another descriptor.
 */
extern void cusbd_alt_interface_add_string(struct cusbd_alt_interface *me,
                                           struct cusbd_string *string);

/**
 * @brief Returns true if the supplied alternate interface descriptor contains
 * valid data and was properly constructed via @ref cusbd_alt_interface_ctor(). 
 * False otherwise.
 * 
 * @param me Alternate interface descriptor to check.
 */
extern bool cusbd_alt_interface_valid(const struct cusbd_alt_interface *me);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* CUSBD_INTERFACE_H_ */
