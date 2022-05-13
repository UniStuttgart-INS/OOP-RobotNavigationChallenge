#include "RandomNumber.hpp"

namespace oop
{

/// @brief Random number generator for user triggered game logic
internal::RandomNumberGenerator& RandomNumber::userRngGen()
{
    return internal::RandomNumberGenerator::userRngGenerator();
}

} // namespace oop