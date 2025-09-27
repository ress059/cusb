/**
 * @file
 * @brief Object representing a configuration descriptor.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-05-16
 * @copyright Copyright (c) 2025
 */

#ifndef CUSBD_CONFIGURATION_H_
#define CUSBD_CONFIGURATION_H_

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <stdbool.h>
#include <stdint.h>

/* CUSB. */
#include "cusbd/descriptor.h"

/* ECU. */
#include "ecu/attributes.h"
#include "ecu/dlist.h"

/*------------------------------------------------------------*/
/*--------------------- CUSBD CONFIGURATION ------------------*/
/*------------------------------------------------------------*/

/* Forward declarations. */
struct cusbd_interface;
struct cusbd_string;

/**
 * @brief Data in a standard configuration descriptor.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_configuration_descriptor
{
    /// @brief Number of bytes of this descriptor.
    uint8_t bLength;

    /// @brief Descriptor type. Always 0x02.
    uint8_t bDescriptorType;

    /// @brief Total length in bytes of entire descriptor's subtree, including
    /// this configuration descriptor. Set when USB device first starts.
    uint16_t wTotalLength;

    /// @brief The number of interface descriptors attached to this
    /// configuration. Must always be >= 1 after device is fully setup
    /// since all configuration descriptors must have at least one 
    /// interface descriptor. Updated when interfaces added to configuration.
    uint8_t bNumInterfaces;

    /// @brief Unique ID sent to host to identify this configuration.
    /// Starts at 1. Assigned when configuration added to USB device.
    uint8_t bConfigurationValue;

    /// @brief Index of string descriptor describing this configuration.
    /// Strings are optional. Equals 0 if unused. Otherwise assigned
    /// when USB device first starts.
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
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_configuration
{
    /// @brief Inherit base descriptor class
    /// @warning MUST be first member.
    struct cusbd_descriptor base;

    /// @brief Descriptor data. A copy is stored so the API can
    /// automatically adjust it as the device is updated.
    /// @warning This struct is packed and will always be in 
    /// little endian.
    struct cusbd_configuration_descriptor descriptor;

    /// @brief String descriptors attached to this configuration 
    /// descriptor. Optional. Empty if unused.
    /// @warning If used, the device must have a string descriptor zero. 
    /// I.e. @ref cusbd.string0 must be populated.
    struct ecu_dlist strings;
};

/*------------------------------------------------------------*/
/*------------ CUSBD CONFIGURATION MEMBER FUNCTIONS ----------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Constructor
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @brief Configuration descriptor constructor.
 * 
 * @warning This cannot be called on an active configuration 
 * descriptor. Doing so is undefined behavior.
 * 
 * @param me Configuration descriptor to construct.
 * @param bmAttributes bmAttributes field of configuration descriptor.
 * See USB spec.
 * @param bMaxPower bMaxPower field of configuration descriptor.
 * See USB spec.
 */
extern void cusbd_configuration_ctor(struct cusbd_configuration *me,
                                     uint8_t bmAttributes,
                                     uint8_t bMaxPower);
/**@}*/

/**
 * @name Member Functions
 */
/**@{*/
/**
 * @pre @p me previously constructed via @ref cusbd_configuration_ctor().
 * @pre @p interface previously constructed via @ref cusbd_interface_ctor().
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
extern void cusbd_configuration_add_interface(struct cusbd_configuration *me,
                                              struct cusbd_interface *interface);

/**
 * @pre @p me previously constructed via @ref cusbd_configuration_ctor().
 * @pre @p string previously constructed via @ref cusbd_string_ctor().
 * @brief Adds a string descriptor to the supplied configuration
 * descriptor.
 * 
 * @warning This must only be called on setup, before @ref cusbd_start() 
 * is called. Otherwise behavior is undefined.
 * @warning This can only be used if the USB device associated with
 * this descriptor uses a string descriptor zero.
 * 
 * @param me Configuration descriptor to add to.
 * @param string String descriptor to add. This cannot already be within
 * another descriptor.
 */
extern void cusbd_configuration_add_string(struct cusbd_configuration *me,
                                           struct cusbd_string *string);

/**
 * @brief Returns true if the supplied configuration descriptor contains
 * valid data and was properly constructed via @ref cusbd_configuration_ctor(). 
 * False otherwise.
 * 
 * @param me Configuration descriptor to check.
 */
extern bool cusbd_configuration_valid(const struct cusbd_configuration *me);

#pragma message("TODO: Cleanup")
// /**
//  * @pre @p me previously constructed via @ref cusbd_configuration_ctor().
//  * @brief Returns the number of interface descriptors attached to the
//  * supplied configuration descriptor.
//  * 
//  * @param me Configuration descriptor to check.
//  */
// extern size_t cusbd_configuration_interface_count(const struct cusbd_configuration *me);

// /**
//  * @pre @p me previously constructed via @ref cusbd_configuration_ctor().
//  * @brief Returns number of bytes in the entire configuration 
//  * descriptor's subtree. I.e. sizeof(configuration descriptor) + 
//  * sizeof(all interface descriptors) + sizeof(all alternate interface descriptors) + ...
//  * The size of the descriptor's data is used, NOT the size of the 
//  * CUSBD objects. The return value of this function is meant to be 
//  * used to update wTotalLength in the configuration descriptor.
//  * 
//  * @warning This value is returned in native endianness, not
//  * little endian.
//  * 
//  * @param me Configuration descriptor to check.
//  */
// !!!! TODO Make return value uint16_t but use size_t in function to assert size <= UINT16_MAX
// extern size_t cusbd_configuration_size(const struct cusbd_configuration *me);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* CUSBD_CONFIGURATION_H_ */
