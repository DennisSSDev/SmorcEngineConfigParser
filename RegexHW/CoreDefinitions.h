#pragma once
#include <string>

typedef unsigned char uint8;

#define CONSOLE_COLOR_DEFAULT 7
#define CONSOLE_COLOR_SUCCESS (FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define CONSOLE_COLOR_ERROR (FOREGROUND_RED | FOREGROUND_INTENSITY)
#define CONSOLE_COLOR_WARNING (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY)

enum class MESSAGE_DOMAIN: uint8
{
	DEFAULT = 0x01,
	TYPE = 0x02,
	COMMENT = 0x04,
	STRING = 0x08,
	FILE_STATUS = 0x10
};

enum class DATA_TYPE: uint8
{
	SECTION = 0x01,
	SUBSECTION = 0x02,
	PAIR = 0x04,
	NONE = 0x08
};

enum class DATA_CONFIG_TYPE : uint8 
{
	DEFAULT_T = 0x01,
	BOOLEAN_T = 0x02,
	INT_T = 0x04,
	FLOAT_T = 0x08,
	STRING_T = 0x10
};

#include <string>

enum class MESSAGE_VERBOSITY : uint8
{
	DEFAULT_MESSAGE = 0x01,
	WARNING = 0x02,
	CRITICAL_ERROR = 0x04,
	SUCCESS = 0x08
};

struct DB_Data
{
	std::string key;
	std::string value;
	DATA_CONFIG_TYPE dbType;
	bool isArray;

	bool operator==(const DB_Data& p) const
	{
		return key == p.key;
	}
};