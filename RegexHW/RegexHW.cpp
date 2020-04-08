// RegexHW.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <fstream>
#include <regex>
#include <chrono>
#include <windows.h>
#include <sstream>
#include <algorithm>

#include "CoreDefinitions.h"
#include "Utility.h"
#include "DataBase.h"

/**
 * STRUCTURES
 * 
 * section to subsection hashtable
 * section(subsection) to key=value pair hashtable
 *
 *
 * store key=value pairs together until look up asks for separation of data
 */

bool TestSectionValidity(std::string setion, std::string subsection, ConsoleManager* consoleManager, size_t lineCount) 
{
	if (setion == "" && subsection == "")
	{
		consoleManager->PrintConsoleMessage
		(
			MESSAGE_VERBOSITY::CRITICAL_ERROR,
			MESSAGE_DOMAIN::STRING,
			"Invalid section on line: " + std::to_string(lineCount) + ". Reason: There hasn't been a valid section or subsection defined yet. Please define a section or subsection first before creating a key=value pair"
		);
		return true;
	}
	return false;
}


DATA_CONFIG_TYPE RetrieveDataTypeFromString(std::string data, /*OUT*/ MESSAGE_VERBOSITY& message) 
{
	std::smatch matches;

	// check if this is a string
	if (std::regex_search(data, matches, std::regex("\".*?\"$")))
	{
		// this is a string
		message = MESSAGE_VERBOSITY::SUCCESS;
		return DATA_CONFIG_TYPE::STRING_T;
	}
	// check if it's a bool true or false case insensitive
	if (std::regex_search(data, matches, std::regex("(true$|false$)", std::regex_constants::icase)))
	{
		// this is a bool
		message = MESSAGE_VERBOSITY::SUCCESS;
		return DATA_CONFIG_TYPE::BOOLEAN_T;
	}
	// check if this a floating point number
	// I explicitly want to make sure that the first digit is not a leading zero
	else if (std::regex_search(data, matches, std::regex("^[-+]?[0-9]*\\.(([0-9]+f)|f)$")))
	{
		// this is a float
		message = MESSAGE_VERBOSITY::SUCCESS;
		return DATA_CONFIG_TYPE::FLOAT_T;
	}
	// check if this is a int
	// the leading number cannot be 0
	else if (std::regex_search(data, matches, std::regex("^[1-9]+[0-9]*$")))
	{
		// this is an int
		message = MESSAGE_VERBOSITY::SUCCESS;
		return DATA_CONFIG_TYPE::INT_T;
	}
	message = MESSAGE_VERBOSITY::CRITICAL_ERROR;
	return DATA_CONFIG_TYPE::DEFAULT_T;
}

int main()
{
    std::ifstream file; // the file to be read
    std::string line; // the string to apply regex expressions on

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // open the engine config file
    file.open("DefaultEngine.config");
    
	ConsoleManager* consoleManager = new ConsoleManager();
	DataBase* db = new DataBase();

    if(file.bad()) 
    {
		consoleManager->PrintConsoleMessage
		(
			MESSAGE_VERBOSITY::CRITICAL_ERROR, 
			MESSAGE_DOMAIN::FILE_STATUS, 
			"The specified file doesn't exist"
		);
		return 0;
    }

    // detect how large is the file and if it's empty - don't do anything
    file.seekg(0, std::ios::end);
    int length = (int)file.tellg();
    if(length <= 0) 
    {
        consoleManager->PrintConsoleMessage
		(
			MESSAGE_VERBOSITY::WARNING, 
			MESSAGE_DOMAIN::FILE_STATUS, 
			"The given file has no data"
		);
		SetConsoleTextAttribute(hConsole, CONSOLE_COLOR_DEFAULT);
        return 0;
    }
    // return back to the front
    file.seekg(0, std::ios::beg); 
    SetConsoleTextAttribute(hConsole, CONSOLE_COLOR_DEFAULT);
    if(file.is_open())
    {
        // Read a single line at a time and print it
        std::string lastSection = "";
        std::string lastSubsection = "";
        DATA_TYPE lastKnownDataType = DATA_TYPE::NONE;
        size_t lineCount = 0;
        while(getline(file, line))
        {
            try
            {
				lineCount++;
				std::smatch matches;
				std::string modifiedLine;
				// get rid of anything after the #
				if (std::regex_search(line, matches, std::regex(".*(?=#.*$)")))
				{
					std::string formattedLine = matches.str(0);
					if (formattedLine.length() == 0)
					{
						// the entire line is a comment, move on
						continue;
					}
					else if (std::all_of(formattedLine.begin(), formattedLine.end(),isspace))
					{
						// this entire line is empty
						continue;
					}
					modifiedLine = matches.str(0);
				}
				else 
				{
					// the regex failed meaning there are no # symbols
					if (std::all_of(line.begin(), line.end(), isspace))
					{
						// this entire line is empty
						continue;
					}
					modifiedLine = line;
				}

				// check if this line has "[" and "]" with the existing match
				if (std::regex_search(modifiedLine, matches, std::regex("\\[.*?\\]")))
				{
					std::string potentialSection = matches.str(0);
					// check for white space
					if (std::regex_search(potentialSection, matches, std::regex("[\\s]")))
					{
						consoleManager->PrintConsoleMessage
						(
							MESSAGE_VERBOSITY::CRITICAL_ERROR,
							MESSAGE_DOMAIN::STRING,
							"Invalid section on line: " + std::to_string(lineCount) + ". Reason: no whitespace in section definition is allowed"
						);
						continue;
					}

					// check if it's an empty section: []
					if (std::regex_search(potentialSection, matches, std::regex("(\\[\\])")))
					{
						consoleManager->PrintConsoleMessage
						(
							MESSAGE_VERBOSITY::CRITICAL_ERROR,
							MESSAGE_DOMAIN::STRING,
							"Invalid section on line: " + std::to_string(lineCount) + ". Reason: empty sections are not allowed"
						);
						continue;
					}

					// check if it's a subsection (:)
					if(std::regex_search(potentialSection, matches, std::regex(":"))) 
					{
						std::string section = matches.prefix().str();
						section = section.substr(1, section.length());
						std::string subsection = matches.suffix();
						subsection = subsection.substr(0, subsection.length()-1);
						if(subsection == "" || section == "") 
						{
							consoleManager->PrintConsoleMessage
							(
								MESSAGE_VERBOSITY::CRITICAL_ERROR,
								MESSAGE_DOMAIN::STRING,
								"Invalid section on line: " + std::to_string(lineCount) + ". Reason: the subsection or section is empty"
							);
							continue;
						}
						// validate that the subsection is under the correct last section to preserve file order
						if(lastSection == "") 
						{
							consoleManager->PrintConsoleMessage
							(
								MESSAGE_VERBOSITY::CRITICAL_ERROR,
								MESSAGE_DOMAIN::STRING,
								"Invalid section on line: " + std::to_string(lineCount) + ". Reason: There hasn't been a section defined yet to use subsections. Please define a section first"
							);
							continue;
						}
						else if(section != lastSection) 
						{
							consoleManager->PrintConsoleMessage
							(
								MESSAGE_VERBOSITY::CRITICAL_ERROR,
								MESSAGE_DOMAIN::STRING,
								"Invalid section on line: " + std::to_string(lineCount) + ". Reason: the section you specified for the subsection is out of order or doesn't match. Please reorganize"
							);
							continue;
						}
						lastSubsection = subsection;
						db->AddSubSection(subsection, section);
						continue;
					}
					if(std::regex_search(modifiedLine, matches, std::regex("[^\\[\\]\\w\\:]")))
					{
						consoleManager->PrintConsoleMessage
						(
							MESSAGE_VERBOSITY::CRITICAL_ERROR,
							MESSAGE_DOMAIN::STRING,
							"Invalid section on line: " + std::to_string(lineCount) + ". Reason: special characters are not allowed inside sections or subsections"
						);
						continue;
					}
					std::string section = potentialSection;
					section = section.substr(1, section.length()-2);
					lastSubsection = ""; // since we started a new section the subsection must be cleared
					lastSection = section;
					db->AddSection(section);
					continue;
				}

				// Check the original line for a section.
				if (std::regex_search(line, matches, std::regex("\\[.*?\\]")))
				{
					// There is a # in between [ ] -> error
					consoleManager->PrintConsoleMessage
					(
						MESSAGE_VERBOSITY::CRITICAL_ERROR,
						MESSAGE_DOMAIN::STRING,
						"Invalid section on line: " + std::to_string(lineCount) + ". Reason: remove all # signs in the definition of a section"
					);
					continue;
				}
				
				///////////////////////
				// we now know it's not a section/subsection at all or is completely invalid to begin with.
				///////////////////////

				// we can now remove all whitspaces as for sections and subsections we must test for it as it's explicitly unallowed
				modifiedLine.erase(std::remove_if(modifiedLine.begin(), modifiedLine.end(), isspace), modifiedLine.end());

				// begin testing for key=value
				// try the potentially modified line (potentially without the pound)
				if (std::regex_search(modifiedLine, matches, std::regex("(^\\w+)=([^\\s\\#]+)")))
				{
					std::string matchedStr = matches.str(0);
					std::string key;
					// found a potential key=value pair, but could be invalid due to the removal of the # (if there are quotes). Also check for double quotes
					if(std::regex_search(matchedStr, matches, std::regex("="))) 
					{
						key = matches.prefix();
					}
					else 
					{
						consoleManager->PrintConsoleMessage
						(
							MESSAGE_VERBOSITY::CRITICAL_ERROR,
							MESSAGE_DOMAIN::STRING,
							"Invalid section on line: " + std::to_string(lineCount) + ". Reason: A valid key in the keyvalue pair could not be found"
						);
						continue;
					}
					// is there a quote at the start of the value?
					if (std::regex_search(matchedStr, matches, std::regex("=\"")))
					{
						// There is a quote at the start of the value
						std::string match = matches.suffix();
						size_t quoteCount = std::count(match.begin(), match.end(), '\"');
						if(quoteCount == 0) 
						{
							// potentially the # sign was inside the string and thus we can't process it. Or the user just put only one quote
							consoleManager->PrintConsoleMessage
							(
								MESSAGE_VERBOSITY::CRITICAL_ERROR,
								MESSAGE_DOMAIN::STRING,
								"Invalid section on line: " + std::to_string(lineCount) + ". Reason: the value that you defined has only 1 \" signs. Please fix it. It could be because you used the # sign as it denotes a comment"
							);
							continue;
						}
						else if(quoteCount > 1) 
						{
							// there are more than 2 quotes spotted error
							consoleManager->PrintConsoleMessage
							(
								MESSAGE_VERBOSITY::CRITICAL_ERROR,
								MESSAGE_DOMAIN::STRING,
								"Invalid section on line: " + std::to_string(lineCount) + ". Reason: the value that you defined has too many \" signs. Please fix it"
							);
							continue;
						}
						else 
						{
							if(match[match.length()-1] == '\"') 
							{
								// this is a valid string if the 1 found quote is at the end of the suffix, otherwise -> malformed line error
								if(TestSectionValidity(lastSection, lastSubsection, consoleManager, lineCount)) 
								{
									continue;
								}
								std::string finalValue = "\"" + match;
								db->AddKeyValuePair(key, finalValue, DATA_CONFIG_TYPE::STRING_T, false, lastSection, lastSubsection);
								continue;
							}
							consoleManager->PrintConsoleMessage
							(
								MESSAGE_VERBOSITY::CRITICAL_ERROR,
								MESSAGE_DOMAIN::STRING,
								"Invalid section on line: " + std::to_string(lineCount) + ". Reason: the value that you defined has incorrect \" location"
							);
							continue;
						}
					}
					else
					{
						// No quotes at the start of the value -> check the rest of the value for quotes. 
							// If no quotes found -> not a string. 
							// If found -> error out.
						if(std::regex_search(matchedStr, matches, std::regex("\"")))
						{
							std::string prefixMatch = matches.prefix();
							if(!std::regex_search(prefixMatch, matches, std::regex("\\{"))) 
							{
								consoleManager->PrintConsoleMessage
								(
									MESSAGE_VERBOSITY::CRITICAL_ERROR,
									MESSAGE_DOMAIN::STRING,
									"Invalid section on line: " + std::to_string(lineCount) + ". Reason: a malformed string has been found. Validate that a \" is after the = sign"
								);
								continue;
							}
						}
					}
					// at this point we know the value is not a string. There have been no quotes
					
					// check if it's a bool true or false case insensitive
					if (std::regex_search(matchedStr, matches, std::regex("(=true$|=false$)", std::regex_constants::icase)))
					{
						// this is a bool
						if (TestSectionValidity(lastSection, lastSubsection, consoleManager, lineCount))
						{
							continue;
						}
						std::string finalValue = matches.str(0);
						finalValue = finalValue.substr(1);
						db->AddKeyValuePair(key, finalValue, DATA_CONFIG_TYPE::BOOLEAN_T, false, lastSection, lastSubsection);
						continue;
					}
					// check if this a floating point number
					// I explicitly want to make sure that the first digit is not a leading zero
					else if(std::regex_search(matchedStr, matches, std::regex("=[-+]?[0-9]*\\.(([0-9]+f)|f)$")))
					{
						// this is a float
						if (TestSectionValidity(lastSection, lastSubsection, consoleManager, lineCount))
						{
							continue;
						}
						std::string finalValue = matches.str(0);
						finalValue = finalValue.substr(1);
						db->AddKeyValuePair(key, finalValue, DATA_CONFIG_TYPE::FLOAT_T, false, lastSection, lastSubsection);
						continue;
					}
					// check if this is a int
					// the leading number cannot be 0
					else if(std::regex_search(matchedStr, matches, std::regex("=[1-9]+[0-9]*$"))) 
					{
						// this is an int
						if (TestSectionValidity(lastSection, lastSubsection, consoleManager, lineCount))
						{
							continue;
						}
						std::string finalValue = matches.str(0);
						finalValue = finalValue.substr(1);
						db->AddKeyValuePair(key, finalValue, DATA_CONFIG_TYPE::INT_T, false, lastSubsection, lastSection);
						continue;
					}
					// check if this is an array
					else if(std::regex_search(matchedStr, matches, std::regex("=\\{.*?\\}$")))
					{
						std::string arrayString = matches.str(0);
						// now remove the = { } at the start and end of the string
						arrayString = arrayString.substr(2, arrayString.length()-3);

						// split the data based on ; 
						std::stringstream splitter(arrayString);
						std::string segment;
						std::vector<std::string> seglist;

						// disallow any speial character besides ; and "
						if (std::regex_search(arrayString, matches, std::regex("[^\\w^;\"\\.]")))
						{
							consoleManager->PrintConsoleMessage
							(
								MESSAGE_VERBOSITY::CRITICAL_ERROR,
								MESSAGE_DOMAIN::STRING,
								"Invalid section on line: " + std::to_string(lineCount) + ". Reason: special characters besides \", . and ; are not allowed inside arrays"
							);
							continue;
						}

						while (std::getline(splitter, segment, ';'))
						{
							seglist.push_back(segment);
						}

						// access the type of the first element. If it's a valid type, the rest of the elements must keep the same type
						if(seglist.size() == 0) 
						{
							// the given array was empty.
							consoleManager->PrintConsoleMessage
							(
								MESSAGE_VERBOSITY::CRITICAL_ERROR,
								MESSAGE_DOMAIN::STRING,
								"Invalid section on line: " + std::to_string(lineCount) + ". Reason: the given array is empty"
							);
							continue;
						}
						auto& initMember = seglist[0];
						MESSAGE_VERBOSITY message;
						DATA_CONFIG_TYPE initialType = RetrieveDataTypeFromString(initMember, message);
						if(message == MESSAGE_VERBOSITY::CRITICAL_ERROR) 
						{
							consoleManager->PrintConsoleMessage
							(
								MESSAGE_VERBOSITY::CRITICAL_ERROR,
								MESSAGE_DOMAIN::STRING,
								"Invalid section on line: " + std::to_string(lineCount) + ". Reason: a malformed member of an array has been found. It's type cannot be deducted"
							);
							continue;
						}
						for(size_t i = 1; i < seglist.size(); i++) 
						{
							DATA_CONFIG_TYPE currentType = RetrieveDataTypeFromString(seglist[i], message);
							if(message == MESSAGE_VERBOSITY::CRITICAL_ERROR || currentType != initialType) 
							{
								message = MESSAGE_VERBOSITY::CRITICAL_ERROR;
								break;
							}
						}
						if (message == MESSAGE_VERBOSITY::CRITICAL_ERROR)
						{
							consoleManager->PrintConsoleMessage
							(
								MESSAGE_VERBOSITY::CRITICAL_ERROR,
								MESSAGE_DOMAIN::STRING,
								"Invalid section on line: " + std::to_string(lineCount) + ". Reason: the types inside the array are not the same"
							);
							continue;
						}
						// all the types in the array match and we can store the array
						if (TestSectionValidity(lastSection, lastSubsection, consoleManager, lineCount))
						{
							continue;
						}
						db->AddKeyValuePair(key, arrayString, initialType, true, lastSection, lastSubsection);
						continue;
					}

					if (std::regex_search(modifiedLine, matches, std::regex("=\\{")))
					{
						consoleManager->PrintConsoleMessage
						(
							MESSAGE_VERBOSITY::CRITICAL_ERROR,
							MESSAGE_DOMAIN::STRING,
							"Invalid section on line: " + std::to_string(lineCount) + ". Reason: the array is incomplete. Please eliminate it"
						);
						continue;
					}
					consoleManager->PrintConsoleMessage
					(
						MESSAGE_VERBOSITY::CRITICAL_ERROR,
						MESSAGE_DOMAIN::STRING,
						"Invalid section on line: " + std::to_string(lineCount) + ". Reason: no valid data type could have been deducted from the value"
					);
					continue;
				}

				if(std::regex_search(modifiedLine, matches, std::regex("=")))
				{
					std::string prefMatch = matches.prefix();
					std::string suffMatch = matches.suffix();
					if(prefMatch[prefMatch.length()-1] == ' ' || suffMatch[0] == ' ') 
					{
						// found the sign used for defining a key=value, but the previous regex didn't find the correct way of the definition -> error out
						consoleManager->PrintConsoleMessage
						(
							MESSAGE_VERBOSITY::CRITICAL_ERROR,
							MESSAGE_DOMAIN::STRING,
							"Invalid section on line: " + std::to_string(lineCount) + ". Reason: your key value pair is defined incorrectly. To fix: eliminate any whitespace between the = sign"
						);
						continue;
					}
				}

				if (std::regex_search(modifiedLine, matches, std::regex("[\\W^=]\\w*\\d*="))) 
				{
					// special characters were used for the key -> error
					consoleManager->PrintConsoleMessage
					(
						MESSAGE_VERBOSITY::CRITICAL_ERROR,
						MESSAGE_DOMAIN::STRING,
						"Invalid section on line: " + std::to_string(lineCount) + ". Reason: the key contains special characters that are not allowed. Please remove them"
					);
					continue;
				}

				consoleManager->PrintConsoleMessage
				(
					MESSAGE_VERBOSITY::WARNING,
					MESSAGE_DOMAIN::STRING,
					"Invalid section on line: " + std::to_string(lineCount) + ". data written: " + modifiedLine + ". The parser couldn't extract data from it. Please read the documentation."
				);
            }
            catch(const std::regex_error& e)
            {
				std::cout << "regex_error caught after processing line: " << lineCount << " " << e.what() << '\n';
            }
        }
        file.close();

		// done parsing. Enable Data Base

		SetConsoleTextAttribute(hConsole, CONSOLE_COLOR_DEFAULT);
		std::cout << std::endl;
		std::cout << "Parsing is done! DB is up! Commands available: \n\nListAllSections" << 
			"\nListNamedSection sectionName\nListSubsections sectionName\nListKeyValuePairsInSection section/section:subsection\nGetEntry section/section:subsection,key\nGetKey\nGetValue\nGetType\n" << 
			std::endl;
		std::cout << "Please Enter Command: ";
		std::string input;
		std::smatch matched;
		while(getline(std::cin, input)) 
		{
			input.erase(std::remove_if(input.begin(), input.end(), isspace), input.end());
			if(input == "exit") 
			{
				SetConsoleTextAttribute(hConsole, CONSOLE_COLOR_SUCCESS);
				std::cout << "\nThank you for using my parser. See you next time!" << std::endl;
				SetConsoleTextAttribute(hConsole, CONSOLE_COLOR_DEFAULT);
				break;
			}
			else if (std::regex_search(input, matched, std::regex("ListAllSections", std::regex_constants::icase)))
			{
				db->ListAllSections();
			}
			else if(std::regex_search(input, matched, std::regex("ListSubsections", std::regex_constants::icase)))
			{
				std::string subsectionName = matched.suffix();
				db->ListSubsections(subsectionName);
			}
			else if (std::regex_search(input, matched, std::regex("ListNamedSection", std::regex_constants::icase)))
			{
				std::string sectionName = matched.suffix();
				db->ListNamedSection(sectionName);
			}
			else if (std::regex_search(input, matched, std::regex("ListKeyValuePairsInSection", std::regex_constants::icase)))
			{
				std::string sectionName = matched.suffix();
				db->ListKeyValuePairsInSection(sectionName);
			}
			else if (std::regex_search(input, matched, std::regex("GetEntry", std::regex_constants::icase)))
			{
				std::string sectionNameAndKey = matched.suffix();
				if (std::regex_search(sectionNameAndKey, matched, std::regex(",")))
				{
					std::string section = matched.prefix();
					std::string key = matched.suffix();
					db->GetEntry(section, key);
				}
				else 
				{
					std::cout << "The values you entered for command \"GetEntry\" were not comma separated" << std::endl;
				}
			}
			else if (std::regex_search(input, matched, std::regex("GetKey", std::regex_constants::icase)))
			{
				db->GetKey();
			}
			else if (std::regex_search(input, matched, std::regex("GetValue", std::regex_constants::icase)))
			{
				db->GetValue();
			}
			else if (std::regex_search(input, matched, std::regex("GetType", std::regex_constants::icase)))
			{
				db->GetType();
			}
			else
			{
				std::cout << "Your command was invalid. Please try again" << std::endl;
			}
			std::cout << "Please Enter Command: ";
		}
    }
	delete consoleManager;
	delete db;
    return 0;
}
