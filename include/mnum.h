#ifndef MNUM_H
#define MNUM_H
#include <algorithm>
#include "fmath.h"

class mnum
{
public:
    bool sign;
    std::vector<udigit> integer;
    std::vector<udigit> fraction;

public:
    mnum(const mnum &self) noexcept
    {
        this->sign = self.sign;
        this->integer = self.integer;
        this->fraction = self.fraction;
    }
    mnum(const std::vector<udigit> &integer,
         const std::vector<udigit> &fraction,
         const bool sign) noexcept
    {
        this->sign = sign;
        this->integer = integer;
        this->fraction = fraction;
    }
    mnum(const int_fast64_t &num) noexcept
    {
        this->sign = num < 0;
        this->integer = convert(std::abs(num));
        this->fraction = {0};
    }
    mnum(const std::string &num_str)
    {
        this->sign = convert_num_str(
            num_str, this->integer,
            this->fraction);
    }
    bool operator==(const mnum &y) const noexcept
    {
        if (this->integer.front() == 0 && y.integer.front() == 0 &&
            this->fraction.size() == 1 && this->fraction.front() == 0 &&
            y.fraction.size() == 1 && y.fraction.front() == 0)
            return true;
        if (this->sign != y.sign)
            return false;
        return cmp(this->integer, this->fraction, y.integer, y.fraction) == 0;
    }
    bool operator!=(const mnum &y) const noexcept
    {
        if (this->integer.front() == 0 && y.integer.front() == 0 &&
            this->fraction.size() == 1 && this->fraction.front() == 0 &&
            y.fraction.size() == 1 && y.fraction.front() == 0)
            return false;
        if (this->sign != y.sign)
            return true;
        return cmp(this->integer, this->fraction, y.integer, y.fraction) != 0;
    }
    bool operator<(const mnum &y) const noexcept
    {
        const short comp = cmp(
            this->integer, this->fraction, y.integer, y.fraction);
        if (comp == 0)
        {
            if (this->integer.front() == 0 &&
                y.integer.front() == 0)
                return false;
            return this->sign > y.sign;
        }
        if (this->sign && y.sign) // both negative
            return comp == -1;
        if (this->sign < y.sign) // y is negative
            return false;
        return this->sign > y.sign || comp == -1;
    }
    bool operator>(const mnum &y) const noexcept
    {
        const short comp = cmp(
            this->integer, this->fraction, y.integer, y.fraction);
        if (comp == 0)
        {
            if (this->integer.front() == 0 &&
                y.integer.front() == 0)
                return false;
            return this->sign < y.sign;
        }
        if (this->sign && y.sign) // both negative
            return comp == -1;
        if (this->sign > y.sign) // y is negative
            return false;
        return this->sign < y.sign || comp == 1;
    }
    bool operator<=(const mnum &y) const noexcept
    {
        const short comp = cmp(
            this->integer, this->fraction, y.integer, y.fraction);
        if (comp == 0)
        {
            if (this->integer.front() == 0 &&
                y.integer.front() == 0)
                return true;
            return this->sign > y.sign || this->sign == y.sign;
        }
        if (this->sign && y.sign) // both negative
            return comp == -1;
        if (this->sign < y.sign) // y is negative
            return false;
        return this->sign > y.sign || comp == -1;
    }
    bool operator>=(const mnum &y) const noexcept
    {
        const short comp = cmp(
            this->integer, this->fraction, y.integer, y.fraction);
        if (comp == 0)
        {
            if (this->integer.front() == 0 &&
                y.integer.front() == 0)
                return true;
            return this->sign < y.sign || this->sign == y.sign;
        }
        if (this->sign && y.sign) // both negative
            return comp == -1;
        if (this->sign > y.sign) // y is negative
            return false;
        return this->sign < y.sign || comp == 1;
    }
    mnum abs() const noexcept
    {
        return mnum(this->integer, this->fraction, false);
    }
    mnum operator+() const noexcept
    {
        return mnum(this->integer, this->fraction, this->sign);
    }
    mnum operator-() const noexcept
    {
        return mnum(this->integer, this->fraction, !this->sign);
    }
    mnum operator+(const mnum &y) const
    {
        std::vector<udigit> x, fx;
        x = this->integer;
        fx = this->fraction;
        const bool sign = signed_iadd(
            x, fx, y.integer, y.fraction,
            this->sign, y.sign);
        return mnum(x, fx, sign);
    }
    void operator+=(const mnum &y)
    {
        this->sign = signed_iadd(
            this->integer, this->fraction,
            y.integer, y.fraction,
            this->sign, y.sign);
    }
    mnum operator-(const mnum &y) const
    {
        std::vector<udigit> x, fx;
        x = this->integer;
        fx = this->fraction;
        const bool sign = signed_isub(
            x, fx, y.integer, y.fraction,
            this->sign, y.sign);
        return mnum(x, fx, sign);
    }
    void operator-=(const mnum &y)
    {
        this->sign = signed_isub(
            this->integer, this->fraction,
            y.integer, y.fraction,
            this->sign, y.sign);
    }
    mnum operator*(const mnum &y) const
    {
        std::vector<udigit> x, fx;
        x = this->integer, fx = this->fraction;
        imul(x, fx, y.integer, y.fraction);
        return mnum(x, fx, this->sign ^ y.sign);
    }
    void operator*=(const mnum &y)
    {
        imul(this->integer, this->fraction,
             y.integer, y.fraction);
        this->sign = this->sign ^ y.sign;
    }
    mnum operator/(const mnum &y) const
    {
        std::pair<std::vector<udigit>, std::vector<udigit>> div;
        div = true_div(this->integer, this->fraction, y.integer, y.fraction);
        return mnum(div.first, div.second, this->sign ^ y.sign);
    }
    void operator/=(const mnum &y)
    {
        std::pair<std::vector<udigit>, std::vector<udigit>> div;
        div = true_div(this->integer, this->fraction, y.integer, y.fraction);
        this->integer.swap(div.first);
        this->fraction.swap(div.second);
        this->sign = this->sign ^ y.sign;
    }
    mnum div(const mnum &y) const
    {
        std::vector<udigit> quot = \
        divmod(this->integer, this->fraction, y.integer, y.fraction).first;
        return mnum(quot, {0}, this->sign ^ y.sign);
    }
    void idiv(const mnum &y)
    {
        std::vector<udigit> zero(1, 0), quot = \
        divmod(this->integer, this->fraction, y.integer, y.fraction).first;
        this->integer.swap(quot);
        this->fraction.swap(zero);
        this->sign = this->sign ^ y.sign;
    }
    // perform floor division
    mnum floor_div(const mnum &y) const
    {
        const std::pair<std::vector<udigit>, bool> div =
            signed_floor_div(
                this->integer, this->fraction,
                y.integer, y.fraction, this->sign, y.sign);
        return mnum(div.first, {0}, div.second);
    }
    // perform inplace floor division
    void ifloor_div(const mnum &y)
    {
        std::pair<std::vector<udigit>, bool> div;
        div = signed_floor_div(
            this->integer, this->fraction,
            y.integer, y.fraction, this->sign, y.sign);
        std::vector<udigit> zero(1, 0);
        this->integer.swap(div.first);
        this->fraction.swap(zero);
        this->sign = div.second;
    }
    mnum operator%(const mnum &y) const
    {
        std::vector<udigit> x, fx;
        x = this->integer, fx = this->fraction;
        const bool sign = signed_imod(
            x, fx, y.integer, y.fraction,
            this->sign, y.sign);
        return mnum(x, fx, sign);
    }
    void operator%=(const mnum &y)
    {
        this->sign = signed_imod(
            this->integer, this->fraction,
            y.integer, y.fraction,
            this->sign, y.sign);
    }
    mnum pow(const mnum &y) const
    {
        const std::pair<std::vector<udigit>, std::vector<udigit>> res =
            signed_power(this->integer, this->fraction,
                         y.integer, y.fraction, this->sign, y.sign);
        return mnum(res.first, res.second, this->sign);
    }
    mnum as_int() const noexcept
    {
        return mnum(this->integer, {0}, this->sign);
    }
    mnum as_float() const noexcept
    {
        return mnum({0}, this->fraction, this->sign);
    }
    mnum frac_part() const noexcept
    {
        return mnum(this->fraction, {0}, this->sign);
    }
    size_t size() const noexcept
    {
        const size_t frac_size = this->fraction.size();
        const bool is_frac_zero = frac_size == 1 && this->fraction[0] == 0;
        return this->integer.size() + (is_frac_zero ? 0 : frac_size);
    }
    udigit &operator[](const size_t &index) noexcept
    {
        const size_t size = this->integer.size();
        if (index < size)
            return this->integer[index];
        return this->fraction[index - size];
    }
    static const bool contains(
        const std::vector<udigit> &x,
        const std::vector<udigit> &y) noexcept
    {
        const size_t x_size = x.size();
        const size_t y_size = y.size();
        for (size_t idx = 0; idx < y_size; ++idx)
        {
            bool match = true;
            for (size_t j = 0; j < x_size && match; ++j)
            {
                const size_t i = idx + j;
                if (i >= y_size)
                {
                    match = false;
                    break;
                }
                match = x[j] == y[i];
            }
            if (match)
                return true;
        }
        return false;
    }
    bool contains(const mnum &y) const noexcept
    {
        if (y.fraction.size() == 1 && y.fraction[0] == 0)
            return this->contains(y.integer, this->integer) ||
                (!(this->fraction.size() == 1 && this->fraction[0] == 0) &&
                this->contains(y.integer, this->fraction));

        return this->contains(y.integer, this->integer) &&
                this->contains(y.fraction, this->fraction);
    }
    int_fast64_t find(const udigit &value) const noexcept
    {
        size_t idx = 0;
        const udigit *ptr = this->integer.data();
        const udigit *end = ptr + this->integer.size();
        for (; ptr != end; ++ptr, ++idx)
            if (*ptr == value)
                return idx;
        const size_t frac_size = this->fraction.size();
        if (frac_size == 1 && this->fraction[0] == 0)
            return -1; // stop search if fraction is zero
        const udigit *fptr = this->fraction.data();
        const udigit *fend = fptr + frac_size;
        for (; fptr != fend; ++fptr, ++idx)
            if (*fptr == value)
                return idx;
        return -1;
    }
    int_fast64_t rfind(const udigit &value) const noexcept
    {
        const size_t int_size = this->integer.size();
        const size_t frac_size = this->fraction.size();
        size_t idx = int_size - 1;
        if ((frac_size == 1 && this->fraction[0] == 0))
            idx = int_size - 1;
        else
        {
            idx += frac_size;
            auto it_fend = this->fraction.rend();
            for (auto it = this->fraction.rbegin();
                 it != it_fend; ++it, --idx)
                if (*it == value)
                    return idx;
        }
        auto it_end = this->integer.rend();
        for (auto it = this->integer.rbegin();
             it != it_end; ++it, --idx)
            if (*it == value)
                return idx;
        return -1;
    }
    size_t index(const udigit &value) const
    {
        const int_fast64_t idx = this->find(value);
        if (idx == -1)
            throw std::range_error("value not found");
        return idx;
    }
    size_t count(const udigit &value) const noexcept
    {
        size_t c = 0;
        const udigit *ptr = this->integer.data();
        const udigit *end = ptr + this->integer.size();
        for (; ptr != end; ++ptr)
            if (*ptr == value)
                ++c;
        const size_t frac_size = this->fraction.size();
        if (frac_size == 1 && this->fraction[0] == 0)
            return c; // stop counting if fraction is zero
        const udigit *fptr = this->fraction.data();
        const udigit *fend = fptr + frac_size;
        for (; fptr != fend; ++fptr)
            if (*fptr == value)
                ++c;
        return c;
    }
    void join(const mnum &y) noexcept
    {
        this->sign = this->sign || y.sign;
        const size_t int_size = y.integer.size();
        const size_t frac_size = y.fraction.size();
        const bool int_zero = int_size == 1 && y.integer[0] == 0;
        const bool frac_zero = frac_size == 1 && y.fraction[0] == 0;
        if (!int_zero || (int_zero && frac_zero))
        {
            this->integer.insert(this->integer.end(), y.integer.begin(), y.integer.end());
            lstrip_zeros(this->integer); // normalize integer
        }
        if (!frac_zero)
        {   
            if (this->fraction.size() == 1 && this->fraction[0] == 0)
                this->fraction.clear(); // remove zero before inserting new fractions
            this->fraction.insert(this->fraction.end(), y.fraction.begin(), y.fraction.end());
            rstrip_zeros(this->fraction); // normalize fraction
        }
    }
    mnum add(const mnum &y) const noexcept
    {
        std::vector<udigit> x, fx;
        x = this->integer, fx = this->fraction;
        const size_t int_size = y.integer.size();
        const size_t frac_size = y.fraction.size();
        const bool int_zero = int_size == 1 && y.integer[0] == 0;
        const bool frac_zero = frac_size == 1 && y.fraction[0] == 0;
        if (!int_zero || (int_zero && frac_zero))
        {
            x.insert(x.end(), y.integer.begin(), y.integer.end());
            lstrip_zeros(x); // normalize integer
        }
        if (!frac_zero)
        {
            if (fx.size() == 1 && fx[0] == 0)
                fx.clear(); // remove zero before inserting new fractions
            fx.insert(fx.end(), y.fraction.begin(), y.fraction.end());
            rstrip_zeros(fx); // normalize fraction
        }
        return mnum(x, fx, this->sign || y.sign);
    }
    void insert(const size_t &index, udigit value)
    {
        const size_t size = this->integer.size();
        if (index < size)
        {
            this->integer.insert(this->integer.begin() + index, value);
            lstrip_zeros(this->integer); // normalize integer
        }
        else
        {
            this->fraction.insert(this->fraction.begin() + index - size, value);
            rstrip_zeros(this->fraction); // normalize fraction
        }
    }
    void erase(const size_t &index) noexcept
    {
        const size_t size = this->integer.size();
        if (index < size)
        {
            this->integer.erase(this->integer.begin() + index);
            lstrip_zeros(this->integer); // normalize integer
        }
        else
        {
            this->fraction.erase(this->fraction.begin() + index - size);
            rstrip_zeros(this->fraction); // normalize fraction
        }
    }
    mnum pop(const int_fast64_t &index) noexcept
    {
        const size_t idx = index < 0 ? this->size() + index : index;
        const udigit value = (*this)[idx];
        this->erase(idx);
        return mnum({value}, {0}, false);
    }
    void remove(const udigit &value)
    {
        const int_fast64_t index = this->find(value);
        if (index == -1)
            throw std::range_error("value not found");
        this->erase(index);
    }
    void clear() noexcept
    {
        this->sign = false;
        std::vector<udigit> zero1(1, 0);
        std::vector<udigit> zero2(1, 0);
        this->integer.swap(zero1);
        this->fraction.swap(zero2);
    }
    void reverse() noexcept
    {
        std::reverse(this->integer.begin(), this->integer.end());
        std::reverse(this->fraction.begin(), this->fraction.end());
        lstrip_zeros(this->integer); // normalize integer
        rstrip_zeros(this->fraction); // normalize fraction
    }
    void sort() noexcept
    {
        std::sort(this->integer.begin(), this->integer.end());
        std::sort(this->fraction.begin(), this->fraction.end());
        lstrip_zeros(this->integer); // normalize integer
        rstrip_zeros(this->fraction); // normalize fraction
    }
    std::string float_str() const
    {
        std::string result;
        const size_t alloc_size =
            this->integer.size() +
            this->fraction.size();
        if (!this->sign)
            result.reserve(alloc_size);
        else
        {
            result.reserve(alloc_size + 1);
            result.push_back('-');
        }
        result.append(convert(this->integer));
        result.push_back('.');
        result.append(convert(this->fraction));
        return result;
    }
    std::string str() const
    {
        const size_t frac_size = this->fraction.size();
        const bool is_frac_zero = frac_size == 1 && this->fraction[0] == 0;

        std::string result;
        const size_t alloc_size =
            this->integer.size() +
            frac_size + (is_frac_zero ? 0 : 1);
        if (!this->sign)
            result.reserve(alloc_size);
        else
        {
            result.reserve(alloc_size + 1);
            result.push_back('-');
        }
        result.append(convert(this->integer));
        if (is_frac_zero)
            return result;
        result.push_back('.');
        result.append(convert(this->fraction));
        return result;
    }
};

#endif // MNUM_H
