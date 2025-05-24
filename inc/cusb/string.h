/**
 * @file
 * @brief Object representing a string descriptor.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-05-19
 * @copyright Copyright (c) 2025
 */

#ifndef CUSB_STRING_H_
#define CUSB_STRING_H_

#ifndef __STDC_UTF_16__
#   error "C11 compiler that encodes char16_t in UTF-16 format is required."
#else

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <stdbool.h>
#include <stdint.h>
#include <uchar.h> /* char16_t */

/* CUSB. */
#include "cusb/descriptors.h"

/* ECU. */
#include "ecu/dlist.h"
#include "ecu/endian.h"
#include "ecu/utils.h" /* ECU_FIELD_SIZEOF(). */

/*------------------------------------------------------------*/
/*---------------------- DEFINES AND MACROS ------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Creates a @ref cusb_string_descriptor_zero
 * at compile-time. Example usage:
 * @code{.c}
 * static const uint16_t languages[] = {0x0409, 0x040C};
 * static const struct cusb_string_descriptor_zero string0 = CUSB_STRING_DESCRIPTOR_ZERO_CTOR(languages);
 * @endcode
 * 
 * @param languages_ Languages the device supports. This is
 * a uint16_t[] array of language ID codes (@ref cusb_string_descriptor_zero.wLANGID).
 * See USB spec as these codes are defined by USB. In the 
 * code example above the supported languages are English 
 * (0x0409) and French (0x040C).
 */
#define CUSB_STRING_DESCRIPTOR_ZERO_CTOR(languages_)                                \
    {                                                                               \
        .bLength = ECU_FIELD_SIZEOF(struct cusb_string_descriptor_zero, bLength) +  \
            ECU_FIELD_SIZEOF(struct cusb_string_descriptor_zero, bDescriptorType) + \
            sizeof(languages_)/sizeof(languages_[0]),                               \
        .bDescriptorType = CUSB_STRING_DESCRIPTOR_TYPE,                             \
        .wLANGID = (&languages_[0])                                                 \
    }

/**
 * @brief Creates a @ref cusb_string_descriptor at
 * compile-time. Example usage:
 * @code{.c}
 * static const struct cusb_string_descriptor string = CUSB_STRING_DESCRIPTOR_CTOR("hello world");
 * @endcode
 * 
 * @param string_ String literal to store in the descriptor.
 */
#define CUSB_STRING_DESCRIPTOR_CTOR(string_)                                            \
    {                                                                                   \
        .bLength = ECU_FIELD_SIZEOF(struct cusb_string_descriptor, bLength) +           \
            ECU_FIELD_SIZEOF(struct cusb_string_descriptor, bDescriptorType) +          \
            sizeof(u##string_) - sizeof(cusb_utf16_t), /* Subtract NULL character. */   \
        .bDescriptorType = CUSB_STRING_DESCRIPTOR_TYPE,                                 \
        .bString = u##string_                                                           \
    }

/*------------------------------------------------------------*/
/*------------------------- CUSB STRING ----------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Custom type representing a character encoded in
 * UTF-16 format. Typedeffed in case this has to change in
 * the future.
 */
typedef char16_t cusb_utf16_t;

/**
 * @brief Data in string descriptor zero. Must be created
 * at compile-time with @ref CUSB_STRING_DESCRIPTOR_ZERO_CTOR().
 * Not packed since some data lengths are variable and stored by
 * reference.
 *
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusb_string_descriptor_zero
{
    /// @brief Number of bytes of this descriptor.
    const uint8_t bLength;

    /// @brief Descriptor type. Always 0x03 == String Descriptor.
    const uint8_t bDescriptorType;

    /// @brief All languages this device supports.
    /// @warning Stored in native endianness, not little endian.
    const uint16_t *const wLANGID;
};

/**
 * @brief Data in standard string descriptor. Must be created at
 * compile-time with the @ref CUSB_STRING_DESCRIPTOR_CTOR()
 * macro. Not packed since string literal can be variable 
 * length and is stored by reference.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusb_string_descriptor
{
    /// @brief Number of bytes of this descriptor. Number of
    /// bytes in bString used to determine this value, NOT the
    /// number of unicode characters in bString. Also note that
    /// the NULL character in bString is NOT included in this value.
    const uint8_t bLength;

    /// @brief Descriptor type. Always 0x03 == String Descriptor.
    const uint8_t bDescriptorType;

    /// @brief UTF-16 encoded string.
    /// @warning A NULL terminated string is stored here since this is 
    /// initialized from a string literal. Note however that bLength 
    /// does not include the NULL character.
    /// @warning Characters are stored in native endianness, not little 
    /// endian.
    const cusb_utf16_t *const bString;
};

/**
 * @brief Object representing a USB string descriptor 
 * zero. This is optional but must be used if the device
 * uses any string descriptors. A device uses this by passing
 * this object to @ref cusb_device_ctor(). Currently
 * only contains descriptor data but this is wrapped
 * in an object in case new members have to be added
 * in the future.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusb_string_zero
{
    /// @brief Descriptor data. NOT packed since string
    /// can be variable length. This is stored by reference
    /// since all @ref cusb_string_descriptor_zero objects
    /// must be initialized at compile-time and will never
    /// change.
    /// @warning wLANGID codes stored in native endianness,
    /// not little endian.
    const struct cusb_string_descriptor_zero *descriptor;
};

/**
 * @brief Object representing a standard USB string 
 * descriptor. A device must have string descriptor zero 
 * in order to use any string descriptors.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusb_string
{
    /// @brief Node in linked list.
    struct ecu_dnode dnode;

    /// @brief Descriptor data. NOT packed since string
    /// can be variable length. This is stored by reference
    /// since all @ref cusb_string_descriptor objects
    /// must be initialized at compile-time and will never
    /// change.
    /// @warning String characters stored in native endianness,
    /// not little endian.
    const struct cusb_string_descriptor *descriptor;

    /// @brief Identifies language the string is in. Codes 
    /// assigned by USB.
    /// @warning Stored in native endianness, not little endian.
    uint16_t wLANGID;
};

/*------------------------------------------------------------*/
/*------------ CUSB STRING ZERO MEMBER FUNCTIONS -------------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name CUSB String Zero Constructors
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @pre @p descriptor previously constructed via @ref CUSB_STRING_DESCRIPTOR_ZERO_CTOR().
 * @brief String descriptor zero constructor.
 * 
 * @warning This cannot be called on an active string
 * descriptor zero. Doing so is undefined behavior.
 * 
 * @param me String descriptor zero to construct.
 * @param descriptor String descriptor zero's data.
 */
extern void cusb_string_zero_ctor(struct cusb_string_zero *me,
                                  const struct cusb_string_descriptor_zero *descriptor);
/**@}*/

/**
 * @name CUSB String Zero Member Functions
 */
/**@{*/
/**
 * @pre @p me previously constructed via @ref cusb_string_zero_ctor().
 * @brief Returns true if the supplied string descriptor zero contains
 * valid data and was properly constructed via @ref cusb_string_zero_ctor(). 
 * False otherwise.
 * 
 * @param me String descriptor zero to check.
 */
extern bool cusb_string_zero_valid(const struct cusb_string_zero *me);

/**
 * @pre @p me previously constructed via @ref cusb_string_zero_ctor().
 * @brief Returns true if the string descriptor zero supports the
 * queried language. False otherwise.
 * 
 * @param me String descriptor zero to check.
 * @param wLANGID Language to check. See USB spec since this is
 * an ID code assigned by USB. This must be a raw number, do not
 * try to convert this to little endian.
 */
extern bool cusb_string_zero_has_langid(const struct cusb_string_zero *me,
                                        uint16_t wLANGID);

/**
 * @pre @p me previously constructed via @ref cusb_string_zero_ctor().
 * @brief Returns the number of langID codes stored in the string
 * descriptor zero. I.e. number of elements in @ref cusb_string_descriptor_zero.wLANGID
 * array.
 * 
 * @param me String descriptor zero to check.
 */
extern size_t cusb_string_zero_langid_count(const struct cusb_string_zero *me);

/**
 * @pre @p me previously constructed via @ref cusb_string_zero_ctor().
 * @brief Copies string descriptor zero's data into @p buf such that @p buf
 * can be directly placed in an endpoint buffer without additional 
 * formatting. Data is continuous and all multibyte values are copied 
 * in little endian.
 * 
 * @param me String descriptor zero to send.
 * @param buf String descriptor zero is copied into this buffer. 
 * @param len Number of bytes available in @p buf. This must be
 * greater than or equal to the number of bytes of the string
 * descriptor. I.e. >= @ref cusb_string_descriptor_zero.bLength.
 */
extern void cusb_string_zero_send(const struct cusb_string_zero *me, void *buf, size_t len);
/**@}*/

/*------------------------------------------------------------*/
/*---------------- CUSB STRING MEMBER FUNCTIONS --------------*/
/*------------------------------------------------------------*/

/**
 * @name CUSB String Constructors
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @pre @p descriptor previously constructed via @ref CUSB_STRING_DESCRIPTOR_CTOR().
 * @brief String descriptor constructor.
 * 
 * @warning This cannot be called on an active string
 * descriptor. Doing so is undefined behavior.
 * 
 * @param me String descriptor to construct.
 * @param descriptor The string descriptor's data.
 * @param wLANGID Language the string is in. See USB spec since this is
 * an ID code assigned by USB. This must be a raw number, do not
 * try to convert this to little endian.
 */
extern void cusb_string_ctor(struct cusb_string *me,
                             const struct cusb_string_descriptor *descriptor,
                             uint16_t wLANGID);
/**@}*/

/**
 * @name CUSB String Member Functions
 */
/**@{*/
/**
 * @pre @p me previously constructed via @ref cusb_string_ctor().
 * @brief Returns true if the supplied string descriptor contains
 * valid data and was properly constructed via @ref cusb_string_ctor(). 
 * False otherwise.
 * 
 * @param me String descriptor to check.
 */
extern bool cusb_string_valid(const struct cusb_string *me);

/**
 * @pre @p me previously constructed via @ref cusb_string_ctor().
 * @brief Returns true if the string is in the queried language.
 * False otherwise.
 * 
 * @param me String descriptor to check.
 * @param wLANGID Check if string descriptor is in this language. 
 * See USB spec since this is an ID code assigned by USB. This 
 * must be a raw number, do not try to convert this to little endian.
 */
extern bool cusb_string_has_langid(const struct cusb_string *me, uint16_t wLANGID);

/**
 * @pre @p me previously constructed via @ref cusb_string_ctor().
 * @brief Returns the number of characters stored in the string
 * descriptor. I.e. number of elements in @ref cusb_string_descriptor.bString
 * array.
 * 
 * @param me String descriptor to check.
 */
extern size_t cusb_string_character_count(const struct cusb_string *me);

/**
 * @pre @p me previously constructed via @ref cusb_string_zero_ctor().
 * @brief Copies string descriptor's data into @p buf such that @p buf
 * can be directly placed in an endpoint buffer without additional 
 * formatting. Data is continuous and all multibyte values are copied 
 * in little endian.
 * 
 * @param me String descriptor to send.
 * @param buf String descriptor is copied into this buffer. 
 * @param len Number of bytes available in @p buf. This must be
 * greater than or equal to the number of bytes of the string
 * descriptor. I.e. >= @ref cusb_string_descriptor.bLength.
 */
extern void cusb_string_send(const struct cusb_string *me, void *buf, size_t len);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* STDC_UTF_16__ */

#endif /* CUSB_STRING_H_ */
