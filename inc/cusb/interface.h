/**
 * @file
 * @brief Object representing an interface descriptor.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-05-16
 * @copyright Copyright (c) 2025
 */

#ifndef CUSB_INTERFACE_H_
#define CUSB_INTERFACE_H_

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <stdint.h>

/* CUSB. */
#include "cusb/descriptors.h"

/* ECU. */
#include "ecu/attributes.h"
#include "ecu/dlist.h"

/*------------------------------------------------------------*/
/*---------------------- DEFINES AND MACROS ------------------*/
/*------------------------------------------------------------*/

#define CUSB_INTERFACE_DESCRIPTOR_CTOR(bInterfaceClass_,        \
                                       bInterfaceSubClass_,     \
                                       bInterfaceProtocol_,     \
                                       iInterface_)             \
    {                                                           \
        .bLength = sizeof(struct cusb_interface_descriptor),    \
        .bDescriptorType = CUSB_INTERFACE_DESCRIPTOR_TYPE,      \
        .bInterfaceNumber = 0,                                  \
        .bAlternateSetting = 0,                                 \
        .bNumEndpoints = 0,                                     \
        .bInterfaceClass = (bInterfaceClass_),                  \
        .bInterfaceSubClass = (bInterfaceSubClass_),            \
        .bInterfaceProtocol = (bInterfaceProtocol_),            \
        .iInterface = (iInterface_)                             \
    }

/*------------------------------------------------------------*/
/*------------------------ CUSB INTERFACE --------------------*/
/*------------------------------------------------------------*/

struct cusb_interface_descriptor
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
    uint8_t bNumEndpoints;
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
    uint8_t iInterface;
} ECU_ATTRIBUTE_PACKED;

struct cusb_interface
{
    struct ecu_dnode dnode;
    struct cusb_interface_descriptor descriptor; // WARNING: PACKED!!
    struct ecu_dlist alternate_settings;
    struct ecu_dlist endpoints;
};

/*------------------------------------------------------------*/
/*--------------- CUSB INTERFACE MEMBER FUNCTIONS ------------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name CUSB Interface Constructors
 */
/**@{*/
extern void cusb_interface_ctor(struct cusb_interface *me,
                                const struct cusb_interface_descriptor *descriptor);

// return number of bytes of all DESCRIPTORS in the subtree size.
extern size_t cusb_interface_size(const struct cusb_interface *me);

// extern void cusb_interface_add_alternate_setting(struct cusb_interface *me,
//                                                  struct cusb_interface *alternate_setting);

// extern void cusb_interface_add_endpoint(struct cusb_interface *me,
//                                         struct cusb_endpoint *endpoint);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* CUSB_INTERFACE_H_ */
