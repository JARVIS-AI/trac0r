#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <cstdint>
#include <chrono>
#include <array>
#include <type_traits>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "utils.hpp"

namespace trac0r {

// From http://xorshift.di.unimi.it/xorshift64star.c
inline uint64_t xorshift64star(uint64_t x) {
    x ^= x >> 12; // a
    x ^= x << 25; // b
    x ^= x >> 27; // c
    return x * 2685821657736338717LL;
}

// From http://xorshift.di.unimi.it/xorshift1024star.c
inline int64_t xorshift1024star(uint64_t &p, std::array<uint64_t, 16> &s) {
    uint64_t s0 = s[p];
    uint64_t s1 = s[p = (p + 1) & 15];
    s1 ^= s1 << 31; // a
    s1 ^= s1 >> 11; // b
    s0 ^= s0 >> 30; // c
    return (s[p] = s0 ^ s1) * 1181783497276652981LL;
}

class PRNG {
  public:
    PRNG() {
        // Generate an initial seed based on time
        auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();

        // Then use the result of another PRNG as our actual seed
        for (auto i = 0; i < 16; i++)
            m_seed[i] = xorshift64star(ns);
    }

    uint64_t next() {
        return xorshift1024star(m_p, m_seed);
    }

  private:
    std::array<uint64_t, 16> m_seed;
    uint64_t m_p = 0;
};

template <typename T>
std::enable_if_t<std::is_integral<T>::value, T> inline rand_range(const T min, const T max) {
    static thread_local PRNG generator;
    return generator.next() % max + min;
}

template <typename T>
std::enable_if_t<std::is_floating_point<T>::value, T> inline rand_range(const T min, const T max) {
    static thread_local PRNG generator;
    return min +
           static_cast<T>(generator.next()) /
               (static_cast<T>(std::numeric_limits<uint64_t>::max() / (max - min)));
}

/**
 * @brief Selects a random point on a sphere with uniform distribution.
 * point on a uniform.
 *
 * @return A random point on the surface of a sphere
 */
inline glm::vec3 uniform_sample_sphere() {
    glm::vec3 rand_vec =
        glm::vec3(rand_range(-1.f, 1.f), rand_range(-1.f, 1.f), rand_range(-1.f, 1.f));
    return glm::normalize(rand_vec);
}

/**
 * @brief Given a direction vector, this will return a random uniform point on a sphere
 * on the hemisphere around dir.
 *
 * @param dir A vector that represents the hemisphere's center
 *
 * @return A random point the on the hemisphere
 */
inline glm::vec3 oriented_uniform_hemisphere_sample(glm::vec3 dir) {
    glm::vec3 v = uniform_sample_sphere();
    return v * glm::sign(glm::dot(v, dir));
}

/**
 * @brief Selects a random point on a sphere with uniform or cosine-weighted distribution.
 *
 * @param dir A vector around which the hemisphere will be centered
 * @param power 0.f means uniform distribution while 1.f means cosine-weighted
 * @param angle When a full hemisphere is desired, use pi/2. 0 equals perfect reflection. The value
 * should therefore be between 0 and pi/2. This angle is equal to half the cone width.
 *
 * @return A random point on the surface of a sphere
 */
inline glm::vec3 sample_hemisphere(glm::vec3 dir, float power, float angle) {
    // Code adapted from Mikael Hvidtfeldt Christensen's resource
    // at http://blog.hvidtfeldts.net/index.php/2015/01/path-tracing-3d-fractals/
    // Thanks!

    glm::vec3 o1 = glm::normalize(ortho(dir));
    glm::vec3 o2 = glm::normalize(glm::cross(dir, o1));
    glm::vec2 r = glm::vec2{rand_range(0.f, 1.f), rand_range(glm::cos(angle), 1.f)};
    r.x = r.x * glm::two_pi<float>();
    r.y = glm::pow(r.y, 1.f / (power + 1.f));
    float oneminus = glm::sqrt(1.f - r.y * r.y);
    return glm::cos(r.x) * oneminus * o1 + glm::sin(r.x) * oneminus * o2 + r.y * dir;
}

/**
 * @brief Given a direction vector, this will return a random cosine-weighted point on a sphere
 * on the hemisphere around dir.
 *
 * @param dir A vector that represents the hemisphere's center
 *
 * @return A random point the on the hemisphere
 */
inline glm::vec3 oriented_cosine_weighted_hemisphere_sample(glm::vec3 dir) {
    return sample_hemisphere(dir, 1.f, glm::half_pi<float>());
}

/**
 * @brief Selects a random point on a cone with uniform distribution.
 *
 * @param dir Direction in which the cone is oriented
 * @param angle Angle of the cone in radians
 *
 * @return A random point on the surface of a cone
 */
inline glm::vec3 oriented_cosine_weighted_cone_sample(glm::vec3 dir, float angle) {
    return sample_hemisphere(dir, 1.f, angle);
}
}

#endif /* end of include guard: RANDOM_HPP */
