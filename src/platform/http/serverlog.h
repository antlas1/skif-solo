#pragma once
#include <string>
#include <ctime>
#include <cstdlib>
#include <iostream>
//todo: ifdef win32
#include <Windows.h>

/** 
 * Contains possible colors to print into the console.
 */
enum class ConsoleColor
{
	BLUE = 1,
	GREEN = 2,
	CYAN = 3,
	RED = 4,
	PINK = 5,
	YELLOW = 6,
	WHITE = 7,
	GREY = 8,
	BRIGHT_BLUE = 9,
	BRIGHT_GREEN = 10,
	BRIGHT_CYAN = 11,
	BRIGHT_RED = 12,
	BRIGHT_PINK = 13,
	BRIGHT_YELLOW = 14,
	BRIGHT_WHITE = 15
};

/** 
 * Set the color in which the console will print.
 */
void setConsoleColor(int color = (int)ConsoleColor::WHITE);

/** 
 * Utility class to log efficiently in the console.
 */
class Log
{
public:

	/**
	 * \public
	 * Creates a log of the form "DATE | first : second".
	 * \param first First string.
	 * \param second Second string.
	 * \param firstColor Color of the first string.
	 * \param secondColor Color of the second string.
	 * \note If you don't intend to store your logs, you can chain the constructor with print() method.
	 */
	Log(std::string first, std::string second, int firstColor = (int)ConsoleColor::WHITE, int secondColor = (int)ConsoleColor::WHITE)
		: first(first), second(second), firstColor(firstColor), secondColor(secondColor)
	{
		time_t t = time(0);
		localtime_s(&date, &t);
	}

	/** 
	 * \public
	 * The color of the first string.
	 */
	int firstColor;

	/** 
	 * \public
	 * The color of the second string. 
	 */
	int secondColor;

	/** 
	 * \public
	 * The first string.
	 */
	std::string first;

	/** 
	 * \public
	 * The second string.
	 */
	std::string second;

	/** 
	 * \public
	 * The date of the log construction.
	 */
	tm date;

	/** 
	 * \public
	 * Prints the formatted log in the console.
	 */
	void print()
	{
		std::cout << "(" << (date.tm_mday < 10 ? "0" : "") << date.tm_mday << "/" << (date.tm_mon < 10 ? "0" : "") << date.tm_mon + 1 << "/" << 1900 + date.tm_year <<
			" - " << (date.tm_hour < 10 ? "0" : "") << date.tm_hour << ":" << (date.tm_min < 10 ? "0" : "") << date.tm_min <<
			":" << (date.tm_sec < 10 ? "0" : "") << date.tm_sec << ") | ";
		setConsoleColor(firstColor);
		std::cout << first << " : ";
		setConsoleColor(secondColor);
		std::cout << second << std::endl;
		setConsoleColor();
	}
};

