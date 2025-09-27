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

/* CUSB. */
#include "cusbd/descriptor.h"

/* ECU. */
#include "ecu/attributes.h"
#include "ecu/dlist.h"
#include "ecu/hsm.h"

/*------------------------------------------------------------*/
/*---------------------- DEFINES AND MACROS ------------------*/
/*------------------------------------------------------------*/

/// @brief Helper macro supplied to @ref cusbd_ctor() if
/// bcdDevice field in device descriptor is unused.
#define CUSBD_BCDDEVICE_UNUSED \
    ((uint16_t)0)

/// @brief Helper macro supplied to @ref cusbd_ctor() if
/// idProduct field in device descriptor is unused.
#define CUSBD_IDPRODUCT_UNUSED \
    ((uint16_t)0)

/// @brief Helper macro supplied to @ref cusbd_ctor() if
/// the device does not use string descriptor zero.
#define CUSBD_WLANGIDS_SIZE_UNUSED \
    ((uint8_t)0)

/// @brief Helper macro supplied to @ref cusbd_ctor() if
/// the device does not use string descriptor zero.
#define CUSBD_WLANGIDS_UNUSED \
    ((const uint16_t *)0)



// /**
//  * @brief Value of bDescriptorType in a standard
//  * devoce descriptor.
//  */
// #define CUSBD_BDESCRIPTORTYPE \
//     ((uint8_t)0x02)

// /**
//  * @brief Helper macro passed to @ref cusbd_ctor() if
//  * device uses no string descriptors. If this is the case
//  * then no cusbd_add_string() functions can be used and on
//  * the device and no cusbd_add_string() functions can be used 
//  * on any of the device's descriptors.
//  */
// #define CUSBD_STRING_ZERO_UNUSED \
//     ((const struct cusbd_string_zero *)0)

// /**
//  * @brief Passed to API if optional callback object(s) are unused.
//  */
// #define CUSBD_OBJ_UNUSED \
//     ((void *)0)

// /**
//  * @brief TODO:
//  */
// #define CUSBD_API_CTOR()
    
// /**
//  * @brief Creates a @ref cusbd_device_descriptor at either
//  * compile-time or run-time. Example usage below creates
//  * a USB 2.0 device with maximum endpoint0 packet size of 
//  * 8 bytes, that is revision v00.0.1:
//  * @code{.c}
//  * static const struct cusbd_device_descriptor device = CUSBD_DEVICE_DESCRIPTOR_CTOR(
//  *      0x0200, 0, 0, 8, 0, 0, 0x0001
//  * )
//  * @endcode
//  * 
//  * @warning This macro performs all necessary byte swapping 
//  * to store multi-byte values in little endian. Do not attempt 
//  * to send multi-byte values in little endian. Raw numbers 
//  * must be supplied.
//  * 
//  * @param bcdUSB_ USB version of this device in BCD format. See 
//  * USB spec.
//  * @param bDeviceClass_ Device's class code. See USB spec.
//  * @param bDeviceSubClass_ Device's subclass code. See USB spec.
//  * @param bDeviceProtocol_ Device's protocol code. See USB spec.
//  * @param bMaxPacketSize0_ Maximum packet size, in bytes, of endpoint0.
//  * See USB spec.
//  * @param idVendor_ Vendor's ID. Vendors register themselves with
//  * USB org and the USB org assigns them a unique ID to supply in
//  * this field.
//  * @param idProduct_ Device's product ID. Manufacturer-specific.
//  * @param bcdDevice_ Device's version in BCD format. Manufacturer-specific.
//  */
// #define CUSBD_DEVICE_DESCRIPTOR_CTOR(bcdUSB_,                   \
//                                      bDeviceClass_,             \
//                                      bDeviceSubClass_,          \
//                                      bDeviceProtocol_,          \
//                                      bMaxPacketSize0_,          \
//                                      idVendor_,                 \
//                                      idProduct_,                \
//                                      bcdDevice_)                \
//     {                                                           \
//         .bLength = sizeof(struct cusbd_device_descriptor),      \
//         .bDescriptorType = CUSBD_DESCRIPTOR_TYPE_DEVICE,        \
//         .bcdUSB = ECU_CPU_TO_LE16_COMPILETIME(bcdUSB_),         \
//         .bDeviceClass = (bDeviceClass_),                        \
//         .bDeviceSubClass = (bDeviceSubClass_),                  \
//         .bDeviceProtocol = (bDeviceProtocol_),                  \
//         .bMaxPacketSize0 = (bMaxPacketSize0_),                  \
//         .idVendor = ECU_CPU_TO_LE16_COMPILETIME(idVendor_),     \
//         .idProduct = ECU_CPU_TO_LE16_COMPILETIME(idProduct_),   \
//         .bcdDevice = ECU_CPU_TO_LE16_COMPILETIME(bcdDevice_),   \
//         .iManufacturer = 0,                                     \
//         .iProduct = 0,                                          \
//         .iSerialNumber = 0,                                     \
//         .bNumConfigurations = 0                                 \
//     }

/*------------------------------------------------------------*/
/*---------------------------- CUSBD -------------------------*/
/*------------------------------------------------------------*/

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
    /// Otherwise assigned when device starts up.
    uint8_t iManufacturer;

    /// @brief Index of string descriptor describing the device.
    /// Strings ae optional. Equals 0 if unused.
    /// Otherwise assigned when device starts up.
    uint8_t iProduct;

    /// @brief Index of string descriptor describing the device's.
    /// serial number. Strings ae optional. Equals 0 if unused.
    /// Otherwise assigned when device starts up.
    uint8_t iSerialNumber;

    /// @brief Number of configuration descriptors attached to
    /// this device. Must always be >= 1 after device is fully
    /// setup since all devices must have at least one configuration
    /// descriptor. Updated when configuration(s) added.
    uint8_t bNumConfigurations;
} ECU_ATTRIBUTE_PACKED;

/**
 * @brief Dependency injection. Links library with user's 
 * hardware-specific code that controls endpoint0 and the
 * USB device. Must be initialized at compile-time.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_api
{
    /// @brief Called when remote wakeup request event dispatched and
    /// device is capable of waking up host (remote wakeup enabled).
    void (*const device_remote_wakeup)(void *device_obj);

    /// @brief Called when a SET_ADDRESS() request is successfully processed.
    /// USB device's address must be set to the supplied value.
    void (*const device_set_address)(uint8_t address, void *device_obj);

    /// @brief Optional object to pass to device API.
    void *const device_obj;

    /// @brief Called when device first starts up. Endpoint0 must be
    /// configured with the supplied packet size. This value originates
    /// from the device descriptor.
    void (*const endpoint_zero_configure)(uint8_t bMaxPacketSize0, void *endpoint_zero_obj);

    /// @brief Called when user requests endpoint0 to be halted or when
    /// endpoint0 is halted from a SET_FEATURE() request.
    void (*const endpoint_zero_halt)(void *endpoint_zero_obj);

    /// @brief Called when the device must reply back to the host with
    /// a request error after processing the setup packet of a control transfer.
    void (*const endpoint_zero_stall)(void *endpoint_zero_obj);

    /// @brief Called when the device must send data back to the host
    /// after processing the setup packet of a control transfer.
    void (*const endpoint_zero_send)(const void *data, size_t len, void *endpoint_zero_obj);

    /// @brief Optional object to pass to endpoint0 API.
    void *const endpoint_zero_obj;
};

/**
 * @brief Object representing a USB device. This is the main
 * object that organizes all of the device's descriptors
 * and behavior.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd
{
    /// @brief Inherit base descriptor class
    /// @warning MUST be first member.
    struct cusbd_descriptor base;

    /// @brief Dependency injection. Links library with user's
    /// hardware-specific code that controls the device.
    const struct cusbd_api *api;

    /// @brief Address of device set by host in SET_ADDRESS().
    /// Resets to 0.
    uint8_t address;

    /// @brief Descriptor data. A copy is stored so the API can
    /// automatically adjust iManufacturer, bNumConfigurations, etc
    /// as the device's descriptor tree is updated.
    /// @warning This struct is packed and will always be in 
    /// little endian.
    struct cusbd_device_descriptor descriptor;

    /// @brief Device behavior modeled with hierarchical state machine.
    struct ecu_hsm hsm;

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

    /// @brief Device's string descriptor zero, which lists
    /// the languages this device supports. Optional.
    /// @warning If this is unused the device will not support
    /// any string descriptors.
    struct cusbd_string_zero string_zero;

    /// @brief True if device uses string descriptor zero.
    /// Otherwise false. If false this device will not
    /// support any string descriptors.
    bool string_zero_used;

    // /// @brief Currently active configuration set by a SET_CONFIGURATION()
    // /// request. NULL if no configuration active, meaning the device is
    // /// either in the Default or Address state.
    // struct cusbd_configuration *active_configuration;

    // /// @brief Part of device's status returned in GET_STATUS().
    // /// True = remote wakeup enabled. False = remote 
    // /// wakeup disabled. Updated in SET_FEATURE() and CLEAR_FEATURE().
    // /// Reset to 0 when device is reset or first starting up.
    // bool remote_wakeup;

    // /// @brief Part of device's status returned in GET_STATUS().
    // /// Set in constructor and updated by user during runtime.
    // /// True = device is currently self powered. 
    // /// False = device is currently bus powered.
    // bool self_powered;
};

/*------------------------------------------------------------*/
/*-------------------- CUSBD MEMBER FUNCTIONS ----------------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name CUSBD Constructor
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @brief USB device constructor.
 * 
 * @param me USB device to construct.
 * @param api See @ref cusbd_api. Collection of hardware-specific functions
 * that control the user's USB controller.
 * @param bcdUSB bcdUSB field of device descriptor. See USB spec.
 * Supply the raw value. Do not convert this into little endian.
 * @param bDeviceClass bDeviceClass field of device descriptor. See USB spec.
 * @param bDeviceSubClass bDeviceSubClass field of device descriptor. See USB spec.
 * @param bDeviceProtocol bDeviceProtocol field of device descriptor. See USB spec.
 * @param bMaxPacketSize0 bMaxPacketSize0 field of device descriptor. See USB spec.
 * @param idVendor idVendor field of device descriptor. See USB spec.
 * Supply the raw value. Do not convert this into little endian.
 * @param idProduct idProduct field of device descriptor. See USB spec.
 * Supply the raw value. Do not convert this into little endian.
 * Otherwise supply @ref CUSBD_IDPRODUCT_UNUSED if unused.
 * @param bcdDevice bcdDevice field of device descriptor. See USB spec.
 * Supply the raw value. Do not convert this into little endian.
 * Otherwise supply @ref CUSBD_BCDDEVICE_UNUSED if unused.
 * @param wLANGIDs Array of language ID codes this device supports,
 * which will be populated in this device's string descriptor zero.
 * See USB spec. Codes should be stored as raw values. Do not convert 
 * them into little endian. Supply @ref CUSBD_WLANGIDS_UNUSED if unused.
 * This means the device does not have a string descriptor zero and
 * will not support any string descriptors.
 * @param wLANGIDs_size Number of bytes of @p wLANGIDs array. This
 * is uint8_t because it must be able to be held in bLength.
 * Supply @ref CUSBD_WLANGIDS_SIZE_UNUSED if @p wLANGIDs is unused.
 */
extern void cusbd_ctor(struct cusbd *me,
                       const struct cusbd_api* api,
                       uint16_t bcdUSB,
                       uint8_t bDeviceClass,
                       uint8_t bDeviceSubClass,
                       uint8_t bDeviceProtocol,
                       uint8_t bMaxPacketSize0,
                       uint16_t idVendor,
                       uint16_t idProduct,
                       uint16_t bcdDevice,
                       const uint16_t *wLANGIDs,
                       uint8_t wLANGIDs_size);
/**@}*/

/**
 * @name CUSBD Member Functions
 */
/**@{*/
/**
 * @pre @p me previously constructed via @ref cusbd_ctor().
 * @pre @p configuration previously constructed via @ref cusbd_configuration_ctor().
 * @brief Adds a configuration descriptor to the USB device.
 * 
 * @warning This must only be called on setup, before @ref cusbd_start() 
 * is called. Otherwise behavior is undefined.
 * 
 * @param me USB device to add to.
 * @param configuration Configuration descriptor to add. This cannot 
 * already be within another USB device.
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
 * @warning This can only be used if the device uses a string descriptor
 * zero.
 * 
 * @param me USB device to add to.
 * @param string String descriptor to add. This cannot already
 * be within another USB device.
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
 * @warning This can only be used if the device uses a string descriptor
 * zero.
 * 
 * @param me USB device to add to.
 * @param string String descriptor to add. This cannot already
 * be within another USB device.
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
 * @warning This can only be used if the device uses a string descriptor
 * zero.
 * 
 * @param me USB device to add to.
 * @param string String descriptor to add. This cannot already
 * be within another USB device.
 */
extern void cusbd_add_serial_number_string(struct cusbd *me,
                                           struct cusbd_string *string);











                                           
extern void cusbd_dispatch(struct cusbd *me, const struct ecu_event *event);
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
