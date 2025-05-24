/**
 * @file
 * @brief Object representing a string descriptor.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-05-19
 * @copyright Copyright (c) 2025
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* Translation unit. */
#include "cusb/string.h"

/* STDLib. */
#include <string.h> /* memcpy. */

/* ECU. */
#include "ecu/asserter.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_NAME("cusb/string.c")

/*------------------------------------------------------------*/
/*--------------------------- DEFINES ------------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Returns size of @ref cusb_string_descriptor_zero.bLength.
 */
#define STRING_ZERO_BLENGTH_SIZE \
    (ECU_FIELD_SIZEOF(struct cusb_string_descriptor_zero, bLength))

/**
 * @brief Returns size of @ref cusb_string_descriptor_zero.bDescriptorType.
 */
#define STRING_ZERO_BDESCRIPTOR_TYPE_SIZE \
    (ECU_FIELD_SIZEOF(struct cusb_string_descriptor_zero, bDescriptorType))

/**
 * @brief Returns size of @ref cusb_string_descriptor.bLength.
 */
#define STRING_BLENGTH_SIZE \
    (ECU_FIELD_SIZEOF(struct cusb_string_descriptor, bLength))

/**
 * @brief Returns size of @ref cusb_string_descriptor.bDescriptorType.
 */
#define STRING_BDESCRIPTOR_TYPE_SIZE \
    (ECU_FIELD_SIZEOF(struct cusb_string_descriptor, bDescriptorType))

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DECLARATIONS --------------*/
/*------------------------------------------------------------*/

/**
 * @brief Returns true if supplied descriptor was properly
 * constructed via @ref CUSB_STRING_DESRIPTOR_ZERO_CTOR().
 * False otherwise.
 * 
 * @param descriptor Descriptor to check.
 */
static bool string_descriptor_zero_valid(const struct cusb_string_descriptor_zero *descriptor);

/**
 * @brief Returns true if supplied descriptor was properly
 * constructed via @ref CUSB_STRING_DESRIPTOR_CTOR().
 * False otherwise.
 * 
 * @param descriptor Descriptor to check.
 */
static bool string_descriptor_valid(const struct cusb_string_descriptor *descriptor);

/*------------------------------------------------------------*/
/*---------------- STATIC FUNCTION DEFINITIONS ---------------*/
/*------------------------------------------------------------*/

static bool string_descriptor_zero_valid(const struct cusb_string_descriptor_zero *descriptor)
{
    bool status = false;
    ECU_RUNTIME_ASSERT( (descriptor) );
    size_t array_size = descriptor->bLength - STRING_ZERO_BLENGTH_SIZE - STRING_ZERO_BDESCRIPTOR_TYPE_SIZE;

    if ((descriptor->wLANGID) &&
        /* There must be at least one language ID code stored. */
        (array_size >= sizeof(descriptor->wLANGID[0])) &&
        /* Must have whole number of language ID codes... I.e. can't be length of 2.5 codes. */
        ((array_size % sizeof(descriptor->wLANGID[0])) == (size_t)0) &&
        (descriptor->bDescriptorType == (uint8_t)CUSB_STRING_DESCRIPTOR_TYPE))
    {
        status = true;
    }

    return status;
}

static bool string_descriptor_valid(const struct cusb_string_descriptor *descriptor)
{
    bool status = false;
    ECU_RUNTIME_ASSERT( (descriptor) );

    /* Subtract the NULL character from array size calculation. */
    size_t array_size = descriptor->bLength - STRING_BLENGTH_SIZE - STRING_BDESCRIPTOR_TYPE_SIZE - sizeof(cusb_utf16_t);

    if ((descriptor->bString) &&
        /* There must be at least one character stored. */
        (array_size >= sizeof(descriptor->bString[0])) &&
        /* String must have whole number of characters... I.e. can't be length of 2.5 characters. */
        ((array_size % sizeof(descriptor->bString[0])) == (size_t)0) &&
        (descriptor->bDescriptorType == (uint8_t)CUSB_STRING_DESCRIPTOR_TYPE))
    {
        status = true;
    }
    
    return status;
}

/*------------------------------------------------------------*/
/*------------ CUSB STRING ZERO MEMBER FUNCTIONS -------------*/
/*------------------------------------------------------------*/

void cusb_string_zero_ctor(struct cusb_string_zero *me,
                           const struct cusb_string_descriptor_zero *descriptor)
{
    ECU_RUNTIME_ASSERT( (me && descriptor) );
    ECU_RUNTIME_ASSERT( (string_descriptor_zero_valid(descriptor)) );
    me->descriptor = descriptor;
}

bool cusb_string_zero_valid(const struct cusb_string_zero *me)
{
    /* This is a wrapper function in case logic specific to the 
    cusb_string_zero object has to be added in the future. */
    ECU_RUNTIME_ASSERT( (me) );
    return string_descriptor_zero_valid(me->descriptor);
}

bool cusb_string_zero_has_langid(const struct cusb_string_zero *me,
                                 uint16_t wLANGID)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (cusb_string_zero_valid(me)) );
    bool status = false;

    for (size_t i = 0; i < cusb_string_zero_langid_count(me); i++)
    {
        if (me->descriptor->wLANGID[i] == wLANGID)
        {
            status = true;
            break;
        }
    }

    return status;
}

size_t cusb_string_zero_langid_count(const struct cusb_string_zero *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (me->descriptor) );
    ECU_RUNTIME_ASSERT( (me->descriptor->wLANGID) );
    size_t array_size = me->descriptor->bLength - STRING_ZERO_BLENGTH_SIZE - STRING_ZERO_BDESCRIPTOR_TYPE_SIZE;

    /* Must have whole number of language ID codes... I.e. can't be length of 2.5 codes. */
    ECU_RUNTIME_ASSERT( ((array_size % sizeof(me->descriptor->wLANGID[0])) == (size_t)0) );
    return (array_size / sizeof(me->descriptor->wLANGID[0]));
}

void cusb_string_zero_send(const struct cusb_string_zero *me, void *buf, size_t len)
{
    ECU_RUNTIME_ASSERT( (me && buf) );
    ECU_RUNTIME_ASSERT( (cusb_string_zero_valid(me)) );
    ECU_RUNTIME_ASSERT( (len >= me->descriptor->bLength) );
    uint8_t *curr = (uint8_t *)buf;
    uint16_t id = 0;

    /* We can have pointers to members since struct cusb_string_descriptor_zero is not packed. */
    memcpy(curr, &me->descriptor->bLength, STRING_ZERO_BLENGTH_SIZE);
    curr += STRING_ZERO_BLENGTH_SIZE;
    memcpy(curr, &me->descriptor->bDescriptorType, STRING_ZERO_BDESCRIPTOR_TYPE_SIZE);
    curr += STRING_ZERO_BDESCRIPTOR_TYPE_SIZE;

    /* Copy lang ID codes in little endian. */
    for (size_t i = 0; i < cusb_string_zero_langid_count(me); i++)
    {
        id = ECU_CPU_TO_LE16_RUNTIME(me->descriptor->wLANGID[i]);
        memcpy(curr, &id, sizeof(me->descriptor->wLANGID[0]));
        curr += sizeof(me->descriptor->wLANGID[0]);
    }
}

/*------------------------------------------------------------*/
/*---------------- CUSB STRING MEMBER FUNCTIONS --------------*/
/*------------------------------------------------------------*/

void cusb_string_ctor(struct cusb_string *me,
                      const struct cusb_string_descriptor *descriptor,
                      uint16_t wLANGID)
{
    ECU_RUNTIME_ASSERT( (me && descriptor) );
    ECU_RUNTIME_ASSERT( (string_descriptor_valid(descriptor)) );
    ecu_dnode_ctor(&me->dnode, ECU_DNODE_DESTROY_UNUSED, CUSB_STRING_DESCRIPTOR_TYPE);
    me->descriptor = descriptor;
    me->wLANGID = wLANGID;
}

bool cusb_string_valid(const struct cusb_string *me)
{
    /* This is a wrapper function in case logic specific to the 
    cusb_string object has to be added in the future. */
    ECU_RUNTIME_ASSERT( (me) );
    return string_descriptor_valid(me->descriptor);
}

bool cusb_string_has_langid(const struct cusb_string *me, uint16_t wLANGID)
{
    ECU_RUNTIME_ASSERT( (me) );
    return (me->wLANGID == wLANGID);
}

size_t cusb_string_character_count(const struct cusb_string *me)
{
    ECU_RUNTIME_ASSERT( (me) );
    ECU_RUNTIME_ASSERT( (me->descriptor) );
    ECU_RUNTIME_ASSERT( (me->descriptor->bString) );
    size_t array_size = me->descriptor->bLength - STRING_BLENGTH_SIZE - STRING_BDESCRIPTOR_TYPE_SIZE;

    /* Must have whole number of characters... I.e. can't be length of 2.5 characters. */
    ECU_RUNTIME_ASSERT( ((array_size % sizeof(me->descriptor->bString[0])) == (size_t)0) );
    return (array_size / sizeof(me->descriptor->bString[0]));
}

void cusb_string_send(const struct cusb_string *me, void *buf, size_t len)
{
    ECU_STATIC_ASSERT( (sizeof(cusb_utf16_t) == sizeof(uint16_t)),
                        "Sizes of two types must be equal since casts between these two types are performed." );
    ECU_RUNTIME_ASSERT( (me && buf) );
    ECU_RUNTIME_ASSERT( (cusb_string_valid(me)) );
    ECU_RUNTIME_ASSERT( (len >= me->descriptor->bLength) );
    uint8_t *curr = (uint8_t *)buf;
    uint16_t c = 0;

    /* We can have pointers to members since struct cusb_string_descriptor is not packed. */
    memcpy(curr, &me->descriptor->bLength, STRING_BLENGTH_SIZE);
    curr += STRING_BLENGTH_SIZE;
    memcpy(curr, &me->descriptor->bDescriptorType, STRING_BDESCRIPTOR_TYPE_SIZE);
    curr += STRING_BDESCRIPTOR_TYPE_SIZE;

    /* Copy UTF16 characters in little endian. */
    for (size_t i = 0; i < cusb_string_character_count(me); i++)
    {
        c = ECU_CPU_TO_LE16_RUNTIME((uint16_t)me->descriptor->bString[i]);
        memcpy(curr, &c, sizeof(me->descriptor->bString[0]));
        curr += sizeof(me->descriptor->bString[0]);
    }
}
