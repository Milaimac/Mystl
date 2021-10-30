#include <iostream>
#include <cstdlib>
#include <new>          //use the placement new 

using namespace std;

// 实现vector模块
// 实现自增长， 大小，判空函数
// 现在知道了 Vector_base 的好处了，  ———— 可以减少重复的操作

template <typename _T1, typename _Tp>
    inline void
    _Construct(_T1* __p, const _Tp& val){
        new (static_cast<void*>(__p)) _T1(val);   
    }

template <typename _T1>
    inline void 
    __Construct_novalue(_T1* __p)
    { ::new(static_cast<void*>(__p)) _T1(); }

// 调用析构函数
template <typename _Tp>
    inline void _Destory(_Tp* __pointer)
    {
        __pointer->~_Tp();
    }
    
// can use the pod_type to optimize the program
template <typename iter>
    inline void destroy(iter first, iter last)
    {
        for(auto curr = first; first != last; ++first){
            _Destory(curr);
        }
    }

// template <class Tp_>
// class Vector_base{

// };

template <class Tp_>
class Vector{

public:
    typedef Tp_ value_type;
    typedef value_type* pionter_type;
    typedef const value_type* const_pointer;
    // 由于vector的数据结构是一块连续的数组， 所以直接使用指针作为迭代器即可
    typedef  value_type* iterator;
    typedef  const value_type* const_iterator;
    typedef  value_type& reference;
    typedef  const value_type& const_reference;
    typedef  size_t size_type;
    typedef  ptrdiff_t difference_type;


protected:
    Tp_ * M_start;     
    Tp_ * M_finish;
    Tp_ * M_end_of_storage;


public:


    explicit Vector():M_start(0), M_finish(0), M_end_of_storage(0){

    }

    // in the end, I decide to use the allocate and then init, not new [];
    // becuase there is trouble to initliaze to all type. and it will initialize twice
    explicit Vector(size_type __n)      // explicit only in the one parameter constructor, it will avoid the the implicit conversion in operator= 
    :M_start(0), M_finish(0), M_end_of_storage(0)
    {
        M_start = static_cast<iterator>(malloc(__n * sizeof(Tp_)));
        M_end_of_storage = M_start + __n;           
        // Initliaze the Tp_ and set the M_finish
        auto curr = M_start;
        for(;__n > 0;--__n, ++curr){
            _Construct(&*curr, Tp_());
        }
        M_finish = curr;
    }

    Vector(size_type __n,const Tp_& value)
    :M_start(0), M_finish(0), M_end_of_storage(0)
    {
        M_start = static_cast<iterator>(malloc(__n * sizeof(Tp_)));
        M_end_of_storage = M_start + __n;

        auto curr = M_start;
        for(;__n > 0;--__n, ++curr){
            _Construct(&*curr, value);
        }
        M_finish = curr;
    } 

    Vector(const Vector<Tp_>& _x)
    :M_start(0), M_finish(0), M_end_of_storage(0)
    {   
        size_type n = _x.size();
        M_start = static_cast<iterator> (malloc(n * sizeof(Tp_)));      // FIXME  convert to Tp_*
        M_end_of_storage = M_start + n;

        auto curr = M_start;
        auto iter = _x.begin();
        for(; n > 0; curr++, iter++){
            _Construct(&*curr, *iter);                                 
        }
        M_finish = curr;
    }


    ~Vector(){
        destroy(begin(), end());
    }
protected:
    void insert_aux(iterator __position, const Tp_& x);
    

public:
    iterator begin() {
        return M_start;
    }
    const_iterator begin() const{
        return M_start;
    }
    iterator end(){
        return M_finish;
    }
    const_iterator end() const{
        return M_finish;
    }



    size_type size() const{
        return size_type(end() - begin());
    }

    size_type capacity() const{
        return M_end_of_storage - begin();
    }

    bool empty() const{
        return begin() == end();
    }
    reference operator[] (size_type n){
        return *(begin() + n);
    }

    const_reference operator[] (size_type n) const{
        return *(begin() + n);
    }
    void push_back(const Tp_& x){
        if(M_finish != M_end_of_storage){
            _Construct(M_finish, x);
            ++M_finish;
        }
        else
        {
            insert_aux(M_finish, x);
        }
    }

    // when the position is end() and the finish not excess the end_of_storage, we can construct element in the finish
    // otherwise we must reallocate the vector
    iterator insert(iterator __position, const Tp_& x){
        size_type n = __position - begin();
        if(M_finish != M_end_of_storage && __position == end()){
            _Construct(M_finish, x);
            ++M_finish;
        }
        else
        {   // if there no space to use 
            insert_aux(__position, x);
        }

        return begin() + n;
    }

    // pop one element
    void pop_back(){
        --M_finish;
        _Destory(M_finish);         //don't need to deallocate, keep it
    }


};

template<class Tp_>
void 
Vector<Tp_>::insert_aux(iterator __position, const Tp_& x){
    // if there is space to use 
    if(M_finish != M_end_of_storage){
        // move the finish to finish + 1
        _Construct(M_finish, *(M_finish-1));
        ++M_finish;
        
        auto x_copy = x;
        // 将_postion - M_finish-2 copy到 M_finish-1前面的内存
        auto result = M_finish-1;
        auto last  = M_finish-2;
        for(auto n = last-__position; n > 0; --n){
            *--result = *--last;
        }
        *__position = x_copy;
    }
    else    // need to increment the capacity to 2 times
    {
        const size_type old_size = size();
        const size_type len = old_size > 0 ? old_size * 2 : 1;
        // allocate the memeroy1
        iterator new_start = static_cast<iterator>(malloc(len * sizeof(Tp_)));
        iterator new_finish = new_start;
        //TODO for safe, we should consider the oom , and use try    
        // now need to copy old vector to new memory
        // here to copy the [start, position] to newstart
        auto curr1 = new_start;
        for(auto curr = M_start; curr != __position;++curr, ++curr1){
            *curr1 = *curr;
        }
        new_finish = curr1;
        _Construct(new_finish, x);
        ++new_finish;

        // here to copy the [position , finish] to newfinish
        auto curr2 = new_finish;        //this is destination
        for(auto curr = __position;curr != M_finish;++curr, ++curr2){
            *curr2 = *curr;
        }
        new_finish = curr2;

        // after allocate the new memory space, then destroy the old vector
        // FIXME here should deallcoate
        destroy(begin(), end());
        // free can release what the malloc create
        free(begin());
        M_start = new_start;
        M_finish = new_finish;
        M_end_of_storage = new_start + len;
    }
}

