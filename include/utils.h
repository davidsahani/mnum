#ifndef UTILS_H
#define UTILS_H
#include <iostream>
#include <vector>

typedef uint_fast8_t udigit;

/**
 * @brief Remove leading zeros.
 * @attention if vec is empty, a zero is added.
 * @param vec The vector of integers to modify.
 */
void lstrip_zeros(std::vector<udigit> &vec) noexcept
{
    if (vec.empty())
    { // a zero should be there
        vec.push_back(0);
        return;
    }
    std::vector<udigit>::iterator it;
    it = vec.begin();
    while (it != vec.end() && *it == 0)
        ++it;
    if (it == vec.end())
        --it; // spare a zero
    vec.erase(vec.begin(), it);
}

/**
 * @brief Remove trailing zeros.
 * @attention if vec is empty, a zero is added.
 * @param vec The vector of integers to modify.
 */
void rstrip_zeros(std::vector<udigit> &vec) noexcept
{
    if (vec.empty())
    { // a zero should be there
        vec.push_back(0);
        return;
    }
    std::vector<udigit>::reverse_iterator it;
    it = vec.rbegin();
    while (it != vec.rend() && *it == 0)
        ++it;
    if (it == vec.rend())
        --it; // spare a zero
    vec.erase(it.base(), vec.end());
}

/**
 * Compare two vectors of integers
 * assuming vectors size have impact on comparison.
 *
 * @param x The first vector of integers to compare.
 * @param y The second vector of integers to compare.
 * @return 0 if x == y, -1 if x < y, 1 if x > y.
 */
const short cmp(const std::vector<udigit> &x, const std::vector<udigit> &y) noexcept
{
    const size_t s1 = x.size();
    const size_t s2 = y.size();
    if (s1 != s2)
        return s1 > s2 ? 1 : -1;
    const udigit *p1 = x.data();
    const udigit *p2 = y.data();
    const udigit *end = p1 + s1;
    for (; p1 != end; ++p1, ++p2)
    {
        const udigit &vx = *p1;
        const udigit &vy = *p2;
        if (vx == vy)
            continue;
        return vx > vy ? 1 : -1;
    }
    return 0;
}

/**
 * Compare two vectors of integers
 *
 * @param x The first vector of integers to compare.
 * @param y The second vector of integers to compare.
 * @return 0 if x == y, -1 if x < y, 1 if x > y.
 */
const short fcmp(const std::vector<udigit> &x, const std::vector<udigit> &y) noexcept
{
    const size_t size = std::max(x.size(), y.size());
    auto xi = x.begin();
    auto yi = y.begin();
    for (size_t i = 0; i < size; ++i)
    {
        const udigit &vx = xi != x.end() ? *xi++ : 0;
        const udigit &vy = yi != y.end() ? *yi++ : 0;
        if (vx == vy)
            continue;
        return vx > vy ? 1 : -1;
    }
    return 0;
}

/**
 * Compare vectors of integers
 *
 * @param x integer part.
 * @param fx fraction part.
 * @param y integer part.
 * @param fy fraction part.
 * @return 0 if equal, -1 if lesser, 1 if greater.
 */
inline const short cmp(const std::vector<udigit> &x, const std::vector<udigit> &fx,
                       const std::vector<udigit> &y, const std::vector<udigit> &fy) noexcept
{
    const short icmp = cmp(x, y);
    return (icmp == 0) ? fcmp(fx, fy) : icmp;
}

/**
 * @brief Convert string to vector of integers.
 * @param num_str string of ascii chars 0-9.
 * @return vector of integers 0-9.
 */
const std::vector<udigit> convert(const std::string &num_str)
{
    std::vector<udigit> result;
    result.reserve(num_str.size());
    for (size_t i = 0; i < num_str.size(); ++i)
        result.emplace_back(num_str[i] - '0');
    return result;
}

/**
 * @brief Convert vector of integers to string.
 * @param vec vector of integers 0-9.
 * @return string of ascii chars 0-9.
 */
const std::string convert(const std::vector<udigit> &vec)
{
    std::string result;
    result.reserve(vec.size());
    for (size_t i = 0; i < vec.size(); ++i)
        result.push_back(vec[i] + '0');
    return result;
}

/**
 * @brief Convert number to vector of integers.
 * @param num unsigned long long.
 * @return vector of integers 0-9.
 */
const std::vector<udigit> convert(uint_fast64_t num)
{
    if (num < 10)
        return {static_cast<udigit>(num)};

    size_t size = 2;
    uint_fast64_t tens = 100;
    while (tens < num)
    { // calculate int size
        ++size;
        tens *= 10;
    }
    std::vector<udigit> result;
    result.reserve(size);
    while (num > 0)
    {
        result.insert(result.begin(), num % 10);
        num /= 10;
    }
    return result;
}

/**
 * Convert int/float represented string to vector of integers.
 *
 * @param num_str string of digits to be converted.
 * @param integer vector to store the integer part of the float.
 * @param fraction vector to store the fractional part of the float.
 * @return A boolean indicating the sign of the float.
 * @throws std::invalid_argument, If the string contains non-digit characters.
 */
bool convert_num_str(const std::string &num_str,
                     std::vector<udigit> &integer, std::vector<udigit> &fraction)
{
    size_t size = num_str.size();
    if (size == 0) // if string is empty
        throw std::invalid_argument("Invalid number");
    const size_t pos = num_str.find('.');
    size_t int_size = (pos == std::string::npos) ? size : pos;
    const char *ptr = num_str.data();
    const bool sign = *ptr == '-';
    if (sign || *ptr == '+')
    {
        ++ptr;
        --size;
        --int_size;
    }
    const char *end = ptr + int_size;
    integer.reserve(int_size);
    for (; ptr != end; ++ptr)
    {
        if (*ptr < 48 || *ptr > 58) // if not in 0-9 range
            throw std::invalid_argument("Invalid number");
        integer.emplace_back(*ptr - '0');
    }
    const size_t frac_size = size - int_size;
    if (frac_size == 0)
    { // no fraction
        fraction.assign(1, 0);
        return sign;
    }
    const char *fend = ptr + frac_size;
    fraction.reserve(frac_size);
    for (++ptr; ptr != fend; ++ptr)
    {
        if (*ptr < 48 || *ptr > 58) // if not in 0-9 range
            throw std::invalid_argument("Invalid number");
        fraction.emplace_back(*ptr - '0');
    }
    lstrip_zeros(integer);  // normalize integer
    rstrip_zeros(fraction); // normalize fraction
    return sign;
}

#endif // UTILS_H