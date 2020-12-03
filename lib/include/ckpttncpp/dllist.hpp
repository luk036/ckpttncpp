#pragma once

// #include <boost/coroutine2/all.hpp>
#include <cassert>
#include <utility> // import std::move()
#include <cstdint>

// Forward declaration for begin() end()
template <typename T, typename Int>
struct dll_iterator;

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
#pragma pack(push, 1)
template <typename T, typename Int = int16_t>
class dllink
{
    friend dll_iterator<T, Int>;

  private:
    dllink* next {this}; /*!< pointer to the next node */
    dllink* prev {this}; /*!< pointer to the previous node */

  public:
    T data{};  /*!< data */
    Int key{}; /*!< key */

    /*!
     * @brief Construct a new dllink object
     *
     * @param[in] key the key
     */
    explicit dllink(T data, Int key = Int(0))
        : data {std::move(data)}
        , key {std::move(key)}
    {
        static_assert(sizeof(dllink) <= 24, "keep this class small");
    }

    /*!
     * @brief Copy construct a new dllink object (deleted intentionally)
     *
     */
    dllink() = default;
    dllink(const dllink&) = delete;                    // don't copy
    auto operator=(const dllink&) -> dllink& = delete; // don't assign
    dllink(dllink&&) noexcept = default;
    auto operator=(dllink&&) noexcept -> dllink& = default; // don't assign
    ~dllink() = default;

    /*!
     * @brief detach from a list
     *
     */
    auto detach() -> void
    {
        assert(!this->is_locked());
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
    [[nodiscard]] auto is_locked() const -> bool
    {
        return this->next == nullptr;
    }

    /*!
     * @brief whether the list is empty
     *
     * @return true
     * @return false
     */
    [[nodiscard]] auto is_empty() const -> bool
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
     * @param[in,out] node
     */
    auto appendleft(dllink& node) -> void
    {
        node.next = this->next;
        this->next->prev = &node;
        this->next = &node;
        node.prev = this;
    }

    /*!
     * @brief append the node to the back
     *
     * @param[in,out] node
     */
    auto append(dllink& node) -> void
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
    auto popleft() -> dllink&
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
    auto pop() -> dllink&
    {
        auto res = this->prev;
        this->prev = res->prev;
        this->prev->next = this;
        return *res;
    }

    // For iterator

    /*!
     * @brief
     *
     * @return dll_iterator
     */
    auto begin() -> dll_iterator<T, Int>;

    /*!
     * @brief
     *
     * @return dll_iterator
     */
    auto end() -> dll_iterator<T, Int>;

    auto items() -> dllink&
    {
        return *this;
    }

    auto items() const -> const dllink&
    {
        return *this;
    }

    // using coro_t = boost::coroutines2::coroutine<dllink&>;
    // using pull_t = typename coro_t::pull_type;

    // /**
    //  * @brief item generator
    //  *
    //  * @return pull_t
    //  */
    // auto items() -> pull_t
    // {
    //     auto func = [&](typename coro_t::push_type& yield) {
    //         auto cur = this->next;
    //         while (cur != this)
    //         {
    //             yield(*cur);
    //             cur = cur->next;
    //         }
    //     };
    //     return pull_t(func);
    // }
};
#pragma pack(pop)

/*!
 * @brief list iterator
 *
 * List iterator. Traverse the list from the first item. Usually it is
 * safe to attach/detach list items during the iterator is active.
 */
template <typename T, typename Int = int16_t>
struct dll_iterator
{
    dllink<T, Int>* cur; /*!< pointer to the current item */

    /*!
     * @brief Construct a new dll iterator object
     *
     * @param[in] cur
     */
    explicit dll_iterator(dllink<T, Int>* cur)
        : cur {cur}
    {
    }

    /*!
     * @brief move to the next item
     *
     * @return dllink&
     */
    auto operator++() -> dll_iterator&
    {
        this->cur = this->cur->next;
        return *this;
    }

    /*!
     * @brief get the reference of the current item
     *
     * @return dllink&
     */
    auto operator*() -> dllink<T, Int>&
    {
        return *this->cur;
    }

    /*!
     * @brief eq operator
     *
     * @param[in] rhs
     * @return true
     * @return false
     */
    friend auto operator==(
        const dll_iterator& lhs, const dll_iterator& rhs) -> bool
    {
        return lhs.cur == rhs.cur;
    }

    /*!
     * @brief neq operator
     *
     * @param[in] rhs
     * @return true
     * @return false
     */
    friend auto operator!=(
        const dll_iterator& lhs, const dll_iterator& rhs) -> bool
    {
        return !(lhs == rhs);
    }
};

/*!
 * @brief begin
 *
 * @return dll_iterator
 */
template <typename T, typename Int>
inline auto dllink<T, Int>::begin() -> dll_iterator<T, Int>
{
    return dll_iterator<T, Int> {this->next};
}

/*!
 * @brief end
 *
 * @return dll_iterator
 */
template <typename T, typename Int>
inline auto dllink<T, Int>::end() -> dll_iterator<T, Int>
{
    return dll_iterator<T, Int> {this};
}
