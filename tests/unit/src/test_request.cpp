/**
 * @file
 * @brief Unit tests for @ref request.h. Test Summary:
 * 
 * @ref cusbd_request_direction()
 *      - TEST(Request, DirectionIsIn)
 *      - TEST(Request, DirectionIsOut)
 * 
 * @ref cusbd_request_recipient()
 *      - TEST(Request, RecipientIsDevice)
 *      - TEST(Request, RecipientIsInterface)
 *      - TEST(Request, RecipientIsEndpoint)
 *      - TEST(Request, RecipientIsOther)
 *      - TEST(Request, RecipientIsGarbage)
 * 
 * @ref cusbd_request_type()
 *      - TEST(Request, TypeIsStandard)
 *      - TEST(Request, TypeIsClass)
 *      - TEST(Request, TypeIsVendor)
 *      - TEST(Request, TypeIsGarbage)
 * 
 * @ref cusbd_request_brequest()
 *      - TEST(Request, BRequest)
 * 
 * @ref cusbd_request_windex()
 *      - TEST(Request, WIndex)
 * 
 * @ref cusbd_request_wlength()
 *      - TEST(Request, WLength)
 * 
 * @ref cusbd_request_wvalue()
 *      - TEST(Request, WValue)
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-08-09
 * @copyright Copyright (c) 2025
 */

/*------------------------------------------------------------*/
/*------------------------- INCLUDES -------------------------*/
/*------------------------------------------------------------*/

/* Files under test. */
#include "cusbd/request.h"

/* Stubs. */
#include "stubs/stub_asserter.hpp"

/* ECU. */
#include "ecu/endian.h"

/* CppUTest. */
#include "CppUTestExt/MockSupport.h"
#include "CppUTest/TestHarness.h"

/*------------------------------------------------------------*/
/*---------------------- HELPER CLASSES ----------------------*/
/*------------------------------------------------------------*/
namespace {
/**
 * @brief C++ wrapper around @ref cusbd_request structure
 * under test.
 */
struct request : public cusbd_request
{
    /// @brief Constructs setup packet contents of request.
    request(std::uint8_t bmRequestType_, std::uint8_t bRequest_, std::uint16_t wValue_, std::uint16_t wIndex_, std::uint16_t wLength_)
    {
        bmRequestType = bmRequestType_;
        bRequest = bRequest_;
        wValue = wValue_;
        wIndex = wIndex_;
        wLength = wLength_;
    }
};
} /* namespace. */

/*------------------------------------------------------------*/
/*----------------------- TEST GROUPS ------------------------*/
/*------------------------------------------------------------*/

TEST_GROUP(Request)
{
    void setup() override
    {
        set_assert_handler(AssertResponse::FAIL);
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }
};

/*------------------------------------------------------------*/
/*------------ TESTS - cusbd_request_direction() -------------*/
/*------------------------------------------------------------*/

/**
 * @brief @ref CUSBD_REQUEST_DIRECTION_OUT returned 
 * if bmRequestType[7] == 0b0.
 */
TEST(Request, DirectionIsIn)
{
    try
    {
        /* Step 1: Arrange. bmRequestType = 0b01111111*/
        request msg{0x7F, 0xFF, 0xFFFF, 0xFFFF, 0xFFFF};

        /* Step 2: Action. */
        auto direction = cusbd_request_direction(&msg);

        /* Step 3: Assert. */
        CHECK_TRUE( (direction == CUSBD_REQUEST_DIRECTION_OUT) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief @ref CUSBD_REQUEST_DIRECTION_IN returned 
 * if bmRequestType[7] == 0b1.
 */
TEST(Request, DirectionIsOut)
{
    try
    {
        /* Step 1: Arrange. bmRequestType = 0b11111111*/
        request msg{0xFF, 0xFF, 0xFFFF, 0xFFFF, 0xFFFF};

        /* Step 2: Action. */
        auto direction = cusbd_request_direction(&msg);

        /* Step 3: Assert. */
        CHECK_TRUE( (direction == CUSBD_REQUEST_DIRECTION_IN) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*------------ TESTS - cusbd_request_recipient() -------------*/
/*------------------------------------------------------------*/

/**
 * @brief @ref CUSBD_REQUEST_RECIPIENT_DEVICE returned
 * if bmRequestType[4-0] == 0b00000.
 */
TEST(Request, RecipientIsDevice)
{
    try
    {
        /* Step 1: Arrange. bmRequestType = 0b11100000 */
        request msg{0xE0, 0xFF, 0xFFFF, 0xFFFF, 0xFFFF};

        /* Step 2: Action. */
        auto recipient = cusbd_request_recipient(&msg);

        /* Step 3: Assert. */
        CHECK_TRUE( (recipient == CUSBD_REQUEST_RECIPIENT_DEVICE) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief @ref CUSBD_REQUEST_RECIPIENT_INTERFACE returned
 * if bmRequestType[4-0] == 0b00001.
 */
TEST(Request, RecipientIsInterface)
{
    try
    {
        /* Step 1: Arrange. bmRequestType = 0b11100001 */
        request msg{0xE1, 0xFF, 0xFFFF, 0xFFFF, 0xFFFF};

        /* Step 2: Action. */
        auto recipient = cusbd_request_recipient(&msg);

        /* Step 3: Assert. */
        CHECK_TRUE( (recipient == CUSBD_REQUEST_RECIPIENT_INTERFACE) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief @ref CUSBD_REQUEST_RECIPIENT_ENDPOINT returned
 * if bmRequestType[4-0] == 0b00010.
 */
TEST(Request, RecipientIsEndpoint)
{
    try
    {
        /* Step 1: Arrange. bmRequestType = 0b11100010 */
        request msg{0xE2, 0xFF, 0xFFFF, 0xFFFF, 0xFFFF};

        /* Step 2: Action. */
        auto recipient = cusbd_request_recipient(&msg);

        /* Step 3: Assert. */
        CHECK_TRUE( (recipient == CUSBD_REQUEST_RECIPIENT_ENDPOINT) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief @ref CUSBD_REQUEST_RECIPIENT_OTHER returned
 * if bmRequestType[4-0] == 0b00011.
 */
TEST(Request, RecipientIsOther)
{
    try
    {
        /* Step 1: Arrange. bmRequestType = 0b11100011 */
        request msg{0xE3, 0xFF, 0xFFFF, 0xFFFF, 0xFFFF};

        /* Step 2: Action. */
        auto recipient = cusbd_request_recipient(&msg);

        /* Step 3: Assert. */
        CHECK_TRUE( (recipient == CUSBD_REQUEST_RECIPIENT_OTHER) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief @ref CUSBD_REQUEST_RECIPIENT_RESERVED returned
 * if bmRequestType[4-0] == 4 to 31.
 */
TEST(Request, RecipientIsGarbage)
{
    try
    {
        /* Step 1: Arrange. bmRequestType = 0b11111111 */
        request msg{0xFF, 0xFF, 0xFFFF, 0xFFFF, 0xFFFF};

        /* Step 2: Action. */
        auto recipient = cusbd_request_recipient(&msg);

        /* Step 3: Assert. */
        CHECK_TRUE( (recipient == CUSBD_REQUEST_RECIPIENT_RESERVED) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*--------------- TESTS - cusbd_request_type() ---------------*/
/*------------------------------------------------------------*/

/**
 * @brief @ref CUSBD_REQUEST_TYPE_STANDARD returned
 * if bmRequestType[6-5] == 0b00.
 */
TEST(Request, TypeIsStandard)
{
    try
    {
        /* Step 1: Arrange. bmRequestType = 0b10011111. */
        request msg{0x9F, 0xFF, 0xFFFF, 0xFFFF, 0xFFFF};

        /* Step 2: Action. */
        auto type = cusbd_request_type(&msg);

        /* Step 3: Assert. */
        CHECK_TRUE( (type == CUSBD_REQUEST_TYPE_STANDARD) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief @ref CUSBD_REQUEST_TYPE_CLASS returned
 * if bmRequestType[6-5] == 0b01.
 */
TEST(Request, TypeIsClass)
{
    try
    {
        /* Step 1: Arrange. bmRequestType = 0b10111111. */
        request msg{0xBF, 0xFF, 0xFFFF, 0xFFFF, 0xFFFF};

        /* Step 2: Action. */
        auto type = cusbd_request_type(&msg);

        /* Step 3: Assert. */
        CHECK_TRUE( (type == CUSBD_REQUEST_TYPE_CLASS) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief @ref CUSBD_REQUEST_TYPE_VENDOR returned
 * if bmRequestType[6-5] == 0b10.
 */
TEST(Request, TypeIsVendor)
{
    try
    {
        /* Step 1: Arrange. bmRequestType = 0b11011111. */
        request msg{0xDF, 0xFF, 0xFFFF, 0xFFFF, 0xFFFF};

        /* Step 2: Action. */
        auto type = cusbd_request_type(&msg);

        /* Step 3: Assert. */
        CHECK_TRUE( (type == CUSBD_REQUEST_TYPE_VENDOR) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/**
 * @brief @ref CUSBD_REQUEST_TYPE_RESERVED returned
 * if bmRequestType[6-5] == 0b11.
 */
TEST(Request, TypeIsGarbage)
{
    try
    {
        /* Step 1: Arrange. bmRequestType = 0b01100000. */
        request msg{0x60, 0xFF, 0xFFFF, 0xFFFF, 0xFFFF};

        /* Step 2: Action. */
        auto type = cusbd_request_type(&msg);

        /* Step 3: Assert. */
        CHECK_TRUE( (type == CUSBD_REQUEST_TYPE_RESERVED) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*-------------- TESTS - cusbd_request_brequest() ------------*/
/*------------------------------------------------------------*/

/**
 * @brief Raw value of bRequest returned.
 */
TEST(Request, BRequest)
{
    try
    {
        /* Step 1: Arrange. */
        std::uint8_t expected_brequest = 0x05;
        request msg{0xFF, expected_brequest, 0xFFFF, 0xFFFF, 0xFFFF};

        /* Step 2: Action. */
        std::uint8_t brequest = cusbd_request_brequest(&msg);

        /* Step 3: Assert. */
        CHECK_TRUE( (brequest == expected_brequest) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*--------------- TESTS - cusbd_request_windex() -------------*/
/*------------------------------------------------------------*/

/**
 * @brief Store wIndex in little endian. Verify
 * correct value returned.
 */
TEST(Request, WIndex)
{
    try
    {
        /* Step 1: Arrange. */
        std::uint16_t expected_windex = 0x1234;
        request msg{0xFF, 0xFF, 0xFFFF, ECU_CPU_TO_LE16_RUNTIME(expected_windex), 0xFFFF};
        
        /* Step 2: Action. */
        std::uint16_t windex = cusbd_request_windex(&msg);

        /* Step 3: Assert. */
        CHECK_TRUE( (windex == expected_windex) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*-------------- TESTS - cusbd_request_wlength() -------------*/
/*------------------------------------------------------------*/

/**
 * @brief Store wLength in little endian. Verify
 * correct value returned.
 */
TEST(Request, WLength)
{
    try
    {
        /* Step 1: Arrange. */
        std::uint16_t expected_wlength = 0x1234;
        request msg{0xFF, 0xFF, 0xFFFF, 0xFFFF, ECU_CPU_TO_LE16_RUNTIME(expected_wlength)};
        
        /* Step 2: Action. */
        std::uint16_t wlength = cusbd_request_wlength(&msg);

        /* Step 3: Assert. */
        CHECK_TRUE( (wlength == expected_wlength) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}

/*------------------------------------------------------------*/
/*--------------- TESTS - cusbd_request_wvalue() -------------*/
/*------------------------------------------------------------*/

/**
 * @brief Store wValue in little endian. Verify
 * correct value returned.
 */
TEST(Request, WValue)
{
    try
    {
        /* Step 1: Arrange. */
        std::uint16_t expected_wvalue = 0x1234;
        request msg{0xFF, 0xFF, ECU_CPU_TO_LE16_RUNTIME(expected_wvalue), 0xFFFF, 0xFFFF};

        /* Step 2: Action. */
        std::uint16_t wvalue = cusbd_request_wvalue(&msg);

        /* Step 3: Assert. */
        CHECK_TRUE( (wvalue == expected_wvalue) );
    }
    catch (const AssertException& e)
    {
        /* FAIL. */
        (void)e;
    }
}
