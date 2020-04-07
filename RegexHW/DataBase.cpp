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
	auto it = sectionToSubSectionMultiMap.find(section);
	std::cout << "Available Subsections for the specified section:" << std::endl;
	while(it != sectionToSubSectionMultiMap.end()) 
	{
		std::cout << it->second << std::endl;
		it++;
	}
	std::cout << "END" << std::endl;
}

void DataBase::ListKeyValuePairsInSection(std::string section)
{
	auto it = sectionToKeyValuePairMultiMap.find(section);
	std::cout << "Available key=value pairs for the specified section:" << std::endl;
	while (it != sectionToKeyValuePairMultiMap.end())
	{
		auto& data = it->second;
		if(data.isArray) 
		{
			std::cout << data.key << "={" << data.value << "}" << std::endl;
		}
		else 
		{
			std::cout << data.key << "=" << data.value << std::endl;
		}
		it++;
	}
	std::cout << "END" << std::endl;
}

void DataBase::GetEntry(std::string section, std::string key)
{
	auto it = sectionToKeyValuePairMultiMap.find(section);
	bool bFoundEntry = false;
	DB_Data foundData = {};
	while (it != sectionToKeyValuePairMultiMap.end())
	{
		if(it->second.key == key) 
		{
			bFoundEntry = true;
			foundData = it->second;
			break;
		}
		it++;
	}
	if(bFoundEntry) 
	{
		if(lastKeyValuePair.key != "") 
		{
			std::cout << "The last entry has been unloaded" << std::endl;
		}
		lastKeyValuePair = foundData;
		std::cout << "Entry has been found and is loaded in" << std::endl;
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
		std::cout << "Type: Array of " << DataTypeToString(lastKeyValuePair.dbType) << "}" << std::endl;
		return;
	}
	std::cout << "Type: " << DataTypeToString(lastKeyValuePair.dbType) << "}" << std::endl;
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
	auto iter = sectionToSubSectionMultiMap.find(section);
	while(iter != sectionToSubSectionMultiMap.end()) 
	{
		// validating for copies
		if(iter->second == subsection) 
		{
			return;
		}
		iter++;
	}
	sectionToSubSectionMultiMap.insert({section, subsection});
}

void DataBase::AddKeyValuePair(std::string key, std::string value, DATA_CONFIG_TYPE valueType, bool isArray, std::string section)
{
	// validate for copies
	auto iterpair = sectionToKeyValuePairMultiMap.equal_range(section);
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
	sectionToKeyValuePairMultiMap.insert({section, data});
}