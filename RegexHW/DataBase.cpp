#include "DataBase.h"
#include "Utility.h"
#include <iostream>

DataBase::DataBase()
{
	lastKeyValuePair = {};
}

DataBase::~DataBase()
{
	sectionsList.clear();
	sectionToKeyValuePairMultiMap.clear();
	sectionToSubSectionMultiMap.clear();
}

void DataBase::ListAllSections()
{
	std::cout << "Available Sections:" << std::endl;
	for (auto& section : sectionsList)
	{
		std::cout << "[" << section << "]" << std::endl;
	}
}

void DataBase::ListNamedSection(std::string section)
{
	std::cout << "Searching:" << std::endl;
	for (auto& sec : sectionsList)
	{
		if(sec == section) 
		{
			std::cout << "The specified section does exist:" << std::endl;
			ListSubsections(section);
			std::cout << std::endl;
			ListKeyValuePairsInSection(section);
			return;
		}
	}
	std::cout << "The specified section doesn't exist" << std::endl;
}

void DataBase::ListSubsections(std::string section)
{
	auto it = sectionToSubSectionMultiMap.equal_range(section);
	std::cout << "Available Subsections for the specified section:" << std::endl;
	for (auto iter = it.first; iter != it.second; iter++) 
	{
		std::cout << iter->second << std::endl;
	}
	std::cout << "END" << std::endl;
}

void DataBase::ListKeyValuePairsInSection(std::string section)
{
	auto finder = sectionToKeyValuePairMultiMap.equal_range(section);
	std::cout << "Available key=value pairs for the specified section:" << std::endl;
	for (auto iter = finder.first; iter != finder.second; iter++)
	{
		auto& data = iter->second;
		if (data.isArray)
		{
			std::cout << data.key << "={" << data.value << "}" << std::endl;
		}
		else
		{
			std::cout << data.key << "=" << data.value << std::endl;
		}
	}
	std::cout << "END" << std::endl;
}

void DataBase::GetEntry(std::string section, std::string key)
{
	auto finder = sectionToKeyValuePairMultiMap.equal_range(section);
	bool bFoundEntry = false;
	DB_Data foundData = {};
	for (auto iter = finder.first; iter != finder.second; iter++)
	{
		if (iter->second.key == key)
		{
			bFoundEntry = true;
			foundData = iter->second;
			break;
		}
	}
	if(bFoundEntry) 
	{
		if(lastKeyValuePair.key != "") 
		{
			std::cout << "The last entry has been unloaded" << std::endl;
		}
		lastKeyValuePair = foundData;
		std::cout << "Entry has been found and is loaded in. What would you like to do with it:\nGetKey?\nGetValue?\nGetType?" << std::endl;
		return;
	}
	std::cout << "This query didn't have any valid response" << std::endl;
}

void DataBase::GetKey()
{
	std::cout << "Key: " << lastKeyValuePair.key << std::endl;
}

void DataBase::GetValue()
{
	if(lastKeyValuePair.isArray) 
	{
		std::cout << "Value: {" << lastKeyValuePair.value << "}" << std::endl;
		return;
	}
	std::cout << "Value: " << lastKeyValuePair.value << std::endl;
}

void DataBase::GetType()
{
	if (lastKeyValuePair.isArray)
	{
		std::cout << "Type: Array of " << DataTypeToString(lastKeyValuePair.dbType) << std::endl;
		return;
	}
	std::cout << "Type: " << DataTypeToString(lastKeyValuePair.dbType) << std::endl;
}

void DataBase::AddSection(std::string section)
{
	// validate for copies
	for (auto& sec : sectionsList)
	{
		if(sec == section) 
		{
			return;
		}
	}
	sectionsList.push_back(section);
}

void DataBase::AddSubSection(std::string subsection, std::string section)
{
	auto finder = sectionToSubSectionMultiMap.equal_range(section);
	for (auto iter = finder.first; iter != finder.second; iter++)
	{
		if (iter->second == subsection)
		{
			return;
		}
	}
	sectionToSubSectionMultiMap.insert({section, subsection});
}

void DataBase::AddKeyValuePair(std::string key, std::string value, DATA_CONFIG_TYPE valueType, bool isArray, std::string section, std::string subsection)
{
	// validate for copies
	std::string query;

	if(subsection != "") 
	{
		query = section + ":" + subsection;
	}
	else 
	{
		query = section;
	}

	auto iterpair = sectionToKeyValuePairMultiMap.equal_range(query);
	auto it = iterpair.first;
	
	for(; it != iterpair.second; it++) 
	{
		if(it->second.key == key) 
		{
			// found a copy. Erase it
			sectionToKeyValuePairMultiMap.erase(it);
			break;
		}
	}

	DB_Data data = {};
	data.key = key;
	data.value = value;
	data.dbType = valueType;
	data.isArray = isArray;

	if(subsection != "") 
	{
		// use both section and subsection to hash 
		sectionToKeyValuePairMultiMap.insert({section + ":" + subsection, data});
		return;
	}
	sectionToKeyValuePairMultiMap.insert({section, data});
}