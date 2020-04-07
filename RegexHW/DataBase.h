#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "CoreDefinitions.h"

class DataBase 
{
public:
	DataBase();
	~DataBase();

	void ListAllSections();
	// returns the section based on its name
	void ListNamedSection(std::string section);
	// returns all the subsections under a given section
	void ListSubsections(std::string section);
	// returns all the key=value pairs in the given section. (like folders work)
	void ListKeyValuePairsInSection(std::string section);
	// get a keyvalue pair based on the specified section/subsection and key
	// sets the last key value string
	void GetEntry(std::string section, std::string key);
	// gets the key from a keyvalue pair
	void GetKey();
	// gets the value from the last looked at keyvalue pair
	void GetValue();
	// gets the type of the value from the last looked at keyvalue pair
	void GetType();

	void AddSection(std::string section);

	void AddSubSection(std::string subsection, std::string section);

	void AddKeyValuePair(std::string key, std::string value, DATA_CONFIG_TYPE valueType, bool isArray, std::string section);

private:
	std::vector<std::string> sectionsList;
	std::unordered_multimap<std::string, std::string> sectionToSubSectionMultiMap;
	std::unordered_multimap<std::string, DB_Data> sectionToKeyValuePairMultiMap;
	DB_Data lastKeyValuePair;
};