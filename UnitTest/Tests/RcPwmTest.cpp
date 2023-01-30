
#include "../../src/RcPwm/RcPwm.hpp"
#include <gtest/gtest.h>


TEST(RcPwmtest, Return_zero_after_creation)
{
    RcPwm PwmInput;

    ASSERT_EQ(PwmInput.Get(), 0U);
}