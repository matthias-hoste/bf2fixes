#pragma once
#include <vector>         // For std::vector
#include <string>         // For std::wstring and std::wstring::npos
#include <sstream>        // For std::wstringstream
#include <cstddef>        // For std::size_t

std::vector<std::wstring> splitManyW(const std::wstring& original, const std::wstring& delimiters)
{
	std::wstringstream stream(original);
	std::wstring line;
	std::vector<std::wstring> wordVector;

	while (std::getline(stream, line))
	{
		std::size_t prev = 0, pos;
		while ((pos = line.find_first_of(delimiters, prev)) != std::wstring::npos)
		{
			if (pos > prev)
				wordVector.emplace_back(line.substr(prev, pos - prev));

			prev = pos + 1;
		}

		if (prev < line.length())
			wordVector.emplace_back(line.substr(prev, std::wstring::npos));
	}

	return wordVector;
}

template<typename Element, class Traits, class Allocator>
class bf2_basic_string
{
private:
	void* myProxy;

	enum
	{
		BUF_SIZE = 16 / sizeof(Element) < 1 ? 1 : 16 / sizeof(Element)
	};

	enum
	{
		ALLOC_MASK = sizeof(Element) <= 1 ? 15 : sizeof(Element) <= 2 ? 7 : sizeof(Element) <= 4 ? 3 : sizeof(Element) <= 8 ? 1 : 0
	};

	union BufferContainer
	{
		Element buffer[BUF_SIZE];
		Element* ptr;
		char alias[BUF_SIZE];
	} bufferContainer;

	typedef typename Allocator::template rebind<Element>::other alty;
	typedef typename alty::size_type size_type;
	typedef std::basic_string<Element, Traits, Allocator> myType;

	size_type mySize;
	size_type myRes;
	alty allocatorInstance;

	void EndOfString(size_type newSize)
	{
		Traits::assign(MyPtr()[this->mySize = newSize], Element());
	}

	Element* MyPtr()
	{
		return (this->BUF_SIZE <= this->myRes ? this->bufferContainer.ptr : this->bufferContainer.buffer);
	}

	void assign(const Element* ptr, size_type count)
	{
		if (Grow(count))
		{
			Traits::copy(MyPtr(), ptr, count);
			EndOfString(count);
		}
	}

	bool Grow(size_type newSize)
	{
		if (this->myRes < newSize)
			Copy(newSize, this->mySize);
		return (0 < newSize);
	}

	void Copy(size_type newSize, size_type oldLength)
	{
		size_type newRes = newSize | this->ALLOC_MASK;

		Element* ptr;
		try
		{
			ptr = this->allocatorInstance.allocate(newRes + 1);
		}
		catch (...)
		{
			newRes = newSize;
			try
			{
				ptr = this->allocatorInstance.allocate(newRes + 1);
			}
			catch (...)
			{
				throw;
			}
		}

		this->bufferContainer.ptr = ptr;
		this->myRes = newRes;
		EndOfString(oldLength);
	}

public:
	bf2_basic_string()
	{

	}

	bf2_basic_string(const Element* ptr)
	{
		this->myRes = this->BUF_SIZE - 1;
		EndOfString(0);
		assign(ptr, Traits::length(ptr));
	}

	// FIX START ------------------------------------
	~bf2_basic_string()
	{
		Tidy();
	}

	void Tidy(bool _Built = true, size_type _Newsize = 0)
	{	// initialize buffer, deallocating any storage
		if (!_Built)
			;
		else if (this->BUF_SIZE <= this->myRes)
		{	// copy any leftovers to small buffer and deallocate
			Element* _Ptr = MyPtr();
			if (0 < _Newsize)
				Traits::copy(MyPtr(), _Ptr, _Newsize);
			this->allocatorInstance.deallocate(_Ptr, this->myRes + 1);
		}
		this->myRes = this->BUF_SIZE - 1;
		EndOfString(_Newsize);
	}
	// FIX END ------------------------------------

	size_type size()
	{
		return mySize;
	}

	const Element* c_str() const
	{
		return (this->BUF_SIZE <= this->myRes ? this->bufferContainer.ptr : this->bufferContainer.buffer);
	}

	const myType str() const
	{
		return myType(c_str());
	}
};
typedef bf2_basic_string<char, std::char_traits<char>, std::allocator<char> > bf2string;