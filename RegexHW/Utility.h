#pragma once

#include "CoreDefinitions.h"
#include <windows.h>
#include <chrono>
#include <string>
#include <iostream>

/**
 * Lives throughout the lifetime of the app
 * Helps with printing stuff
 */
class ConsoleManager 
{
public:
	ConsoleManager();
	~ConsoleManager();

	/**
	 * Handles all console output
	 */
	void PrintConsoleMessage(MESSAGE_VERBOSITY verbosity, MESSAGE_DOMAIN domain, std::string message);

private: // functions
	// allocate mem
	void Reserve();
	// deallocate mem
	void Free();

	char* GetIRLTime();

private: // variables
	char* realTime = nullptr;
}; 


std::string DataTypeToString(DATA_CONFIG_TYPE type);