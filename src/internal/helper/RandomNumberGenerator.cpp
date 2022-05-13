#include "RandomNumberGenerator.hpp"
#include "internal/GameApplication.hpp"

namespace oop::internal
{

/// @brief Random number generator for game logic
RandomNumberGenerator& RandomNumberGenerator::gameRngGenerator()
{
    static RandomNumberGenerator rng;
    return rng;
}

/// @brief Random number generator for logic triggered by the user
RandomNumberGenerator& RandomNumberGenerator::userRngGenerator()
{
    static RandomNumberGenerator rng;
    static RandomNumberGenerator rngPause;

    if (GameApplication::gameRunning)
    {
        return rng;
    }

    return rngPause;
}

} // namespace oop::internal