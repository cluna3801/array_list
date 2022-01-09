#ifndef ARRAY_LIST_HPP
#define ARRAY_LIST_HPP

#include <algorithm>
#include <memory>
#include <iostream>
#include <type_traits>
// TODO: add in thread safe versions of functions
// #include <mutex>


template<typename _Ty, typename allocator_type=std::allocator<_Ty> >
class array_list
{

    typedef size_t size_type;
    typedef _Ty value_type;
    typedef _Ty* iterator;
    typedef iterator reverse_iterator;
    typedef const _Ty* const_iterator;
    typedef const_iterator const_reverse_iterator;
    typedef std::ptrdiff_t difference_type;
    typedef _Ty& reference;
    typedef const _Ty& const_reference;

    static constexpr size_type _MAX_SIZE = 9223372036854775807;

    _Ty* m_data;
    size_type m_capacity;
    size_type m_size;
    allocator_type m_alloc;


    // linear algebra based fields
    struct sum_func
    {
        _Ty result;

        sum_func() = delete;
        sum_func(const _Ty& _init) : result(_init) {}

        void operator()(_Ty value) { result += value; }
    };

    struct Shape
    {
        size_t n;
        size_t m;

        Shape() = delete;
        Shape(size_t) = delete;
        Shape(size_t _n, size_t _m) : n(_n), m(_m) {}

        bool operator==(const Shape& _shape)
        {
            return (n == _shape.n) && (m == _shape.m);
        }

        bool operator!=(const Shape& _shape)
        {
            return !(*this == _shape);
        }

        bool valid_mult(const Shape& _shape)
        {
            return (n == _shape.m);
        }
    };
    
public:
    // figure out why this doesn't work
    array_list()
        : m_data(nullptr), m_capacity(0), m_size(0), m_alloc( std::allocator<_Ty>() ) { }
    
    explicit array_list(const allocator_type& alloc)
        : m_data(nullptr), m_capacity(0), m_size(0), m_alloc(alloc) {}
    
    explicit array_list(const size_t& n, const allocator_type& alloc=std::allocator<_Ty>())
        : m_data(nullptr), m_capacity(n), m_size(0), m_alloc(alloc)
        {
            m_data = m_alloc.allocate(n);
        }
    
    array_list(_Ty* _src, const size_t& _n, const size_t& _capacity, const allocator_type& _alloc=std::allocator<_Ty>()) 
        : m_data(new _Ty[_capacity]), m_capacity(_capacity), m_size(_n), m_alloc(_alloc)
        {
            std::copy(&_src[0], &_src[0] + _n, &m_data[0]);
        }
    
    array_list(const size_type& n, const value_type& _value, const allocator_type& alloc=std::allocator<value_type>() )
        : m_data( nullptr ), m_capacity(n), m_size(0), m_alloc(alloc)
        {
            m_data = m_alloc.allocate(n);
            std::fill(begin(), end(), _value);
        }
    array_list(std::initializer_list<_Ty> lst, const allocator_type& alloc=std::allocator<_Ty>() )
         : m_data( new _Ty[lst.size() * 2] ), m_capacity( lst.size() * 2), m_size(lst.size() )
         {
             std::move( lst.begin(), lst.end(), this->begin() );
         }

    explicit array_list(array_list<_Ty, allocator_type>&& llist)
        : m_data( llist.m_data ), m_capacity( llist.m_capacity ), m_size( llist.m_size ), m_alloc( llist.m_alloc )
        {
            llist.m_data = nullptr;
            m_capacity = 0;
            m_size = 0;
        }
    explicit array_list(const array_list<_Ty, allocator_type>& _src) 
        : m_data(new _Ty[_src.capacity()]), m_capacity(_src.capacity()), m_size(_src.size()), m_alloc(_src.get_allocator())
        {
            std::copy(_src.begin(), _src.end(), &m_data[0]);
        }
    
    ~array_list()
    {
        m_alloc.deallocate(m_data, m_capacity);
        m_data = nullptr;
        m_capacity = 0;
        m_size = 0;
    }

    constexpr array_list<_Ty, allocator_type>& operator=(const array_list<_Ty, allocator_type>& _src)
    {
        if(this == &_src)
            return *this;
        m_alloc.deallocate(m_data, m_capacity);
        m_size = _src.size();
        m_capacity = _src.capacity();
        m_data = m_alloc.allocate(m_capacity);
        std::move( _src.begin(), _src.end(), this->begin() );
        return *this;
    }
    constexpr array_list<_Ty, allocator_type>& operator=(array_list<_Ty, allocator_type>&& _src)
    {
        if(this == &_src)
            return *this;
        m_alloc.deallocate(m_data, m_capacity);
        m_size = _src.size();
        m_capacity = _src.capacity();
        m_data = _src.m_data;
        _src.m_data = nullptr;
        return *this;
    }


    // iterator stuff
    constexpr allocator_type get_allocator() const noexcept { return m_alloc; }

    constexpr iterator begin() const { return &m_data[0]; }
    constexpr const_iterator cbegin() const { return &m_data[0]; }

    constexpr iterator end() const { return &m_data[m_size]; }
    constexpr const_iterator cend() const { return &m_data[m_size]; }

    constexpr reverse_iterator rbegin() const { return &m_data[m_size - 1]; }
    constexpr reverse_iterator rend() const { return &m_data[-1]; }
    constexpr const_reverse_iterator crbegin() const { return &m_data[m_size - 1]; }
    constexpr const_reverse_iterator crend() const { return &m_data[-1]; }

    // accessors and container information
    constexpr _Ty& at(const size_type& _idx)
    {
        if(_idx < m_size)
            return m_data[_idx];
        throw std::invalid_argument("Out of bounds index");
    }

    constexpr _Ty at(const size_type& _idx) const
    {
        if(_idx < m_size)
            return m_data[_idx];
        throw std::invalid_argument("Out of bounds index");
    }

    constexpr _Ty& operator[](const size_type& _idx) { return at(_idx); }
    constexpr _Ty operator[](const size_type& _idx) const { return at(_idx); }

    constexpr _Ty& front() { return at(0); }
    constexpr _Ty front() const { return at(0); }

    constexpr _Ty& back() { return at(m_size - 1); }
    constexpr _Ty back() const { return at(m_size - 1); }

    constexpr void print(std::ostream& os=std::cout, const char& end='\n') { os << *this << end; }

    constexpr bool empty() const noexcept { return m_size == 0; }
    constexpr bool empty() noexcept { return m_size == 0; }

    constexpr _Ty* data() noexcept { return m_data != nullptr ? &m_data[0] : nullptr; }
    constexpr const _Ty* data() const noexcept { return m_data != nullptr ? &m_data[0] : nullptr; }

    constexpr size_type size() const { return m_size; }
    constexpr size_type size() { return m_size; }

    constexpr size_type max_size() const noexcept { return _MAX_SIZE; }

    constexpr size_type capacity() const noexcept { return m_capacity; }

    constexpr bool operator==(const array_list<_Ty, allocator_type>& rhs) {
        
        // self comparison
        if(this == &rhs) {
            return true;
        }

        if(m_size != rhs.size()) {
            return false;
        }

        auto lIter = begin();
        auto rIter = rhs.begin();
        for(; lIter != end() && rIter != rhs.end(); lIter++, rIter++) {
            if( !(*lIter == *rIter) ) {
                return false;
            }            
        }

        return true;
    }

    // idk what this is but I'll add it later
    // constexpr operator<=>(const std::vector<_Ty, allocator_type>& lhs, const std::vector<_Ty, allocator_type>& rhs) {
    //     return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), lhs.end(), std::partial_ordering::less);
    // }


    // modifiers
    constexpr void assign(const size_type& count, const _Ty& value)
    {
        if(m_capacity < count) {
            _inc_cap__no_copy( (count + 1) * 2);
        }

        m_size = count;
        for(size_t i = 0; i < count; i++) {
            m_data[i] = value;
        }
    }

    template< class InputIt >
    constexpr void assign( InputIt first, InputIt last )
    {
        if(first == last) {
            return;
        }

        size_t dist = (size_t) std::distance(first, last);
        if(m_capacity < std::distance(first, last) ) {
            _inc_cap__no_copy( (dist + 1) * 2);
        }

        m_size = dist;
        std::move(first, last, begin());

    }

    constexpr void assign( std::initializer_list<_Ty> ilist)
    {
        if(m_capacity < ilist.size()) {
            _inc_cap__no_copy( (ilist.size() + 1) * 2); // no need to copy the data
        }

        m_size = ilist.size();
        std::move(ilist.begin(), ilist.end(), begin());
    }

    // this version of the insert and remove functions are pretty bad. There's definitely a better way to do it
    constexpr void insert(size_t index, const _Ty& data) {
        if(index > m_size) {
            throw std::out_of_range("Attempted to insert out of bounds");
        }

        if(m_size + 1 > m_capacity) {
            _inc_cap( (m_capacity + 1) * 2);
        }

        if(m_size > 0) {
            for(size_t i = m_size; i >= index + 1; --i) {
                m_data[i] = m_data[i - 1];
            }
        }

        m_data[index] = data;
        m_size++;
    }

    constexpr void remove(size_type index) {
        if(index >= m_size) {
            throw std::out_of_range("Attempted to remove out of range element");
        }

        for(size_t i = index; i < m_size - 1; i++) {
            m_data[i] = m_data[i + 1];
        }
        m_size--;
    }

    constexpr void clear() {
        delete[] m_data;
        m_size = 0;
        m_data = new _Ty[m_capacity];
    }

    constexpr void reserve(const size_type& _cap)
    {
        if( !(_cap > m_capacity) )
            return;
        if( _cap > max_size() )
            throw std::length_error("new capacity is too large");
        _inc_cap(_cap);
    }

    constexpr void shrink_to_fit()
    {
        _dec_cap(m_size);
    }

    constexpr void push_back(const _Ty& _value)
    {
        if(m_size + 1 >= m_capacity)
            _inc_cap( (m_capacity + 1) * 2);

        m_data[m_size] = _value;
        m_size++;
    }

    constexpr void pop_back() noexcept
    {
        if(m_size > 0)
            m_size--;
    }

    constexpr void swap(array_list<_Ty, allocator_type>& target) noexcept {
        std::swap(m_size, target.m_size);
        std::swap(m_capacity, target.m_capacity);
        std::swap(m_data, target.m_data);
    }



    // linear algebra functions
    Shape shape() const
    {   
        // doesn't support 2d matricies yet
        return Shape(m_size, _MAX_SIZE);
    }

    double dot(const array_list<_Ty>& _vect) {
        if( shape() != _vect.shape()) {
            throw std::invalid_argument("Vector shapes not equal for dot product.");
        }

        auto first = begin();
        auto second = _vect.begin();
        double result = 0.0;

        while(first != end()) {
            result += (*first) * (*second);
            first++;
            second++;
        }

        return result;
    }


    constexpr double sum() {
        struct sum_func f(m_data[0]);
        f = std::for_each(begin() + 1, end(), f);
        return f.result;
    }

private:
    constexpr void _inc_cap(const size_t& _cap)
    {
        _Ty* new_data = m_alloc.allocate(_cap);
        std::move(begin(), end(), &new_data[0]);
        m_alloc.deallocate(m_data, m_capacity);
        m_data = new_data;
        m_capacity = _cap;
    }

    constexpr void _inc_cap__no_copy(const size_t& _cap) {
        _Ty* new_data = m_alloc.allocate(_cap);
        m_alloc.deallocate(m_data, m_capacity);
        m_data = new_data;
        m_capacity = _cap;
        m_size = 0;
    }

    // resizes the container down to a set capacity. If _target > m_size, m_size will be decreased
    constexpr void _dec_cap(const size_type& _target)
    {
        if(_target < m_size)
            return;

        _Ty* new_data = m_alloc.allocate(_target);
        std::move(begin(), end(), &new_data[0]);
        m_alloc.deallocate(m_data, m_capacity);
        m_data = new_data;
        m_capacity = _target;
    }
};

// extra stuff
template<typename _Ty>
constexpr std::ostream& operator<<(std::ostream& os, array_list<_Ty>& lst)
{
    if(lst.size() == 0)
    {
        os << '[' << ']';
        return os;
    }

    os << '[';
    for(auto i = 0; i < lst.size() - 1; i++)
        os << lst.at(i) << ',' << ' ';
    os << lst.at(lst.size() - 1) << ']';
    return os;
}


#endif /* ARRAY_LIST_HPP */
