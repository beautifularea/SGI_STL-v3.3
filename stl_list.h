/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1996,1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

/* NOTE: This is an internal header file, included by other STL headers.
 *   You should not attempt to use it directly.
 */

//list 双向链表

#ifndef __SGI_STL_INTERNAL_LIST_H
#define __SGI_STL_INTERNAL_LIST_H

#include <concept_checks.h>

__STL_BEGIN_NAMESPACE

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma set woff 1174
#pragma set woff 1375
#endif

//结点数据结构
struct _List_node_base {//指针域
    _List_node_base *_M_next;
    _List_node_base *_M_prev;
};

template<class _Tp>
struct _List_node : public _List_node_base {//继承指针域结构体
    _Tp _M_data;//数据域
};

struct _List_iterator_base {//迭代器数据结构
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef bidirectional_iterator_tag iterator_category;

    _List_node_base *_M_node;

    _List_iterator_base(_List_node_base *__x) : _M_node(__x) {}

    _List_iterator_base() {}

    void _M_incr() { _M_node = _M_node->_M_next; }//自增函数

    void _M_decr() { _M_node = _M_node->_M_prev; }//自减函数

    bool operator==(const _List_iterator_base &__x) const {//赋值运算符=
        return _M_node == __x._M_node;
    }

    bool operator!=(const _List_iterator_base &__x) const {//不等运算符!=
        return _M_node != __x._M_node;
    }
};

template<class _Tp, class _Ref, class _Ptr>
struct _List_iterator : public _List_iterator_base {
    typedef _List_iterator<_Tp, _Tp &, _Tp *> iterator;
    typedef _List_iterator<_Tp, const _Tp &, const _Tp *> const_iterator;
    typedef _List_iterator<_Tp, _Ref, _Ptr> _Self;

    typedef _Tp value_type;
    typedef _Ptr pointer;
    typedef _Ref reference;
    typedef _List_node<_Tp> _Node;

    _List_iterator(_Node *__x) : _List_iterator_base(__x) {}

    _List_iterator() {}

    _List_iterator(const iterator &__x) : _List_iterator_base(__x._M_node) {}

    reference operator*() const { return ((_Node *) _M_node)->_M_data; }

#ifndef __SGI_STL_NO_ARROW_OPERATOR

    pointer operator->() const { return &(operator*()); }

#endif /* __SGI_STL_NO_ARROW_OPERATOR */

    _Self &operator++() {//前缀自增：先增后返回
        this->_M_incr();
        return *this;
    }

    _Self operator++(int) {//后缀自增：先创建副本，自增，再返回副本
        _Self __tmp = *this;
        this->_M_incr();
        return __tmp;
    }

    _Self &operator--() {//前缀自减：先减后返回
        this->_M_decr();
        return *this;
    }

    _Self operator--(int) {//后缀自减：先创建副本，自减，再返回副本
        _Self __tmp = *this;
        this->_M_decr();
        return __tmp;
    }
};

#ifndef __STL_CLASS_PARTIAL_SPECIALIZATION

inline bidirectional_iterator_tag
iterator_category(const _List_iterator_base &) {
    return bidirectional_iterator_tag();
}

template<class _Tp, class _Ref, class _Ptr>
inline _Tp *
value_type(const _List_iterator<_Tp, _Ref, _Ptr> &) {
    return 0;
}

inline ptrdiff_t *
distance_type(const _List_iterator_base &) {
    return 0;
}

#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */


// Base class that encapsulates details of allocators.  Three cases:
// an ordinary standard-conforming allocator, a standard-conforming
// allocator with no non-static data, and an SGI-style allocator.
// This complexity is necessary only because we're worrying about backward
// compatibility and because we want to avoid wasting storage on an 
// allocator instance if it isn't necessary.

#ifdef __STL_USE_STD_ALLOCATORS

// Base for general standard-conforming allocators.
template <class _Tp, class _Allocator, bool _IsStatic>
class _List_alloc_base {
public:
  typedef typename _Alloc_traits<_Tp, _Allocator>::allocator_type
          allocator_type;
  allocator_type get_allocator() const { return _Node_allocator; }

  _List_alloc_base(const allocator_type& __a) : _Node_allocator(__a) {}

protected:
  _List_node<_Tp>* _M_get_node()
   { return _Node_allocator.allocate(1); }
  void _M_put_node(_List_node<_Tp>* __p)
    { _Node_allocator.deallocate(__p, 1); }

protected:
  typename _Alloc_traits<_List_node<_Tp>, _Allocator>::allocator_type
           _Node_allocator;
  _List_node<_Tp>* _M_node;
};

// Specialization for instanceless allocators.

template <class _Tp, class _Allocator>
class _List_alloc_base<_Tp, _Allocator, true> {
public:
  typedef typename _Alloc_traits<_Tp, _Allocator>::allocator_type
          allocator_type;
  allocator_type get_allocator() const { return allocator_type(); }

  _List_alloc_base(const allocator_type&) {}

protected:
  typedef typename _Alloc_traits<_List_node<_Tp>, _Allocator>::_Alloc_type
          _Alloc_type;
  _List_node<_Tp>* _M_get_node() { return _Alloc_type::allocate(1); }
  void _M_put_node(_List_node<_Tp>* __p) { _Alloc_type::deallocate(__p, 1); }

protected:
  _List_node<_Tp>* _M_node;//链表头结点，本身不存储数据
};

template <class _Tp, class _Alloc>
class _List_base 
  : public _List_alloc_base<_Tp, _Alloc,
                            _Alloc_traits<_Tp, _Alloc>::_S_instanceless>
{
public:
  typedef _List_alloc_base<_Tp, _Alloc,
                           _Alloc_traits<_Tp, _Alloc>::_S_instanceless>
          _Base; 
  typedef typename _Base::allocator_type allocator_type;

  _List_base(const allocator_type& __a) : _Base(__a) {
    _M_node = _M_get_node();
    _M_node->_M_next = _M_node;
    _M_node->_M_prev = _M_node;
  }
  ~_List_base() {
    clear();
    _M_put_node(_M_node);
  }

  void clear();
};

#else /* __STL_USE_STD_ALLOCATORS */

template<class _Tp, class _Alloc>
class _List_base {
public:
    typedef _Alloc allocator_type;

    allocator_type get_allocator() const { return allocator_type(); }

    _List_base(const allocator_type &) {
        _M_node = _M_get_node();
        _M_node->_M_next = _M_node;
        _M_node->_M_prev = _M_node;
    }

    ~_List_base() {
        clear();
        _M_put_node(_M_node);
    }

    void clear();

protected:
    typedef simple_alloc <_List_node<_Tp>, _Alloc> _Alloc_type;

    _List_node<_Tp> *_M_get_node() { return _Alloc_type::allocate(1); }

    void _M_put_node(_List_node<_Tp> *__p) { _Alloc_type::deallocate(__p, 1); }

protected:
    _List_node<_Tp> *_M_node;//链表头结点，本身不存储数据
};

#endif /* __STL_USE_STD_ALLOCATORS */

template<class _Tp, class _Alloc>
void
_List_base<_Tp, _Alloc>::clear() {//清空链表
    _List_node<_Tp> *__cur = (_List_node<_Tp> *) _M_node->_M_next;//存有数据的第一个结点
    while (__cur != _M_node) {//遍历所有结点
        _List_node<_Tp> *__tmp = __cur;
        __cur = (_List_node<_Tp> *) __cur->_M_next;
        _Destroy(&__tmp->_M_data);
        _M_put_node(__tmp);
    }
    _M_node->_M_next = _M_node;
    _M_node->_M_prev = _M_node;
}

template<class _Tp, class _Alloc = __STL_DEFAULT_ALLOCATOR(_Tp)>
class list : protected _List_base<_Tp, _Alloc> {
    // requirements:

    __STL_CLASS_REQUIRES(_Tp, _Assignable
    );

    typedef _List_base<_Tp, _Alloc> _Base;
protected:
    typedef void *_Void_pointer;

public:
    typedef _Tp value_type;
    typedef value_type *pointer;
    typedef const value_type *const_pointer;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef _List_node<_Tp> _Node;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    typedef typename _Base::allocator_type allocator_type;

    allocator_type get_allocator() const { return _Base::get_allocator(); }

public:
    typedef _List_iterator<_Tp, _Tp &, _Tp *> iterator;
    typedef _List_iterator<_Tp, const _Tp &, const _Tp *> const_iterator;

#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION
    typedef reverse_iterator<const_iterator> const_reverse_iterator;
    typedef reverse_iterator<iterator>       reverse_iterator;
#else /* __STL_CLASS_PARTIAL_SPECIALIZATION */
    typedef reverse_bidirectional_iterator <const_iterator, value_type,
    const_reference, difference_type>
            const_reverse_iterator;
    typedef reverse_bidirectional_iterator <iterator, value_type, reference,
    difference_type>
            reverse_iterator;
#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

protected:
#ifdef __STL_HAS_NAMESPACES
    using _Base::_M_node;
    using _Base::_M_put_node;
    using _Base::_M_get_node;
#endif /* __STL_HAS_NAMESPACES */

protected:
    _Node *_M_create_node(const _Tp &__x) {
        _Node *__p = _M_get_node();
        __STL_TRY {
                _Construct(&__p->_M_data, __x);
        }
        __STL_UNWIND(_M_put_node(__p));
        return __p;
    }

    _Node *_M_create_node() {
        _Node *__p = _M_get_node();
        __STL_TRY {
                _Construct(&__p->_M_data);
        }
        __STL_UNWIND(_M_put_node(__p));
        return __p;
    }

public:
    explicit list(const allocator_type &__a = allocator_type()) : _Base(__a) {}

    iterator begin() { return (_Node *) (_M_node->_M_next); }

    const_iterator begin() const { return (_Node *) (_M_node->_M_next); }

    iterator end() { return _M_node; }

    const_iterator end() const { return _M_node; }

    reverse_iterator rbegin() { return reverse_iterator(end()); }//reverse_begin = end

    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }

    reverse_iterator rend() { return reverse_iterator(begin()); }//reverse_end = begin

    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

    bool empty() const { return _M_node->_M_next == _M_node; }//头结点指向自身表明链表中无元素

    size_type size() const {
        size_type __result = 0;
        distance(begin(), end(), __result);
        return __result;
    }

    size_type max_size() const { return size_type(-1); }

    reference front() { return *begin(); }

    const_reference front() const { return *begin(); }

    reference back() { return *(--end()); }//end()前一个是存有数据的尾结点

    const_reference back() const { return *(--end()); }

    void swap(list<_Tp, _Alloc> &__x) { __STD::swap(_M_node, __x._M_node); }

    iterator insert(iterator __position, const _Tp &__x) {//将__x插入到原结点__position的位置，返回迭代器
        _Node *__tmp = _M_create_node(__x);//创建新结点__tmp
        //注意以下四步的顺序
        __tmp->_M_next = __position._M_node;//新结点的next指向原结点
        __tmp->_M_prev = __position._M_node->_M_prev;//新结点的prev指向原结点prev
        __position._M_node->_M_prev->_M_next = __tmp;//原结点的prev的next指向新结点
        __position._M_node->_M_prev = __tmp;//原结点的prev指向新结点
        return __tmp;
    }

    iterator insert(iterator __position) { return insert(__position, _Tp()); }

#ifdef __STL_MEMBER_TEMPLATES
    // Check whether it's an integral type.  If so, it's not an iterator.

    template<class _Integer>
    void _M_insert_dispatch(iterator __pos, _Integer __n, _Integer __x,
                            __true_type) {
      _M_fill_insert(__pos, (size_type) __n, (_Tp) __x);
    }

    template <class _InputIterator>
    void _M_insert_dispatch(iterator __pos,
                            _InputIterator __first, _InputIterator __last,
                            __false_type);

    template <class _InputIterator>
    void insert(iterator __pos, _InputIterator __first, _InputIterator __last) {
      typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
      _M_insert_dispatch(__pos, __first, __last, _Integral());
    }

#else /* __STL_MEMBER_TEMPLATES */

    void insert(iterator __position, const _Tp *__first, const _Tp *__last);

    void insert(iterator __position,
                const_iterator __first, const_iterator __last);

#endif /* __STL_MEMBER_TEMPLATES */

    void insert(iterator __pos, size_type __n, const _Tp &__x) { _M_fill_insert(__pos, __n, __x); }

    void _M_fill_insert(iterator __pos, size_type __n, const _Tp &__x);

    void push_front(const _Tp &__x) { insert(begin(), __x); }

    void push_front() { insert(begin()); }

    void push_back(const _Tp &__x) { insert(end(), __x); }

    void push_back() { insert(end()); }

    iterator erase(iterator __position) {//删除结点__position，返回迭代器
        _List_node_base *__next_node = __position._M_node->_M_next;//创建指向下一结点的指针
        _List_node_base *__prev_node = __position._M_node->_M_prev;//创建指向上一结点的指针
        _Node *__n = (_Node *) __position._M_node;//创建指向结点的指针；_M_node也是指针，使用了强制类型转换
        __prev_node->_M_next = __next_node;//__position上一个结点的右指针域指向__position下一个结点
        __next_node->_M_prev = __prev_node;//__position下一个结点的左指针域指向__position上一个结点
        _Destroy(&__n->_M_data);//删除结点
        _M_put_node(__n);
        return iterator((_Node *) __next_node);//返回原结点下一结点的迭代器
    }

    iterator erase(iterator __first, iterator __last);

    void clear() { _Base::clear(); }

    void resize(size_type __new_size, const _Tp &__x);

    void resize(size_type __new_size) { this->resize(__new_size, _Tp()); }

    void pop_front() { erase(begin()); }//删除头结点，头结点是_M_node->next

    void pop_back() {//删除尾结点
        iterator __tmp = end();//end()获取 _M_node
        erase(--__tmp);//尾结点是_M_node_prev
    }

    list(size_type __n, const _Tp &__value,
         const allocator_type &__a = allocator_type())
            : _Base(__a) { insert(begin(), __n, __value); }

    explicit list(size_type __n)
            : _Base(allocator_type()) { insert(begin(), __n, _Tp()); }

#ifdef __STL_MEMBER_TEMPLATES

    // We don't need any dispatching tricks here, because insert does all of
    // that anyway.
    template <class _InputIterator>
    list(_InputIterator __first, _InputIterator __last,
         const allocator_type& __a = allocator_type())
      : _Base(__a)
      { insert(begin(), __first, __last); }

#else /* __STL_MEMBER_TEMPLATES */

    list(const _Tp *__first, const _Tp *__last,
         const allocator_type &__a = allocator_type())
            : _Base(__a) { this->insert(begin(), __first, __last); }

    list(const_iterator __first, const_iterator __last,
         const allocator_type &__a = allocator_type())
            : _Base(__a) { this->insert(begin(), __first, __last); }

#endif /* __STL_MEMBER_TEMPLATES */

    list(const list<_Tp, _Alloc> &__x) : _Base(__x.get_allocator()) { insert(begin(), __x.begin(), __x.end()); }

    ~list() {}

    list<_Tp, _Alloc> &operator=(const list<_Tp, _Alloc> &__x);

public:
    // assign(), a generalized assignment member function.  Two
    // versions: one that takes a count, and one that takes a range.
    // The range version is a member template, so we dispatch on whether
    // or not the type is an integer.

    void assign(size_type __n, const _Tp &__val) { _M_fill_assign(__n, __val); }

    void _M_fill_assign(size_type __n, const _Tp &__val);

#ifdef __STL_MEMBER_TEMPLATES

    template <class _InputIterator>
    void assign(_InputIterator __first, _InputIterator __last) {
      typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
      _M_assign_dispatch(__first, __last, _Integral());
    }

    template <class _Integer>
    void _M_assign_dispatch(_Integer __n, _Integer __val, __true_type)
      { _M_fill_assign((size_type) __n, (_Tp) __val); }

    template <class _InputIterator>
    void _M_assign_dispatch(_InputIterator __first, _InputIterator __last,
                            __false_type);

#endif /* __STL_MEMBER_TEMPLATES */

protected:
    void transfer(iterator __position, iterator __first, iterator __last) {// 将[first, last)从原链表中剪切到__position前
        if (__position != __last) {
            // Remove [first, last) from its old position. 先修改所有的_M_next
            __last._M_node->_M_prev->_M_next = __position._M_node;
            __first._M_node->_M_prev->_M_next = __last._M_node;
            __position._M_node->_M_prev->_M_next = __first._M_node;

            // Splice [first, last) into its new position. 再修改所有的_M_prev
            _List_node_base *__tmp = __position._M_node->_M_prev;
            __position._M_node->_M_prev = __last._M_node->_M_prev;
            __last._M_node->_M_prev = __first._M_node->_M_prev;
            __first._M_node->_M_prev = __tmp;
        }
    }

public:
    void splice(iterator __position, list &__x) {//将链表__x移动到__position之前
        if (!__x.empty())
            this->transfer(__position, __x.begin(), __x.end());
    }

    void splice(iterator __position, list &, iterator __i) {//将链表中迭代器__i指向的内容移动到__position之前
        iterator __j = __i;
        ++__j;
        if (__position == __i || __position == __j) return;
        this->transfer(__position, __i, __j);
    }

    void splice(iterator __position, list &, iterator __first, iterator __last) {//将链表中[first, last)移动到__position之前
        if (__first != __last)
            this->transfer(__position, __first, __last);
    }

    void remove(const _Tp &__value);

    void unique();

    void merge(list &__x);

    void reverse();

    void sort();

#ifdef __STL_MEMBER_TEMPLATES
    template <class _Predicate> void remove_if(_Predicate);
    template <class _BinaryPredicate> void unique(_BinaryPredicate);
    template <class _StrictWeakOrdering> void merge(list&, _StrictWeakOrdering);
    template <class _StrictWeakOrdering> void sort(_StrictWeakOrdering);
#endif /* __STL_MEMBER_TEMPLATES */
};

template<class _Tp, class _Alloc>
inline bool
operator==(const list<_Tp, _Alloc> &__x, const list<_Tp, _Alloc> &__y) {
    typedef typename list<_Tp, _Alloc>::const_iterator const_iterator;
    const_iterator __end1 = __x.end();
    const_iterator __end2 = __y.end();

    const_iterator __i1 = __x.begin();
    const_iterator __i2 = __y.begin();
    while (__i1 != __end1 && __i2 != __end2 && *__i1 == *__i2) {
        ++__i1;
        ++__i2;
    }
    return __i1 == __end1 && __i2 == __end2;
}

template<class _Tp, class _Alloc>
inline bool operator<(const list<_Tp, _Alloc> &__x,
                      const list<_Tp, _Alloc> &__y) {
    return lexicographical_compare(__x.begin(), __x.end(),
                                   __y.begin(), __y.end());
}

#ifdef __STL_FUNCTION_TMPL_PARTIAL_ORDER

template <class _Tp, class _Alloc>
inline bool operator!=(const list<_Tp,_Alloc>& __x,
                       const list<_Tp,_Alloc>& __y) {
  return !(__x == __y);
}

template <class _Tp, class _Alloc>
inline bool operator>(const list<_Tp,_Alloc>& __x,
                      const list<_Tp,_Alloc>& __y) {
  return __y < __x;
}

template <class _Tp, class _Alloc>
inline bool operator<=(const list<_Tp,_Alloc>& __x,
                       const list<_Tp,_Alloc>& __y) {
  return !(__y < __x);
}

template <class _Tp, class _Alloc>
inline bool operator>=(const list<_Tp,_Alloc>& __x,
                       const list<_Tp,_Alloc>& __y) {
  return !(__x < __y);
}

template <class _Tp, class _Alloc>
inline void 
swap(list<_Tp, _Alloc>& __x, list<_Tp, _Alloc>& __y)
{
  __x.swap(__y);
}

#endif /* __STL_FUNCTION_TMPL_PARTIAL_ORDER */

#ifdef __STL_MEMBER_TEMPLATES

template <class _Tp, class _Alloc> template <class _InputIter>
void 
list<_Tp, _Alloc>::_M_insert_dispatch(iterator __position,
                                      _InputIter __first, _InputIter __last,
                                      __false_type)
{
  for ( ; __first != __last; ++__first)
    insert(__position, *__first);
}

#else /* __STL_MEMBER_TEMPLATES */

template<class _Tp, class _Alloc>
void
list<_Tp, _Alloc>::insert(iterator __position,
                          const _Tp *__first, const _Tp *__last) {
    for (; __first != __last; ++__first)
        insert(__position, *__first);
}

template<class _Tp, class _Alloc>
void
list<_Tp, _Alloc>::insert(iterator __position,
                          const_iterator __first, const_iterator __last) {
    for (; __first != __last; ++__first)
        insert(__position, *__first);
}

#endif /* __STL_MEMBER_TEMPLATES */

template<class _Tp, class _Alloc>
void
list<_Tp, _Alloc>::_M_fill_insert(iterator __position,
                                  size_type __n, const _Tp &__x) {
    for (; __n > 0; --__n)
        insert(__position, __x);
}

template<class _Tp, class _Alloc>
typename list<_Tp, _Alloc>::iterator list<_Tp, _Alloc>::erase(iterator __first,
                                                              iterator __last) {
    while (__first != __last)
        erase(__first++);
    return __last;
}

template<class _Tp, class _Alloc>
void list<_Tp, _Alloc>::resize(size_type __new_size, const _Tp &__x) {
    iterator __i = begin();
    size_type __len = 0;
    for (; __i != end() && __len < __new_size; ++__i, ++__len);
    if (__len == __new_size)
        erase(__i, end());
    else                          // __i == end()
        insert(end(), __new_size - __len, __x);
}

template<class _Tp, class _Alloc>
list<_Tp, _Alloc> &list<_Tp, _Alloc>::operator=(const list<_Tp, _Alloc> &__x) {//赋值运算符=
    if (this != &__x) {
        iterator __first1 = begin();
        iterator __last1 = end();
        const_iterator __first2 = __x.begin();
        const_iterator __last2 = __x.end();
        while (__first1 != __last1 && __first2 != __last2)
            *__first1++ = *__first2++;
        if (__first2 == __last2)
            erase(__first1, __last1);
        else
            insert(__last1, __first2, __last2);
    }
    return *this;
}

template<class _Tp, class _Alloc>
void list<_Tp, _Alloc>::_M_fill_assign(size_type __n, const _Tp &__val) {
    iterator __i = begin();
    for (; __i != end() && __n > 0; ++__i, --__n)
        *__i = __val;
    if (__n > 0)
        insert(end(), __n, __val);
    else
        erase(__i, end());
}

#ifdef __STL_MEMBER_TEMPLATES

template <class _Tp, class _Alloc> template <class _InputIter>
void
list<_Tp, _Alloc>::_M_assign_dispatch(_InputIter __first2, _InputIter __last2,
                                      __false_type)
{
  iterator __first1 = begin();
  iterator __last1 = end();
  for ( ; __first1 != __last1 && __first2 != __last2; ++__first1, ++__first2)
    *__first1 = *__first2;
  if (__first2 == __last2)
    erase(__first1, __last1);
  else
    insert(__last1, __first2, __last2);
}

#endif /* __STL_MEMBER_TEMPLATES */

template<class _Tp, class _Alloc>
void list<_Tp, _Alloc>::remove(const _Tp &__value) {
    iterator __first = begin();
    iterator __last = end();
    while (__first != __last) {
        iterator __next = __first;
        ++__next;
        if (*__first == __value) erase(__first);
        __first = __next;
    }
}

template<class _Tp, class _Alloc>
void list<_Tp, _Alloc>::unique() {
    iterator __first = begin();
    iterator __last = end();
    if (__first == __last) return;
    iterator __next = __first;
    while (++__next != __last) {
        if (*__first == *__next)
            erase(__next);
        else
            __first = __next;
        __next = __first;
    }
}

template<class _Tp, class _Alloc>
void list<_Tp, _Alloc>::merge(list<_Tp, _Alloc> &__x) {
    iterator __first1 = begin();
    iterator __last1 = end();
    iterator __first2 = __x.begin();
    iterator __last2 = __x.end();
    while (__first1 != __last1 && __first2 != __last2)
        if (*__first2 < *__first1) {
            iterator __next = __first2;
            transfer(__first1, __first2, ++__next);
            __first2 = __next;
        } else
            ++__first1;
    if (__first2 != __last2) transfer(__last1, __first2, __last2);
}

inline void __List_base_reverse(_List_node_base *__p) {
    _List_node_base *__tmp = __p;
    do {
        __STD::swap(__tmp->_M_next, __tmp->_M_prev);
        __tmp = __tmp->_M_prev;     // 原来的next在执行上一句后变为了prev.
    } while (__tmp != __p);
}

template<class _Tp, class _Alloc>
inline void list<_Tp, _Alloc>::reverse() {
    __List_base_reverse(this->_M_node);
}

template<class _Tp, class _Alloc>
void list<_Tp, _Alloc>::sort() {//sort排序
    // Do nothing if the list has length 0 or 1. 如果list长度是0或1，不执行任何操作
    if (_M_node->_M_next != _M_node && _M_node->_M_next->_M_next != _M_node) {
        list<_Tp, _Alloc> __carry;
        list<_Tp, _Alloc> __counter[64];
        int __fill = 0;
        while (!empty()) {
            __carry.splice(__carry.begin(), *this, begin());
            int __i = 0;
            while (__i < __fill && !__counter[__i].empty()) {
                __counter[__i].merge(__carry);
                __carry.swap(__counter[__i++]);
            }
            __carry.swap(__counter[__i]);
            if (__i == __fill) ++__fill;
        }

        for (int __i = 1; __i < __fill; ++__i)
            __counter[__i].merge(__counter[__i - 1]);
        swap(__counter[__fill - 1]);
    }
}

#ifdef __STL_MEMBER_TEMPLATES

template <class _Tp, class _Alloc> template <class _Predicate>
void list<_Tp, _Alloc>::remove_if(_Predicate __pred)
{
  iterator __first = begin();
  iterator __last = end();
  while (__first != __last) {
    iterator __next = __first;
    ++__next;
    if (__pred(*__first)) erase(__first);
    __first = __next;
  }
}

template <class _Tp, class _Alloc> template <class _BinaryPredicate>
void list<_Tp, _Alloc>::unique(_BinaryPredicate __binary_pred)
{
  iterator __first = begin();
  iterator __last = end();
  if (__first == __last) return;
  iterator __next = __first;
  while (++__next != __last) {
    if (__binary_pred(*__first, *__next))
      erase(__next);
    else
      __first = __next;
    __next = __first;
  }
}

template <class _Tp, class _Alloc> template <class _StrictWeakOrdering>
void list<_Tp, _Alloc>::merge(list<_Tp, _Alloc>& __x,
                              _StrictWeakOrdering __comp)
{
  iterator __first1 = begin();
  iterator __last1 = end();
  iterator __first2 = __x.begin();
  iterator __last2 = __x.end();
  while (__first1 != __last1 && __first2 != __last2)
    if (__comp(*__first2, *__first1)) {
      iterator __next = __first2;
      transfer(__first1, __first2, ++__next);
      __first2 = __next;
    }
    else
      ++__first1;
  if (__first2 != __last2) transfer(__last1, __first2, __last2);
}

template <class _Tp, class _Alloc> template <class _StrictWeakOrdering>
void list<_Tp, _Alloc>::sort(_StrictWeakOrdering __comp)
{
  // Do nothing if the list has length 0 or 1.
  if (_M_node->_M_next != _M_node && _M_node->_M_next->_M_next != _M_node) {
    list<_Tp, _Alloc> __carry;
    list<_Tp, _Alloc> __counter[64];
    int __fill = 0;
    while (!empty()) {
      __carry.splice(__carry.begin(), *this, begin());
      int __i = 0;
      while(__i < __fill && !__counter[__i].empty()) {
        __counter[__i].merge(__carry, __comp);
        __carry.swap(__counter[__i++]);
      }
      __carry.swap(__counter[__i]);         
      if (__i == __fill) ++__fill;
    } 

    for (int __i = 1; __i < __fill; ++__i) 
      __counter[__i].merge(__counter[__i-1], __comp);
    swap(__counter[__fill-1]);
  }
}

#endif /* __STL_MEMBER_TEMPLATES */

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma reset woff 1174
#pragma reset woff 1375
#endif

__STL_END_NAMESPACE

#endif /* __SGI_STL_INTERNAL_LIST_H */

// Local Variables:
// mode:C++
// End:
