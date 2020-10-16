// The template and inlines for the -*- C++ -*- fraction classes.
// Initially implemented by Wai-Shing Luk <luk036@gmail.com>
//

/*! @file include/fractions.hpp
 *  This is a C++ Library header.
 */

#pragma once

#include <cmath>
#include <numeric>
#include <type_traits>

namespace fun
{

/*!
 * @brief Greatest common divider
 *
 * @tparam _Mn
 * @param[in] __m
 * @param[in] __n
 * @return _Mn
 */
template <typename Mn>
constexpr Mn gcd(Mn _m, Mn _n)
{
    return _m == 0 ? abs(_n) : _n == 0 ? abs(_m) : gcd(_n, _m % _n);
}

/*!
 * @brief Least common multiple
 *
 * @tparam _Mn
 * @param[in] __m
 * @param[in] __n
 * @return _Mn
 */
template <typename _Mn>
constexpr _Mn lcm(_Mn __m, _Mn __n)
{
    return (__m != 0 && __n != 0) ? (abs(__m) / gcd(__m, __n)) * abs(__n) : 0;
}

template <typename Z>
struct Fraction
{
    using _Self = Fraction<Z>;

    Z _numerator;
    Z _denominator;

    /*!
     * @brief Construct a new Fraction object
     *
     * @param[in] numerator
     * @param[in] denominator
     */
    constexpr Fraction(Z&& common, const Z& numerator, const Z& denominator)
        : _numerator {(common == Z(0)) ? Z(0) : numerator / common}
        , _denominator {(common == Z(0)) ? Z(0) : denominator / common}
    {
    }

    /*!
     * @brief Construct a new Fraction object
     *
     * @param[in] numerator
     * @param[in] denominator
     */
    constexpr Fraction(const Z& numerator, const Z& denominator)
       : Fraction(gcd(numerator, denominator), numerator, denominator)
    {
    }

    /*!
     * @brief Construct a new Fraction object
     *
     * @param[in] numerator
     */
    constexpr explicit Fraction(const Z& numerator)
        : _numerator {numerator}
        , _denominator {1}
    {
    }

    /*!
     * @brief Construct a new Fraction object
     *
     */
    constexpr Fraction() = default;
    // Fraction(const _Self &) = delete;
    // Fraction(_Self &&) = default;

    /*!
     * @brief
     *
     * @return const Z&
     */
    constexpr auto numerator() const -> const Z&
    {
        return _numerator;
    }

    /*!
     * @brief
     *
     * @return const Z&
     */
    constexpr auto denominator() const -> const Z&
    {
        return _denominator;
    }

    /*!
     * @brief
     *
     * @return _Self
     */
    constexpr auto abs() const -> _Self
    {
        return _Self(std::abs(_numerator), std::abs(_denominator));
    }

    /*!
     * @brief
     *
     */
    constexpr void reciprocal()
    {
        std::swap(_numerator, _denominator);
    }

    /*!
     * @brief
     *
     * @return _Self
     */
    constexpr auto operator-() const -> _Self
    {
        return _Self(-_numerator, _denominator);
    }

    /*!
     * @brief
     *
     * @param[in] frac
     * @return _Self
     */
    constexpr auto operator+(const _Self& frac) const -> _Self
    {
        if (_denominator == frac._denominator)
        {
            return _Self(_numerator + frac._numerator, _denominator);
        }

        const auto common = lcm(_denominator, frac._denominator);
        const auto n = common / _denominator * _numerator +
            common / frac._denominator * frac._numerator;
        return _Self(n, common);
    }

    /*!
     * @brief
     *
     * @param[in] frac
     * @return _Self
     */
    constexpr auto operator-(const _Self& frac) const -> _Self
    {
        return *this + (-frac);
    }

    /*!
     * @brief
     *
     * @param[in] frac
     * @return _Self
     */
    constexpr auto operator*(const _Self& frac) const -> _Self
    {
        const auto n = _numerator * frac._numerator;
        const auto d = _denominator * frac._denominator;
        return _Self(n, d);
    }

    /*!
     * @brief
     *
     * @param[in] frac
     * @return _Self
     */
    constexpr auto operator/(_Self frac) const -> _Self
    {
        frac.reciprocal();
        return *this * frac;
    }

    /*!
     * @brief
     *
     * @param[in] i
     * @return _Self
     */
    constexpr auto operator+(const Z& i) const -> _Self
    {
        const auto n = _numerator + _denominator * i;
        return _Self(n, _denominator);
    }

    /*!
     * @brief
     *
     * @param[in] i
     * @return _Self
     */
    constexpr auto operator-(const Z& i) const -> _Self
    {
        return *this + (-i);
    }

    /*!
     * @brief
     *
     * @param[in] i
     * @return _Self
     */
    constexpr auto operator*(const Z& i) const -> _Self
    {
        const auto n = _numerator * i;
        return _Self(n, _denominator);
    }

    /*!
     * @brief
     *
     * @param[in] i
     * @return _Self
     */
    constexpr auto operator/(const Z& i) const -> _Self
    {
        const auto d = _denominator * i;
        return _Self(_numerator, d);
    }

    /*!
     * @brief
     *
     * @param[in] frac
     * @return _Self
     */
    constexpr auto operator+=(const _Self& frac) -> _Self
    {
        return *this = *this + frac;
    }

    /*!
     * @brief
     *
     * @param[in] frac
     * @return _Self
     */
    constexpr auto operator-=(const _Self& frac) -> _Self
    {
        return *this = *this - frac;
    }

    /*!
     * @brief
     *
     * @param[in] frac
     * @return _Self
     */
    constexpr auto operator*=(const _Self& frac) -> _Self
    {
        return *this = *this * frac;
    }

    /*!
     * @brief
     *
     * @param[in] frac
     * @return _Self
     */
    constexpr auto operator/=(const _Self& frac) -> _Self
    {
        return *this = *this / frac;
    }

    /*!
     * @brief
     *
     * @param[in] i
     * @return _Self
     */
    constexpr auto operator+=(const Z& i) -> _Self
    {
        return *this = *this + i;
    }

    /*!
     * @brief
     *
     * @param[in] i
     * @return _Self
     */
    constexpr auto operator-=(const Z& i) -> _Self
    {
        return *this = *this - i;
    }

    /*!
     * @brief
     *
     * @param[in] i
     * @return _Self
     */
    constexpr auto operator*=(const Z& i) -> _Self
    {
        return *this = *this * i;
    }

    /*!
     * @brief
     *
     * @param[in] i
     * @return _Self
     */
    constexpr auto operator/=(const Z& i) -> _Self
    {
        return *this = *this / i;
    }

    /*!
     * @brief Three way comparison
     *
     * @param[in] frac
     * @return auto
     */
    template <typename U>
    constexpr auto cmp(const Fraction<U>& frac) const
    {
        // if (_denominator == frac._denominator) {
        //     return _numerator - frac._numerator;
        // }
        return _numerator * frac._denominator - _denominator * frac._numerator;
    }

    /*!
     * @brief
     *
     * @tparam U
     * @param[in] frac
     * @return true
     * @return false
     */
    template <typename U>
    constexpr auto operator==(const Fraction<U>& frac) const -> bool
    {
        if (_denominator == frac._denominator)
        {
            return _numerator == frac._numerator;
        }
        return this->cmp(frac) == 0;
    }

    /*!
     * @brief
     *
     * @tparam U
     * @param[in] frac
     * @return true
     * @return false
     */
    template <typename U>
    constexpr auto operator!=(const Fraction<U>& frac) const -> bool
    {
        return !(*this == frac);
    }

    /*!
     * @brief
     *
     * @tparam U
     * @param[in] frac
     * @return true
     * @return false
     */
    template <typename U>
    constexpr auto operator<(const Fraction<U>& frac) const -> bool
    {
        if (_denominator == frac._denominator)
        {
            return _numerator < frac._numerator;
        }
        return this->cmp(frac) < 0;
    }

    /*!
     * @brief
     *
     * @tparam U
     * @param[in] frac
     * @return true
     * @return false
     */
    template <typename U>
    constexpr auto operator>(const Fraction<U>& frac) const -> bool
    {
        return frac < *this;
    }

    /*!
     * @brief
     *
     * @tparam U
     * @param[in] frac
     * @return true
     * @return false
     */
    template <typename U>
    constexpr auto operator<=(const Fraction<U>& frac) const -> bool
    {
        return !(frac < *this);
    }

    /*!
     * @brief
     *
     * @tparam U
     * @param[in] frac
     * @return true
     * @return false
     */
    template <typename U>
    constexpr auto operator>=(const Fraction<U>& frac) const -> bool
    {
        return !(*this < frac);
    }

    /*!
     * @brief
     *
     * @param[in] c
     * @return auto
     */
    constexpr auto cmp(const Z& c) const
    {
        return _numerator - _denominator * c;
    }

    /*!
     * @brief
     *
     * @param[in] c
     * @return true
     * @return false
     */
    constexpr auto operator==(const Z& c) const -> bool
    {
        return this->cmp(c) == 0;
    }

    /*!
     * @brief
     *
     * @param[in] c
     * @return true
     * @return false
     */
    constexpr auto operator!=(const Z& c) const -> bool
    {
        return this->cmp(c) != 0;
    }

    /*!
     * @brief
     *
     * @param[in] c
     * @return true
     * @return false
     */
    constexpr auto operator<(const Z& c) const -> bool
    {
        return this->cmp(c) < 0;
    }

    /*!
     * @brief
     *
     * @param[in] c
     * @return true
     * @return false
     */
    constexpr auto operator>(const Z& c) const -> bool
    {
        return this->cmp(c) > 0;
    }

    /*!
     * @brief
     *
     * @param[in] c
     * @return true
     * @return false
     */
    constexpr auto operator<=(const Z& c) const -> bool
    {
        return this->cmp(c) <= 0;
    }

    /*!
     * @brief
     *
     * @param[in] c
     * @return true
     * @return false
     */
    constexpr auto operator>=(const Z& c) const -> bool
    {
        return this->cmp(c) >= 0;
    }

    /*!
     * @brief
     *
     * @return double
     */
    constexpr explicit operator double()
    {
        return double(_numerator) / _denominator;
    }

    /*!
     * @brief
     *
     * @param[in] c
     * @param[in] frac
     * @return Fraction<Z>
     */
    friend constexpr auto operator+(const Z& c, const _Self& frac) -> _Self
    {
        return frac + c;
    }

    /*!
     * @brief
     *
     * @param[in] c
     * @param[in] frac
     * @return Fraction<Z>
     */
    friend constexpr auto operator-(const Z& c, const _Self& frac) -> _Self
    {
        return (-frac) + c;
    }

    /*!
     * @brief
     *
     * @param[in] c
     * @param[in] frac
     * @return Fraction<Z>
     */
    friend constexpr auto operator*(const Z& c, const _Self& frac) -> _Self
    {
        return frac * c;
    }

    /*!
     * @brief
     *
     * @param[in] c
     * @param[in] frac
     * @return Fraction<Z>
     */
    friend constexpr auto operator+(int&& c, const _Self& frac) -> _Self
    {
        return frac + c;
    }

    /*!
     * @brief
     *
     * @param[in] c
     * @param[in] frac
     * @return Fraction<Z>
     */
    friend constexpr auto operator-(int&& c, const _Self& frac) -> _Self
    {
        return (-frac) + c;
    }

    /*!
     * @brief
     *
     * @param[in] c
     * @param[in] frac
     * @return Fraction<Z>
     */
    friend constexpr auto operator*(int&& c, const _Self& frac) -> _Self
    {
        return frac * c;
    }

    /*!
     * @brief
     *
     * @tparam _Stream
     * @tparam Z
     * @param[in] os
     * @param[in] frac
     * @return _Stream&
     */
    template <typename Stream>
    friend auto operator<<(Stream& os, const _Self& frac) -> Stream&
    {
        os << frac.numerator() << "/" << frac.denominator();
        return os;
    }
};


// For template deduction
// typename{Z} Fraction(const Z &, const Z &)->Fraction<Z>;

} // namespace fun
