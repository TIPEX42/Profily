//
// Created by Nathan Jennes on 5/25/22.
//

#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include "Application.h"

void Application::Start(std::string _programToProfile)
{
	programToProfile = std::move(_programToProfile);

	InitializeFunctionDict();
	InitializeFunctionCalls();
	InitializeFunctionHotTrace();

	for (auto &func : calledFunctionsNames)
	{
		std::cout << "[" << func << "] Was used and profiled" << std::endl;
	}

	for (auto &func : functionHotTrace)
	{
		std::cout << "[" << func.second << "] Took: " << func.first.time_sec * 1000000 + func.first.time_usec << "us" << std::endl;
	}

	CreateJson();
}

void Application::InitializeFunctionDict()
{
	std::string command = std::string("objdump -t " + programToProfile);
	std::vector<std::string> objdump = GetCommandOutput(command);

	for (std::string &line : objdump)
	{
		if (line.find(" F ") != std::string::npos ||
			line.find("dF ") != std::string::npos ||
			line.find("DF ") != std::string::npos)
		{
			// Getting the address of the function
			int64_t address = strtoll(line.c_str(), nullptr, 16);

			// Getting the function's name
			std::string funcName = line.substr(line.find('\t') + 2);
			funcName.erase(funcName.size() - 1);
			functionsDict[address] = funcName;

			if (funcName == "main")
				mainOffset = address;
		}
	}

	for (const auto& pair : functionsDict)
	{
		std::cout << "Adress: " << pair.first << ", Name: " << pair.second << std::endl;
	}
}

void Application::InitializeFunctionCalls()
{
	std::ifstream timestampFile("output.txt");
	if (timestampFile.fail())
	{
		std::cout << "Could not open the output.txt file of your program" << std::endl;
		exit(1);
	}

	char c;
	int64_t ts_sec, ts_usec;
	std::string address;

	timestampFile >> c >> address >> ts_sec >> ts_usec;
	mainOffset = strtoll(&address[2], nullptr, 16) - mainOffset;
	programTimeOffset_sec = ts_sec;
	programTimeOffset_usec = ts_usec;
	AddFunctionCall(timestampFile, address, ts_sec, ts_usec);
}

void Application::AddFunctionCall(std::ifstream &stream, const std::string &address, int64_t ts_sec, int64_t ts_usec)
{
	FunctionCall call{};

	int64_t realAddress = strtoll(&address[2], nullptr, 16) - mainOffset;
	call.name = functionsDict[realAddress];
	if (call.name.empty())
		std::cout << "function address not found : " << address << std::endl;
	call.start_time_sec = ts_sec - programTimeOffset_sec;
	call.start_time_usec = ts_usec - programTimeOffset_usec;
	programEndTime_sec = call.start_time_sec;
	programEndTime_usec = call.start_time_usec;

	char _c;
	int64_t _ts_sec, _ts_usec;
	std::string _address;

	while (stream >> _c >> _address >> _ts_sec >> _ts_usec)
	{
		if (_c != '<')
			AddFunctionCall(stream, _address, _ts_sec, _ts_usec);
		else if (functionsDict[strtoll(&_address[2], nullptr, 16) - mainOffset] == call.name)
		{
			call.end_time_sec = _ts_sec - programTimeOffset_sec;
			call.end_time_usec = _ts_usec - programTimeOffset_usec;
			programEndTime_sec = call.end_time_sec;
			programEndTime_usec = call.end_time_usec;
			functionCalls.emplace_back(call);
			if (!ContainsCalledFunction(call.name))
				calledFunctionsNames.emplace_back(call.name);
			return ;
		}
		else
			std::cout << "End does not match start" << std::endl;
	}
	call.end_time_sec = programEndTime_sec;
	call.end_time_usec = programEndTime_usec;
	functionCalls.emplace_back(call);
	if (!ContainsCalledFunction(call.name))
		calledFunctionsNames.emplace_back(call.name);
}

bool Application::ContainsCalledFunction(std::string &name)
{
	for (auto &func: calledFunctionsNames)
		if (func == name)
			return true;
	return false;
}

std::vector<std::string> Application::GetCommandOutput(std::string &command)
{
	char buffer[128];
	std::vector<std::string> result;

	FILE* pipe = popen(command.c_str(), "r");
	if (!pipe) {
		return result;
	}

	while (!feof(pipe)) {
		if (fgets(buffer, 128, pipe) != nullptr)
			result.emplace_back(buffer);
	}

	pclose(pipe);
	return result;
}

void Application::CreateJson()
{
	std::ofstream outputStream;
	outputStream.open("result.json");

	// Header
	outputStream << "{\"otherData\": {},\"traceEvents\":[";
	outputStream.flush();

	// Functions
	for (auto &call : functionCalls)
	{
		if (profileCount++ > 0)
			outputStream << ",";

		std::string name = call.name;
		int64_t dur = (call.end_time_sec * 1000000 + call.end_time_usec) - (call.start_time_sec * 1000000 + call.start_time_usec);

		outputStream << "{";
		outputStream << "\"cat\":\"function\",";
		outputStream << "\"dur\":" << dur << ',';
		outputStream << "\"name\":\"" << name << "\",";
		outputStream << "\"ph\":\"X\",";
		outputStream << "\"pid\":0,";
		outputStream << "\"tid\":" << 0 << ",";
		outputStream << "\"ts\":" << call.start_time_sec * 1000000 + call.start_time_usec;
		outputStream << "}";

		outputStream.flush();
	}

	// Footer
	outputStream << "]}";
	outputStream.flush();
}

void Application::InitializeFunctionHotTrace()
{
	for (auto &func : calledFunctionsNames)
	{
		int64_t time_sec = 0, time_usec = 0;

		for (auto &call: functionCalls)
		{
			if (call.name == func)
			{
				time_sec += call.end_time_sec - call.start_time_sec;
				time_usec += call.end_time_usec - call.start_time_usec;
			}
		}
		while (time_usec > 1000000)
		{
			time_sec++;
			time_usec -= 1000000;
		}
		FuncTime time{time_sec, time_usec};
		functionHotTrace.emplace(time, func);
	}
}

bool FuncTime::operator<(const FuncTime other) const
{
	if (time_sec > other.time_sec)
		return false;
	if (time_sec == other.time_sec && time_usec >= other.time_usec)
		return false;
	return true;
}
