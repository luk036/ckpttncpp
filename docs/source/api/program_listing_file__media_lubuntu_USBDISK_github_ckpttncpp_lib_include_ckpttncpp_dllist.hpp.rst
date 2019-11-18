
.. _program_listing_file__media_lubuntu_USBDISK_github_ckpttncpp_lib_include_ckpttncpp_dllist.hpp:

Program Listing for File dllist.hpp
===================================

|exhale_lsh| :ref:`Return to documentation for file <file__media_lubuntu_USBDISK_github_ckpttncpp_lib_include_ckpttncpp_dllist.hpp>` (``/media/lubuntu/USBDISK/github/ckpttncpp/lib/include/ckpttncpp/dllist.hpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include <boost/coroutine2/all.hpp>
   #include <cassert>
   
   // forward declare
   // template <typename T> struct dll_iterator;
   
   template <typename T>
   struct dllink
   {
       dllink<T>* next {this}; 
       dllink<T>* prev {this}; 
       T key;                  
       explicit dllink(T key = T(0))
           : key {key}
       {
           static_assert(sizeof(dllink<T>) <= 24);
       }
   
       dllink(dllink<T>&) = delete;
   
       auto detach() -> void
       {
           assert(!this->is_locked());
           auto n = this->next;
           auto p = this->prev;
           p->next = n;
           n->prev = p;
       }
   
       auto lock() -> void
       {
           this->next = nullptr;
       }
   
       auto is_locked() const -> bool
       {
           return this->next == nullptr;
       }
   
       auto is_empty() const -> bool
       {
           return this->next == this;
       }
   
       auto clear() -> void
       {
           this->next = this->prev = this;
       }
   
       auto appendleft(dllink<T>& node) -> void
       {
           node.next = this->next;
           this->next->prev = &node;
           this->next = &node;
           node.prev = this;
       }
   
       auto append(dllink<T>& node) -> void
       {
           node.prev = this->prev;
           this->prev->next = &node;
           this->prev = &node;
           node.next = this;
       }
   
       auto popleft() -> dllink<T>&
       {
           auto res = this->next;
           this->next = res->next;
           this->next->prev = this;
           return *res;
       }
   
       auto pop() -> dllink<T>&
       {
           auto res = this->prev;
           this->prev = res->prev;
           this->prev->next = this;
           return *res;
       }
   
       // For iterator
   
       // /*!
       //  * @brief
       //  *
       //  * @return dll_iterator
       //  */
       // auto begin() -> dll_iterator<T>;
   
       // /*!
       //  * @brief
       //  *
       //  * @return dll_iterator
       //  */
       // auto end() -> dll_iterator<T>;
   
       using coro_t = boost::coroutines2::coroutine<dllink<T>&>;
       using pull_t = typename coro_t::pull_type;
   
       auto items() -> pull_t
       {
           auto func = [&](typename coro_t::push_type& yield) {
               auto cur = this->next;
               while (cur != this)
               {
                   yield(*cur);
                   cur = cur->next;
               }
           };
           return pull_t(func);
       }
   
       // auto& items() { return *this; }
       // const auto& items() const { return *this; }
   };
   
   // /*!
   //  * @brief list iterator
   //  *
   //  * List iterator. Traverse the list from the first item. Usually it is
   //  * safe to attach/detach list items during the iterator is active.
   //  */
   // template <typename T> struct dll_iterator {
   //     dllink<T> *cur; /*!< pointer to the current item */
   
   //     /*!
   //      * @brief Construct a new dll iterator object
   //      *
   //      * @param cur
   //      */
   //     explicit dll_iterator(dllink<T> *cur) : cur{cur} {}
   
   //     /*!
   //      * @brief move to the next item
   //      *
   //      * @return dllink&
   //      */
   //     auto operator++() -> dll_iterator<T> & {
   //         this->cur = this->cur->next;
   //         return *this;
   //     }
   
   //     /*!
   //      * @brief get the reference of the current item
   //      *
   //      * @return dllink&
   //      */
   //     auto operator*() -> dllink<T> & { return *this->cur; }
   
   //     /*!
   //      * @brief eq operator
   //      *
   //      * @param rhs
   //      * @return true
   //      * @return false
   //      */
   //     auto operator==(const dll_iterator<T> &rhs) -> bool {
   //         return this->cur == rhs.cur;
   //     }
   
   //     /*!
   //      * @brief neq operator
   //      *
   //      * @param rhs
   //      * @return true
   //      * @return false
   //      */
   //     auto operator!=(const dll_iterator<T> &rhs) -> bool {
   //         return !(*this == rhs);
   //     }
   // };
   
   // /*!
   //  * @brief begin
   //  *
   //  * @return dll_iterator
   //  */
   // template <typename T> inline auto dllink<T>::begin() -> dll_iterator<T> {
   //     return dll_iterator{this->next};
   // }
   
   // /*!
   //  * @brief end
   //  *
   //  * @return dll_iterator
   //  */
   // template <typename T> inline auto dllink<T>::end() -> dll_iterator<T> {
   //     return dll_iterator{this};
   // }
