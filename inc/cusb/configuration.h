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
#include <stdint.h>

/* CUSB. */
#include "cusb/descriptors.h"
#include "cusb/interface.h"

/* ECU. */
#include "ecu/attributes.h"
#include "ecu/dlist.h"
#include "ecu/endian.h"

/*------------------------------------------------------------*/
/*---------------------- DEFINES AND MACROS ------------------*/
/*------------------------------------------------------------*/

#define CUSB_CONFIGURATION_DESCRIPTOR_CTOR(iConfiguration_,                                         \
                                           bmAttributes_,                                           \
                                           bMaxPower_)                                              \
    {                                                                                               \
        .bLength = sizeof(struct cusb_configuration_descriptor),                                    \
        .bDescriptorType = CUSB_CONFIGURATION_DESCRIPTOR_TYPE,                                      \
        .wTotalLength = ECU_CPU_TO_LE16_COMPILETIME(sizeof(struct cusb_configuration_descriptor)),  \
        .bNumInterfaces = 1,                                                                        \
        .bConfigurationValue = 0,                                                                   \
        .iConfiguration = (iConfiguration_),                                                        \
        .bmAttributes = (bmAttributes_),                                                            \
        .bMaxPower = (bMaxPower_)                                                                   \
    }

/*------------------------------------------------------------*/
/*---------------------- CUSB CONFIGURATION ------------------*/
/*------------------------------------------------------------*/

struct cusb_configuration_descriptor
{
    /// @brief Number of byte sof this descriptor. Always 9.
    uint8_t bLength;

    /// @brief Type of descriptor. Always 0x02 == Configuration Descriptor.
    uint8_t bDescriptorType;

    /// @brief Total length in bytes of entire descriptor's subtree, including
    /// this configuration descriptor.
    uint16_t wTotalLength;

    /// @brief The number of interface descriptors attached to this
    /// configuration. Always >= 1 since all configuration descriptors
    // must have at least one interface descriptor.
    uint8_t bNumInterfaces;

    /// @brief Unique ID sent to host to identify this configuration.
    /// TODO:!! STARTS at 0 OR 1?????????????
    uint8_t bConfigurationValue;
    uint8_t iConfiguration;
    uint8_t bmAttributes;
    uint8_t bMaxPower;
} ECU_ATTRIBUTE_PACKED;

struct cusb_configuration
{
    struct ecu_dnode dnode;
    struct cusb_configuration_descriptor descriptor; // WARNING: PACKED!!
    struct ecu_dlist interfaces; // starts at interface0 and all other interfaces added on.
    struct cusb_interface interface0; // configuration always has at least one interface descriptor.
};

/*------------------------------------------------------------*/
/*------------ CUSB CONFIGURATION MEMBER FUNCTIONS -----------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name CUSB Endpoint Constructors
 */
/**@{*/
extern void cusb_configuration_ctor(struct cusb_configuration *me,
                                    const struct cusb_configuration_descriptor *descriptor);

// returns number of bytes of entire descriptor subtree.
extern size_t cusb_configuration_size(const struct cusb_configuration *me);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* CUSB_CONFIGURATION_H_ */
