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

#ifndef CUSB_DEVICE_H_
#define CUSB_DEVICE_H_

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <stdint.h>

/* CUSB. */
#include "cusb/configuration.h"
#include "cusb/descriptors.h"
#include "cusb/endpoint.h"

/* ECU. */
#include "ecu/attributes.h"
#include "ecu/dlist.h"
#include "ecu/endian.h"
#include "ecu/hsm.h"

/*------------------------------------------------------------*/
/*---------------------- DEFINES AND MACROS ------------------*/
/*------------------------------------------------------------*/

#define CUSB_DEVICE_DESCRIPTOR_CTOR(bcdUSB_,                    \
                                    bDeviceClass_,              \
                                    bDeviceSubClass_,           \
                                    bDeviceProtocol_,           \
                                    bMaxPacketSize_,            \
                                    idVendor_,                  \
                                    idProduct_,                 \
                                    bcdDevice_,                 \
                                    iManufacturer_,             \
                                    iProduct_,                  \
                                    iSerialNumber_)             \
    {                                                           \
        .bLength = sizeof(struct cusb_device_descriptor),       \
        .bDescriptorType = CUSB_DEVICE_DESCRIPTOR_TYPE,         \
        .bcdUSB = ECU_CPU_TO_LE16_COMPILETIME(bcdUSB_),         \
        .bDeviceClass = (bDeviceClass_),                        \
        .bDeviceSubClass = (bDeviceSubClass_),                  \
        .bDeviceProtocol = (bDeviceProtocol_),                  \
        .bMaxPacketSize = (bMaxPacketSize_),                    \
        .idVendor = ECU_CPU_TO_LE16_COMPILETIME(idVendor_),     \
        .idProduct = ECU_CPU_TO_LE16_COMPILETIME(idProduct_),   \
        .iManufacturer = (iManufacturer_),                      \
        .iProduct = (iProduct_),                                \
        .iSerialNumber = (iSerialNumber_),                      \
        .bNumConfigurations = 0                                 \
    }

/*------------------------------------------------------------*/
/*------------------------- CUSB DEVICE ----------------------*/
/*------------------------------------------------------------*/

struct cusb_device_descriptor
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdUSB;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bMaxPacketSize;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t iManufacturer;
    uint8_t iProduct;
    uint8_t iSerialNumber;
    uint8_t bNumConfigurations;
} ECU_ATTRIBUTE_PACKED;

struct cusb_device
{
    struct ecu_hsm hsm; // must be first.

    struct cusb_device_descriptor descriptor; // WARNING: PACKED!!
    struct ecu_dlist configurations;
    struct cusb_configuration config0;
};

/*------------------------------------------------------------*/
/*---------------- CUSB DEVICE MEMBER FUNCTIONS --------------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name CUSB Device Constructors
 */
/**@{*/
extern void cusb_device_ctor(struct cusb_device *me,
                             const struct cusb_device_descriptor *descriptor);
/**@}*/

/**
 * @name CUSB Device Member Functions
 */
/**@{*/
extern void cusb_device_add_configuration(struct cusb_device *me,
                                          struct cusb_configuration *configuration);

extern void cusb_device_add_interface(struct cusb_device *me,
                                      struct cusb_configuration *configuration, // config descriptor interface belongs to
                                      struct cusb_interface *interface);

extern void cusb_device_add_alternate_setting(struct cusb_device *me,
                                              struct cusb_interface *interface,
                                              struct cusb_interface *alternate_setting);

extern void cusb_device_add_endpoint(struct cusb_device *me,
                                     struct cusb_interface *interface, // interface descriptor endpoint belongs to.
                                     struct cusb_endpoint *endpoint);
/**@}*/


#ifdef __cplusplus
}
#endif

#endif /* CUSB_DEVICE_H_ */
