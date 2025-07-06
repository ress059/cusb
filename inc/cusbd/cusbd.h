/**
 * @file
 * @brief Object representing USB device. Device descriptor
 * is contained within this object.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-05-16
 * @copyright Copyright (c) 2025
 */

#ifndef CUSBD_H_
#define CUSBD_H_

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <stdbool.h>
#include <stdint.h>

/* CUSB. Include all headers so user only includes cusbd.h. */
#include "cusbd/configuration.h"
#include "cusbd/descriptor.h"
#include "cusbd/endpoint.h"
#include "cusbd/event.h"
#include "cusbd/interface.h"
#include "cusbd/string.h"

/* ECU. */
#include "ecu/attributes.h"
#include "ecu/dlist.h"
#include "ecu/endian.h"

/*------------------------------------------------------------*/
/*---------------------- DEFINES AND MACROS ------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Value of bDescriptorType in a standard
 * devoce descriptor.
 */
#define CUSBD_BDESCRIPTORTYPE \
    ((uint8_t)0x02)

/**
 * @brief Helper macro passed to @ref cusbd_ctor() if
 * device uses no string descriptors. If this is the case
 * then no cusbd_add_string() functions can be used and on
 * the device and no cusbd_add_string() functions can be used 
 * on any of the device's descriptors.
 */
#define CUSBD_STRING_ZERO_UNUSED \
    ((const struct cusbd_string_zero *)0)

/**
 * @brief Passed to API if optional callback object(s) are unused.
 */
#define CUSBD_OBJ_UNUSED \
    ((void *)0)
    
/**
 * @brief Creates a @ref cusbd_device_descriptor at either
 * compile-time or run-time. Example usage below creates
 * a USB 2.0 device with maximum endpoint0 packet size of 
 * 8 bytes, that is revision v00.0.1:
 * @code{.c}
 * static const struct cusbd_device_descriptor device = CUSBD_DEVICE_DESRIPTOR_CTOR(
 *      0x0200, 0, 0, 8, 0, 0, 0x0001
 * )
 * @endcode
 * 
 * @warning This macro performs all necessary byte swapping 
 * to store multi-byte values in little endian. Do not attempt 
 * to send multi-byte values in little endian. Raw numbers 
 * must be supplied.
 * 
 * @param bcdUSB_ USB version of this device in BCD format. See 
 * USB spec.
 * @param bDeviceClass_ Device's class code. See USB spec.
 * @param bDeviceSubClass_ Device's subclass code. See USB spec.
 * @param bDeviceProtocol_ Device's protocol code. See USB spec.
 * @param bMaxPacketSize0_ Maximum packet size, in bytes, of endpoint0.
 * See USB spec.
 * @param idVendor_ Vendor's ID. Vendors register themselves with
 * USB org and the USB org assigns them a unique ID to supply in
 * this field.
 * @param idProduct_ Device's product ID. Manufacturer-specific.
 * @param bcdDevice_ Device's version in BCD format. Manufacturer-specific.
 */
#define CUSBD_DEVICE_DESCRIPTOR_CTOR(bcdUSB_,                   \
                                     bDeviceClass_,             \
                                     bDeviceSubClass_,          \
                                     bDeviceProtocol_,          \
                                     bMaxPacketSize0_,          \
                                     idVendor_,                 \
                                     idProduct_,                \
                                     bcdDevice_)                \
    {                                                           \
        .bLength = sizeof(struct cusbd_device_descriptor),      \
        .bDescriptorType = CUSBD_DESCRIPTOR_TYPE_DEVICE,        \
        .bcdUSB = ECU_CPU_TO_LE16_COMPILETIME(bcdUSB_),         \
        .bDeviceClass = (bDeviceClass_),                        \
        .bDeviceSubClass = (bDeviceSubClass_),                  \
        .bDeviceProtocol = (bDeviceProtocol_),                  \
        .bMaxPacketSize0 = (bMaxPacketSize0_),                  \
        .idVendor = ECU_CPU_TO_LE16_COMPILETIME(idVendor_),     \
        .idProduct = ECU_CPU_TO_LE16_COMPILETIME(idProduct_),   \
        .bcdDevice = ECU_CPU_TO_LE16_COMPILETIME(bcdDevice_),   \
        .iManufacturer = 0,                                     \
        .iProduct = 0,                                          \
        .iSerialNumber = 0,                                     \
        .bNumConfigurations = 0                                 \
    }

/*------------------------------------------------------------*/
/*---------------------------- CUSBD -------------------------*/
/*------------------------------------------------------------*/

// /**
//  * @brief String index. I.e. iManufacturer, iProduct, iSerialNumber,
//  * iConfiguration, iInterface, etc.
//  */
// enum cusbd_string_id
// {
//     CUSBD_MANUFACTURER_STRING_ID = 1,   /**< ID given to all manufacturer strings. */
//     CUSBD_PRODUCT_STRING_ID,            /**< ID given to all product strings. */
//     CUSBD_SERIAL_NUMBER_STRING_ID,      /**< ID given to all serial number strings. */
//     /*********************************/
//     CUSBD_USER_STRING_ID_BEGIN          /**< Strings attached to descriptors start at this ID. */
// };

/**
 * @brief Data in a standard device descriptor. Using
 * the API ensures this is always encoded in little
 * endian format.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_device_descriptor
{
    /// @brief Number of bytes of this descriptor.
    uint8_t bLength;

    /// @brief Descriptor type. Always 0x01 == Device Descriptor.
    uint8_t bDescriptorType;

    /// @brief USB version of this device in BCD format. See USB spec.
    uint16_t bcdUSB;

    /// @brief Device's class code. See USB spec.
    uint8_t bDeviceClass;

    /// @brief Device's subclass code. See USB spec.
    uint8_t bDeviceSubClass;

    /// @brief Device's protocol code. See USB spec.
    uint8_t bDeviceProtocol;

    /// @brief Maximum packet size, in bytes, of endpoint0.
    uint8_t bMaxPacketSize0;

    /// @brief Vendors register themselves with USB org and the 
    /// USB org assigns them a unique ID to supply in this field.
    uint16_t idVendor;

    /// @brief Manufacturer-specific product ID.
    uint16_t idProduct;

    /// @brief Device's version in BCD format. Manufacturer-specific.
    uint16_t bcdDevice;

    /// @brief Index of string descriptor describing the device's
    /// manufacturer. Strings ae optional. Equals 0 if unused.
    uint8_t iManufacturer;

    /// @brief Index of string descriptor describing the device.
    /// Strings ae optional. Equals 0 if unused.
    uint8_t iProduct;

    /// @brief Index of string descriptor describing the device's.
    /// serial number. Strings ae optional. Equals 0 if unused.
    uint8_t iSerialNumber;

    /// @brief Number of configuration descriptors attached to
    /// this device. Must always be >= 1 after device is fully
    /// setup since all devices must have at least one configuration
    /// descriptor.
    uint8_t bNumConfigurations;
} ECU_ATTRIBUTE_PACKED;

/**
 * @brief Object representing a USB device. This is the main
 * object that organizes all of the device's descriptors
 * and behavior.
 */
struct cusbd
{
    /// @brief Inherit @ref cusbd_descriptor base class.
    /// @warning MUST be first member.
    struct cusbd_descriptor base;

    /// @brief Descriptor data. A copy is stored so the API can
    /// automatically adjust iManufacturer, bNumConfigurations, etc
    /// as the device's descriptor tree is updated.
    /// @warning This struct is packed and will always be in 
    /// little endian.
    struct cusbd_device_descriptor descriptor;

    /// @brief Device's string descriptor zero, which lists
    /// the languages this device supports. Optional. Equals 
    /// @ref CUSBD_STRING_ZERO_UNUSED if unused.
    /// @warning If this is unused the device can not use any
    /// string descriptors. This means no cusbd_add_string()
    /// functions can be used and no cusbd_add_string() functions
    /// can be used on any of the device's descriptors.
    const struct cusbd_string_zero *string0;

    /// @brief All manufacturer strings associated with this
    /// device. iManufacturer. Optional. Empty if unused.
    /// @warning Device must use string0 if this is used.
    struct ecu_dlist manufacturer_strings;

    /// @brief All product strings associated with this
    /// device. iProduct. Optional. Empty if unused.
    /// @warning Device must use string0 if this is used.
    struct ecu_dlist product_strings;

    /// @brief All serial number strings associated with this
    /// device. iSerialNumber. Optional. Empty if unused.
    /// @warning Device must use string0 if this is used.
    struct ecu_dlist serial_number_strings;

    /// @brief True = remote wakeup enabled. 
    /// False = remote wakeup disabled.
    bool remote_wakeup;

    /// @brief bConfigurationValue of the currently active 
    /// configuration. Must be 0 if device is unconfigured.
    uint8_t configuration_value;

    /// @brief Dependency injection. Links library with hardware-specific 
    /// code controlling the USB device controller.
    struct
    {
        /// @brief Called when the characteristics of an endpoint
        /// must change due to a SET_CONFIGURATION() or SET_INTERFACE()
        /// being processed. The endpoint's existing and new characteristics
        /// can be retrieved using the cusbd_endpoint() API. 
        void (*ep_configure)(const struct cusbd_endpoint *me, void *obj);

        /// @brief Called when a SET_ADDRESS() request is successfully processed.
        /// User must set the USB device's address to the value supplied.
        void (*set_address)(uint8_t address, void *obj);

        /// @brief Optional object passed into API functions above.
        void *obj;
    } device;

    /// @brief Dependency injection. Links library with hardware-specific
    /// code controlling endpoint0.
    struct
    {
        /// @brief Called when device must send data back to host during
        /// enumeration.
        void (*send)(const void *data, size_t len, void *obj);

        /// @brief Called when request is processed. Function indicates
        /// whether ACK, NAK, or STALL should be sent back to the host.
        /// This should be done in either Data or Status stage.
        void (*handshake)(enum cusbd_endpoint_status status, void *obj);

        /// @brief Optional object passed into API functions above.
        void *obj;
    } ep0;
};

/*------------------------------------------------------------*/
/*-------------------- CUSBD MEMBER FUNCTIONS ----------------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name CUSBD Constructors
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @pre @p descriptor previously constructed via @ref CUSBD_DEVICE_DESCRIPTOR_CTOR().
 * @pre If used, @p string0 previously constructed via @ref CUSBD_STRING_DESCRIPTOR_ZERO_CTOR().
 * @brief CUSBD device constructor.
 * 
 * @warning This cannot be called on an active CUSBD device.
 * Doing so is undefined behavior.
 * 
 * @param me CUSBD device to construct.
 * @param descriptor The device descriptor associated with this CUSBD device. 
 * @param string0 String descriptor zero associated with this CUSBD device.
 * Optional. Supply @ref CUSBD_STRING_ZERO_UNUSED if unused. If this is unused,
 * the device can not use any string descriptors, meaning no cusbd_add_string()
 * functions can be called on this device and no cusbd_add_string() functions
 * can be called on any of the device's descriptors.
 * @param configure User-defined function that configures the specified
 * endpoint. See @ref cusbd.endpoint.configure.
 * @param post User-defined function that places supplied data into
 * the specified endpoint's (IN) buffer. See @ref cusbd.endpoint.post.
 * @param obj Optional object to supply to @p configure and @p post
 * functions. Supply @ref CUSBD_ENDPOINT_OBJ_UNUSED if unused.
 */
// extern void cusbd_ctor(struct cusbd *me,
//                        const struct cusbd_device_descriptor *descriptor,
//                        const struct cusbd_string_zero *string0,
//                        void (*configure)(cusbd_endpoint_id_t id, enum cusbd_endpoint_type type, uint16_t packet_size, void *obj),
//                        void (*post)(cusbd_endpoint_id_t id, const void *data, size_t len, void *obj),
//                        void *obj);
/**@}*/

/**
 * @name CUSBD Member Functions
 */
/**@{*/
/**
 * @pre @p me previously constructed via @ref cusbd_ctor().
 * @pre @p configuration previously constructed via @ref cusbd_configuration_ctor().
 * @brief Adds a configuration descriptor to the CUSBD device.
 * 
 * @warning This must only be called on setup, before @ref cusbd_start() 
 * is called. Otherwise behavior is undefined.
 * 
 * @param me CUSBD device to add to.
 * @param configuration Configuration descriptor to add. This cannot 
 * already be within a CUSBD device.
 */
extern void cusbd_add_configuration(struct cusbd *me,
                                    struct cusbd_configuration *configuration);

/**
 * @pre @p me previously constructed via @ref cusbd_ctor().
 * @pre @p string previously constructed via @ref cusbd_string_ctor().
 * @brief Adds a manufacturer string to the device.
 * 
 * @warning This must only be called on setup, before @ref cusbd_start() 
 * is called. Otherwise behavior is undefined.
 * @warning This can only be used if the device has a string descriptor
 * zero. I.e. a populated string0 was passed to @ref cusbd_ctor().
 * 
 * @param me CUSBD device to add to.
 * @param string String descriptor to add. This cannot already
 * be within a CUSBD device.
 */
extern void cusbd_add_manufacturer_string(struct cusbd *me,
                                          struct cusbd_string *string);

/**
 * @pre @p me previously constructed via @ref cusbd_ctor().
 * @pre @p string previously constructed via @ref cusbd_string_ctor().
 * @brief Adds a product string to the device.
 * 
 * @warning This must only be called on setup, before @ref cusbd_start() 
 * is called. Otherwise behavior is undefined.
 * @warning This can only be used if the device has a string descriptor
 * zero. I.e. a populated string0 was passed to @ref cusbd_ctor().
 * 
 * @param me CUSBD device to add to.
 * @param string String descriptor to add. This cannot already
 * be within a CUSBD device.
 */
extern void cusbd_add_product_string(struct cusbd *me,
                                     struct cusbd_string *string);

/**
 * @pre @p me previously constructed via @ref cusbd_ctor().
 * @pre @p string previously constructed via @ref cusbd_string_ctor().
 * @brief Adds a serial number string to the device.
 * 
 * @warning This must only be called on setup, before @ref cusbd_start() 
 * is called. Otherwise behavior is undefined.
 * @warning This can only be used if the device has a string descriptor
 * zero. I.e. a populated string0 was passed to @ref cusbd_ctor().
 * 
 * @param me CUSBD device to add to.
 * @param string String descriptor to add. This cannot already
 * be within a CUSBD device.
 */
extern void cusbd_add_serial_number_string(struct cusbd *me,
                                           struct cusbd_string *string);

extern void cusbd_dispatch(struct cusbd *me, const void *event);
extern void cusbd_start(struct cusbd *me);
extern void cusbd_stop(struct cusbd *me);

/**
 * @brief Returns true if the supplied device contains
 * valid data and was properly constructed via @ref cusbd_ctor(). 
 * False otherwise.
 * 
 * @param me Device to check.
 */
extern bool cusbd_valid(const struct cusbd *me);
/**@}*/


#ifdef __cplusplus
}
#endif

#endif /* CUSBD_H_ */
