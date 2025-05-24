/**
 * @file
 * @brief Object representing a configuration descriptor.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-05-16
 * @copyright Copyright (c) 2025
 */

#ifndef CUSB_CONFIGURATION_H_
#define CUSB_CONFIGURATION_H_

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* CUSB. */
#include "cusb/descriptors.h"
#include "cusb/interface.h"
#include "cusb/string.h"

/* ECU. */
#include "ecu/attributes.h"
#include "ecu/dlist.h"

/*------------------------------------------------------------*/
/*---------------------- DEFINES AND MACROS ------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Helper macro that supplies bMaxPower parameter to
 * @ref CUSB_CONFIGURATION_DESCRIPTOR_CTOR(). This is the
 * bMaxPower field in the configuration descriptor, which
 * represents max power in 2mA units. For example 50 == 100mA 
 * max power. This macro allows the user to supply a raw number
 * and converts it to the proper format (divides by 2).
 * 
 * @param milliamps_ Max power consumption of USB device in
 * milliAmps. This cannot exceed 500.  
 */
#define CUSB_CONFIGURATION_SET_BMAXPOWER(milliamps_) \
    ((milliamps_) >> 1U)

/**
 * @brief Creates a @ref cusb_configuration_descriptor at
 * either compile-time or run-time. Example usage:
 * @code{.c}
 * static const struct cusb_configuration_descriptor config = CUSB_CONFIGURATION_DESCRIPTOR_CTOR(
 *      0, CUSB_CONFIGURATION_SET_BMAXPOWER(500)
 * );
 * @endcode
 * 
 * @param bmAttributes_ Bitmap of characteristics. I.e. if device is self-powered,
 * remote wakeup, etc. See USB spec.
 * @param bMaxPower_ Max power consumption of the USB device in 2mA units.
 * I.e. 50 == 100mA max power. Cannot exceed 250. @ref CUSB_CONFIGURATION_SET_BMAXPOWER() 
 * allows a raw number to be supplied instead. Cannot exceed 500 if this
 * is a raw number.
 */
#define CUSB_CONFIGURATION_DESCRIPTOR_CTOR(bmAttributes_,           \
                                           bMaxPower_)              \
    {                                                               \
        .bLength = sizeof(struct cusb_configuration_descriptor),    \
        .bDescriptorType = CUSB_CONFIGURATION_DESCRIPTOR_TYPE,      \
        .wTotalLength = 0,                                          \
        .bNumInterfaces = 0,                                        \
        .bConfigurationValue = 0,                                   \
        .iConfiguration = 0,                                        \
        .bmAttributes = (bmAttributes_),                            \
        .bMaxPower = (bMaxPower_)                                   \
    }

/*------------------------------------------------------------*/
/*---------------------- CUSB CONFIGURATION ------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Data in a standard configuration descriptor.
 * Using the API ensures this is always encoded in
 * little endian format.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusb_configuration_descriptor
{
    /// @brief Number of bytes of this descriptor.
    uint8_t bLength;

    /// @brief Descriptor type. Always 0x02 == Configuration Descriptor.
    uint8_t bDescriptorType;

    /// @brief Total length in bytes of entire descriptor's subtree, including
    /// this configuration descriptor.
    uint16_t wTotalLength;

    /// @brief The number of interface descriptors attached to this
    /// configuration. Must always be >= 1 after device is fully setup
    /// since all configuration descriptors must have at least one 
    /// interface descriptor.
    uint8_t bNumInterfaces;

    /// @brief Unique ID sent to host to identify this configuration.
    /// Starts at 1.
    uint8_t bConfigurationValue;

    /// @brief Index of string descriptor describing this configuration.
    /// Strings are optional. Equals 0 if unused.
    uint8_t iConfiguration;

    /// @brief Bitmap of characteristics. I.e. if device is self-powered,
    /// remote wakeup, etc. See USB spec.
    uint8_t bmAttributes;

    /// @brief Max power consumption of the USB device in 2mA
    /// units. I.e. 50 == 100mA max power.
    uint8_t bMaxPower;
} ECU_ATTRIBUTE_PACKED;

/**
 * @brief Object representing a USB configuration descriptor.
 * Once the device is fully setup, this must have at least 1
 * interface since all configuration descriptors must have at
 * least one interface descriptor.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusb_configuration
{
    /// @brief Node in linked list.
    struct ecu_dnode dnode;

    /// @brief Descriptor data. A copy is stored so the API can automatically
    /// adjust bNumInterfaces, bConfigurationValue, etc as the device's
    /// descriptor tree is updated.
    /// @warning This struct is packed and will always be in little endian.
    struct cusb_configuration_descriptor descriptor;

    /// @brief Interface descriptors attached to this configuration descriptor.
    /// @warning Once the device is fully setup this must contain at 
    /// least 1 interface since all configuration descriptors must have at
    /// least one interface descriptor.
    struct ecu_dlist interfaces;

    /// @brief String descriptors attached to this configuration 
    /// descriptor. Optional. Empty if unused.
    /// @warning If used, the device must have a string descriptor zero. 
    /// I.e. @ref cusbd.string0 must be populated.
    struct ecu_dlist strings;
};

/*------------------------------------------------------------*/
/*------------ CUSB CONFIGURATION MEMBER FUNCTIONS -----------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name CUSB Configuration Constructors
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @pre @p descriptor previously constructed via @ref CUSB_CONFIGURATION_DESCRIPTOR_CTOR().
 * @brief Configuration descriptor constructor.
 * 
 * @warning This cannot be called on an active configuration 
 * descriptor. Doing so is undefined behavior.
 * 
 * @param me Configuration descriptor to construct.
 * @param descriptor The configuration descriptor's data.
 */
extern void cusb_configuration_ctor(struct cusb_configuration *me,
                                    const struct cusb_configuration_descriptor *descriptor);
/**@}*/

/**
 * @name CUSB Configuration Member Functions
 */
/**@{*/
/**
 * @pre @p me previously constructed via @ref cusb_configuration_ctor().
 * @pre @p interface previously constructed via @ref cusb_interface_ctor().
 * @brief Adds an interface descriptor to the supplied configuration 
 * descriptor.
 * 
 * @warning This must only be called on setup, before @ref cusbd_start() 
 * is called. Otherwise behavior is undefined.
 * 
 * @param me Configuration descriptor to add to.
 * @param interface Interface descriptor to add. This cannot already 
 * be within another configuration descriptor.
 */
extern void cusb_configuration_add_interface(struct cusb_configuration *me,
                                             struct cusb_interface *interface);

/**
 * @pre @p me previously constructed via @ref cusb_configuration_ctor().
 * @pre @p string previously constructed via @ref cusb_string_ctor().
 * @brief Adds a string descriptor to the supplied configuration
 * descriptor.
 * 
 * @warning This must only be called on setup, before @ref cusbd_start() 
 * is called. Otherwise behavior is undefined.
 * @warning This can only be used if the USB device associated with
 * this descriptor has a string descriptor zero. I.e. a populated 
 * string0 was passed to @ref cusbd_ctor().
 * 
 * @param me Configuration descriptor to add to.
 * @param string String descriptor to add. This cannot already be within
 * another descriptor.
 */
extern void cusb_configuration_add_string(struct cusb_configuration *me,
                                          struct cusb_string *string);

/**
 * @pre @p me previously constructed via @ref cusb_configuration_ctor().
 * @brief Returns true if the supplied configuration descriptor contains
 * valid data and was properly constructed via @ref cusb_configuration_ctor(). 
 * False otherwise.
 * 
 * @param me Configuration descriptor to check.
 */
extern bool cusb_configuration_valid(const struct cusb_configuration *me);

/**
 * @pre @p me previously constructed via @ref cusb_configuration_ctor().
 * @brief Returns the number of interface descriptors attached to the
 * supplied configuration descriptor.
 * 
 * @param me Configuration descriptor to check.
 */
extern size_t cusb_configuration_interface_count(const struct cusb_configuration *me);

/**
 * @pre @p me previously constructed via @ref cusb_configuration_ctor().
 * @brief Returns number of bytes in the entire configuration 
 * descriptor's subtree. I.e. sizeof(configuration descriptor) + 
 * sizeof(all interface descriptors) + sizeof(all alternate interface descriptors) + ...
 * The size of the descriptor's data is used, NOT the size of the 
 * CUSB objects. The return value of this function is meant to be 
 * used to update wTotalLength in the configuration descriptor.
 * 
 * @warning This value is returned in native endianness, not
 * little endian.
 * 
 * @param me Configuration descriptor to check.
 */
extern size_t cusb_configuration_size(const struct cusb_configuration *me);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* CUSB_CONFIGURATION_H_ */
