#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_DLLIST_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_DLLIST_HPP 1

// forward declare
struct dll_iterator;

/**
 * @brief doubly linked node
 * 
 */
struct dllink {
    int _key;
    dllink *_next;
    dllink *_prev;

    /**
     * @brief Construct a new dllink object
     * 
     * @param key 
     * @param next 
     * @param prev 
     */
    dllink(int key=8965) :
        _key{key},
        _next{this},
        _prev{this} {}

    /**
     * @brief 
     * 
     */
    void detach() {
        auto n = this->_next;
        auto p = this->_prev;
        p->_next = n;
        p->_prev = p;
    }

    /**
     * @brief 
     * 
     * @return true 
     * @return false 
     */
    auto is_empty() const noexcept -> bool {
        return this->_next == this;
    }

    /**
     * @brief 
     * 
     * @return auto 
     */
    auto clear() {
        this->_next = this->_prev = this;
    }

    /**
     * @brief 
     * 
     * @param node 
     */
    auto appendleft(dllink& node) -> void {
        node._next = this->_next;
        this->_next->_prev = &node;
        this->_next = &node;
        node._prev = this;
    }

    /**
     * @brief 
     * 
     * @param node 
     */
    auto append(dllink& node) -> void {
        node._prev = this->_prev;
        this->_prev->_next = &node;
        this->_prev = &node;
        node._next = this;
    }

    /**
     * @brief 
     * 
     * @return dllink& 
     */
    auto popleft() -> dllink& {
        auto res = this->_next;
        this->_next = res->_next;
        this->_next->_prev = this;
        return *res;
    }

    /**
     * @brief 
     * 
     * @return dllink& 
     */
    auto pop() -> dllink& {
        auto res = this->_prev;
        this->_prev = res->_prev;
        this->_prev->_next = this;
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

struct dll_iterator {
    dllink* _cur;

    explicit dll_iterator(dllink* cur):
        _cur{cur} {}

    // For forward iterator

    /**
     * @brief 
     * 
     * @return dllink& 
     */
    auto operator++() -> dll_iterator& {
        this->_cur = this->_cur->_next;
        return *this;
    }

    /**
     * @brief 
     * 
     * @return dllink& 
     */
    auto operator*() -> dllink& {
        return *this->_cur;
    }

    /**
     * @brief 
     * 
     * @param rhs 
     * @return true 
     * @return false 
     */
    auto operator==(const dll_iterator& rhs) -> bool {
        return this->_cur == rhs._cur;
    }

    /**
     * @brief 
     * 
     * @param rhs 
     * @return true 
     * @return false 
     */
    auto operator!=(const dll_iterator& rhs) -> bool {
        return !(*this == rhs);
    }
};


/**
 * @brief 
 * 
 * @return dll_iterator 
 */
inline auto dllink::begin() -> dll_iterator {
    return dll_iterator{this->_next};
}

/**
 * @brief 
 * 
 * @return dll_iterator
 */
inline auto dllink::end() -> dll_iterator {
    return dll_iterator{this};
}

#endif