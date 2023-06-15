#ifndef FMATH_H
#define FMATH_H
#include <iostream>
#include <vector>
#include <cassert>
#include "utils.h"
#include "imath.h"

/**
 * @brief Perform inplace float addition.
 * @param x augend integer part.
 * @param fx augend fraction part.
 * @param y addend integer part.
 * @param fy addend fraction part.
 * @param * vectors of intergers 0-9.
 */
void iadd(std::vector<udigit> &x, std::vector<udigit> &fx,
          const std::vector<udigit> &y, const std::vector<udigit> &fy)
{
    /* perform float addition */
    const size_t fx_size = fx.size(), fy_size = fy.size();
    const size_t fmax_size = std::max(fx_size, fy_size);
    if (fx_size < fy_size)
        fx.resize(fy_size);

    auto fxi = fx.rbegin();
    udigit nx, ny, res, carry = 0;
    for (int64_t i = fmax_size - 1; i > -1; --i, ++fxi)
    {
        const size_t idx = static_cast<size_t>(i);
        ny = fy_size > idx ? fy[idx] : 0;
        res = *fxi + ny + carry;
        carry = (res < 10) ? 0 : (res -= 10, 1);
        fx[idx] = res;
    }
    /* perform integer addition */
    const size_t x_size = x.size(), y_size = y.size();
    const size_t max_size = std::max(x_size, y_size);
    if (x_size < y_size)
        x.resize(y_size);

    auto xi = x.rbegin();
    auto yi = y.rbegin();

    // skip zeros caused from resizing.
    if (x_size < y_size)
        xi += y_size - x_size;

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
 * @brief Perform inplace float subtraction.
 * @param x minuend integer part.
 * @param fx minuend fraction part.
 * @param y subtrahend integer part.
 * @param fy subtrahend fraction part.
 * @param * vectors of intergers 0-9.
 */
void isub(std::vector<udigit> &x, std::vector<udigit> &fx,
          const std::vector<udigit> &y, const std::vector<udigit> &fy,
          const short &icmp, const short &comp)
{
    if (icmp == 0)
    { // set x => 0 if x == y
        x.assign(1, 0);
        if (comp == 0)
        { // set fx => 0 if fx == fy
            fx.assign(1, 0);
            return;
        }
    }
    const bool rev_sub = comp == -1;

    /* perform float subtraction */
    std::vector<udigit> *px = &fx, *py;
    py = const_cast<std::vector<udigit> *>(&fy);
    size_t x_size = fx.size(), y_size = fy.size();
    size_t max_size = std::max(x_size, y_size);
    if (x_size < y_size)
        fx.resize(y_size);
    if (rev_sub)
    {
        std::swap(px, py);
        std::swap(x_size, y_size);
    }
    int_fast8_t res;
    udigit nx, ny, borrow = 0;
    for (int64_t i = max_size - 1; i > -1; --i)
    {
        const size_t idx = static_cast<size_t>(i);
        nx = x_size > idx ? (*px)[idx] : 0;
        ny = y_size > idx ? (*py)[idx] : 0;
        res = nx - ny - borrow;
        borrow = res >= 0 ? 0 : 1;
        res = borrow ? res + 10 : res;
        fx[idx] = res;
    }
    if (icmp == 0) // x == y
    {
        assert(borrow == 0 &&
               "invalid integer subtraction");
        return;
    }
    /* perform integer subtraction */
    px = &x, py = const_cast<std::vector<udigit> *>(&y);
    x_size = x.size(), y_size = y.size();
    max_size = std::max(x_size, y_size);
    if (x_size < y_size)
        x.resize(y_size);
    if (rev_sub)
        std::swap(px, py);

    auto xi = px->rbegin();
    auto yi = py->rbegin();
    // skip zeros caused from resizing.
    if (x_size < y_size)
        (rev_sub ? yi : xi) += y_size - x_size;

    for (int64_t i = max_size - 1; i > -1; --i)
    {
        nx = xi != px->rend() ? *xi++ : 0;
        ny = yi != py->rend() ? *yi++ : 0;
        res = nx - ny - borrow;
        borrow = res >= 0 ? 0 : 1;
        res = borrow ? res + 10 : res;
        x[i] = res;
    }
    lstrip_zeros(x);
    assert(borrow == 0 && "invalid float subtraction");
}

/**
 * @brief Perform inplace float multiplication.
 * @param x multiplicand integer part.
 * @param fx multiplicand fraction part.
 * @param y multiplier integer part.
 * @param fy multiplier fraction part.
 * @param * vectors of intergers 0-9.
 */
void imul(std::vector<udigit> &x, std::vector<udigit> &fx,
          const std::vector<udigit> &y, const std::vector<udigit> &fy)
{
    const size_t fx_size = fx.size();
    const size_t fy_size = fy.size();
    if ((fx_size == 1 && fx[0] == 0) &&
        (fy_size == 1 && fy[0] == 0))
    { // if fractions are zero
        std::vector<udigit>
            res = mul(x, y);
        x.swap(res);
        return;
    }
    if ((x[0] == 0 && (fx_size == 1 && fx[0] == 0)) ||
        (y[0] == 0 && (fy_size == 1 && fy[0] == 0)))
    { // if multiplied by zero
        x.assign(1, 0);
        fx.assign(1, 0);
        return;
    }
    std::vector<udigit> xfx;
    xfx.reserve(x.size() + fx_size);
    xfx = std::move(x);
    std::move(fx.begin(), fx.end(), std::back_inserter(xfx));

    std::vector<udigit> yfy;
    yfy.reserve(y.size() + fy_size);
    yfy.assign(y.begin(), y.end());
    yfy.insert(yfy.end(), fy.begin(), fy.end());

    std::vector<udigit> result(xfx.size() + yfy.size(), 0);
    for (auto xi = xfx.rbegin(); xi != xfx.rend(); ++xi)
    {
        udigit carry = 0;
        auto res_it = result.rbegin() + (xi - xfx.rbegin());
        for (auto yi = yfy.rbegin(); yi != yfy.rend(); ++yi, ++res_it)
        {
            udigit prod = *xi * *yi + carry + *res_it;
            carry = prod / 10;
            *res_it = prod % 10;
        }
        *res_it += carry;
    }
    x.clear();  // clear previous values
    fx.clear(); // clear previous values

    const size_t dec_point = result.size() - (fx_size + fy_size);
    x.reserve(dec_point ? dec_point : 1);
    fx.reserve(result.size() - dec_point);

    std::move(result.begin(), result.begin() + dec_point, std::back_inserter(x));
    std::move(result.begin() + dec_point, result.end(), std::back_inserter(fx));
    lstrip_zeros(x);  // remove trailing zeros (if any present in x if empty add 0)
    rstrip_zeros(fx); // remove trailing zeros (if any present in fx if empty add 0)
}

/**
 * @brief Perform float division.
 * @param x dividend integer part.
 * @param fx dividend fraction part.
 * @param y divisor integer part.
 * @param fy divisor fraction part.
 * @param * vectors of intergers 0-9.
 * @return quotient integer part and remainder.
 * @throw std::domain_error if division by zero.
 */
inline const std::pair<std::vector<udigit>, double> divmod(
    const std::vector<udigit> &x, const std::vector<udigit> &fx,
    const std::vector<udigit> &y, const std::vector<udigit> &fy)
{
    size_t fx_size = fx.size();
    size_t fy_size = fy.size();
    if (fx_size == 1 && (fx.front() == 0))
        fx_size = 0;
    if (fy_size == 1 && (fy.front() == 0))
        fy_size = 0;

    std::vector<udigit> xfx, yfy;
    xfx.reserve(x.size() + fx_size);
    yfy.reserve(y.size() + fy_size);
    xfx.assign(x.begin(), x.end());
    yfy.assign(y.begin(), y.end());

    if (fx_size) // if fx doesn't only contain zero
        xfx.insert(xfx.end(), fx.begin(), fx.end());
    if (fy_size) // if fy doesn't only contain zero
        yfy.insert(yfy.end(), fy.begin(), fy.end());

    if (fx_size < fy_size) // normalize denominator
        xfx.insert(xfx.end(), fy_size - fx_size, 0);
    else if (fx_size > fy_size) // normalize nominator
        yfy.insert(yfy.end(), fx_size - fy_size, 0);

    return divmod(xfx, yfy); // perform division
}

/**
 * @brief Perform float true division.
 * @param x dividend integer part.
 * @param fx dividend fraction part.
 * @param y divisor integer part.
 * @param fy divisor fraction part.
 * @param * vectors of intergers 0-9.
 * @return quotient integer and fraction parts.
 * @throw std::domain_error if division by zero.
 */
const std::pair<std::vector<udigit>, std::vector<udigit>>
true_div(const std::vector<udigit> &x, const std::vector<udigit> &fx,
         const std::vector<udigit> &y, const std::vector<udigit> &fy)
{
    size_t fx_size = fx.size();
    size_t fy_size = fy.size();
    if (fx_size == 1 && (fx.front() == 0))
        fx_size = 0;
    if (fy_size == 1 && (fy.front() == 0))
        fy_size = 0;

    std::vector<udigit> xfx, yfy;
    xfx.reserve(x.size() + fx_size);
    yfy.reserve(y.size() + fy_size);
    xfx.assign(x.begin(), x.end());
    yfy.assign(y.begin(), y.end());

    if (fx_size) // if fx doesn't only contain zero
        xfx.insert(xfx.end(), fx.begin(), fx.end());
    if (fy_size) // if fy doesn't only contain zero
        yfy.insert(yfy.end(), fy.begin(), fy.end());

    if (fx_size < fy_size) // normalize denominator
        xfx.insert(xfx.end(), fy_size - fx_size, 0);
    else if (fx_size > fy_size) // normalize nominator
        yfy.insert(yfy.end(), fx_size - fy_size, 0);

    return true_div(xfx, yfy); // perform true division
}

/* ========================================================
   *                 SIGNED OPERATIONS                    *
   =======================================================*/

/**
 * @brief Perform inplace signed float addition.
 * @param x augend integer part.
 * @param fx augend fraction part.
 * @param y addend integer part.
 * @param fy addend fraction part.
 * @param xsign sign of the augend.
 * @param ysign sign of the addend.
 * @param * vector of integers 0-9.
 * @return sign of the result.
 */
bool signed_iadd(
    std::vector<udigit> &x, std::vector<udigit> &fx,
    const std::vector<udigit> &y, const std::vector<udigit> &fy,
    const bool &xsign, const bool &ysign)
{
    // positive
    if (xsign == ysign)
    {
        iadd(x, fx, y, fy);
        return xsign && ysign;
    }
    // negative
    const short icmp = cmp(x, y);
    const short comp = (icmp == 0) ? fcmp(fx, fy) : icmp;
    isub(x, fx, y, fy, icmp, comp);
    if (comp == 0)
        return false;
    return (comp == 1) ? xsign && !ysign : !xsign && ysign;
}

/**
 * @brief Perform inplace signed float subtraction.
 * @param x minuend integer part.
 * @param fx minuend fraction part.
 * @param y subtrahend integer part.
 * @param fy subtrahend fraction part.
 * @param xsign sign of the minuend.
 * @param ysign sign of the subtrahend.
 * @param * vector of integers 0-9.
 * @return sign of the result.
 */
bool signed_isub(
    std::vector<udigit> &x, std::vector<udigit> &fx,
    const std::vector<udigit> &y, const std::vector<udigit> &fy,
    const bool &xsign, const bool &ysign)
{
    // positive
    if (xsign != ysign)
    {
        iadd(x, fx, y, fy);
        return xsign && !ysign;
    }
    // negative
    const short icmp = cmp(x, y);
    const short comp = (icmp == 0) ? fcmp(fx, fy) : icmp;
    isub(x, fx, y, fy, icmp, comp);
    if (comp == 0)
        return false;
    return (comp == 1) ? xsign && ysign : !xsign && !ysign;
}

/**
 * @brief Perform signed floor division.
 * @param x dividend integer part.
 * @param fx dividend fraction part.
 * @param y divisor integer part.
 * @param fy divisor fraction part.
 * @param xsign sign of the dividend.
 * @param ysign sign of the divisor.
 * @param * vector of integers 0-9.
 * @return quotient and it's sign.
 * @throw std::domain_error if division by zero.
 */
const std::pair<std::vector<udigit>, bool> signed_floor_div(
    const std::vector<udigit> &x, const std::vector<udigit> &fx,
    const std::vector<udigit> &y, const std::vector<udigit> &fy,
    const bool &xsign, const bool &ysign)
{
    std::pair<std::vector<udigit>, double> div;
    div = divmod(x, fx, y, fy);
    if (xsign == ysign)
        return {div.first, false};

    if (div.second != 0)
    { // round towards negative infinity
        std::vector<udigit> one(1, 1);
        iadd(div.first, one);
    }
    return {div.first, true};
}

/**
 * @brief Perform inplace signed float modulus.
 * @param x dividend integer part.
 * @param fx dividend fraction part.
 * @param y divisor integer part.
 * @param fy divisor fraction part.
 * @param xsign sign of the dividend.
 * @param ysign sign of the divisor.
 * @param * vectors of intergers 0-9.
 * @throw std::domain_error if division by zero.
 */
bool signed_imod(std::vector<udigit> &x, std::vector<udigit> &fx,
                 const std::vector<udigit> &y, const std::vector<udigit> &fy,
                 const bool &xsign, const bool &ysign)
{ // using formula: remainder = dividend - divisor * quotient
    std::pair<std::vector<udigit>, bool> div;
    div = signed_floor_div(x, fx, y, fy, xsign, ysign);
    std::vector<udigit> zero(1, 0);
    imul(div.first, zero, y, fy);
    return signed_isub(x, fx, div.first, zero, xsign, div.second != ysign);
}

/**
 * @brief Perform signed float power.
 * @param x base integer part.
 * @param fx base fraction part.
 * @param fx base fraction part.
 * @param y exponent integer part.
 * @param fy exponent fraction part.
 * @param xsign sign of the base.
 * @param ysign sign of the exponent.
 * @param * vector of integers 0-9.
 * @return base ** exp (integer and fraction parts).
 * @attention power sign is determined by the base sign.
 */
const std::pair<std::vector<udigit>, std::vector<udigit>>
signed_power(const std::vector<udigit> &x, const std::vector<udigit> &fx,
             const std::vector<udigit> &y, const std::vector<udigit> &fy,
             const bool &xsign, const bool &ysign)
{
    const bool is_exp_frac_zero = (fy.size() == 1 && fy[0] == 0);
    const bool is_frac_zeros = fx.size() == 1 && fx[0] == 0 && is_exp_frac_zero;

    if (!is_exp_frac_zero)
        throw std::domain_error("can't power fractional exponent");

    if (is_frac_zeros)
        return signed_power(x, y, xsign, ysign);

    std::vector<udigit> exp = y;
    std::vector<udigit> res_x(1, 1), res_fx(1, 0), one(1, 1);
    for (; exp.front() != 0; isub(exp, one, cmp(exp, one)))
    {
        imul(res_x, res_fx, x, fx);
    }
    if (!ysign) // positive
        return {res_x, res_fx};
    return true_div({1}, {0}, res_x, res_fx);
}

#endif // FMATH_H