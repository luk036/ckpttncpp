
.. _program_listing_file__media_lubuntu_USBDISK_github_ckpttncpp_lib_include_py2cpp_py2cpp.hpp:

Program Listing for File py2cpp.hpp
===================================

|exhale_lsh| :ref:`Return to documentation for file <file__media_lubuntu_USBDISK_github_ckpttncpp_lib_include_py2cpp_py2cpp.hpp>` (``/media/lubuntu/USBDISK/github/ckpttncpp/lib/include/py2cpp/py2cpp.hpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   // -*- coding: utf-8 -*-
   #pragma once
   
   #include <initializer_list>
   #include <tuple>
   #include <type_traits>
   #include <unordered_map>
   #include <unordered_set>
   #include <utility>
   
   template <typename T>
   using Value_type = typename T::value_type;
   
   namespace py
   {
   
   template <typename T, typename TIter = decltype(std::begin(std::declval<T>())),
       typename = decltype(std::end(std::declval<T>()))>
   constexpr auto enumerate(T&& iterable)
   {
       struct iterator
       {
           size_t i;
           TIter iter;
           bool operator!=(const iterator& other) const
           {
               return iter != other.iter;
           }
           void operator++()
           {
               ++i;
               ++iter;
           }
           auto operator*() const
           {
               return std::tie(i, *iter);
           }
           auto operator*()
           {
               return std::tie(i, *iter);
           }
       };
       struct iterable_wrapper
       {
           T iterable;
           auto begin()
           {
               return iterator {0, std::begin(iterable)};
           }
           auto end()
           {
               return iterator {0, std::end(iterable)};
           }
       };
       return iterable_wrapper {std::forward<T>(iterable)};
   }
   
   // template <typename T>
   // constexpr auto range(T stop) {
   //     struct iterator {
   //         T i;
   //         constexpr bool operator!=(const iterator &other) const { return i !=
   //         other.i; } constexpr bool operator==(const iterator &other) const {
   //         return i == other.i; } constexpr T operator*() const { return i; }
   //         constexpr iterator &operator++() {
   //             ++i;
   //             return *this;
   //         }
   //     };
   
   //     struct iterable_wrapper {
   //         using value_type = T; // luk
   //         T stop;
   //         constexpr auto begin() const { return iterator{0}; }
   //         constexpr auto end() const { return iterator{stop}; }
   //         constexpr auto empty() const -> bool { return stop == 0; }
   //         constexpr auto size() const -> size_t { return stop; }
   //         constexpr auto operator[](size_t n) const -> T { return n; } // no
   //         bounds checking constexpr auto contains(T n) const -> bool { return n
   //         < stop; }
   //     };
   
   //     if (stop < 0) stop = 0;
   //     return iterable_wrapper{stop};
   // }
   
   template <typename T>
   inline constexpr auto range(T start, T stop)
   {
       struct __iterator
       {
           T i;
           constexpr bool operator!=(const __iterator& other) const
           {
               return i != other.i;
           }
           constexpr bool operator==(const __iterator& other) const
           {
               return i == other.i;
           }
           constexpr T operator*() const
           {
               return i;
           }
           constexpr __iterator& operator++()
           {
               ++i;
               return *this;
           }
       };
   
       struct iterable_wrapper
       {
           using value_type = T;        // luk
           using iterator = __iterator; // luk
           T start;
           T stop;
           constexpr auto begin() const
           {
               return iterator {start};
           }
           constexpr auto end() const
           {
               return iterator {stop};
           }
           constexpr auto empty() const -> bool
           {
               return stop == start;
           }
           constexpr auto size() const -> size_t
           {
               return stop - start;
           }
           constexpr auto operator[](size_t n) const -> T
           {
               return start + n;
           } // no bounds checking
           constexpr auto contains(T n) const -> bool
           {
               return !(n < start) && n < stop;
           }
       };
   
       if (stop < start)
           stop = start;
       return iterable_wrapper {start, stop};
   }
   
   template <typename T>
   inline constexpr auto range(T stop)
   {
       return range(T(0), stop);
   }
   
   template <typename Key>
   class set : public std::unordered_set<Key>
   {
       using _Self = set<Key>;
   
     public:
       set()
           : std::unordered_set<Key> {}
       {
       }
   
       template <typename FwdIter>
       set(const FwdIter& start, const FwdIter& stop)
           : std::unordered_set<Key>(start, stop)
       {
       }
   
       explicit set(std::initializer_list<Key> init)
           : std::unordered_set<Key> {init}
       {
       }
   
       bool contains(const Key& key) const
       {
           return this->find(key) != this->end();
       }
   
       _Self copy() const
       {
           return *this;
       }
   
       _Self& operator=(const _Self&) = delete;
   
       _Self& operator=(_Self&&) = default;
   
       set(set<Key>&&) = default;
   
     private:
       set(const set<Key>&) = default;
   };
   
   template <typename Key>
   inline bool operator<(const Key& key, const set<Key>& m)
   {
       return m.contains(key);
   }
   
   template <typename Key>
   inline size_t len(const set<Key>& m)
   {
       return m.size();
   }
   
   template <typename Key>
   set(std::initializer_list<Key>)->set<Key>;
   
   // template <typename Key>
   // set(std::initializer_list<const char*> ) -> set<std::string>;
   
   template <typename Iter>
   struct key_iterator : Iter
   {
       explicit key_iterator(Iter it)
           : Iter(it)
       {
       }
       auto operator*() const
       {
           return Iter::operator*().first;
       }
       key_iterator& operator++()
       {
           Iter::operator++();
           return *this;
       }
   };
   
   template <typename Key, typename T>
   class dict : public std::unordered_map<Key, T>
   {
       using _Self = dict<Key, T>;
       using _Base = std::unordered_map<Key, T>;
   
     public:
       using value_type = std::pair<const Key, T>;
   
       dict()
           : std::unordered_map<Key, T> {}
       {
       }
   
       explicit dict(std::initializer_list<value_type> init)
           : std::unordered_map<Key, T> {init}
       {
       }
   
       // template <class Sequence>
       // explicit dict(const Sequence &S) {
       //     this->reserve(S.size());
       //     for (auto [i_v, v] : py::enumerate(S)) {
       //         (*this)[v] = i_v;
       //     }
       // }
   
       bool contains(const Key& key) const
       {
           return this->find(key) != this->end();
       }
   
       T get(const Key& key, const T& default_value)
       {
           if (!contains(key))
           {
               return default_value;
           }
           return (*this)[key];
       }
   
       auto begin() const
       {
           return key_iterator {std::unordered_map<Key, T>::begin()};
       }
   
       auto end() const
       {
           return key_iterator {std::unordered_map<Key, T>::end()};
       }
   
       std::unordered_map<Key, T>& items()
       {
           return *this;
       }
   
       const std::unordered_map<Key, T>& items() const
       {
           return *this;
       }
   
       _Self copy() const
       {
           return *this;
       }
   
       _Self& operator=(const _Self&) = delete;
   
       _Self& operator=(_Self&&) = default;
   
       dict(dict<Key, T>&&) = default;
   
     private:
       dict(const dict<Key, T>&) = default;
   };
   
   template <typename Key, typename T>
   inline bool operator<(const Key& key, const dict<Key, T>& m)
   {
       return m.contains(key);
   }
   
   template <typename Key, typename T>
   inline size_t len(const dict<Key, T>& m)
   {
       return m.size();
   }
   
   template <typename Key, typename T>
   dict(std::initializer_list<std::pair<const Key, T>>)->dict<Key, T>;
   
   template <class Sequence>
   dict(const Sequence& S)
       ->dict<std::remove_cv_t<decltype(*std::begin(S))>, size_t>;
   
   } // namespace py
