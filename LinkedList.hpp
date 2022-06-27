#pragma once

#include <iterator>

namespace list_utils {
template<typename T>
class Iterator {
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename std::iterator_traits<T *>::value_type;
    using difference_type = typename std::iterator_traits<T *>::difference_type;
    using pointer = typename std::iterator_traits<T *>::pointer;
    using reference = typename std::iterator_traits<T *>::reference;


    explicit Iterator(T *ptr) : ptr_{ptr} {}

    Iterator(Iterator const &other) : ptr_{other.ptr_} {}

    Iterator &operator=(Iterator const &other) = default;

    Iterator &operator++() {
        ++ptr_;
        return *this;
    }

    Iterator &operator--() {
        --ptr_;
        return *this;
    }

    Iterator operator++(int) {
        Iterator i{*this};
        ++ptr_;
        return i;
    }

    Iterator operator--(int) {
        Iterator i{*this};
        --ptr_;
        return i;
    }

    bool equals(Iterator<T> const &rhs) const { return ptr_ == rhs.ptr_; }

    bool greater(Iterator<T> const &rhs) const { return ptr_ > rhs; }

    bool greater_or_equals(Iterator<T> const &rhs) const { return ptr_ >= rhs.ptr_; }

    reference operator*() const { return *ptr_; }

    pointer operator->() const { return ptr_; }

    reference operator[](difference_type id) const { return *(ptr_ + id); }

private:
    pointer ptr_;
};

template<typename T>
struct Node {
    T value{};
    Node *prev{};
    Node *next{};
};
}

template<typename T>
class linked_list
{
public:
    linked_list() = default;
    linked_list(const linked_list& other)
    {

    }

    linked_list(linked_list&& other)
    {

    }

    linked_list& operator=(const linked_list& other)
    {

    }

    linked_list& operator=(linked_list&& other)
    {

    }
    void pushBack(const T& value);
    void pushFront(const T& value);
    void popBack();
    void popFront();

    void insert(const list_utils::Iterator<T>& it, const T& value)
    {
        list_utils::Node<T> * past_current{it};
        list_utils::Node<T> * future_current{new list_utils::Node<T>};
        list_utils::Node<T>* prev{std::prev(it)};
        prev->next = future_current;
        future_current->prev = prev;
        past_current->prev = future_current;
        future_current->next = past_current;
        future_current->value = value;
    }

    void erase(const list_utils::Iterator<T>& it, const T& value)
    {
        list_utils::Node<T> * pr{std::prev(it)};
        list_utils::Node<T> * nx{std::next(it)};
        pr->next = nx;
        nx->prev = pr;
        delete it;
    }

    T& back() const { return (std::prev(end))->value; }
    T& front() const { return begin->value; }
    [[nodiscard]] bool isEmpty() const { return size == 0; }

    ~linked_list()
    {
        if(isEmpty())
            return;
        while(begin != end)
            begin = begin->next; delete begin->prev;

    }
private:
    list_utils::Node<T>* begin{};
    list_utils::Node<T>* end{};
    std::size_t size{};
};