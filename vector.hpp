#pragma once

#include <cstdio>
#include <cstring>
template<typename T>
class Storage
{
public:
    using pointer = T*;

    explicit Storage(const size_t size_memory=4) noexcept :
                                            size_{size_memory},
                                            used_{size_memory},
                                            data_{static_cast<T*>(::operator new(size_memory * sizeof(T)))}

    {}
    Storage(const size_t size, const T& elem) : size_{size},
                                                used_{size},
                                                data_{static_cast<T*>(::operator new(size * sizeof(T)))}
    {}
    Storage(Storage&& other) noexcept
    {
        std::swap(size_, other.size_);
        std::swap(used_, other.used_);
        std::swap(data_, other.data_);
    }
    Storage& operator=(Storage&& other) noexcept
    {
        if(this != &other)
        {
            std::swap(size_, other.size_);
            std::swap(used_, other.used_);
            std::swap(data_, other.data_);
        }
        return *this;
    }
    Storage(Storage const& other)
    {
        data_ = static_cast<T*>(::operator new(other.size_ * sizeof(T)));
        for(; size_ < other.size_; ++size_)
        {
            new(data_+size_) T{other.data_[size_]};
        }

        size_ = other.size_;
        used_ = other.used_;
    }
    Storage& operator=(Storage const& other)
    {
        if(this != &other){
            Storage s{other};
            std::swap(this, s);
        }
        return *this;
    }
    ~Storage()
    {
        for(int i = 0; i < used_; ++i)
        {
            data_[i].~T();
        }
        ::operator delete(data_);
    }
protected:
    pointer data_{};
    size_t size_{};
    size_t used_{};
};

template<typename T>
class Iterator
{
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type        = typename std::iterator_traits<T*>::value_type;
    using difference_type   = typename std::iterator_traits<T*>::difference_type;
    using pointer           = typename std::iterator_traits<T*>::pointer;
    using reference         = typename std::iterator_traits<T*>::reference;


    explicit Iterator(T* ptr) : ptr_{ptr}
    {}
    Iterator(Iterator const& other) : ptr_{other.ptr_}
    {}
    Iterator& operator=(Iterator const& other)=default;

    Iterator& operator++()   { ++ptr_; return *this;                }
    Iterator& operator--()   { --ptr_; return *this;                }
    Iterator operator++(int) { Iterator i{*this}; ++ptr_; return i; }
    Iterator operator--(int) { Iterator i{*this}; --ptr_; return i; }

    bool equals(Iterator<T> const& rhs)            const { return ptr_ == rhs.ptr_;}
    bool greater(Iterator<T> const& rhs)           const { return ptr_ > rhs;      }
    bool greater_or_equals(Iterator<T> const& rhs) const { return ptr_ >= rhs.ptr_;}

    reference operator*() const { return *ptr_; }
    pointer operator->() const  { return ptr_;  }

    reference operator[](difference_type id) const { return *(ptr_ + id); }

    difference_type operator-(Iterator const& it) const { return ptr_ - it.ptr_; }

    Iterator operator+(difference_type const dif) const { return Iterator{ptr_ + dif}; }
    Iterator operator-(difference_type const dif) const { return Iterator{ptr_ - dif}; }
    Iterator operator=(difference_type const dif) {*ptr_ = dif; return *this;}
private:
    pointer ptr_;
};
template<typename T>
bool operator== (Iterator<T> const& lhs, Iterator<T> const& rhs)  { return lhs.equals(rhs); }

template<typename T>
bool operator!= (Iterator<T> const& lhs, Iterator<T> const& rhs)  { return !(lhs.equals(rhs)); }

template<typename T>
bool operator<  (Iterator<T> const& lhs, Iterator<T> const& rhs)  { return !(lhs.greater_or_equals(rhs)); }

template<typename T>
bool operator>  (Iterator<T> const& lhs, Iterator<T> const& rhs)  { return lhs.greater(rhs);   }

template<typename T>
bool operator>= (Iterator<T> const& lhs, Iterator<T> const& rhs)  { return lhs.greater_or_equals(rhs); }

template<typename T>
bool operator<= (Iterator<T> const& lhs, Iterator<T> const& rhs)  { return !(lhs.greater(rhs)); }


template<typename T>
class vector : public Storage<T>
{
    using pointer           = typename std::iterator_traits<T*>::pointer;
    using const_pointer     = typename std::iterator_traits<const T*>::pointer;
    using reference         = typename std::iterator_traits<T*>::reference;
    using const_reference   = typename std::iterator_traits<const T*>::reference;
    using value_type        = typename std::iterator_traits<T*>::value_type;
    using difference_type   = typename std::iterator_traits<T*>::difference_type;

    using iterator          = Iterator<T>;
    using const_iterator    = const Iterator<T>;

    using Storage<T>::size_;
    using Storage<T>::used_;
    using Storage<T>::data_;
public:
    vector()=default;
    ~vector()=default;

    explicit vector(std::size_t size) : Storage<value_type>{size}
    {}
    vector(const std::size_t size, const value_type& elem) : Storage<value_type>{size, elem}
    {
        std::fill(data_, data_+used_, elem);
    }
    vector(std::initializer_list<T> list) : Storage<value_type>{list.size()}
    {
        int i{0};
        for(auto it = list.begin(); it != list.end(); ++it, ++i)
        {
            new(data_ + i) value_type{*it};
        }
    }
    template<typename It, typename = std::void_t<decltype(*std::declval<It&>()), decltype(++std::declval<It&>())>>
    vector(It first, It last) : Storage<value_type>{static_cast<size_t>(std::distance(first, last))}
    {
        auto it = this->begin();
        while(it != this->end())
        {
            *it = *first;
            ++it; ++first;
        }
    }
    vector(vector const& other) : Storage<value_type>{other.size_}
    {
        for(int i = 0; i < other.size_; ++i)
        {
            new(data_+i) value_type{other.data_[i]};
        }
        used_ = other.used_;
    }
    vector& operator=(vector const& other)
    {
        if(this != &other) {
            vector<value_type> s{other};
            swap(*this, s);
        }
        return *this;
    }
    vector(vector&& other) noexcept : Storage<value_type>{other}
    {}
    vector& operator=(vector&& other) noexcept
    {
        if(this != &other)
            swap(*this, other);
        return *this;
    }

    template<typename It>
    void insert(It pos, T const& elem)
    {
        if(used_ == size_)
            realloc(size_*2);

        std::copy(data_, data_+used_, data_+1);
        *data_ = elem;
    }
    template<typename Arg>
    void push_back(Arg&& elem)
    {
        if(size_ == used_)
            realloc(size_ * 2);

        new(data_ + used_) value_type{std::forward<Arg>(elem)};
        ++used_;
    }
    template<typename ... Args>
    void emplase_back(Args&& ... args)
    {
        if(size_ == used_)
            realloc(size_*2);

        new(data_ + used_) value_type {std::forward<Args>(args)...};
        ++used_;
    }
    void pop()
    {
        if(empty())
            throw std::length_error{"vector is empty"};
        data_[used_-1].~value_type();
        --used_;
    }
    T& top() const { return data_[used_-1]; }
    [[nodiscard]] bool empty() const
    {
        if(size_ == 0)
            return true;
        return false;
    }
    size_t size() { return used_; }
    reference operator[](size_t id) noexcept { return data_[id]; }

    void swap(vector& v1, vector& v2)
    {
        std::swap(v1.size_, v2.size_);
        std::swap(v1.used_, v2.used_);
        std::swap(v1.data_, v2.data_);
    }
    const_pointer data() const  { return data_; }
    pointer data()              { return data_; }

    iterator begin()              { return Iterator{data_};       }
    iterator end()                { return Iterator{data_+used_}; }
    const_iterator begin()  const { return Iterator{data_};       }
    const_iterator end()    const { return Iterator{data_+used_}; }
private:
    void realloc(size_t size)
    {
        vector v(size);
        std::memcpy(static_cast<void*>(v.data()),data_,sizeof(T) * size);
        /*for(int i = 0; i < used_; ++i)
        {
            new(v.data_+i) T{data_[i]};
        }*/
        v.used_ = used_;

        swap(*this, v);
    }
};
