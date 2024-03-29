#pragma once

#include <map>
#include <string>
#include <iterator>
#include <vector>

#define MapContains(MAP, KEY) (MAP.find(KEY) != MAP.end())

class Utilities{
public:
	static std::string readFile(const char* file);
	static std::vector<unsigned char> readFileBytes(const char* file);
	static void SaveFile(const char* file, const char* content);

	template<typename K, typename V>
	static bool findByValue(std::vector<K>& vec, std::map<K, V> mapOfElement, V value){
		bool success = false;
		auto it = mapOfElement.begin();
		while (it != mapOfElement.end()){
			if (it->second == value){
				success = true;
				vec.push_back(it->first);
			}
			it++;
		}
		return success;
	}
};

