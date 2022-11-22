#pragma once

#include "../../common/ayto_detail_define_macros.hpp"

#include "../../utility.hpp"

#define CONSOLE_NAMESPACE namespace AYTO_PLATFORM { namespace console {
#define CONSOLE_NS_BEGIN namespace AYTO_ROOT_NAMESPACE { AYTO_INLINE_NAMESPACE AYTO_CURRENT_ABI { CONSOLE_NAMESPACE
#define CONSOLE_NS_END } } } }

#include <type_traits>
#include <string>
#include <stdexcept>

#ifdef _WIN32
#include <Windows.h>

CONSOLE_NS_BEGIN

using size_t = SHORT;

enum class color : WORD
{
	black = 0,
	dark_blue = FOREGROUND_BLUE,
	dark_green = FOREGROUND_GREEN,
	dark_cyan = FOREGROUND_GREEN | FOREGROUND_BLUE,
	dark_red = FOREGROUND_RED,
	dark_magenta = FOREGROUND_RED | FOREGROUND_BLUE,
	dark_yellow = FOREGROUND_RED | FOREGROUND_GREEN,
	dark_gray = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	gray = FOREGROUND_INTENSITY,
	blue = FOREGROUND_INTENSITY | FOREGROUND_BLUE,
	green = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
	cyan = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
	red = FOREGROUND_INTENSITY | FOREGROUND_RED,
	magenta = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
	yellow = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
	white = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
};

HANDLE get_output_handle()
{
	auto output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (output_handle == INVALID_HANDLE_VALUE)
	{
		// Can't get output handle
		throw std::runtime_error("Can't get standard output handle");
	}
	else if (output_handle == NULL)
	{
		throw std::runtime_error("There is no default standard output handle");
	}

	return output_handle;
}

SHORT get_console_line_width()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo(
		get_output_handle(),
		&csbi
	);

	return csbi.srWindow.Right - csbi.srWindow.Left + 1;
}

SHORT get_console_height()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo(
		get_output_handle(),
		&csbi
	);

	return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}


struct coord
{
	size_t x;
	size_t y;

	coord() = default;

	coord(COORD crd)
	{
		this->x = crd.X;
		this->y = crd.Y;
	}

	coord(SHORT X, SHORT Y)
	{
		this->x = X;
		this->y = Y;
	}

	operator COORD()
	{
		return COORD{ x, y };
	}

	DWORD operator-(const coord& other) const
	{
		const auto width = get_console_line_width();
		return (x - other.x) + (y - other.y) * width;
	}
};


coord get_cursor_coord()
{
	auto output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbInfo;
	if (
		GetConsoleScreenBufferInfo(
			output_handle,
			&csbInfo
		)
		== ERROR)
	{
		throw std::runtime_error("Can't get current cursor position");
	}

	return csbInfo.dwCursorPosition;
}

void set_range_color(color new_color, coord starting_coord, DWORD length )
{
	auto output_handle = get_output_handle();
	
	DWORD number_of_written = 0;
	FillConsoleOutputAttribute(
		output_handle,
		static_cast<::std::underlying_type_t<color>>(new_color),
		length, //csbInfo.dwCursorPosition,
		starting_coord,
		&number_of_written
	);
}

void set_text_color(color new_color)
{
	set_range_color(new_color, coord{}, 1000);
}


// WARNING: Issue of new line breaking fixed, but still experimental.
// Needs further experimentation.
template <typename T>
void print_colored(const T&& text, color text_color)
{
	const auto output_handle = get_output_handle();
	const auto text_length = utility::length(text);

	const coord starting_coord{ get_cursor_coord() };

	DWORD number_of_written = 0;
	
	if constexpr (std::is_same_v < std::decay_t<T>, const wchar_t *> )
	{
		WriteConsoleW(output_handle, utility::get_c_string(text), text_length, &number_of_written, NULL);
	}
	else
	{
		WriteConsoleA(output_handle, utility::get_c_string(text), text_length, &number_of_written, NULL);
	}

	set_range_color(text_color, starting_coord, get_cursor_coord() - starting_coord);
}

void print_colored(const ::std::string& text, color text_color)
{
	auto output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	const auto text_length = text.length();

	coord starting_coord(get_cursor_coord());

	DWORD number_of_written = 0;
	WriteConsoleA(output_handle, text.c_str(), text_length, &number_of_written, NULL);

	// coord end_coord(get_cursor_coord());

	set_range_color(text_color, starting_coord, text_length);

}


// TODO: Fix this overload
template <class T>
void set_console_font(const ::std::basic_string<T>& font_name)
{
	const auto output_handle = get_output_handle();

	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = 0;
	cfi.dwFontSize.Y = 20;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;

	if constexpr (::std::is_same_v<T, wchar_t>)
		::std::wcscpy(cfi.FaceName, font_name.c_str());
	else
		::std::strcpy(cfi.FaceName, font_name.c_str());

	SetCurrentConsoleFontEx(output_handle, FALSE, &cfi);
}

template <typename CharT, ::std::size_t N>
void set_console_font(const CharT(&font_name)[N])
{
	static_assert(N <= 32, "Font Name length can be 32 at maximum");

	const auto output_handle = get_output_handle();

	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = 0;
	cfi.dwFontSize.Y = 18;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;

	if constexpr (::std::is_same_v<CharT, wchar_t>)
	{
		wcsncpy_s(cfi.FaceName, font_name, N);
	}
	else
	{
		std::size_t return_value;
		mbstowcs_s(&return_value, cfi.FaceName, font_name, N);
	}

	SetCurrentConsoleFontEx(output_handle, FALSE, &cfi);
}

CONSOLE_NS_END
#endif

#undef CONSOLE_NAMESPACE
#undef CONSOLE_NS_BEGIN
#undef CONSOLE_NS_END
#include "../../common/ayto_detail_undefine_macros.hpp"

