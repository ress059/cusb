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
#include <stdint.h>

/* CUSB. */
#include "cusb/configuration.h"
#include "cusb/descriptors.h"
#include "cusb/string.h"

/* ECU. */
#include "ecu/attributes.h"
#include "ecu/dlist.h"
#include "ecu/endian.h"
#include "ecu/hsm.h"

/*------------------------------------------------------------*/
/*---------------------- DEFINES AND MACROS ------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Helper macro passed to @ref cusbd_ctor() if
 * device uses no string descriptors. If this is the case
 * then no cusbd_add_string() functions can be used and on
 * the device and no cusb_add_string() functions can be used 
 * on any of the device's descriptors.
 */
#define CUSBD_STRING_ZERO_UNUSED \
    ((const struct cusb_string_zero *)0)

/**
 * @brief Helper macro passed to @ref cusbd_ctor() if
 * object passed to user-defined endpoint functions is
 * unused.
 */
#define CUSBD_ENDPOINT_OBJ_UNUSED \
    ((void *)0)
    
/**
 * @brief Creates a @ref cusb_device_descriptor at either
 * compile-time or run-time. Example usage below creates
 * a USB 2.0 device with maximum endpoint0 packet size of 
 * 8 bytes, that is revision v00.0.1:
 * @code{.c}
 * static const struct cusb_device_descriptor device = CUSB_DEVICE_DESRIPTOR_CTOR(
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
#define CUSB_DEVICE_DESCRIPTOR_CTOR(bcdUSB_,                    \
                                    bDeviceClass_,              \
                                    bDeviceSubClass_,           \
                                    bDeviceProtocol_,           \
                                    bMaxPacketSize0_,           \
                                    idVendor_,                  \
                                    idProduct_,                 \
                                    bcdDevice_)                 \
    {                                                           \
        .bLength = sizeof(struct cusb_device_descriptor),       \
        .bDescriptorType = CUSB_DEVICE_DESCRIPTOR_TYPE,         \
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
/*------------------------- CUSB DEVICE ----------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Data in a standard device descriptor. Using
 * the API ensures this is always encoded in little
 * endian format.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusb_device_descriptor
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
 * @brief String index. I.e. iManufacturer, iProduct, iSerialNumber,
 * iConfiguration, iInterface, etc.
 */
enum cusbd_string_id
{
    CUSBD_MANUFACTURER_STRING_ID = 1,   /**<! ID given to all manufacturer strings. */
    CUSBD_PRODUCT_STRING_ID,            /**<! ID given to all product strings. */
    CUSBD_SERIAL_NUMBER_STRING_ID,      /**<! ID given to all serial number strings. */
    /*********************************/
    CUSBD_USER_STRING_ID_BEGIN          /**<! Strings attached to descriptors start at this ID. */
};

#pragma message("TODO: Device qualifier not handled for now.")
/**
 * @brief Object representing a USB device. This is the main
 * object that organizes all of the device's descriptors
 * and behavior.
 */
struct cusbd
{
    /// @brief USB device modeled as a hierarchical state machine.
    /// @warning MUST be first member since state machine
    /// framework requires inheritance.
    struct ecu_hsm hsm;

    /// @brief Descriptor data. A copy is stored so the API can
    /// automatically adjust iManufacturer, bNumConfigurations, etc
    /// as the device's descriptor tree is updated.
    /// @warning This struct is packed and will always be in 
    /// little endian.
    struct cusb_device_descriptor descriptor;

    /// @brief Device's string descriptor zero, which lists
    /// the languages this device supports. Optional. Equals 
    /// @ref CUSBD_STRING_ZERO_UNUSED if unused.
    /// @warning If this is unused the device can not use any
    /// string descriptors. This means no cusbd_add_string()
    /// functions can be used and no cusb_add_string() functions
    /// can be used on any of the device's descriptors.
    const struct cusb_string_zero *string0;

    /// @brief Configuration descriptors attached to this device.
    /// @warning Once the device is fully setup this must contain at 
    /// least 1 configuration since all devices must have at
    /// least one configuration descriptor.
    struct ecu_dlist configurations;

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

    /// @brief Dependency injection that links the CUSB device
    /// with user's hardware USB controller.
    struct 
    {
        /// @brief User-defined function that must configure the
        /// specified endpoint. For the control endpoint,
        /// id equals @ref CUSB_ENDPOINT0_OUT or @ref CUSB_ENDPOINT0_IN,
        /// type equals @ref CUSB_ENDPOINT_CONTROL_TYPE, and packet_size
        /// is derived from what was specified in the @ref cusb_device_descriptor
        /// supplied in @ref cusbd_ctor(). For all other endpoints,
        /// id equals the active endpoint's user-ID specified in @ref cusb_endpoint_ctor(),
        /// Type and packet_size are derived from the active
        /// endpoint's descriptor (@ref cusb_endpoint_descriptor) supplied 
        /// in @ref cusb_endpoint_ctor().
        /// Called during initial device enumeration or when
        /// the device's configuration/interface changes due to a
        /// SET_CONFIGURATION() or SET_INTERFACE() request.
        void (*configure)(endpoint_id_t id, enum cusb_endpoint_type type, uint16_t packet_size, void *obj);

        /// @brief User-defined function that is called when data
        /// needs to be sent to the host. The supplied data must be
        /// placed into the specified endpoint's (IN) buffer. len is 
        /// the number of bytes of data. id equals @ref CUSB_ENDPOINT0_IN 
        /// for the control endpoint. For all other endpoints,
        /// id equals the endpoint's user-ID specified in @ref cusb_endpoint_ctor().
        void (*post)(endpoint_id_t id, const void *data, size_t len, void *obj);

        /// @brief Optional object to pass to endpoint functions.
        /// Equals CUSBD_ENDPOINT_OBJ_UNUSED if unused.
        void *obj;
    } endpoint;
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
 * @pre @p descriptor previously constructed via @ref CUSB_DEVICE_DESCRIPTOR_CTOR().
 * @pre If used, @p string0 previously constructed via @ref CUSB_STRING_DESCRIPTOR_ZERO_CTOR().
 * @brief CUSB device constructor.
 * 
 * @warning This cannot be called on an active CUSB device.
 * Doing so is undefined behavior.
 * 
 * @param me CUSB device to construct.
 * @param descriptor The device descriptor associated with this CUSB device. 
 * @param string0 String descriptor zero associated with this CUSB device.
 * Optional. Supply @ref CUSBD_STRING_ZERO_UNUSED if unused. If this is unused,
 * the device can not use any string descriptors, meaning no cusbd_add_string()
 * functions can be called on this device and no cusb_add_string() functions
 * can be called on any of the device's descriptors.
 * @param configure User-defined function that configures the specified
 * endpoint. See @ref cusbd.endpoint.configure.
 * @param post User-defined function that places supplied data into
 * the specified endpoint's (IN) buffer. See @ref cusbd.endpoint.post.
 * @param obj Optional object to supply to @p configure and @p post
 * functions. Supply @ref CUSBD_ENDPOINT_OBJ_UNUSED if unused.
 */
extern void cusbd_ctor(struct cusbd *me,
                       const struct cusb_device_descriptor *descriptor,
                       const struct cusb_string_zero *string0,
                       void (*configure)(endpoint_id_t id, enum cusb_endpoint_type type, uint16_t packet_size, void *obj),
                       void (*post)(endpoint_id_t id, const void *data, size_t len, void *obj),
                       void *obj);
/**@}*/

/**
 * @name CUSBD Member Functions
 */
/**@{*/
/**
 * @pre @p me previously constructed via @ref cusbd_ctor().
 * @pre @p configuration previously constructed via @ref cusb_configuration_ctor().
 * @brief Adds a configuration descriptor to the CUSB device.
 * 
 * @warning This must only be called on setup, before @ref cusbd_start() 
 * is called. Otherwise behavior is undefined.
 * 
 * @param me CUSB device to add to.
 * @param configuration Configuration descriptor to add. This cannot 
 * already be within a CUSB device.
 */
extern void cusbd_add_configuration(struct cusbd *me,
                                    struct cusb_configuration *configuration);

/**
 * @pre @p me previously constructed via @ref cusbd_ctor().
 * @pre @p string previously constructed via @ref cusb_string_ctor().
 * @brief Adds a manufacturer string to the device.
 * 
 * @warning This must only be called on setup, before @ref cusbd_start() 
 * is called. Otherwise behavior is undefined.
 * @warning This can only be used if the device has a string descriptor
 * zero. I.e. a populated string0 was passed to @ref cusbd_ctor().
 * 
 * @param me CUSB device to add to.
 * @param string String descriptor to add. This cannot already
 * be within a CUSB device.
 */
extern void cusbd_add_manufacturer_string(struct cusbd *me,
                                          struct cusb_string *string);

/**
 * @pre @p me previously constructed via @ref cusbd_ctor().
 * @pre @p string previously constructed via @ref cusb_string_ctor().
 * @brief Adds a product string to the device.
 * 
 * @warning This must only be called on setup, before @ref cusbd_start() 
 * is called. Otherwise behavior is undefined.
 * @warning This can only be used if the device has a string descriptor
 * zero. I.e. a populated string0 was passed to @ref cusbd_ctor().
 * 
 * @param me CUSB device to add to.
 * @param string String descriptor to add. This cannot already
 * be within a CUSB device.
 */
extern void cusbd_add_product_string(struct cusbd *me,
                                     struct cusb_string *string);

/**
 * @pre @p me previously constructed via @ref cusbd_ctor().
 * @pre @p string previously constructed via @ref cusb_string_ctor().
 * @brief Adds a serial number string to the device.
 * 
 * @warning This must only be called on setup, before @ref cusbd_start() 
 * is called. Otherwise behavior is undefined.
 * @warning This can only be used if the device has a string descriptor
 * zero. I.e. a populated string0 was passed to @ref cusbd_ctor().
 * 
 * @param me CUSB device to add to.
 * @param string String descriptor to add. This cannot already
 * be within a CUSB device.
 */
extern void cusbd_add_serial_number_string(struct cusbd *me,
                                           struct cusb_string *string);

extern void cusbd_start(struct cusbd *me);
extern void cusbd_dispatch(struct cusbd *me, const void *event);
extern void cusbd_stop(struct cusbd *me);
/**@}*/


#ifdef __cplusplus
}
#endif

#endif /* CUSBD_H_ */
