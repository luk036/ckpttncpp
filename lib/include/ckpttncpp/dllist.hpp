#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_DLLIST_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_DLLIST_HPP 1

// forward declare
struct dll_iterator;

/**
 * @brief doubly linked node
 * 
 */
struct dllink
{
    int key;
    dllink *next;
    dllink *prev;

    /**
     * @brief Construct a new dllink object
     * 
     * @param key 
     * @param next 
     * @param prev 
     */
    dllink(int key = 0)
        : key{key},
          next{this},
          prev{this} {}

    /**
     * @brief 
     * 
     */
    void detach()
    {
        auto n = this->next;
        auto p = this->prev;
        p->next = n;
        p->prev = p;
    }

    /**
     * @brief 
     * 
     * @return true 
     * @return false 
     */
    auto is_empty() const noexcept -> bool
    {
        return this->next == this;
    }

    /**
     * @brief 
     * 
     * @return auto 
     */
    auto clear()
    {
        this->next = this->prev = this;
    }

    /**
     * @brief 
     * 
     * @param node 
     */
    auto appendleft(dllink &node) -> void
    {
        node.next = this->next;
        this->next->prev = &node;
        this->next = &node;
        node.prev = this;
    }

    /**
     * @brief 
     * 
     * @param node 
     */
    auto append(dllink &node) -> void
    {
        node.prev = this->prev;
        this->prev->next = &node;
        this->prev = &node;
        node.next = this;
    }

    /**
     * @brief 
     * 
     * @return dllink& 
     */
    auto popleft() -> dllink &
    {
        auto res = this->next;
        this->next = res->next;
        this->next->prev = this;
        return *res;
    }

    /**
     * @brief 
     * 
     * @return dllink& 
     */
    auto pop() -> dllink &
    {
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

struct dll_iterator
{
    dllink *cur;

    /**
     * @brief Construct a new dll iterator object
     * 
     * @param cur 
     */
    explicit dll_iterator(dllink *cur)
        : cur{cur} {}

    // For forward iterator

    /**
     * @brief 
     * 
     * @return dllink& 
     */
    auto operator++() -> dll_iterator &
    {
        this->cur = this->cur->next;
        return *this;
    }

    /**
     * @brief 
     * 
     * @return dllink& 
     */
    auto operator*() -> dllink &
    {
        return *this->cur;
    }

    /**
     * @brief 
     * 
     * @param rhs 
     * @return true 
     * @return false 
     */
    auto operator==(const dll_iterator &rhs) -> bool
    {
        return this->cur == rhs.cur;
    }

    /**
     * @brief 
     * 
     * @param rhs 
     * @return true 
     * @return false 
     */
    auto operator!=(const dll_iterator &rhs) -> bool
    {
        return !(*this == rhs);
    }
};

/**
 * @brief 
 * 
 * @return dll_iterator 
 */
inline auto dllink::begin() -> dll_iterator
{
    return dll_iterator{this->next};
}

/**
 * @brief 
 * 
 * @return dll_iterator
 */
inline auto dllink::end() -> dll_iterator
{
    return dll_iterator{this};
}

#endif