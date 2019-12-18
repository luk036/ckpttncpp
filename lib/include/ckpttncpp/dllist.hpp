#pragma once

#include <boost/coroutine2/all.hpp>
#include <cassert>

// forward declare
// template <typename T> struct dll_iterator;

/*!
 * @brief doubly linked node (that may also be a "head" a list)
 *
 * A Doubly-linked List class. This class simply contains a link of
 * node's. By adding a "head" node (sentinel), deleting a node is
 * extremely fast (see "Introduction to Algorithm"). This class does
 * not keep the length information as it is not necessary for the FM
 * algorithm. This saves memory and run-time to update the length
 * information. Note that this class does not own the list node. They
 * are supplied by the caller in order to better reuse the nodes.
 */
template <typename T>
struct dllink
{
    dllink<T>* next {this}; /*!< pointer to the next node */
    dllink<T>* prev {this}; /*!< pointer to the previous node */
    T key;                  /*!< key/data */

    /*!
     * @brief Construct a new dllink object
     *
     * @param key the key
     */
    explicit dllink(T key = T(0))
        : key {key}
    {
        static_assert(sizeof(dllink<T>) <= 24);
    }

    /*!
     * @brief Copy construct a new dllink object (deleted intentionally)
     *
     */
    dllink(const dllink<T>& ) = delete;
    dllink& operator=(const dllink<T>& ) = delete;

    /*!
     * @brief detach from a list
     *
     */
    auto detach() -> void
    {
        assert(not this->is_locked());
        const auto n = this->next;
        const auto p = this->prev;
        p->next = n;
        n->prev = p;
    }

    /*!
     * @brief lock the node (and don't append it to any list)
     *
     */
    auto lock() -> void
    {
        this->next = nullptr;
    }

    /*!
     * @brief whether the node is locked
     *
     * @return true
     * @return false
     */
    auto is_locked() const -> bool
    {
        return this->next == nullptr;
    }

    /*!
     * @brief whether the list is empty
     *
     * @return true
     * @return false
     */
    auto is_empty() const -> bool
    {
        return this->next == this;
    }

    /*!
     * @brief reset the list
     *
     */
    auto clear() -> void
    {
        this->next = this->prev = this;
    }

    /*!
     * @brief append the node to the front
     *
     * @param node
     */
    auto appendleft(dllink<T>& node) -> void
    {
        node.next = this->next;
        this->next->prev = &node;
        this->next = &node;
        node.prev = this;
    }

    /*!
     * @brief append the node to the back
     *
     * @param node
     */
    auto append(dllink<T>& node) -> void
    {
        node.prev = this->prev;
        this->prev->next = &node;
        this->prev = &node;
        node.next = this;
    }

    /*!
     * @brief pop a node from the front
     *
     * @return dllink&
     *
     * Precondition: list is not empty
     */
    auto popleft() -> dllink<T>&
    {
        auto res = this->next;
        this->next = res->next;
        this->next->prev = this;
        return *res;
    }

    /*!
     * @brief pop a node from the back
     *
     * @return dllink&
     *
     * Precondition: list is not empty
     */
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

    /**
     * @brief item generator
     *
     * @return pull_t
     */
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
