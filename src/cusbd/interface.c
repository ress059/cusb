/**
 * @file
 * @brief See @ref interface.h.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-05-16
 * @copyright Copyright (c) 2025
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* Translation unit. */
#include "cusbd/interface.h"

/* STDLib. */
#include <string.h> /* memcpy. */

/* CUSB. */
#include "cusbd/endpoint.h"
#include "cusbd/string.h"

/* ECU. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_NAME("cusbd/interface.c")

/*------------------------------------------------------------*/
/*--------------------------- DEFINES ------------------------*/
/*------------------------------------------------------------*/

#define BINTERFACECLASS_AUDIO ((uint8_t)0x01)
#define BINTERFACECLASS_CDC_CONTROL ((uint8_t)0x02)
#define BINTERFACECLASS_HID ((uint8_t)0x03)
#define BINTERFACECLASS_PHYSICAL ((uint8_t)0x05)
#define BINTERFACECLASS_IMAGE ((uint8_t)0x06)
#define BINTERFACECLASS_PRINTER ((uint8_t)0x07)
#define BINTERFACECLASS_MASS_STORAGE ((uint8_t)0x08)
#define BINTERFACECLASS_CDC_DATA ((uint8_t)0x0A)
#define BINTERFACECLASS_SMART_CARD ((uint8_t)0x0B)
#define BINTERFACECLASS_CONTENT_SECURITY ((uint8_t)0x0D)
#define BINTERFACECLASS_VIDEO ((uint8_t)0x0E)
#define BINTERFACECLASS_PERSONAL_HEALTHCARE ((uint8_t)0x0F)
#define BINTERFACECLASS_AUDIO_AND_VIDEO ((uint8_t)0x10)
#define BINTERFACECLASS_USBC_BRIDGE ((uint8_t)0x12)
#define BINTERFACECLASS_BULK_DISPLAY ((uint8_t)0x13)
#define BINTERFACECLASS_MCTP ((uint8_t)0x14)
#define BINTERFACECLASS_I3C ((uint8_t)0x3C)
#define BINTERFACECLASS_DIAGNOSTIC_DEVICE ((uint8_t)0xDC)
#define BINTERFACECLASS_WIRELESS_CONTROLLER ((uint8_t)0xE0)
#define BINTERFACECLASS_MISC ((uint8_t)0xEF)
#define BINTERFACECLASS_APP_SPECIFIC ((uint8_t)0xFE)
#define BINTERFACECLASS_VENDOR_SPECIFIC ((uint8_t)0xFF)

/*------------------------------------------------------------*/
/*----------- STATIC FUNCTION DECLARATIONS - COMMON ----------*/
/*------------------------------------------------------------*/

/**
 * @brief Returns true if the interface's class code is
 * one of the valid ones listed in https://www.usb.org/defined-class-codes.
 * False otherwise.
 */
static bool binterfaceclass_valid(uint8_t bInterfaceClass);

/*------------------------------------------------------------*/
/*---------- STATIC FUNCTION DECLARATIONS - INTERFACE --------*/
/*------------------------------------------------------------*/

/**
 * @brief Returns true if the supplied interface descriptor contains
 * valid data. False otherwise.
 * 
 * @note bInterfaceSubclass and bInterfaceProtocol are not
 * checked since these are class-specific. It is unreasonable
 * to check for every single possible combination. Instead,
 * validity for these are checked in the relevant
 * cusbd_interface_add_..() function.
 * 
 * @param descriptor Descriptor to check.
 */
static bool interface_descriptor_valid(const struct cusbd_interface_descriptor *descriptor);

/*------------------------------------------------------------*/
/*---- STATIC FUNCTION DECLARATIONS - ALTERNATE INTERFACE ----*/
/*------------------------------------------------------------*/

/**
 * @brief Returns true if the supplied interface descriptor contains
 * valid data. False otherwise.
 * 
 * @note bInterfaceSubclass and bInterfaceProtocol are not
 * checked since these are class-specific. It is unreasonable
 * to check for every single possible combination. Instead,
 * validity for these are checked in the relevant
 * cusbd_interface_add_..() function.
 * 
 * @param descriptor Descriptor to check.
 */
static bool alternate_interface_descriptor_valid(const struct cusbd_interface_descriptor *descriptor);

/*------------------------------------------------------------*/
/*----------- STATIC FUNCTION DEFINITIONS - COMMON -----------*/
/*------------------------------------------------------------*/

static bool binterfaceclass_valid(uint8_t bInterfaceClass)
{
    bool status = false;

    switch (bInterfaceClass)
    {
        /* Intentional fallthroughs. */
        case BINTERFACECLASS_AUDIO:
        case BINTERFACECLASS_CDC_CONTROL:
        case BINTERFACECLASS_HID:
        case BINTERFACECLASS_PHYSICAL:
        case BINTERFACECLASS_IMAGE:
        case BINTERFACECLASS_PRINTER:
        case BINTERFACECLASS_MASS_STORAGE:
        case BINTERFACECLASS_CDC_DATA:
        case BINTERFACECLASS_SMART_CARD:
        case BINTERFACECLASS_CONTENT_SECURITY:
        case BINTERFACECLASS_VIDEO:
        case BINTERFACECLASS_PERSONAL_HEALTHCARE:
        case BINTERFACECLASS_AUDIO_AND_VIDEO:
        case BINTERFACECLASS_USBC_BRIDGE:
        case BINTERFACECLASS_BULK_DISPLAY:
        case BINTERFACECLASS_MCTP:
        case BINTERFACECLASS_I3C:
        case BINTERFACECLASS_DIAGNOSTIC_DEVICE:
        case BINTERFACECLASS_WIRELESS_CONTROLLER:
        case BINTERFACECLASS_MISC:
        case BINTERFACECLASS_APP_SPECIFIC:
        case BINTERFACECLASS_VENDOR_SPECIFIC:
        {
            status = true;
            break;
        }

        default:
        {
            status = false;
            break;
        }
    }
    
    return status;
}

/*------------------------------------------------------------*/
/*---------- STATIC FUNCTION DEFINITIONS - INTERFACE ---------*/
/*------------------------------------------------------------*/

static bool interface_descriptor_valid(const struct cusbd_interface_descriptor *descriptor)
{
    bool status = false;
    ECU_RUNTIME_ASSERT( (descriptor) );

    /* Do not assert bInterfaceNumber, bNumEndpoints, and iInterface since 
    these are automatically updated when device starts. bInterfaceSubclass
    and bInterfaceProtocol are purposefully not checked. See function description. */
    if (descriptor->bLength == sizeof(struct cusbd_interface_descriptor) &&
        descriptor->bDescriptorType == (uint8_t)CUSBD_INTERFACE_BDESCRIPTORTYPE &&
        descriptor->bAlternateSetting == 0 &&
        binterfaceclass_valid(descriptor->bInterfaceClass))
    {
        status = true;
    }

    return status;
}

/*------------------------------------------------------------*/
/*----- STATIC FUNCTION DEFINITIONS - ALTERNATE INTERFACE ----*/
/*------------------------------------------------------------*/

static bool alternate_interface_descriptor_valid(const struct cusbd_interface_descriptor *descriptor)
{
    bool status = false;
    ECU_RUNTIME_ASSERT( (descriptor) );

    /* Do not assert bInterfaceNumber, bNumEndpoints, and iInterface since 
    these are automatically updated when device starts. bInterfaceSubclass
    and bInterfaceProtocol are purposefully not checked. See function description. */
    if (descriptor->bLength == sizeof(struct cusbd_interface_descriptor) &&
        descriptor->bDescriptorType == (uint8_t)CUSBD_INTERFACE_BDESCRIPTORTYPE &&
        descriptor->bAlternateSetting > 0 &&
        binterfaceclass_valid(descriptor->bInterfaceClass))
    {
        status = true;
    }

    return status;
}

/*------------------------------------------------------------*/
/*---------------------- STATIC ASSERTS ----------------------*/
/*------------------------------------------------------------*/

ECU_STATIC_ASSERT( (sizeof(struct cusbd_interface_descriptor) == (size_t)9),
                    "Interface descriptor is 9 bytes." );

/*------------------------------------------------------------*/
/*-------------- CUSBD INTERFACE MEMBER FUNCTIONS ------------*/
/*------------------------------------------------------------*/

void cusbd_interface_ctor(struct cusbd_interface *me, 
                          uint8_t bInterfaceClass,
                          uint8_t bInterfaceSubclass,
                          uint8_t bInterfaceProtocol)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (binterfaceclass_valid(bInterfaceClass)) );

    ecu_ntnode_ctor(&me->ntnode, ECU_NTNODE_DESTROY_UNUSED, (ecu_object_id)CUSBD_INTERFACE_BDESCRIPTORTYPE);
    me->descriptor.bLength = ECU_FIELD_SIZEOF(struct cusbd_interface, descriptor);
    me->descriptor.bDescriptorType = CUSBD_INTERFACE_BDESCRIPTORTYPE;
    me->descriptor.bInterfaceNumber = 0;
    me->descriptor.bAlternateSetting = 0;
    me->descriptor.bNumEndpoints = 0;
    me->descriptor.bInterfaceClass = bInterfaceClass;
    me->descriptor.bInterfaceSubClass = bInterfaceSubclass;
    me->descriptor.bInterfaceProtocol = bInterfaceProtocol;
    me->descriptor.iInterface = 0;
    me->alternate_setting = 0;
    ecu_dlist_ctor(&me->strings);
}

!!! TODO Stopped here !! dont know if i should update fields as descriptors are added or not.

void cusbd_interface_add_alternate_interface(struct cusbd_interface *me,
                                             struct cusbd_alternate_interface *alternate_interface)
{
    /* ECU library asserts if node is already within a tree. Necessary descriptor 
    contents (wTotalLength, bInterfaceNumber, bAlternateSetting) are not updated 
    here since that complexity is centralized to the cusbd_start() function. */
    ECU_RUNTIME_ASSERT( (me && alternate_interface) );
    ECU_RUNTIME_ASSERT( (cusbd_interface_valid(me)) );
    ECU_RUNTIME_ASSERT( (cusbd_alternate_interface_valid(alternate_interface)) );
    ecu_ntnode_push_child_back(&me->ntnode, &alternate_interface->ntnode);
}

void cusbd_interface_add_endpoint(struct cusbd_interface *me,
                                  struct cusbd_endpoint *endpoint)
{
    /* ECU library asserts if node is already within a tree. Necessary descriptor 
    contents (wTotalLength, bNumEndpoints) are not updated 
    here since that complexity is centralized to the cusbd_start() function. */
    ECU_RUNTIME_ASSERT( (me && endpoint) );
    ECU_RUNTIME_ASSERT( (cusbd_interface_valid(me)) );
    ECU_RUNTIME_ASSERT( (cusbd_endpoint_valid(endpoint)) );
    ecu_ntnode_push_child_back(&me->ntnode, &endpoint->ntnode);
}

void cusbd_interface_add_string(struct cusbd_interface *me,
                                struct cusbd_string *string)
{
    /* ECU library asserts if node is already within a list. */
    ECU_RUNTIME_ASSERT( (me && string) );
    ECU_RUNTIME_ASSERT( (cusbd_interface_valid(me)) );
    ECU_RUNTIME_ASSERT( (cusbd_string_valid(string)) );
    ecu_dlist_push_back(&me->strings, &string->dnode);
}

bool cusbd_interface_valid(const struct cusbd_interface *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return (ecu_ntnode_valid(&me->ntnode) &&
            ecu_ntnode_id(&me->ntnode) == (ecu_object_id)CUSBD_INTERFACE_BDESCRIPTORTYPE &&
            interface_descriptor_valid(&me->descriptor) &&
            ecu_dlist_valid(&me->strings));
}

/*------------------------------------------------------------*/
/*--------- CUSBD ALTERNATE INTERFACE MEMBER FUNCTIONS -------*/
/*------------------------------------------------------------*/

void cusbd_alternate_interface_ctor(struct cusbd_alternate_interface *me,
                                    const struct cusbd_interface_descriptor *descriptor)
{
    ECU_RUNTIME_ASSERT( (me && descriptor) );
    ECU_RUNTIME_ASSERT( (alternate_interface_descriptor_valid(descriptor)) );

    ecu_ntnode_ctor(&me->ntnode, ECU_NTNODE_DESTROY_UNUSED, (ecu_object_id)CUSBD_INTERFACE_BDESCRIPTORTYPE);
    memcpy(&me->descriptor, descriptor, sizeof(struct cusbd_interface_descriptor));
    ecu_dlist_ctor(&me->strings);
}

void cusbd_alternate_interface_add_endpoint(struct cusbd_alternate_interface *me,
                                            struct cusbd_endpoint *endpoint)
{
    /* ECU library asserts if node is already within a tree. Necessary descriptor 
    contents (wTotalLength, bNumEndpoints) are not updated 
    here since that complexity is centralized to the cusbd_start() function. */
    ECU_RUNTIME_ASSERT( (me && endpoint) );
    ECU_RUNTIME_ASSERT( (cusbd_alternate_interface_valid(me)) );
    ECU_RUNTIME_ASSERT( (cusbd_endpoint_valid(endpoint)) );
    ecu_ntnode_push_child_back(&me->ntnode, &endpoint->ntnode);
}

void cusbd_alternate_interface_add_string(struct cusbd_alternate_interface *me,
                                          struct cusbd_string *string)
{
    /* ECU library asserts if node is already within a list. */
    ECU_RUNTIME_ASSERT( (me && string) );
    ECU_RUNTIME_ASSERT( (cusbd_alternate_interface_valid(me)) );
    ECU_RUNTIME_ASSERT( (cusbd_string_valid(string)) );
    ecu_dlist_push_back(&me->strings, &string->dnode);
}

bool cusbd_alternate_interface_valid(const struct cusbd_alternate_interface *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    return (ecu_ntnode_valid(&me->ntnode) &&
            ecu_ntnode_id(&me->ntnode) == (ecu_object_id)CUSBD_INTERFACE_BDESCRIPTORTYPE &&
            alternate_interface_descriptor_valid(&me->descriptor) &&
            ecu_dlist_valid(&me->strings));
}
