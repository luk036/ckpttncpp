#pragma once

#include <range/v3/view/repeat_n.hpp>
#include <vector>
#include <any>

template <typename Val>
inline auto get_repeat_array(const Val& a, std::ptrdiff_t n)
{
    using repeat_n_return_type = decltype(ranges::views::repeat_n(a, n));

    struct iterable_wrapper : public repeat_n_return_type
    {
      public:
        using value_type [[maybe_unused]] = Val; // luk:
        using key_type [[maybe_unused]] = size_t;   // luk:

        iterable_wrapper(repeat_n_return_type&& base)
            : repeat_n_return_type{std::forward<repeat_n_return_type>(base)}
        {
        }

        [[nodiscard]] auto operator[](const std::any& /* don't care */) const -> const Val&
        {
            return *this->begin();
        }
    };
    
    return iterable_wrapper {ranges::views::repeat_n(a, n)};
}


template <typename C>
class shift_array : public C
{
  using value_type = typename C::value_type;

  private:
    int _start {0};

  public:
    shift_array(C&& base)
        : C{std::forward<C>(base)}
    {
    }

    void set_start(int start)
    {
        this->_start = start;
    }

    [[nodiscard]] auto operator[](size_t index) const -> const value_type&
    {
        return C::operator[](index - this-> start);
    }

    [[nodiscard]] auto operator[](size_t index) -> value_type&
    {
        return C::operator[](index - this->_start);
    }
};
    
