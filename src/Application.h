//
// Created by Nathan Jennes on 5/25/22.
//

#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>
#include <map>
#include <vector>

struct FunctionCall
{
	int64_t start_time_sec;
	int64_t start_time_usec;
	int64_t end_time_sec;
	int64_t end_time_usec;
	std::string name;
};

class Application
{
public:
	Application() = delete;

	static void Start(std::string _programToProfile);

private:
	inline static std::string programToProfile;

	inline static std::map<int64_t, std::string> functionsDict;
	inline static std::vector<FunctionCall> functionCalls;
	inline static int64_t mainOffset;
	inline static int64_t programTimeOffset_sec;
	inline static int64_t programTimeOffset_usec;
	inline static int64_t programEndTime_sec;
	inline static int64_t programEndTime_usec;
	inline static int64_t profileCount = 0;

	static std::vector<std::string> GetCommandOutput(std::string &command);

	static void InitializeFunctionDict();
	static void InitializeFunctionCalls();
	static void CreateJson();
	static void AddFunctionCall(std::ifstream &stream, const std::string &address, int64_t ts_sec, int64_t ts_usec);
};

#endif //APPLICATION_H
