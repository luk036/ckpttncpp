#ifndef CKPTTNCPP_DLLIST_HPP
#define CKPTTNCPP_DLLIST_HPP 1

#include <cassert>

// forward declare
struct dll_iterator;

/**
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
struct dllink {
    int key = 0;
    dllink *next;  /**< pointer to the next node */
    dllink *prev;  /**< pointer to the previous node */

    /**
     * @brief Construct a new dllink object
     *
     * @param key the key
     */
    dllink() : next{this}, prev{this} {}

    /**
     * @brief Construct a new dllink object (deleted intentionally)
     *
     */
    dllink(dllink &) = delete;

    /**
     * @brief detach from a list
     *
     */
    auto detach() -> void {
        assert(!this->is_locked());
        auto n = this->next;
        auto p = this->prev;
        p->next = n;
        n->prev = p;
    }

    /**
     * @brief lock the node (and don't append it to any list)
     * 
     */
    auto lock() -> void { this->next = nullptr; }

    /**
     * @brief whether the node is locked
     * 
     * @return true 
     * @return false 
     */
    auto is_locked() const noexcept -> bool { return this->next == nullptr; }

    /**
     * @brief whether the list is empty
     *
     * @return true
     * @return false
     */
    [[nodiscard]] auto is_empty() const noexcept -> bool { return this->next == this; }

    /**
     * @brief reset the list
     *
     */
    auto clear() -> void { this->next = this->prev = this; }

    /**
     * @brief append the node to the front
     *
     * @param node
     */
    auto appendleft(dllink &node) -> void {
        node.next = this->next;
        this->next->prev = &node;
        this->next = &node;
        node.prev = this;
    }

    /**
     * @brief append the node to the back
     *
     * @param node
     */
    auto append(dllink &node) -> void {
        node.prev = this->prev;
        this->prev->next = &node;
        this->prev = &node;
        node.next = this;
    }

    /**
     * @brief pop a node from the front
     *
     * @return dllink&
     *
     * Precondition: list is not empty
     */
    auto popleft() -> dllink & {
        auto res = this->next;
        this->next = res->next;
        this->next->prev = this;
        return *res;
    }

    /**
     * @brief pop a node from the back
     *
     * @return dllink&
     *
     * Precondition: list is not empty
     */
    auto pop() -> dllink & {
        auto res = this->prev;
        this->prev = res->prev;
        this->prev->next = this;
        return *res;
    }

    // For iterator

    /**
     * @brief
     *
     * @return dll_iterator
     */
    auto begin() -> dll_iterator;

    /**
     * @brief
     *
     * @return dll_iterator
     */
    auto end() -> dll_iterator;
};

/**
 * @brief list iterator
 *
 * List Cursor. Traverse the list from the first item. Usually it is
 * safe to attach/detach list items during the iterator is active.
 */
struct dll_iterator {
    dllink *cur; /**< pointer to the current item */

    /**
     * @brief Construct a new dll iterator object
     *
     * @param cur
     */
    explicit dll_iterator(dllink *cur) : cur{cur} {}

    /**
     * @brief move to the next item
     *
     * @return dllink&
     */
    auto operator++() -> dll_iterator & {
        this->cur = this->cur->next;
        return *this;
    }

    /**
     * @brief get the reference of the current item
     *
     * @return dllink&
     */
    auto operator*() -> dllink & { return *this->cur; }

    /**
     * @brief eq operator
     *
     * @param rhs
     * @return true
     * @return false
     */
    auto operator==(const dll_iterator &rhs) -> bool {
        return this->cur == rhs.cur;
    }

    /**
     * @brief neq operator
     *
     * @param rhs
     * @return true
     * @return false
     */
    auto operator!=(const dll_iterator &rhs) -> bool { return !(*this == rhs); }
};

/**
 * @brief
 *
 * @return dll_iterator
 */
inline auto dllink::begin() -> dll_iterator { return dll_iterator{this->next}; }

/**
 * @brief
 *
 * @return dll_iterator
 */
inline auto dllink::end() -> dll_iterator { return dll_iterator{this}; }

#endif