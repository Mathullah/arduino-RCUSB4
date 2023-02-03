
#include "../../src/RcPwm/RcPwm.hpp"
#include <gtest/gtest.h>

using namespace testing;

class RcPwmTest : public ::testing::Test
{
protected:
    RcPwm m_UUT;

    void SetUp(void) override
    {
    }

    void TearDown(void) override
    {
    }

    /**
     * @brief Helper function to generate a RC pulse with a defined length
     * @PulseLength Desired pulse length [us]
     */
    void GeneratePulse(uint32_t const PulseLength)
    {
        uint32_t Time{1291};
        m_UUT.Isr(uint8_t(1), Time);
        m_UUT.Isr(uint8_t(0), Time + PulseLength);
    }

    /**
     * @brief Helper function to generate a RC pulse given by two timestamps
     * @RisingEdge Time (stamp) at rising edge [us]
     * @FallingEdge Time (stamp) at falling edge [us]
     */
    void GeneratePulse(uint32_t const RisingEdge, uint32_t const FallingEdge)
    {
        m_UUT.Isr(uint8_t(1), RisingEdge);
        m_UUT.Isr(uint8_t(0), FallingEdge);
    }
};

TEST_F(RcPwmTest, Return_zero_after_creation)
{
    EXPECT_EQ(m_UUT.Get(), 0U);
}

TEST_F(RcPwmTest, Return_non_zero_after_creation)
{
    GeneratePulse(1500);

    EXPECT_EQ(m_UUT.Get(), 0U);
}
