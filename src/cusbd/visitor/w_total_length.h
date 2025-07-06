/**
 * @file
 * @brief Concrete visitor that helps calculate wTotalLength of
 * a configuration descriptor.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-06-28
 * @copyright Copyright (c) 2025
 */

#ifndef CUSBD_VISITOR_W_TOTAL_LENGTH_H_
#define CUSBD_VISITOR_W_TOTAL_LENGTH_H_

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <stdint.h>

/* CUSB. */
#include "cusbd/visitor/visitor.h"

/*------------------------------------------------------------*/
/*-------------------------- VISITOR -------------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Visitor that helps calculate wTotalLength.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_visitor_w_total_length
{
    /// @brief Inherit @ref cusbd_cvisitor base class.
    /// @warning MUST be first member.
    struct cusbd_cvisitor base;

    /// @brief Current wTotalLength calculated so far.
    /// size_t instead of uint16_t used so it can be
    /// verified this value never exceeds UINT16_MAX.
    /// @warning This will always be stored in native
    /// endianness.
    size_t wTotalLength;
};

/*------------------------------------------------------------*/
/*------------------- VISITOR MEMBER FUNCTIONS ---------------*/
/*------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Constructor
 */
/**@{*/
/**
 * @pre Memory already allocated for @p me.
 * @brief wTotalLength visitor constructor.
 * 
 * @param me wTotalLength visitor to construct.
 */
extern void cusbd_visitor_w_total_length_ctor(struct cusbd_visitor_w_total_length *me);
/**@}*/

/**
 * @name Member Functions
 */
/**@{*/
/**
 * @pre @p me previously constructed via @ref cusbd_visitor_w_total_length_ctor().
 * @brief Returns the raw value of the current wTotalLength
 * calculated so far. Value is returned in native endianness.
 * 
 * @param me wTotalLength visitor to check.
 */
extern uint16_t cusbd_visitor_w_total_length_value(const struct cusbd_visitor_w_total_length *me);

/**
 * @pre @p me previously constructed via @ref cusbd_visitor_w_total_length_ctor().
 * @brief Returns the current wTotalLength calculated
 * so far. Value is returned in little endian format.
 * 
 * @param me wTotalLength visitor to check.
 */
extern uint16_t cusbd_visitor_w_total_length_value_le(const struct cusbd_visitor_w_total_length *me);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* CUSBD_VISITOR_W_TOTAL_LENGTH_H_ */
