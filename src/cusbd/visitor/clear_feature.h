/**
 * @file
 * @brief Concrete visitor that processes CLEAR_FEATURE() request.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-07-03
 * @copyright Copyright (c) 2025
 */

#ifndef CUSBD_VISITOR_CLEAR_FEATURE_H_
#define CUSBD_VISITOR_CLEAR_FEATURE_H_

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* CUSB. */
#include "cusbd/visitor/request.h"
#include "cusbd/visitor/visitor.h"

/*------------------------------------------------------------*/
/*-------------------------- VISITOR -------------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Visitor that processes CLEAR_FEATURE() request.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_visitor_clear_feature
{
    /// @brief Inherit @ref cusbd_visitor base class.
    /// @warning MUST be first member.
    struct cusbd_visitor base;
    
    /// @brief Translated setup packet for easier use.
    struct cusbd_request request;
};

!!! TODO Stopped here

#endif /* CUSBD_VISITOR_CLEAR_FEATURE_H_ */
