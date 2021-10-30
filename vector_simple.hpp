#include<iostream>
#include<assert.h>

using namespace std;

//vetcor的实现是采用类模板实现的。
template<class T>
class Vector {
public:
	typedef T* iterator;
	typedef const T* const_iterator;

	Vector()
		:_start(nullptr)
		, _finsh(nullptr)
		, _eos(nullptr)
	{
		//Vector函数地构造函数
	}
	
	void Pushback(const T& val) {
		if (_finsh == _eos) {
			size_t newC = _start == nullptr ? 1 : 2 * Capacity();
			Reserve(newC);
		}
		
		*_finsh = val;
		++_finsh;
	}

	//Reserve是一个重新设置容量大小的函数， 只增不减！ 
	void Reserve(size_t newC) {
		int sz = Size();

		if (newC > Capacity()) {

			T* tmp = new T[newC];

			for (int i = 0; i < sz; ++i) {
				tmp[i] = _start[i];
			}

			delete[] _start;

			_start = tmp;
			_finsh = _start + sz;
			_eos = _start + newC;
		}
	}
	
	T& operator[](size_t pos) {
		if (pos < Size()) {
			return _start[pos];		
		}
		
	}

	const T& operator[](size_t pos) const {
		if (pos < Size()) {
			return _start[pos];
		}
	}

	iterator begin() {
		return _start;
	}

	iterator end() {
		return _finsh;
	}

	const_iterator begin()  const {
		return _start;
	}

	const_iterator end() const {
		return _finsh;
	}

	void Insert(iterator pos, const T& val) {
		//增容会是迭代器失效, 因此重新开辟空间会存在空间迁移， 这样之前迭代器指向的地方就可能丢失！
		
		//我必须要加上断言！
		assert(pos >= begin() && pos <= end());

		int len = pos - _start;

		if (_finsh == _eos) {
			size_t newC = _start == nullptr ? 1 : 2 * Capacity();
			Reserve(newC);
		}

		//更新pos迭代器位置 - 可能重新开辟的空间会导致迭代器失效， 以你需要更新迭代器pos！
		pos = _start + len;

		for (iterator i = _finsh; i > pos; --i) {
			*i = *(i - 1);
		}

		*pos = val;
		++_finsh;
	}

	//任意位置删除函数
	iterator Earse(iterator pos) {
		
		assert(pos >= begin() && pos <= end());
		
		for (iterator i = pos; i < end() - 1; ++i) {
			(*i) = *(i + 1);
		}

		--_finsh;
		return pos;
	}

	//拷贝构造函数 - 是重新开辟的， 这是完全新的！
	Vector(const Vector<T>& vec)
		:_start(new T[vec.Capacity()])
		, _finsh(_start + vec.Size())
		, _eos(_start + vec.Capacity())
	{
		//数据拷贝过来！
		for (int i = 0; i < vec.Size(); ++i) {
			_start[i] = vec._start[i];
		}
	}

	template<class Inputiterator>
	Vector(Inputiterator first, Inputiterator end) {
		while (first != end) {
			Pushback(*first);
			++first;
		}
	}

	// 赋值拷贝函数
	//Vector<T>& operator=(const Vector<T>& vec)
	//{
	//	if (_start) {
	//		//如果开辟好了， 则删除
	//		delete[] _start;
	//	}

	//	_start = new T[vec.Capacity()];
	//	_finsh = _start + vec.Size();
	//	_eos = _start + vec.Capacity();
	//	
	//for (int i = 0; i < vec.size(); ++i) {
	//	_start[i] = vec._start[i];
	//}

	//	return *this;
	//}

	//现代写法
	Vector<T>& operator=(Vector<T> vec)
	{
		Swap(vec);
		return *this;
	}

	void Swap(Vector<T>& vec) {
		swap(_start, vec._start);
		swap(_finsh, vec._finsh);
		swap(_eos, vec._eos);
	}

	~Vector() {
		if (_start) {
			//vector是顺序开辟的， 因此这样删除是可以的！
			delete[] _start;
			_start = _finsh = _eos = nullptr;
		}
	}

	size_t Size() const {
		return _finsh - _start;
	}

	size_t Capacity() const {
		return _eos - _start;
	}

private:
	//我们设置3个指针来这样做， 这样更加方便地完成任务！
	iterator _start;
	iterator _finsh;
	iterator _eos;
};
