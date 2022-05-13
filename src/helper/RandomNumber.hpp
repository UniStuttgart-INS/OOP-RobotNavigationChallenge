/// @file RandomNumberGenerator.hpp
/// @brief Random Number Generator Objects which reset their seed every run
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2022-04-05

#pragma once

#include "internal/helper/RandomNumberGenerator.hpp"

namespace oop
{

class RandomNumber
{
  public:
    RandomNumber() = delete;

    /// @brief Random number generator for user triggered game logic
    static internal::RandomNumberGenerator& userRngGen();
};

} // namespace oop
