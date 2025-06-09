/**
 * @file
 * @brief USB HID device class driver.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-06-04
 * @copyright Copyright (c) 2025
 */

#ifndef CUSBD_HID_H_
#define CUSBD_HID_H_

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* ECU. */
#include "ecu/endian.h"

/*------------------------------------------------------------*/
/*-------------------------- BITMAPS -------------------------*/
/*------------------------------------------------------------*/





/*------------------------------------------------------------*/
/*------------------------ HID ITEM PREFIX -------------------*/
/*------------------------------------------------------------*/

#define CUBSD_HID_BTAG_INPUT \
    (8U)

#define CUSBD_HID_BTAG_OUTPUT \
    (9U)

#define CUSBD_HID_BTAG_FEATURE \
    (11U)

#define CUSBD_HID_BTAG_COLLECTION \
    (10U)

#define CUSBD_HID_BTAG_END_COLLECTION \
    (12U)

#define CUSBD_HID_BTAG_LOGICAL_MIN \
    (1U)

#define CUSBD_HID_BTAG_LOGICAL_MAX \
    (2U)

#define CUSBD_HID_BTAG_PHYSICAL_MIN \
    (3U)

#define CUSBD_HID_BTAG_PHYSICAL_MAX \
    (4U)

#define CUSBD_HID_BTAG_UNIT_EXPONENT \
    (5U)

#define CUSBD_HID_BTAG_UNIT \
    (6U)

#define CUSBD_HID_BTAG_REPORT_SIZE \
    (7U)

#define CUSBD_HID_BTAG_REPORT_ID \
    (8U)

#define CUSBD_HID_BTAG_REPORT_COUNT \
    (9U)

#define CUSBD_HID_BTAG_PUSH \
    (10U)

#define CUSBD_HID_BTAG_POP \
    (11U)

#define CUSBD_HID_BTYPE_MAIN \
    (0U)

#define CUSBD_HID_BTYPE_GLOBAL \
    (1U)

#define CUSBD_HID_BTYPE_LOCAL \
    (2U)

#define CUSBD_HID_BSIZE_0_BYTES \
    (0U)

#define CUSBD_HID_BSIZE_1_BYTE \
    (1U)

#define CUSBD_HID_BSIZE_2_BYTES \
    (2U)

#define CUSBD_HID_BSIZE_4_BYTES \
    (3U)

#define CUSBD_HID_ITEM_PREFIX(bTag_, bType_, bSize_) \
    ((((bTag_) & 0xFU) << 4) | (((bType_) & 0x3U) << 2) | ((bSize_) & 0x3U))

/*------------------------------------------------------------*/
/*------------------------- MAIN ITEMS -----------------------*/
/*------------------------------------------------------------*/

#define CUSBD_HID_IOF_DATA \
    (0U << 0)

#define CUSBD_HID_IOF_CONSTANT \
    (1U << 0)

#define CUSBD_HID_IOF_ARRAY \
    (0U << 1)

#define CUSBD_HID_IOF_VARIABLE \
    (1U << 1)

#define CUSBD_HID_IOF_ABSOLUTE \
    (0U << 2)

#define CUSBD_HID_IOF_RELATIVE \
    (1U << 2)

#define CUSBD_HID_IOF_NO_WRAP \
    (0U << 3)

#define CUSBD_HID_IOF_WRAP \
    (1U << 3)

#define CUSBD_HID_IOF_LINEAR \
    (0U << 4)

#define CUSBD_HID_IOF_NON_LINEAR \
    (1U << 4)

#define CUSBD_HID_IOF_PREFERRED_STATE \
    (0U << 5)

#define CUSBD_HID_IOF_NO_PREFERRED \
    (1U << 5)

#define CUSBD_HID_IOF_NO_NULL \
    (0U << 6)

#define CUSBD_HID_IOF_NULL_STATE \
    (1U << 6)

#define CUBSD_HID_OF_NON_VOLATILE \
    (0U << 7)

#define CUSBD_HID_OF_VOLATILE \
    (1U << 7)

#define CUSBD_HID_IOF_BIT_FIELD \
    (0U << 8)

#define CUSBD_HID_IOF_BUFFERED_BYTES \
    (1U << 8)

#define CUSBD_HID_COLLECTION_PHYSICAL \
    (0x00U)

#define CUSBD_HID_COLLECTION_APPLICATION \
    (0x01U)

#define CUSBD_HID_COLLECTION_LOGICAL \
    (0x02U)

#define CUSBD_HID_COLLECTION_REPORT \
    (0x03U)

#define CUSBD_HID_COLLECTION_NAMED_ARRAY \
    (0x04U)

#define CUSBD_HID_COLLECTION_USAGE_SWITCH \
    (0x05U)

#define CUSBD_HID_COLLECTION_USAGE_MODIFIER \
    (0x06U)

/* Bit 7 is reserved. Currently only bit8 is used. Bits 9-31 are reserved. */
#define CUSBD_HID_INPUT(d_) \
    CUSBD_HID_ITEM_PREFIX(CUSBD_HID_BTAG_INPUT, CUSBD_HID_BTYPE_MAIN, CUSBD_HID_BSIZE_4_BYTES), \
    ((d_) & 0x7FU), \
    (((d_) & (1U << 8)) >> 8U), \
    0, \
    0

#define CUSBD_HID_OUTPUT(d_) \
    CUSBD_HID_ITEM_PREFIX(CUSBD_HID_BTAG_OUTPUT, CUSBD_HID_BTYPE_MAIN, CUSBD_HID_BSIZE_4_BYTES), \
    ((d_) & 0xFFU), \
    (((d_) & (1U << 8)) >> 8U), \
    0, \
    0

#define CUSBD_HID_FEATURE(d_) \
    CUSBD_HID_ITEM_PREFIX(CUSBD_HID_BTAG_FEATURE, CUSBD_HID_BTYPE_MAIN, CUSBD_HID_BSIZE_4_BYTES), \
    ((d_) & 0xFFU), \
    (((d_) & (1U << 8)) >> 8U), \
    0, \
    0

#define CUSBD_HID_COLLECTION(d8_) \
    CUSBD_HID_ITEM_PREFIX(CUSBD_HID_BTAG_COLLECTION, CUSBD_HID_BTYPE_MAIN, CUSBD_HID_BSIZE_1_BYTE), \
    ((d8_) & 0xFFU)

#define CUSBD_HID_END_COLLECTION() \
    CUSBD_HID_ITEM_PREFIX(CUSBD_HID_BTAG_END_COLLECTION, CUSBD_HID_BTYPE_MAIN, CUSBD_HID_BSIZE_0_BYTES)

/*------------------------------------------------------------*/
/*----------------------- GLOBAL ITEMS -----------------------*/
/*------------------------------------------------------------*/

// TODO
#define CUSBD_HID_USAGE_PAGE()

#define CUSBD_HID_LOGICAL_MIN_8(d8_) \
    CUSBD_HID_ITEM_PREFIX(CUSBD_HID_BTAG_LOGICAL_MIN, CUSBD_HID_BTYPE_GLOBAL, CUSBD_HID_BSIZE_1_BYTE), \
    ((d8_) & 0xFFU)

#define CUSBD_HID_LOGICAL_MIN_16(d16_) \
    CUSBD_HID_ITEM_PREFIX(CUSBD_HID_BTAG_LOGICAL_MIN, CUSBD_HID_BTYPE_GLOBAL, CUSBD_HID_BSIZE_2_BYTES), \
    ((d16_) & 0x00FFU), \
    (((d16_) & 0xFF00U) >> 8U)

#define CUSBD_HID_LOGICAL_MIN_32(d32_) \
    CUSBD_HID_ITEM_PREFIX(CUSBD_HID_BTAG_LOGICAL_MIN, CUSBD_HID_BTYPE_GLOBAL, CUSBD_HID_BSIZE_4_BYTES), \
    ((d32_) & 0x000000FFUL), \
    (((d32_) & 0x0000FF00UL) >> 8U), \
    (((d32_) & 0x00FF0000UL) >> 16U), \
    (((d32_) & 0xFF000000UL) >> 24U)

#define CUSBD_HID_LOGICAL_MAX_8(d8_) \
    CUSBD_HID_ITEM_PREFIX(CUSBD_HID_BTAG_LOGICAL_MAX, CUSBD_HID_BTYPE_GLOBAL, CUSBD_HID_BSIZE_1_BYTE), \
    ((d8_) & 0xFFU)

#define CUSBD_HID_LOGICAL_MAX_16(d16_) \
    CUSBD_HID_ITEM_PREFIX(CUSBD_HID_BTAG_LOGICAL_MAX, CUSBD_HID_BTYPE_GLOBAL, CUSBD_HID_BSIZE_2_BYTES), \
    ((d16_) & 0x00FFU), \
    (((d16_) & 0xFF00U) >> 8U)

#define CUSBD_HID_LOGICAL_MAX_32(d32_) \
    CUSBD_HID_ITEM_PREFIX(CUSBD_HID_BTAG_LOGICAL_MAX, CUSBD_HID_BTYPE_GLOBAL, CUSBD_HID_BSIZE_4_BYTES), \
    ((d32_) & 0x000000FFUL), \
    (((d32_) & 0x0000FF00UL) >> 8U), \
    (((d32_) & 0x00FF0000UL) >> 16U), \
    (((d32_) & 0xFF000000UL) >> 24U)

// #define CUSBD_HID_UNIT_EXPONENT(d8_) \
//     CUSBD_HID_ITEM_PREFIX(CUSBD_HID_BTAG_UNIT_EXPONENT, CUSBD_HID_BTYPE_GLOBAL, CUSBD_HID_BSIZE_1_BYTE), \
//     ((d8_) & 0xFFU)

// /* Nibble 7 is reserved. */
// #define CUSBD_HID_UNIT(d32_) \
//     CUSBD_HID_ITEM_PREFIX(CUSBD_HID_BTAG_UNIT, CUSBD_HID_BTYPE_GLOBAL, CUSBD_HID_BSIZE_4_BYTES), \
//     ((d32_) & 0x000000FFUL), \
//     (((d32_) & 0x0000FF00UL) >> 8U), \
//     (((d32_) & 0x00FF0000UL) >> 16U), \
//     (((d32_) & 0x0F000000UL) >> 24U)

// TODO unit and unit exponent.

/*------------------------------------------------------------*/
/*----------------------- LOCAL ITEMS ------------------------*/
/*------------------------------------------------------------*/

// TODO



#endif /* CUSBD_HID_H_ */
