#ifndef AYTO_UTILITY_HPP
#define AYTO_UTILITY_HPP

#ifdef AYTO_ENABLE_PRINT_HELPERS
#include <iostream>
#include <vector>

// TODO: We can concatenate vector printers, using std::conditional_t and std::add_reference
//

// Use copying for fundamental types
template <typename T>
requires std::is_fundamental_v<T>
std::ostream& operator << (std::ostream &ostream, const std::vector<T>& vec)
{
	ostream << "v[ ";
	std::copy(vec.begin(), vec.end(), std::ostream_iterator<T>(ostream, " "));
	return ostream << "]";
}


// Use references for big types to avoid copying
template <typename T>
std::ostream& operator << (std::ostream& ostream, const std::vector<T>& vec)
{
	ostream << "v[ ";
	for (const auto& element : vec)
	{
		ostream << element << ' ';
	}
	return ostream << "]";
}



#endif


#include <type_traits>
#include <string>

#define NS_BEGIN namespace ayto { inline namespace v10 { namespace utility {
#define NS_END } } }


NS_BEGIN

template <typename T, ::std::size_t N>
consteval
inline
::std::size_t length( const T(&text)[N] )
{
	return N - 1;
}

template <class Str>
[[nodiscard]]
inline
::std::size_t length(Str&& str)
{
	return str.length();
}

template <typename T>
[[nodiscard]]
inline
long long length(const T* text_ptr)
{
	if constexpr (std::is_same_v<T, char>)
		return ::std::strlen(text_ptr);
	else
		return ::std::wcslen(text_ptr);
}


template <typename CharT>
inline
const CharT* get_c_string(const std::basic_string<CharT> &string)
{
	return string.c_str();
}

template <typename CharT>
constexpr
inline
const CharT* get_c_string(const CharT* cstring)
{
	return cstring;
}

template <typename CharT, ::std::size_t N>
consteval
inline
auto get_c_string(const CharT (&text_array)[N])
{
	return text_array;
}

// TODO: Do bitwise operations to make this function faster
inline 
constexpr
int boolsign(bool value)
{
	return value ? 1 : -1;
}


// Exceptions On File Operations
template <class FileType>
class file_exception_guard
{
public:

	file_exception_guard(FileType& file)
		:
		file(file)
	{}

	~file_exception_guard()
	{
		if (file.is_open())
			file.close();
	}

private:
	FileType& file;
};

NS_END


NS_BEGIN

class ascii
{
public:
	ascii() = delete;

public:

	[[nodiscard]]
	static 
	inline bool is_upper(char c)
	{
		return c >= 'A' && c <= 'Z';
	}

	[[nodiscard]]
	static
	inline bool is_lower(char c)
	{
		return c >= 'a' && c <= 'z';
	}

	[[nodiscard]]
	static
	inline bool is_number(char c)
	{
		return c >= '0' && c <= '9';
	}

	[[nodiscard]]
	static
	inline bool is_letter(char c)
	{
		return is_upper(c) || is_lower(c);
	}
};

NS_END

#undef NS_BEGIN
#undef NS_END


#endif