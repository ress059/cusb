/**
 * @file
 * @brief Object representing a string descriptor.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-05-19
 * @copyright Copyright (c) 2025
 */

#ifndef CUSBD_STRING_H_
#define CUSBD_STRING_H_

#ifndef __STDC_UTF_16__
#   error "C11 compiler that encodes char16_t in UTF-16 format is required."
#else

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <uchar.h> /* char16_t */

/* ECU. */
#include "ecu/dlist.h"

/*------------------------------------------------------------*/
/*---------------------- DEFINES AND MACROS ------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Creates a UTF-16 encoded string from the supplied
 * string literal. This macro should be used in 
 * @ref cusbd_string_ctor().
 * 
 * @param string_ String literal to convert.
 */
#define CUSBD_STRING(string_) \
    (u##string_)

/**
 * @brief Converts supplied linked list node into @ref cusbd_string
 * type.
 * 
 * @param ecu_dnode_ptr_ Pointer to linked list node. This must be
 * pointer to non-const.
 */
#define CUSBD_STRING_GET_ENTRY(ecu_dnode_ptr_) \
    ECU_DNODE_GET_ENTRY(ecu_dnode_ptr_, struct cusbd_string, dnode)

/**
 * @brief Const-qualified version of @ref CUSBD_STRING_GET_ENTRY().
 * 
 * @param ecu_dnode_ptr_ Pointer to linked list node. This can be
 * pointer to const or non-const.
 */
#define CUSBD_STRING_GET_CONST_ENTRY(ecu_dnode_ptr_) \
    ECU_DNODE_GET_CONST_ENTRY(ecu_dnode_ptr_, struct cusbd_string, dnode)

/*------------------------------------------------------------*/
/*------------------------ CUSBD STRING ----------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Object representing a USB string descriptor 
 * zero. This is optional but must be used if the device
 * uses any string descriptors.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_string_zero
{
    /// @brief Number of bytes of string0 descriptor contents.
    uint8_t bLength;

    /// @brief Descriptor type. Always 0x03.
    uint8_t bDescriptorType;

    /// @brief Array of language IDs this device supports.
    /// @warning Stored in native endianness, not little endian.
    const uint16_t *wLANGIDs;

    /// @brief Number of language IDs (elements) in wLANGIDs array.
    size_t wLANGIDs_count;
};

/**
 * @brief Object representing a standard USB string 
 * descriptor. A device must have string descriptor zero 
 * in order to use any string descriptors.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_string
{
    /// @brief Node in linked list.
    struct ecu_dnode dnode;

    /// @brief Number of bytes of string descriptor contents.
    uint8_t bLength;

    /// @brief Descriptor type. Always 0x03.
    uint8_t bDescriptorType;

    /// @brief NULL-terminated UTF-16 encoded string literal.
    /// @warning Stored in native endianness, not little endian.
    const char16_t *bString;

    /// @brief Index of the string (iProduce, iManufacturer, 
    /// iConfiguration, etc).
    uint8_t index;

    /// @brief Number of characters in bString, NOT including
    /// the NULL terminator.
    size_t length;

    /// @brief Identifies language the string is in. Codes 
    /// assigned by USB.
    /// @warning Stored in native endianness, not little endian.
    uint16_t wLANGID;
};

/*------------------------------------------------------------*/
/*------------ CUSBD STRING ZERO MEMBER FUNCTIONS ------------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name CUSBD String Zero Constructor
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @brief String descriptor zero constructor. Example use:
 * @code{.c}
 * static const uint16_t languages[] = {0x0409, 0x040C}; // English and French supported.
 * cusbd_string_zero_ctor(&string0, &languages[0], sizeof(languages));
 * @endcode
 * 
 * @warning This cannot be called on an active string
 * descriptor zero. Doing so is undefined behavior.
 * 
 * @param me String descriptor zero to construct.
 * @param wLANGIDs Array of language ID codes this device supports.
 * Codes should be stored as raw values. Do not convert them into little endian.
 * @param wLANGIDs_size Number of bytes of @p wLANGIDs array. This
 * is uint8_t because it must be able to be held in bLength.
 */
extern void cusbd_string_zero_ctor(struct cusbd_string_zero *me,
                                   const uint16_t *wLANGIDs,
                                   uint8_t wLANGIDs_size);
/**@}*/

/**
 * @name CUSBD String Zero Member Functions
 */
/**@{*/
/**
 * @pre @p me previously constructed via @ref cusbd_string_zero_ctor().
 * @brief Returns the number of langID codes stored in the string
 * descriptor zero.
 * 
 * @param me String descriptor zero to check.
 */
extern size_t cusbd_string_zero_langid_count(const struct cusbd_string_zero *me);

/**
 * @pre @p me previously constructed via @ref cusbd_string_zero_ctor().
 * @brief Returns true if the string descriptor zero supports the
 * queried language. False otherwise.
 * 
 * @param me String descriptor zero to check.
 * @param wLANGID Language to check. See USB spec since this is
 * an ID code assigned by USB. This must be a raw number, do not
 * try to convert this to little endian.
 */
extern bool cusbd_string_zero_langid_supported(const struct cusbd_string_zero *me,
                                               uint16_t wLANGID);

/**
 * @pre @p me previously constructed via @ref cusbd_string_zero_ctor().
 * @brief Copies string descriptor zero's data into @p buffer such that @p buffer
 * can be directly placed in an endpoint buffer without additional 
 * formatting. Data is continuous and all multibyte values are copied 
 * in little endian.
 * 
 * @param me String descriptor zero to send.
 * @param buffer String descriptor zero is copied into this buffer. 
 * @param len Number of bytes available in @p buffer.
 * 
 * @return 0 if buffer was too small to copy full descriptor into.
 * Otherwise returns the number of bytes copied.
 */
extern size_t cusbd_string_zero_send(const struct cusbd_string_zero *me, void *buffer, size_t len);

/**
 * @pre @p me previously constructed via @ref cusbd_string_zero_ctor().
 * @brief Returns true if the supplied string descriptor zero contains
 * valid data and was properly constructed via @ref cusbd_string_zero_ctor(). 
 * False otherwise.
 * 
 * @param me String descriptor zero to check.
 */
extern bool cusbd_string_zero_valid(const struct cusbd_string_zero *me);
/**@}*/

/*------------------------------------------------------------*/
/*---------------- CUSBD STRING MEMBER FUNCTIONS -------------*/
/*------------------------------------------------------------*/

/**
 * @name CUSBD String Constructor
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @brief String descriptor constructor.
 * 
 * @warning This cannot be called on an active string
 * descriptor. Doing so is undefined behavior.
 * 
 * @param me String descriptor to construct.
 * @param bString This must be a NULL-terminated, UTF-16 encoded 
 * string literal that the descriptor contains. The string will be
 * truncated if its size exceeds what can be held in bLength. @ref CUSBD_STRING()
 * macro should be used to supply this parameter.
 * @param wLANGID Language the string is in. See USB spec since this is
 * an ID code assigned by USB. This must be a raw number, do not
 * try to convert this to little endian.
 */
extern void cusbd_string_ctor(struct cusbd_string *me,
                              const char16_t *bString,
                              uint16_t wLANGID);
/**@}*/

/**
 * @name CUSBD String Member Functions
 */
/**@{*/
/**
 * @pre @p me previously constructed via @ref cusbd_string_ctor().
 * @brief Returns bLength field of string descriptor.
 * 
 * @param me String descriptor to check.
 */
extern uint8_t cusbd_string_b_length(const struct cusbd_string *me);

/**
 * @pre @p me previously constructed via @ref cusbd_string_ctor().
 * @brief Returns string's linked list node data structure
 * so it can be used by other modules without having to directly
 * access @ref cusbd_string struct. 
 * 
 * @warning This should not be used by the application.
 * 
 * @param me String descriptor.
 */
extern struct ecu_dnode *cusbd_string_dnode(struct cusbd_string *me);

/**
 * @pre @p me previously constructed via @ref cusbd_string_ctor().
 * @brief Const-qualified version of @ref cusbd_string_dnode().
 * 
 * @warning This should not be used by the application.
 * 
 * @param me String descriptor.
 */
extern const struct ecu_dnode *cusbd_string_cdnode(const struct cusbd_string *me);

/**
 * @pre @p me previously constructed via @ref cusbd_string_ctor().
 * @brief Returns the number of characters stored in bString, 
 * EXCLUDING the NULL terminator.
 * 
 * @param me String descriptor to check.
 */
extern size_t cusbd_string_length(const struct cusbd_string *me);

/**
 * @pre @p me previously constructed via @ref cusbd_string_ctor().
 * @brief Copies string descriptor's data into @p buffer such that @p buffer
 * can be directly placed in an endpoint buffer without additional 
 * formatting. Data is continuous and all multibyte values are copied 
 * in little endian.
 * 
 * @param me String descriptor to send.
 * @param buffer String descriptor is copied into this buffer. 
 * @param len Number of bytes available in @p buffer.
 * 
 * @return 0 if buffer was too small to copy full descriptor into.
 * Otherwise returns the number of bytes copied.
 */
extern size_t cusbd_string_send(const struct cusbd_string *me, void *buffer, size_t len);

/**
 * @pre @p me previously constructed via @ref cusbd_string_ctor().
 * @brief Returns true if the supplied string descriptor contains
 * valid data and was properly constructed via @ref cusbd_string_ctor(). 
 * False otherwise.
 * 
 * @param me String descriptor to check.
 */
extern bool cusbd_string_valid(const struct cusbd_string *me);

/**
 * @pre @p me previously constructed via @ref cusbd_string_ctor().
 * @brief Returns the language the string supports. This is
 * a wLANGID code.
 * 
 * @warning Value returned in native endianness, NOT little endian.
 * 
 * @param me String descriptor to check.
 */
extern uint16_t cusbd_string_w_lang_id(const struct cusbd_string *me);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* STDC_UTF_16__ */

#endif /* CUSBD_STRING_H_ */
