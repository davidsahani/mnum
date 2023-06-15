#ifndef IMATH_H
#define IMATH_H
#include <iostream>
#include <vector>
#include <cassert>
#include "utils.h"

/**
 * @brief Perform inplace addition.
 * @param x augend.
 * @param y addend.
 * @param * vector of integers 0-9.
 */
void iadd(std::vector<udigit> &x, const std::vector<udigit> &y)
{
    const size_t x_size = x.size(), y_size = y.size();
    const size_t max_size = std::max(x_size, y_size);
    if (x_size < y_size)
        x.resize(y_size);

    auto xi = x.rbegin();
    auto yi = y.rbegin();

    // skip zeros caused from resizing.
    if (x_size < y_size)
        xi += y_size - x_size;

    udigit nx, ny, res, carry = 0;
    for (int64_t i = max_size - 1; i > -1; --i)
    {
        nx = xi != x.rend() ? *xi++ : 0;
        ny = yi != y.rend() ? *yi++ : 0;
        res = nx + ny + carry;
        carry = (res < 10) ? 0 : (res -= 10, 1);
        x[i] = res;
    }
    if (!carry)
        return;
    x.insert(x.begin(), carry);
}

/**
 * @brief Perform inplace subtraction.
 * @param x minuend.
 * @param y subtrahend.
 * @param comp comparison of x and y.
 * @param * vector of integers 0-9.
 */
void isub(std::vector<udigit> &x, const std::vector<udigit> &y, const short &comp)
{
    if (comp == 0)
    {
        x.assign(1, 0);
        return;
    }
    const bool rev_sub = comp == -1;

    std::vector<udigit> *px = &x, *py;
    py = const_cast<std::vector<udigit> *>(&y);
    const size_t x_size = x.size(), y_size = y.size();
    const size_t max_size = std::max(x_size, y_size);
    if (x_size < y_size)
        x.resize(y_size);
    if (rev_sub)
        std::swap(px, py);

    auto xi = px->rbegin();
    auto yi = py->rbegin();
    // skip zeros caused from resizing.
    if (x_size < y_size)
        (rev_sub ? yi : xi) += y_size - x_size;

    int_fast8_t res;
    udigit nx, ny, borrow = 0;
    for (int64_t i = max_size - 1; i > -1; --i)
    {
        nx = xi != px->rend() ? *xi++ : 0;
        ny = yi != py->rend() ? *yi++ : 0;
        res = nx - ny - borrow;
        borrow = res >= 0 ? 0 : 1;
        res = borrow ? res + 10 : res;
        x[i] = res;
    }
    assert(borrow == 0 && "invalid subtraction");
    lstrip_zeros(x);
}

/**
 * @brief Perform integer multiplication.
 * @param x multiplicand
 * @param y multiplier
 * @param * vector of integers 0-9.
 * @return product of the multiplicand and multiplier.
 */
const std::vector<udigit> mul(const std::vector<udigit> &x, const std::vector<udigit> &y)
{
    std::vector<udigit> result(x.size() + y.size(), 0);
    for (auto xi = x.rbegin(); xi != x.rend(); ++xi)
    {
        udigit carry = 0;
        auto res_it = result.rbegin() + (xi - x.rbegin());

        for (auto yi = y.rbegin(); yi != y.rend(); ++yi, ++res_it)
        {
            udigit prod = *xi * *yi + carry + *res_it;
            carry = prod / 10;
            *res_it = prod % 10;
        }
        *res_it += carry;
    }
    lstrip_zeros(result);
    return result;
}

/**
 * @brief Perform division.
 * @param x dividend
 * @param y divisor
 * @param * vector of integers 0-9.
 * @return quotient and remainder.
 * @throw std::domain_error if division by zero.
 */
inline const std::pair<std::vector<udigit>, double>
divmod(const std::vector<udigit> &x, const std::vector<udigit> &y)
{
    double divisor = y.front();
    for (auto it = next(y.begin()); it != y.end(); ++it)
        divisor = divisor * 10 + *it;

    if (divisor == 0)
        throw std::domain_error("division by zero");

    std::vector<udigit> quot;
    udigit quot_div;
    double term_minus, minus_term, rem = 0.0;
    for (auto ix = x.begin(); ix != x.end(); ++ix)
    {
        term_minus = rem * 10 + *ix;
        quot_div = static_cast<udigit>(term_minus / divisor);
        minus_term = divisor * quot_div;
        rem = term_minus - minus_term;
        quot.push_back(quot_div);
    }
    lstrip_zeros(quot);
    return {quot, rem};
}

/**
 * @brief Perform  true division.
 * @param x dividend.
 * @param y divisor.
 * @param * vectors of intergers 0-9.
 * @return quotient integer and fraction parts.
 * @throw std::domain_error if division by zero.
 */
const std::pair<std::vector<udigit>, std::vector<udigit>>
true_div(const std::vector<udigit> &x, const std::vector<udigit> &y)
{
    double divisor = y.front();
    for (auto it = next(y.begin()); it != y.end(); ++it)
        divisor = divisor * 10 + *it;

    if (divisor == 0)
        throw std::domain_error("division by zero");

    /* perform integer division */
    std::vector<udigit> quot;
    udigit quot_div;
    double term_minus, minus_term, rem = 0.0;
    for (auto ix = x.begin(); ix != x.end(); ++ix)
    {
        term_minus = rem * 10 + *ix;
        quot_div = static_cast<udigit>(term_minus / divisor);
        minus_term = divisor * quot_div;
        rem = term_minus - minus_term;
        quot.push_back(quot_div);
    }
    lstrip_zeros(quot);
    if (rem == 0) // reached exact division
        return {quot, {0}};

    /* perform float division */
    constexpr size_t precision = 20U;
    std::vector<udigit> fquot;
    fquot.reserve(precision);
    for (size_t i = 0; i < precision && rem; ++i)
    {
        term_minus = rem * 10;
        quot_div = static_cast<udigit>(term_minus / divisor);
        minus_term = divisor * quot_div;
        rem = term_minus - minus_term;
        fquot.push_back(quot_div);
    }
    // if didn't reach exact division
    if (rem != 0) // round by one
        iadd(fquot, {1});
    return {quot, fquot};
}

/**
 * @brief Calculate the power.
 * @param base number being raised.
 * @param exp base raised to this power.
 * @param * vector of integers 0-9.
 * @return base ** exp.
 */
inline const std::vector<udigit>
power(const std::vector<udigit> &base, std::vector<udigit> exp)
{
    std::vector<udigit> result(1, 1), one(1, 1);
    for (; exp.front() != 0; isub(exp, one, cmp(exp, one)))
    {
        std::vector<udigit> prod;
        prod = mul(result, base);
        result.swap(prod);
    }
    return result;
}

/* ========================================================
   *                 SIGNED OPERATIONS                    *
   =======================================================*/

/**
 * @brief Calculate signed power.
 * @param base number being raised.
 * @param exp base raised to this power.
 * @param xsign sign of the base.
 * @param ysign sign of the exponent.
 * @param * vector of integers 0-9.
 * @return base ** exp (integer and fraction parts).
 * @attention power sign is determined by the base sign.
 */
const std::pair<std::vector<udigit>, std::vector<udigit>>
signed_power(const std::vector<udigit> &base, const std::vector<udigit> &exp,
             const bool &xsign, const bool &ysign)
{
    if (base.size() == 1 && base.front() == 0)
        return {{0}, {0}};

    if (exp.size() == 1 && exp.front() == 0)
        return {{1}, base};

    const std::vector<udigit> pow = power(base, exp);
    if (!ysign) // positive
        return {pow, {0}};
    return true_div({1}, pow);
}

#endif // IMATH_H