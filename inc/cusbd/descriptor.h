/**
 * @file
 * @brief Base class all USB descriptors inherit from. This
 * should not be used by the application.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-09-23
 * @copyright Copyright (c) 2025
 */

#ifndef CUSBD_DESCRIPTOR_H_
#define CUSBD_DESCRIPTOR_H_

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* STDLib. */
#include <stdbool.h>
#include <stddef.h>

/* CUSB. */
#include "cusbd/event.h" /* cusbd_setup_packet. */

/* ECU. */
#include "ecu/ntnode.h"
#include "ecu/utils.h"

/*------------------------------------------------------------*/
/*---------------------- DEFINES AND MACROS ------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Converts supplied tree node into base @ref cusbd_descriptor
 * type.
 * 
 * @param ecu_ntnode_ptr_ Pointer to tree node. This must be
 * pointer to non-const.
 */
#define CUSBD_DESCRIPTOR_GET_ENTRY(ecu_ntnode_ptr_) \
    ECU_NTNODE_GET_ENTRY(ecu_ntnode_ptr_, struct cusbd_descriptor, ntnode)

/**
 * @brief Const-qualified version of @ref CUSBD_DESCRIPTOR_GET_ENTRY().
 * 
 * @param ecu_ntnode_ptr_ Pointer to tree node. This can be
 * pointer to const or non-const.
 */
#define CUSBD_DESCRIPTOR_GET_CONST_ENTRY(ecu_ntnode_ptr_) \
    ECU_NTNODE_GET_CONST_ENTRY(ecu_ntnode_ptr_, struct cusbd_descriptor, ntnode)

/**
 * @brief Creates @ref cusbd_descriptor_vtable instance at compile-time.
 * See cusbd_v_...() function descriptions.
 */
#define CUSBD_DESCRIPTOR_VTABLE_CTOR(process_setup_packet_in_,      \
                                     process_setup_packet_out_,     \
                                     string_index_set_,             \
                                     w_total_length_)               \
    {                                                               \
        .process_setup_packet_in = (size_t (*const)(const struct cusbd_descriptor *, const struct cusbd_setup_packet *, enum cusbd_descriptor_state, void *, size_t))(process_setup_packet_in_), \
        .process_setup_packet_out = (bool (*const)(struct cusbd_descriptor *, const struct cusbd_setup_packet *, enum cusbd_descriptor_state))(process_setup_packet_out_), \
        .string_index_set = (void (*const)(struct cusbd_descriptor *, size_t))(string_index_set_), \
        .w_total_length = (size_t (*const)(const struct cusbd_descriptor *))(w_total_length_) \
    }

/**
 * @brief Upcasts pointer to derived descriptor into
 * a @ref cusbd_descriptor base class pointer for inheritance
 * and polymorphism. This macro encapsulates the cast.
 * 
 * @param me_ Pointer to derived descriptor. This must inherit
 * @ref cusbd_descriptor base class.
 */
#define CUSBD_DESCRIPTOR_BASE_CAST(x_) \
    ((struct cusbd_descriptor *)(x_))

/**
 * @brief Const-qualified version of @ref CUSBD_DESCRIPTOR_BASE_CAST().
 * 
 * @param me_ Pointer to derived descriptor. This must inherit
 * @ref cusbd_descriptor base class.
 */
#define CUSBD_DESCRIPTOR_CONST_BASE_CAST(x_) \
    ((const struct cusbd_descriptor *)(x_))

/**
 * @brief Verifies, at compile-time, the derived descriptor
 * correctly inherits @ref cusbd_descriptor base class.
 * Returns true if correctly inherited. False otherwise.
 * 
 * @param base_ Name of @ref cusbd_descriptor <b>member</b> 
 * within concrete descriptor type.
 * @param derived_ Concrete descriptor type to check.
 */
#define CUSBD_DESCRIPTOR_IS_BASE_OF(base_, derived_) \
    ECU_IS_BASE_OF(base_, derived_)

/*------------------------------------------------------------*/
/*------------------------ DESCRIPTOR ------------------------*/
/*------------------------------------------------------------*/

/**
 * @brief Unique ID that identifies the type of descriptor.
 * Note this is NOT bDescriptorType.
 */
enum cusbd_descriptor_id
{
    CUSBD_DESCRIPTOR_ID_RESREVED = ECU_USER_OBJECT_ID_BEGIN, /**< Dummy ID. Allows library to detect if descriptor was constructed. ID 0-initialized to this value if not. */
    /**************************/
    CUSBD_DESCRIPTOR_ID_DEVICE,         /**< CUSBD device descriptor. */
    CUSBD_DESCRIPTOR_ID_CONFIGURATION,  /**< CUSBD configuration descriptor. */
    CUSBD_DESCRIPTOR_ID_INTERFACE,      /**< CUSBD interface descriptor. */
    CUSBD_DESCRIPTOR_ID_ALT_INTERFACE,  /**< CUSBD alternate interface descriptor. */
    CUSBD_DESCRIPTOR_ID_ENDPOINT,       /**< CUSBD endpoint descriptor. */
    CUSBD_DESCRIPTOR_ID_STRING,         /**< CUSBD string descriptor */
    /**************************/
    CUSBD_DESCRIPTOR_ID_COUNT           /**< Total number of supported descriptor types. */
};

/**
 * @brief State device is in when setup packet is being 
 * processed.
 */
enum cusbd_descriptor_state
{
    CUSBD_DESCRIPTOR_STATE_DEFAULT,     /**< Device in default state. */
    CUSBD_DESCRIPTOR_STATE_ADDRESS,     /**< Device in address state. */
    CUSBD_DESCRIPTOR_STATE_CONFIGURED,  /**< Device in configured state. */
    /******************************/
    CUSBD_DESCRIPTOR_STATE_COUNT
};

// /**
//  * @brief Status returned when descriptor processes setup
//  * packet.
//  */
// enum cusbd_descriptor_status
// {
//     CUSBD_DESCRIPTOR_STATUS_UNPROCESSED,    /**< Setup packet not relevant to descriptor so not processed. */
//     CUSBD_DESCRIPTOR_STATUS_STALL,          /**< Setup packet processed but not valid (error). */
//     CUSBD_DESCRIPTOR_STATUS_ACK,            /**< Setup packet processed. */
//     /******************************/
//     CUSBD_DESCRIPTOR_STATUS_COUNT
// };

/* Forward declarations. */
struct cusbd_descriptor;

/**
 * @brief Virtual table of @ref cusbd_descriptor.
 */
struct cusbd_descriptor_vtable
{
    /// @brief See @ref cusbd_descriptor_v_process_setup_packet_in().
    size_t (*const process_setup_packet_in)(const struct cusbd_descriptor *me, const struct cusbd_setup_packet *packet, enum cusbd_descriptor_state state, void *buffer, size_t buffer_len);

    /// @brief See @ref cusbd_descriptor_v_process_setup_packet_out().
    bool (*const process_setup_packet_out)(struct cusbd_descriptor *me, const struct cusbd_setup_packet *packet, enum cusbd_descriptor_state state);

    /// @brief See @ref cusbd_descriptor_v_string_index_set().
    void (*const string_index_set)(struct cusbd_descriptor *me, size_t index);

    /// @brief See @ref cusbd_descriptor_v_w_total_length().
    size_t (*const w_total_length)(const struct cusbd_descriptor *me);
};

/**
 * @brief Base class that must be inherited by all
 * CUSBD descriptors.
 * 
 * @warning PRIVATE. Unless otherwise specified, all
 * members can only be edited via the public API.
 */
struct cusbd_descriptor
{
    /// @brief Virtual table.
    const struct cusbd_descriptor_vtable *vptr;

    /// @brief All descriptors represented as nodes in a tree.
    struct ecu_ntnode ntnode;

    /// @brief Descriptor's data.
    void *data;

    /// @brief bLength field of descriptor.
    uint8_t bLength;
};

/*------------------------------------------------------------*/
/*---------------- DESCRIPTOR MEMBER FUNCTIONS ---------------*/
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
 * @brief Descriptor constructor.
 * 
 * @param me Descriptor to construct.
 * @param id Identifies descriptor type. NOT bDescriptorType!
 * @param data Pointer to descriptor's data.
 * @param bLength Value of descriptor's bLength field.
 */
extern void cusbd_descriptor_ctor(struct cusbd_descriptor *me, 
                                  enum cusbd_descriptor_id id,
                                  void *data,
                                  uint8_t bLength);
/**@}*/

/**
 * @name Member Functions
 */
/**@{*/
/**
 * @pre @p me is a derived descriptor that has been constructed.
 * @brief Returns descriptor's bLength field.
 * 
 * @param me Descriptor to check.
 */
uint8_t cusbd_descriptor_b_length(const struct cusbd_descriptor *me);

/**
 * @pre @p me is a derived descriptor that has been constructed.
 * @brief Returns pointer to descriptor's data.
 * 
 * @param me Descriptor to check.
 */
void *cusbd_descriptor_data(struct cusbd_descriptor *me);

/**
 * @pre @p me is a derived descriptor that has been constructed.
 * @brief Const-qualified version of @ref cusbd_descriptor().
 * 
 * @param me Descriptor to check.
 */
const void *cusbd_descriptor_cdata(const struct cusbd_descriptor *me);

/**
 * @pre @p me is a derived descriptor that has been constructed.
 * @brief Returns ID of descriptor.
 * 
 * @param me Descriptor to check.
 */
extern enum cusbd_descriptor_id cusbd_descriptor_id(const struct cusbd_descriptor *me);

/**
 * @pre @p me is a derived descriptor that has been constructed.
 * @brief Returns descriptor's tree node data structure so it
 * can be used by other modules without having to directly
 * access struct members. 
 * 
 * @param me Descriptor.
 */
extern struct ecu_ntnode *cusbd_descriptor_ntnode(struct cusbd_descriptor *me);

/**
 * @pre @p me is a derived descriptor that has been constructed.
 * @brief Const-qualified version of @ref cusbd_descriptor_ntnode().
 * 
 * @param me Descriptor.
 */
extern const struct ecu_ntnode *cusbd_descriptor_cntnode(const struct cusbd_descriptor *me);

/**
 * @brief Returns true if base descriptor is valid and has been
 * constructed via @ref cusbd_descriptor_ctor(). False otherwise.
 * 
 * @param me Base descriptor to check.
 */
extern bool cusbd_descriptor_valid(const struct cusbd_descriptor *me);

/**
 * @pre @p me is a derived descriptor that has been constructed.
 * @brief Processes setup packet sent in status stage of control
 * transfer from host. This is a device-to-host request.
 * 
 * @param me Descriptor.
 * @param packet Setup packet sent by host.
 * @param state State the device is in when the setup packet was received.
 * @param buffer If request was processed, data that needs to 
 * be sent back to the host is written to this buffer.
 * @param buffer_len Number of available bytes in @p buffer.
 * 
 * @return Returns the number of bytes written to @p buffer 
 * if the setup packet was successfully processed. Returns 0
 * if the packet was not relevant for this descriptor or if
 * it is an invalid request.
 */
extern size_t cusbd_descriptor_v_process_setup_packet_in(const struct cusbd_descriptor *me, 
                                                         const struct cusbd_setup_packet *packet, 
                                                         enum cusbd_descriptor_state state,
                                                         void *buffer, 
                                                         size_t buffer_len);

/**
 * @pre @p me is a derived descriptor that has been constructed.
 * @brief Processes setup packet sent in status stage of control
 * transfer from host. This is a host-to-device request.
 * 
 * @param me Descriptor.
 * @param packet Setup packet sent by host.
 * @return Returns true if the setup packet was for this descriptor
 * and was successfully processed. Otherwise returns false.
 * @param state State the device is in when the setup packet was received.
 * 
 * @return Returns true if packet successfully processed. Returns false
 * if the packet was not relevant for this descriptor or if it is an invalid
 * request.
 */
extern bool cusbd_descriptor_v_process_setup_packet_out(struct cusbd_descriptor *me, 
                                                        const struct cusbd_setup_packet *packet,
                                                        enum cusbd_descriptor_state state);

/**
 * @pre @p me is a derived descriptor that has been constructed.
 * @brief Virtual call. Composite. If the descriptor uses string
 * descriptors it sets its string index to the specified value
 * then increments it. Otherwise passes the same index along the
 * chain.
 * 
 * @note size_t instead of uint8_t is used to detect index overflow.
 * 
 * @param me Descriptor.
 * @param index Start setting descriptor's string indices at this value.
 */
extern void cusbd_descriptor_v_string_index_set(struct cusbd_descriptor *me, size_t index);

/**
 * @pre @p me is a derived descriptor that has been constructed.
 * @brief Virtual call. Composite. Returns number of bytes of the descriptor's 
 * data if it contributes to the wTotalLength field of the configuration descriptor.
 * Otherwise returns 0.
 * 
 * @note size_t instead of uint16_t is used to detect wTotalLength overflow.
 * 
 * @param me Descriptor.
 */
extern size_t cusbd_descriptor_v_w_total_length(const struct cusbd_descriptor *me);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* CUSBD_DESCRIPTOR_H_ */
