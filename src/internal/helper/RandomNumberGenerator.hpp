/// @file RandomNumberGenerator.hpp
/// @brief Helper functions to generate random numbers which reset every run
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2022-04-05

#pragma once

#include <random>
#include <chrono>

namespace oop
{
class RandomNumber;

namespace internal
{

class RandomNumberGenerator // NOLINT(cert-msc32-c,cert-msc51-cpp)
{
  public:
    // uint64_t cnt = 0; ///< Count of numbers requested

    /// @brief Reset the number generator seed
    void reset()
    {
        // cnt = 0;
        if (!useSeedInsteadOfSystemTime)
        {
            seed = static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count());
        }
        random_engine.seed(seed);
    }

    /// @brief Gives a random number within [min, max] with normal distribution around the mean
    /// @param[in] mean Mean value of the normal distribution
    /// @param[in] stddev Standard deviation of the normal distribution
    /// @param[in] min Minimum value allowed (included)
    /// @param[in] max Maximum value allowed (included)
    template<typename T>
    T normal_distribution(T mean, T stddev, T min, T max)
    {
        while (true)
        {
            auto number = std::normal_distribution<T>{ mean, stddev }(random_engine);
            // ++cnt;
            if (number >= min && number <= max)
            {
                return number;
            }
        }
    }

    /// @brief Gives a random number within [min, max] with normal distribution where the mean is in the center of the range
    /// @param[in] min Minimum value allowed (included)
    /// @param[in] max Maximum value allowed (included)
    template<typename T>
    T normal_distribution(T min, T max)
    {
        // ++cnt;
        return normal_distribution((min + max) / 2, (max - min) / 6, min, max);
    }

    /// @brief Gives a random number within [min, max] with uniform distribution
    /// @param[in] a Min value
    /// @param[in] b Max value
    template<typename T>
    T uniform_real_distribution(T a, T b = 1.0)
    {
        // ++cnt;
        return std::uniform_real_distribution<T>{ a, b }(random_engine);
    }

    /// @brief Gives a random number within [min, max] with uniform distribution
    /// @param[in] a Min value
    /// @param[in] b Max value
    template<typename T>
    T uniform_int_distribution(T a = std::numeric_limits<T>::min(), T b = std::numeric_limits<T>::max())
    {
        // ++cnt;
        return std::uniform_int_distribution<T>{ a, b }(random_engine);
    }

  private:
    std::default_random_engine random_engine; ///< Random number generator
    bool useSeedInsteadOfSystemTime = true;   ///< Flag whether to use the seed instead of the system time
    uint64_t seed = 0;                        ///< Seed for the random number generator

    /// @brief Random number generator for game logic
    static RandomNumberGenerator& gameRngGenerator();

    /// @brief Random number generator for user triggered game logic
    static RandomNumberGenerator& userRngGenerator();

    friend class ControlPanel;
    friend class GameState;
    friend class PlayerBase;
    friend class NeutralPlayer;
    friend class Satellite;
    friend class Unit;
    friend class RobotBase;
    friend class Virus;
    friend class HeadquartersBase;
    friend class oop::RandomNumber;
};

} // namespace internal

} // namespace oop
