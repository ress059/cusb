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

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* Translation unit. */
#include "cusb/device.h"

/* STDLib. */
#include <stddef.h>
#include <string.h> /* memcpy. */

/* ECU. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_NAME("cusb/device.c")

/*------------------------------------------------------------*/
/*---------------------- STATIC ASSERTS ----------------------*/
/*------------------------------------------------------------*/

ECU_STATIC_ASSERT( (sizeof(struct cusb_device_descriptor) == (size_t)18),
                    "Device descriptor is 18 bytes." );

/*------------------------------------------------------------*/
/*---------------- CUSB DEVICE MEMBER FUNCTIONS --------------*/
/*------------------------------------------------------------*/

void cusb_device_ctor(struct cusb_device *me,
                      const struct cusb_device_descriptor *descriptor)
{
    (void)me;
    (void)descriptor;
    // config0.bConfigurationValue == 0.
}

void cusb_device_add_configuration(struct cusb_device *me,
                                   struct cusb_configuration *configuration)
{
    ECU_RUNTIME_ASSERT( (me && configuration) );

    // add to config descriptor linked list.
    // Update device_descriptor->bNumConfigurations. Use ecu_dlist_size().
    // Update config_descriptor->bConfigurationValue.

    // Think thats it?? All other fields like wTotalLength, bNumInterfaces, etc.
    // would be automatically upated in add_interface(), add_alternate_setings(), etc.

}

void cusb_device_add_interface(struct cusb_device *me,
                               struct cusb_configuration *configuration,
                               struct cusb_interface *interface)
{
    (void)me;
    (void)configuration;
    (void)interface;
}

void cusb_device_add_alternate_setting(struct cusb_device *me,
                                       struct cusb_interface *interface,
                                       struct cusb_interface *alternate_setting)
{

}

void cusb_device_add_endpoint(struct cusb_device *me,
                              struct cusb_interface *interface,
                              struct cusb_endpoint *endpoint)
{
    (void)me;
    (void)interface;
    (void)endpoint;
}
