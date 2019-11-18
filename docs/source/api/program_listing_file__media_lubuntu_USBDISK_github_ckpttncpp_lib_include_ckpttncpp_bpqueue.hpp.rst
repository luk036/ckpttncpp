
.. _program_listing_file__media_lubuntu_USBDISK_github_ckpttncpp_lib_include_ckpttncpp_bpqueue.hpp:

Program Listing for File bpqueue.hpp
====================================

|exhale_lsh| :ref:`Return to documentation for file <file__media_lubuntu_USBDISK_github_ckpttncpp_lib_include_ckpttncpp_bpqueue.hpp>` (``/media/lubuntu/USBDISK/github/ckpttncpp/lib/include/ckpttncpp/bpqueue.hpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "dllist.hpp" // import dllink
   #include <cassert>
   #include <type_traits>
   #include <vector>
   
   template <typename T> //
   class bpqueue
   {
       static dllink<T> sentinel; 
     private:
       T max {0};                     
       T offset;                      
       T high;                        
       std::vector<dllink<T>> bucket; 
     public:
       bpqueue(T a, T b)
           : offset {a - 1}
           , high {b - offset}
           , bucket(high + 1)
       {
           assert(a <= b);
           static_assert(std::is_integral_v<T>);
           bucket[0].append(sentinel); // sentinel
       }
   
       auto set_key(dllink<T>& it, T gain) -> void
       {
           it.key = gain - this->offset;
       }
   
       auto get_max() const -> T
       {
           return this->max + this->offset;
       }
   
       auto is_empty() const -> bool
       {
           return this->max == 0;
       }
   
       auto clear() -> void
       {
           while (this->max > 0)
           {
               this->bucket[this->max].clear();
               this->max -= 1;
           }
       }
   
       auto append_direct(dllink<T>& it) -> void
       {
           assert(it.key > this->offset);
           this->append(it, it.key);
       }
   
       auto append(dllink<T>& it, T k) -> void
       {
           assert(k > this->offset);
           it.key = k - this->offset;
           if (this->max < it.key)
           {
               this->max = it.key;
           }
           this->bucket[it.key].append(it);
       }
   
       auto appendfrom(std::vector<dllink<T>>& nodes) -> void
       {
           for (auto& it : nodes)
           {
               it.key -= this->offset;
               assert(it.key > 0);
               this->bucket[it.key].append(it);
           }
           this->max = this->high;
           while (this->bucket[this->max].is_empty())
           {
               this->max -= 1;
           }
       }
   
       auto popleft() -> dllink<T>&
       {
           auto& res = this->bucket[this->max].popleft();
           while (this->bucket[this->max].is_empty())
           {
               this->max -= 1;
           }
           return res;
       }
   
       auto decrease_key(dllink<T>& it, T delta) -> void
       {
           // this->bucket[it.key].detach(it)
           it.detach();
           it.key += delta;
           assert(it.key > 0);
           assert(it.key <= this->high);
           this->bucket[it.key].append(it); // FIFO
           if (this->max < it.key)
           {
               this->max = it.key;
               return;
           }
           while (this->bucket[this->max].is_empty())
           {
               this->max -= 1;
           }
       }
   
       auto increase_key(dllink<T>& it, T delta) -> void
       {
           // this->bucket[it.key].detach(it)
           it.detach();
           it.key += delta;
           assert(it.key > 0);
           assert(it.key <= this->high);
           this->bucket[it.key].appendleft(it); // LIFO
           if (this->max < it.key)
           {
               this->max = it.key;
           }
       }
   
       auto modify_key(dllink<T>& it, T delta) -> void
       {
           if (it.is_locked())
           {
               return;
           }
           if (delta > 0)
           {
               this->increase_key(it, delta);
           }
           else if (delta < 0)
           {
               this->decrease_key(it, delta);
           }
       }
   
       auto detach(dllink<T>& it) -> void
       {
           // this->bucket[it.key].detach(it)
           it.detach();
           while (this->bucket[this->max].is_empty())
           {
               this->max -= 1;
           }
       }
   
       // /*!
       //  * @brief iterator point to begin
       //  *
       //  * @return bpq_iterator
       //  */
       // auto begin() -> bpq_iterator<T>;
   
       // /*!
       //  * @brief iterator point to end
       //  *
       //  * @return bpq_iterator
       //  */
       // auto end() -> bpq_iterator<T>;
   
       using coro_t = boost::coroutines2::coroutine<dllink<T>&>;
       using pull_t = typename coro_t::pull_type;
   
       auto items() -> pull_t
       {
           auto func = [&](typename coro_t::push_type& yield) {
               auto curkey = this->max;
               while (curkey > 0)
               {
                   for (auto& item : this->bucket[curkey].items())
                   {
                       yield(item);
                   }
                   curkey -= 1;
               }
           };
           return pull_t(func);
       }
   
       // auto& items() { return *this; }
       // const auto& items() const { return *this; }
   };
   
   template <typename T>
   inline dllink<T> bpqueue<T>::sentinel {};
   
   // /*!
   //  * @brief Bounded Priority Queue Iterator
   //  *
   //  * Bounded Priority Queue Iterator. Traverse the queue in descending
   //  * order. Detaching queue items may invalidate the iterator because
   //  * the iterator makes a copy of current key.
   //  */
   // template <typename T> class bpq_iterator {
   //   private:
   //     bpqueue<T> &bpq;         /*!< the priority queue */
   //     T curkey;                /*!< the current key value */
   //     dll_iterator<T> curitem; /*!< list iterator pointed to the current item.
   //     */
   
   //     /*!
   //      * @brief get the reference of the current list
   //      *
   //      * @return dllink&
   //      */
   //     auto curlist() -> dllink<T> & { return this->bpq.bucket[this->curkey]; }
   
   //   public:
   //     /*!
   //      * @brief Construct a new bpq iterator object
   //      *
   //      * @param bpq
   //      * @param curkey
   //      */
   //     bpq_iterator(bpqueue<T> &bpq, T curkey)
   //         : bpq{bpq}, curkey{curkey}, curitem{bpq.bucket[curkey].begin()} {}
   
   //     /*!
   //      * @brief move to the next item
   //      *
   //      * @return bpq_iterator&
   //      */
   //     auto operator++() -> bpq_iterator<T> & {
   //         ++this->curitem;
   //         while (this->curitem == this->curlist().end()) {
   //             do {
   //                 this->curkey -= 1;
   //             } while (this->curlist().is_empty());
   //             this->curitem = this->curlist().begin();
   //         }
   //         return *this;
   //     }
   
   //     /*!
   //      * @brief get the reference of the current item
   //      *
   //      * @return bpq_iterator&
   //      */
   //     auto operator*() -> dllink<T> & { return *this->curitem; }
   
   //     /*!
   //      * @brief eq operator
   //      *
   //      * @param rhs
   //      * @return true
   //      * @return false
   //      */
   //     auto operator==(const bpq_iterator &rhs) -> bool {
   //         return this->curitem == rhs.curitem;
   //     }
   
   //     /*!
   //      * @brief neq operator
   //      *
   //      * @param rhs
   //      * @return true
   //      * @return false
   //      */
   //     auto operator!=(const bpq_iterator &rhs) -> bool { return !(*this ==
   //     rhs); }
   // };
   
   // /*!
   //  * @brief
   //  *
   //  * @return bpq_iterator
   //  */
   // template <typename T> inline auto bpqueue<T>::begin() -> bpq_iterator<T> {
   //     return bpq_iterator(*this, this->max);
   // }
   
   // /*!
   //  * @brief
   //  *
   //  * @return bpq_iterator
   //  */
   // template <typename T> inline auto bpqueue<T>::end() -> bpq_iterator<T> {
   //     return bpq_iterator(*this, 0);
   // }
