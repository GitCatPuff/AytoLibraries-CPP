#pragma once

#include <exception>
#include <cwchar>
#include <memory>
#include "ayto/common/typedefs.hpp"

#define NS_BEGIN namespace ayto { namespace v10 { namespace exceptions {
#define NS_END } } }


NS_BEGIN
using namespace ayto::typedefs;

class exception_base
{
protected:
	std::unique_ptr<byte[]> data;

protected:
	exception_base() = default;

public:

	template <typename DataT>
	exception_base(const DataT* const in_data, long long data_size)
		:
		data(std::make_unique<byte[]>(data_size))
	{
		std::memcpy(data.get(), in_data, data_size);
	}

	template <typename CharT, std::size_t N>
	exception_base(const CharT(& in_data)[N])
		:
		data(std::make_unique<byte[]>(N))
	{
		auto size = N;
		std::memcpy(data.get(), &in_data, N);
	}

public:
	template <typename DataType = const char *>
	DataType what() const
	{
		return reinterpret_cast<DataType>(data.get());
	}
};

namespace file
{
	class not_found : public exception_base
	{
	public:
		template <typename DataT>
		not_found(const DataT* const in_data, long long data_size)
			:
			exception_base(in_data, data_size)
		{}

		template <typename CharT, std::size_t N>
		not_found(const CharT(&in_data)[N])
			:
			exception_base(in_data)
		{}
	};

}

class setup_error : public exception_base
{
};


NS_END

#undef NS_BEGIN
#undef NS_END

