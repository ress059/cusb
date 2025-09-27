/**
 * @file
 * @brief See @ref configuration.h.
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
#include "cusbd/configuration.h"

/* STDLib. */
#include <stddef.h>
#include <string.h>

/* CUSB. */
#include "cusbd/descriptor_private.h"
#include "cusbd/interface.h"
#include "cusbd/string.h"

/* ECU. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_FILE("cusbd/configuration.c")

/*------------------------------------------------------------*/
/*-------------------------- DEFINES -------------------------*/
/*------------------------------------------------------------*/

/// @brief bDescriptorType value of configuration descriptors.
#define BDESCRIPTORTYPE_CONFIGURATION \
    ((uint8_t)0x02)

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Returns true if data in configuration descriptor is
 * valid. False otherwise.
 * 
 * @param descriptor Descriptor to check.
 */
static bool configuration_descriptor_valid(const struct cusbd_configuration_descriptor *descriptor);

/**@{*/
/// @brief Overrides for descriptors virtual table. See @ref cusbd_descriptor_vtable
static bool v_assign_string_index(struct cusbd_configuration *me, uint8_t index);
static bool v_get_descriptor(struct cusbd_configuration *me, void *buffer, size_t bytes_remaining);


static size_t v_get_string_descriptor(const struct cusbd_configuration *me, 
                                      uint8_t index, 
                                      uint16_t wLANGID, 
                                      void *buffer, 
                                      size_t buffer_len);
static size_t v_w_total_length(const struct cusbd_configuration *me)
/**@}*/

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
/*------------------------------------------------------------*/

static bool configuration_descriptor_valid(const struct cusbd_configuration_descriptor *descriptor)
{
    bool status = false;
    ECU_ASSERT( (descriptor) );

    /* Do not assert wTotalLength, bNumInterfaces, bConfigurationValue, and 
    iConfiguration since these are automatically updated. Do not
    assert bmAttributes due to its complexity. Do not assert bmAttributes
    and bMaxPower since they may be different across different USB standards. */
    if (descriptor->bLength == sizeof(*descriptor) &&
        descriptor->bDescriptorType == BDESCRIPTORTYPE_CONFIGURATION)
    {
        status = true;
    }

    return status;
}

static bool v_assign_string_index(struct cusbd_configuration *me, uint8_t index)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (cusbd_configuration_valid(me)) );
    ECU_ASSERT( (index > 0) ); /* 0 is only for string descriptor zero. */
    bool status = false;

    if (!ecu_dlist_empty(&me->strings))
    {
        me->descriptor.iConfiguration = index;
        status = true;
    }

    return status;
}

static size_t v_get_descriptor(struct cusbd_configuration *me, void *buffer, size_t bytes_remaining)
{
    
}


static size_t v_get_string_descriptor(const struct cusbd_configuration *me, 
                                      uint8_t index, 
                                      uint16_t wLANGID, 
                                      void *buffer, 
                                      size_t buffer_len)
{
    ECU_ASSERT( (me && buffer) );
    ECU_ASSERT( (cusbd_configuration_valid(me)) );
    /* Do not assert index > 0 since this request can be polled. */
    uint8_t iConfiguration = me->descriptor.iConfiguration;
    size_t num_bytes_written = 0;

    /* Queried index is for strings in this configuration? */
    if ((iConfiguration > 0) &&
        (iConfiguration == index))
    {
        struct ecu_dlist_citerator citerator;

        /* Find string in this configuration that supports the queried language if it exists. */
        ECU_DLIST_CONST_FOR_EACH(dnode, &citerator, &me->strings)
        {
            const struct cusbd_string *string = CUSBD_STRING_GET_CONST_ENTRY(dnode);

            if (cusbd_string_w_lang_id(string) == wLANGID)
            {
                num_bytes_written = cusbd_string_send(string, buffer, buffer_len);
                ECU_ASSERT( (num_bytes_written) ); /* Fails if buffer is too small to hold descriptor. */
                break;
            }
        }
    }

    return num_bytes_written;
}

static size_t v_w_total_length(const struct cusbd_configuration *me)
{
    return (sizeof(me->descriptor));
}



/*------------------------------------------------------------*/
/*---------------------- STATIC ASSERTS ----------------------*/
/*------------------------------------------------------------*/

ECU_STATIC_ASSERT( (sizeof(struct cusbd_configuration_descriptor) == (size_t)9),
                    "Configuration descriptor is 9 bytes." );

ECU_STATIC_ASSERT( (ECU_FIELD_SIZEOF(struct cusbd_configuration, descriptor) == (size_t)9),
                    "Configuration descriptor is 9 bytes." );

/*------------------------------------------------------------*/
/*------------ CUSBD CONFIGURATION MEMBER FUNCTIONS ----------*/
/*------------------------------------------------------------*/

void cusbd_configuration_ctor(struct cusbd_configuration *me,
                              uint8_t bmAttributes,
                              uint8_t bMaxPower)
{
    ECU_ASSERT( (me) );
    /* Do not assert bmAttributes and bMaxPower since they may be different across different USB standards. */

    static const struct cusbd_descriptor_vtable vtable = CUSBD_DESCRIPTOR_VTABLE_CTOR(
        &v_assign_string_index, &v_get_descriptor, &v_w_total_length
    );

    cusbd_descriptor_ctor(CUSBD_DESCRIPTOR_BASE_CAST(me), CUSBD_DESCRIPTOR_ID_CONFIGURATION);
    me->base.vptr = &vtable;
    memset(&me->descriptor, 0, sizeof(me->descriptor));
    me->descriptor.bLength = sizeof(me->descriptor);
    me->descriptor.bDescriptorType = BDESCRIPTORTYPE_CONFIGURATION;
    me->descriptor.bmAttributes = bmAttributes;
    me->descriptor.bMaxPower = bMaxPower;
    ecu_dlist_ctor(&me->strings);
}

void cusbd_configuration_add_interface(struct cusbd_configuration *me,
                                       struct cusbd_interface *interface)
{
    /* ECU library asserts if node is already within a tree. */
    ECU_ASSERT( (me && interface) );
    ECU_ASSERT( (cusbd_configuration_valid(me)) );
    ECU_ASSERT( (cusbd_interface_valid(interface)) );

    struct ecu_ntnode *configuration_ntnode = cusbd_descriptor_ntnode(CUSBD_DESCRIPTOR_BASE_CAST(me));
    struct ecu_ntnode *interface_ntnode = cusbd_descriptor_ntnode(CUSBD_DESCRIPTOR_BASE_CAST(interface));
    ecu_ntnode_push_child_back(configuration_ntnode, interface_ntnode);
    me->descriptor.bNumInterfaces++;
}

void cusbd_configuration_add_string(struct cusbd_configuration *me,
                                    struct cusbd_string *string)
{
    /* ECU library asserts if node is already within a list. iConfiguration
    updated in start() function to centralize complexity. */
    ECU_ASSERT( (me && string) );
    ECU_ASSERT( (cusbd_configuration_valid(me)) );
    ECU_ASSERT( (cusbd_string_valid(string)) );
    ecu_dlist_push_back(&me->strings, cusbd_string_dnode(string));
}

bool cusbd_configuration_valid(const struct cusbd_configuration *me)
{
    ECU_ASSERT( (me) );
    bool status = false;

    if ((cusbd_descriptor_valid(CUSBD_DESCRIPTOR_BASE_CAST(me))) &&
        (cusbd_descriptor_id(CUSBD_DESCRIPTOR_BASE_CAST(me)) == CUSBD_DESCRIPTOR_ID_CONFIGURATION) &&
        (configuration_descriptor_valid(&me->descriptor)) &&
        (ecu_dlist_valid(&me->strings)))
    {
        status = true;
    }

    return status;
}

// size_t cusbd_configuration_interface_count(const struct cusbd_configuration *me)
// {
//     ECU_ASSERT( (me) );
//     return ecu_dlist_size(&me->interfaces);
// }

// size_t cusbd_configuration_size(const struct cusbd_configuration *me)
// {
//     struct ecu_dlist_citerator interface_iterator;
//     const struct cusbd_interface *interface = (const struct cusbd_interface *)0; 
//     size_t bytes = sizeof(struct cusbd_configuration_descriptor); /* This configuration descriptor. */
//     ECU_ASSERT( (me) );

//     ECU_DLIST_CONST_FOR_EACH(i, &interface_iterator, &me->interfaces)
//     {
//         interface = ECU_DNODE_GET_CONST_ENTRY(i, struct cusbd_interface, dnode);
//         bytes += cusbd_interface_size(interface);
//     }

//     return bytes;
// }
