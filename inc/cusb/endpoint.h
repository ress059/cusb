/**
 * @file
 * @brief Object representing an endpoint descriptor.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-05-16
 * @copyright Copyright (c) 2025
 */

#ifndef CUSB_ENDPOINT_H_
#define CUSB_ENDPOINT_H_

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
#include "ecu/endian.h"

/*------------------------------------------------------------*/
/*---------------------- DEFINES AND MACROS ------------------*/
/*------------------------------------------------------------*/

#define CUSB_ENDPOINT_DESCRIPTOR_CTOR(bEndpointAddress_,                                \
                                      bmAttributes_,                                    \
                                      wMaxPacketSize_,                                  \
                                      bInterval_)                                       \
    {                                                                                   \
        .bLength = sizeof(struct cusb_endpoint_descriptor),                        \
        .bDescriptorType = ECU_CPU_TO_LE16_COMPILTIME(CUSB_ENDPOINT_DESCRIPTOR_TYPE),   \
        .bEndpointAddress = (bEndpointAddress_),                                        \
        .bmAttributes = (bmAttributes_),                                                \
        .wMaxPacketSize = (wMaxPacketSize_),                                            \
        .bInterval = (bInterval_)                                                       \
    }

/*------------------------------------------------------------*/
/*------------------------ CUSB ENDPOINT ---------------------*/
/*------------------------------------------------------------*/

struct cusb_endpoint_descriptor
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bEndpointAddress;
    uint8_t bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t bInterval;
} ECU_ATTRIBUTE_PACKED;

struct cusb_endpoint
{
    struct ecu_dnode dnode;
    struct cusb_endpoint_descriptor descriptor; // WARNING: PACKED!!
};

/*------------------------------------------------------------*/
/*----------------- CUSB ENDPOINT MEMBER FUNCTIONS -----------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name CUSB Endpoint Constructors
 */
/**@{*/
extern void cusb_endpoint_ctor(struct cusb_endpoint *me,
                               const struct cusb_endpoint_descriptor *descriptor);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* CUSB_ENDPOINT_H_ */
