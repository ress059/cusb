/**
 * @file
 * @brief See @ref string.h.
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
#include "cusbd/string.h"

/* STDLib. */
#include <string.h> /* memcpy. */

/* ECU. */
#include "ecu/asserter.h"
#include "ecu/endian.h"

/*------------------------------------------------------------*/
/*--------------- DEFINE FILE NAME FOR ASSERTER --------------*/
/*------------------------------------------------------------*/

ECU_ASSERT_DEFINE_FILE("cusbd/string.c")

/*------------------------------------------------------------*/
/*--------------------------- DEFINES ------------------------*/
/*------------------------------------------------------------*/

/// @brief bDescriptorType value of string descriptors.
#define BDESCRIPTORTYPE_STRING \
    ((uint8_t)0x03)

/*------------------------------------------------------------*/
/*------------ CUSBD STRING ZERO MEMBER FUNCTIONS ------------*/
/*------------------------------------------------------------*/

void cusbd_string_zero_ctor(struct cusbd_string_zero *me,
                            const uint16_t *wLANGIDs,
                            uint8_t wLANGIDs_size)
{
    ECU_ASSERT( (me && wLANGIDs) );
    ECU_ASSERT( (wLANGIDs_size >= sizeof(*wLANGIDs)) ); /* Must contain at least one language ID code. */
    ECU_ASSERT( ((wLANGIDs_size % sizeof(*wLANGIDs)) == 0) ); /* Must be multiple of sizeof(langID code). */
    size_t descriptor_size = wLANGIDs_size + sizeof(me->bLength) + sizeof(me->bDescriptorType);
    ECU_ASSERT( (descriptor_size <= UINT8_MAX) ); /* Cannot exceed max value bLength can hold. */

    me->bLength = (uint8_t)descriptor_size;
    me->bDescriptorType = BDESCRIPTORTYPE_STRING;
    me->wLANGIDs = wLANGIDs;
    me->wLANGIDs_count = (wLANGIDs_size / sizeof(*wLANGIDs));
}

size_t cusbd_string_zero_langid_count(const struct cusbd_string_zero *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (string_zero_valid(me)) );
    return (me->wLANGIDs_count);
}

bool cusbd_string_zero_langid_supported(const struct cusbd_string_zero *me,
                                        uint16_t wLANGID)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (cusbd_string_zero_valid(me)) );
    bool status = false;

    for (size_t i = 0; i < me->wLANGIDs_count; i++)
    {
        if (me->wLANGIDs[i] == wLANGID)
        {
            status = true;
            break;
        }
    }

    return status;
}

size_t cusbd_string_zero_send(const struct cusbd_string_zero *me, void *buffer, size_t len)
{
    ECU_ASSERT( (me && buffer) );
    ECU_ASSERT( (cusbd_string_zero_valid(me)) );
    size_t num_bytes_sent = 0;

    if (me->bLength <= len) 
    {
        /* Buffer large enough to contain descriptor. */
        uint8_t *head = buffer;
        uint16_t lang_id = 0;
        ECU_STATIC_ASSERT( (sizeof(lang_id) == sizeof(*me->wLANGIDs)), "Sizes must be equal." );

        /* Copy bLength and bDescriptorType. */
        head[0] = me->bLength;
        head[1] = me->bDescriptorType;
        num_bytes_sent = sizeof(me->bLength) + sizeof(me->bDescriptorType);
        head += (sizeof(me->bLength) + sizeof(me->bDescriptorType));

        /* Copy lang ID codes in little endian. */
        for (size_t i = 0; i < me->wLANGIDs_count; i++)
        {
            lang_id = ECU_CPU_TO_LE16_RUNTIME(me->wLANGIDs[i]);
            memcpy(head, &lang_id, sizeof(lang_id));
            num_bytes_sent += sizeof(lang_id);
            head += sizeof(lang_id);
        }
    }
    else
    {
        /* Buffer too small to contain descriptor. */
        num_bytes_sent = 0;
    }

    return num_bytes_sent;
}

bool cusbd_string_zero_valid(const struct cusbd_string_zero *me)
{
    ECU_ASSERT( (me) );
    bool status = false;

    if ((me->bLength == me->wLANGIDs_count + sizeof(me->bLength) + sizeof(me->bDescriptorType)) &&
        (me->bDescriptorType == BDESCRIPTORTYPE_STRING) &&
        (me->wLANGIDs) &&
        (me->wLANGIDs_count > 0))
    {
        status = true;
    }

    return status;
}

/*------------------------------------------------------------*/
/*---------------- CUSBD STRING MEMBER FUNCTIONS -------------*/
/*------------------------------------------------------------*/

void cusbd_string_ctor(struct cusbd_string *me,
                       const char16_t *bString,
                       uint16_t wLANGID)
{
    /* String length is restricted by the max value bLength can hold:
    1. bLength_max_value = 255
    2. bString_max_size = bLength_max_value - (bLength_max_value % sizeof(char16_t)) = 252 // highest multiple of sizeof(char16_t)
    3. MAX_CHARACTER_COUNT = bString_max_size / sizeof(char16_t).
    */
    static const size_t MAX_CHARACTER_COUNT = 126;
    ECU_ASSERT( (me && bString) );
    size_t character_count = 0;

    for (size_t i = 0; i < MAX_CHARACTER_COUNT; i++)
    {
        if (bString[i])
        {
            character_count++;
        }
        else
        {
            /* NULL terminator reached. */
            break;
        }
    }

    ecu_dnode_ctor(&me->dnode, ECU_DNODE_DESTROY_UNUSED, ECU_OBJECT_ID_UNUSED);
    me->bLength = (character_count * sizeof(*me->bString)) + sizeof(me->bLength) + sizeof(me->bDescriptorType);
    me->bDescriptorType = BDESCRIPTORTYPE_STRING;
    me->bString = bString;
    me->index = 0;
    me->length = character_count;
    me->wLANGID = wLANGID;
}

uint8_t cusbd_string_b_length(const struct cusbd_string *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (cusbd_string_valid(me)) );
    return (me->bLength);
}

struct ecu_dnode *cusbd_string_dnode(struct cusbd_string *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (cusbd_string_valid(me)) );
    return (&me->dnode);
}

const struct ecu_dnode *cusbd_string_cdnode(const struct cusbd_string *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (cusbd_string_valid(me)) );
    return (&me->dnode);
}

size_t cusbd_string_length(const struct cusbd_string *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (cusbd_string_valid(me)) );
    return (me->length);
}

size_t cusbd_string_send(const struct cusbd_string *me, void *buffer, size_t len)
{
    ECU_ASSERT( (me && buffer) );
    ECU_ASSERT( (cusbd_string_zero_valid(me)) );
    size_t num_bytes_sent = 0;

    if (me->bLength <= len) 
    {
        /* Buffer large enough to contain descriptor. */
        uint8_t *head = buffer;
        uint16_t character = 0;
        ECU_STATIC_ASSERT( (sizeof(character) == sizeof(*me->bString)), "Sizes must be equal." );

        /* Copy bLength and bDescriptorType. */
        head[0] = me->bLength;
        head[1] = me->bDescriptorType;
        num_bytes_sent = sizeof(me->bLength) + sizeof(me->bDescriptorType);
        head += (sizeof(me->bLength) + sizeof(me->bDescriptorType));

        /* Copy string in little endian. */
        for (size_t i = 0; i < me->length; i++)
        {
            character = ECU_CPU_TO_LE16_RUNTIME((uint16_t)me->bString[i]);
            memcpy(head, &character, sizeof(character));
            num_bytes_sent += sizeof(character);
            head += sizeof(character);
        }
    }
    else
    {
        /* Buffer too small to contain descriptor. */
        num_bytes_sent = 0;
    }

    return num_bytes_sent;
}

bool cusbd_string_valid(const struct cusbd_string *me)
{
    ECU_ASSERT( (me) );
    bool status = false;

    if ((ecu_dnode_valid(&me->dnode)) && 
        (me->bLength == (uint8_t)(me->length + sizeof(me->bLength) + sizeof(me->bDescriptorType))) &&
        (me->bDescriptorType == BDESCRIPTORTYPE_STRING) && 
        (me->bString))
    {
        status = true;
    }

    return status;
}

uint16_t cusbd_string_w_lang_id(const struct cusbd_string *me)
{
    ECU_ASSERT( (me) );
    ECU_ASSERT( (cusbd_string_valid(me)) );
    return (me->wLANGID);
}
