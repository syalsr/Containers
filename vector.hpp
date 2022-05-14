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
    using value_type        = T;
    using difference_type   = ptrdiff_t;
    using pointer           = T*;
    using reference         = T&;


    explicit Iterator(T* ptr) : ptr_{ptr}
    {}
    Iterator(Iterator const& other) : ptr_{other.ptr_}
    {}
    Iterator& operator=(Iterator const& other)=default;

    Iterator& operator++()   { ++ptr_; return *this;                }
    Iterator& operator--()   { --ptr_; return *this;                }
    Iterator operator++(int) { Iterator i{*this}; ++ptr_; return i; }
    Iterator operator--(int) { Iterator i{*this}; --ptr_; return i; }

    bool operator== (Iterator const& iter) const { return ptr_ == iter.ptr_; }
    bool operator!= (Iterator const& iter) const { return ptr_ != iter.ptr_; }
    bool operator<  (Iterator const& it)   const { return ptr_ < it.ptr_;    }
    bool operator>  (Iterator const& it)   const { return ptr_ > it.ptr_;    }
    bool operator>= (Iterator const& it)   const { return !(ptr_ < it.ptr_); }
    bool operator<= (Iterator const& it)   const { return !(ptr_ > it.ptr_); }

    reference operator*() const { return *ptr_; }
    pointer operator->() const  { return ptr_;  }

    reference operator[](difference_type id) const { return *(ptr_ + id); }

    difference_type operator-(Iterator const& it) const { return ptr_ - it.ptr_; }

    Iterator operator+(difference_type const dif) const { return Iterator{ptr_ + dif}; }
    Iterator operator-(difference_type const dif) const { return Iterator{ptr_ - dif}; }
private:
    pointer ptr_;
};

template<typename T>
class vector : public Storage<T>
{
    using pointer           = T*;
    using const_pointer     = const T*;
    using reference         = T&;
    using const_reference   = T const&;
    using value_type        = T;
    using difference_type   = ptrdiff_t;
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
    template<typename It>
    void insert(It pos, T const& elem)
    {
        if(used_ == size_)
            realloc(size_*2);

        std::copy(data_, data_+used_, data_+1);
        *data_ = elem;
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
        swap(*this, other);
    }

    template<typename Arg>
    void push_back(Arg&& elem)
    {
        if(size_ == used_)
            realloc(size_ * 2);

        new(data_ + used_) value_type{std::forward<Arg>(elem)};
        ++used_;
    }
    void pop()
    {
        if(empty())
            throw std::length_error{"vector is empty"};
        data_[size_-1].~value_type();
        --used_;
    }
    [[nodiscard]] bool empty() const
    {
        if(size_ == 0)
            return true;
        return false;
    }
    size_t size()
    {
        return used_;
    }
    reference operator[](size_t id) noexcept
    {
        return data_[id];
    }
    void swap(vector& v1, vector& v2)
    {
        std::swap(v1.size_, v2.size_);
        std::swap(v1.used_, v2.used_);
        std::swap(v1.data_, v2.data_);
    }
    const_pointer data() const
    {
        return data_;
    }
    pointer data()
    {
        return data_;
    }
    template<typename ... Args>
    void emplase_back(Args&& ... args)
    {
        if(size_ == used_)
            realloc(size_*2);

        new(data_ + used_) value_type {std::forward<Args>(args)...};
        ++used_;
    }
    iterator begin()
    {
        return Iterator{data_};
    }
    iterator end()
    {
        return Iterator{data_+used_};
    }
    const_iterator begin() const
    {
        return Iterator{data_};
    }
    const_iterator end() const
    {
        return Iterator{data_+used_};
    }
private:
    void realloc(size_t size)
    {
        vector v(size);
        memcpy(v.data(),data_,sizeof(T)*size);
        /*for(int i = 0; i < used_; ++i)
        {
            new(v.data_+i) T{data_[i]};
        }*/
        v.used_ = used_;

        swap(*this, v);
    }
};
