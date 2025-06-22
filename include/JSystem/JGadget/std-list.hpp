#ifndef JGADGET_LIST_HPP
#define JGADGET_LIST_HPP

#include <JSystem/JGadget/allocator.hpp>
#include <JSystem/JKernel/JKRHeap.hpp>
#include <types.h>

namespace JGadget {

template <class T, class A = TAllocator<T> > class TList {
public:
	struct TNode_ {
		TNode_* mNext;
		TNode_* mPrev;
	};

	TNode_* CreateNode_(TNode_* next, TNode_* prev, const T& value)
	{
		TNode_* ret
		    = (TNode_*)mAllocator.AllocateRaw(sizeof(TNode_) + sizeof(T));

		if (!ret)
			return nullptr;

		ret->mNext = next;
		ret->mPrev = prev;

		T* casted = (T*)(ret + 1);
		mAllocator.construct(casted, value);

		return ret;
	}

	void DestroyNode_(TNode_* node)
	{
		mAllocator.destroy((T*)(node + 1));
		mAllocator.DeallocateRaw(node);
	}

public:
	class iterator;

	TList(const TAllocator<T>& alloc = TAllocator<T>())
	    : mAllocator(alloc)
	    , mSize(0)
	{
		mSentinel.mNext = &mSentinel;
		mSentinel.mPrev = &mSentinel;
	}

	~TList() { clear(); }

	iterator insert(iterator where, const T& what)
	{
		TNode_* prev    = where.mNode->mPrev;
		TNode_* newNode = CreateNode_(where.mNode, prev, what);

		if (!newNode) {
			return iterator(&mSentinel);
		} else {
			where.mNode->mPrev = newNode;
			prev->mNext        = newNode;
			++mSize;
			return iterator(newNode);
		}
	}

	iterator erase(iterator what)
	{
		TNode_* const next       = what.mNode->mNext;
		TNode_* const curr       = what.mNode;
		what.mNode->mPrev->mNext = next;
		next->mPrev              = curr->mPrev;
		DestroyNode_(curr);
		--mSize;
		return iterator(next);
	}

	iterator erase(iterator start, iterator end)
	{
		while (start != end) {
			start = erase(start);
		}
		return start;
	}

	void clear() { erase(begin(), end()); }

	iterator begin() { return mSentinel.mNext; }
	iterator end() { return &mSentinel; }

	void push_front(const T& what) { insert(begin(), what); }
	void push_back(const T& what) { insert(end(), what); }

	// TODO: delete me when insert is correct
	void push_back2(const T& what) { push_back(what); }

	class iterator {
		friend class TList;

	public:
		iterator()
		    : mNode(nullptr)
		{
		}

		iterator(TNode_* node)
		    : mNode(node)
		{
		}

		T& operator*() { return *(T*)(mNode + 1); }
		T* operator->() { return (T*)(mNode + 1); }

		iterator operator++()
		{
			mNode = mNode->mNext;
			return *this;
		}

		iterator operator++(int)
		{
			iterator copy = *this;
			mNode         = mNode->mNext;
			return copy;
		}

		friend bool operator==(iterator fst, iterator snd)
		{
			return fst.mNode == snd.mNode;
		}

		friend bool operator!=(iterator fst, iterator snd)
		{
			return !(fst == snd);
		}

	protected:
		TNode_* mNode;
	};

private:
	/* 0x0 */ TAllocator<T> mAllocator;
	/* 0x4 */ u32 mSize;
	/* 0x8 */ TNode_ mSentinel;
};

class TList_pointer_void : public TList<void*> {
	typedef TList<void*> Base;
	typedef void* value_type;

public:
	TList_pointer_void(
	    const TAllocator<value_type>& = TAllocator<value_type>());
	TList_pointer_void(u32, const value_type&,
	                   const JGadget::TAllocator<value_type>&);
	~TList_pointer_void();

	iterator insert1234(iterator a, value_type const& b)
	{
		return Base::insert(a, b);
	}

	iterator insert(iterator, value_type const&);
	iterator insert(iterator, u32, value_type const&);
	iterator erase(iterator);
	iterator erase(iterator, iterator);
	void remove(const value_type&);
	void assign(u32, const value_type&);
	void resize(u32, const value_type&);
	void unique();
	void operator=(const JGadget::TList_pointer_void&);
};

template <class T> class TList_pointer : TList_pointer_void {
	typedef TList_pointer_void Base;

public:
	class iterator : Base::iterator {
		friend class TList_pointer;
		typedef Base::iterator Base;

	public:
		iterator() { }

		iterator(Base it)
		    : Base(it)
		{
		}

		T operator*() { return (T)this->Base::operator*(); }
		T operator->() { return (T)this->Base::operator->(); }

		iterator operator++() { return iterator(this->Base::operator++()); }
		iterator operator++(int) { return iterator(this->Base::operator++(0)); }

		friend bool operator==(iterator fst, iterator snd)
		{
			return fst.mNode == snd.mNode;
		}

		friend bool operator!=(iterator fst, iterator snd)
		{
			return !(fst == snd);
		}
	};

	iterator begin() { return iterator(Base::begin()); }
	iterator end() { return iterator(Base::end()); }

	iterator insert(iterator where, const T& what)
	{
		return Base::insert(where, what);
	}

	void push_front(const T& what) { insert(begin(), what); }
	void push_back(const T& what) { insert(end(), what); }
};

}; // namespace JGadget

#endif
