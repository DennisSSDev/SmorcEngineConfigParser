#include "Utility.h"

ConsoleManager::ConsoleManager()
{
	Reserve();
}

ConsoleManager::~ConsoleManager()
{
	Free();
}

void ConsoleManager::PrintConsoleMessage(MESSAGE_VERBOSITY verbosity, MESSAGE_DOMAIN domain, std::string message)
{
	auto localHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	std::string textForVerbosity;
	switch (verbosity)
	{
	case MESSAGE_VERBOSITY::DEFAULT_MESSAGE:
		SetConsoleTextAttribute(localHandle, CONSOLE_COLOR_DEFAULT);
		textForVerbosity = "DEFAULT";
		break;
	case MESSAGE_VERBOSITY::WARNING:
		SetConsoleTextAttribute(localHandle, CONSOLE_COLOR_WARNING);
		textForVerbosity = "WARNING";
		break;
	case MESSAGE_VERBOSITY::SUCCESS:
		SetConsoleTextAttribute(localHandle, CONSOLE_COLOR_SUCCESS);
		textForVerbosity = "SUCCESS";
		break;
	case MESSAGE_VERBOSITY::CRITICAL_ERROR:
		SetConsoleTextAttribute(localHandle, CONSOLE_COLOR_ERROR);
		textForVerbosity = "ERROR";
		break;
	}

	std::string textForDomain;
	switch (domain)
	{
	case MESSAGE_DOMAIN::DEFAULT:
		textForDomain = "Default";
		break;
	case MESSAGE_DOMAIN::COMMENT:
		textForDomain = "Comment";
		break;
	case MESSAGE_DOMAIN::FILE_STATUS:
		textForDomain = "FileStatus";
		break;
	case MESSAGE_DOMAIN::STRING:
		textForDomain = "String";
		break;
	case MESSAGE_DOMAIN::TYPE:
		textForDomain = "Type";
		break;
	}
	GetIRLTime();
	std::cout << "[" << realTime << "] " << textForVerbosity << "::" << textForDomain << "::Message: " << message << std::endl;
}

void ConsoleManager::Reserve()
{
	realTime = new char[25];
}

void ConsoleManager::Free()
{
	delete[] realTime;
}

char* ConsoleManager::GetIRLTime()
{
	time_t result = time(NULL);
	char time_str[26];
	ctime_s(time_str, sizeof(char) * 26, &result);
	size_t i = 0;
	while (i < 25)
	{
		realTime[i] = time_str[i];
		++i;
	}
	realTime[24] = '\0';
	return realTime;
}

std::string DataTypeToString(DATA_CONFIG_TYPE type)
{
	switch (type)
	{
	case DATA_CONFIG_TYPE::BOOLEAN_T:
		return "boolean";
	case DATA_CONFIG_TYPE::FLOAT_T:
		return"float";
	case DATA_CONFIG_TYPE::INT_T:
		return "integer";
	case DATA_CONFIG_TYPE::STRING_T:
		return "string";
	default:
		return "nil";
	}
}
