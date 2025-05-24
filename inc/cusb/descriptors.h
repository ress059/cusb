/**
 * @file
 * @brief Contains common definitions that apply to all USB descriptors.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-05-17
 * @copyright Copyright (c) 2025
 */

#ifndef CUSB_DESCRIPTORS_H_
#define CUSB_DESCRIPTORS_H_

enum cusb_descriptor_type // set node id to this type so you know type when iterating over tree.
{
    CUSB_DEVICE_DESCRIPTOR_TYPE = 0x01,
    CUSB_CONFIGURATION_DESCRIPTOR_TYPE = 0x02,
    CUSB_STRING_DESCRIPTOR_TYPE = 0x03,
    CUSB_INTERFACE_DESCRIPTOR_TYPE = 0x04,
    CUSB_ENDPOINT_DESCRIPTOR_TYPE = 0x05,
    CUSB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE = 0x06,
    CUSB_OTHER_SPEED_CONFIGURATION_DESRIPTOR_TYPE = 0x07,
    CUSB_INTERFACE_POWER_DESCRIPTOR_TYPE = 0x08
};

#endif /* CUSB_DESCRIPTORS_H_ */
