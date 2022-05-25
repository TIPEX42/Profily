//
// Created by Nathan Jennes on 5/25/22.
//

#include <sstream>
#include <iostream>
#include <fstream>
#include "Application.h"
#include "raylib.h"

void Application::Start(int _width, int _height, std::string _programToProfile)
{
	width = _width;
	height = _height;
	programToProfile = std::move(_programToProfile);

	InitializeFunctionDict();
	InitializeFunctionCalls();
	//MainLoop();
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
	address = address.at(2);
	mainOffset = strtoll(address.c_str(), nullptr, 16) - mainOffset;
	AddFunctionCall(timestampFile, address, ts_sec, ts_usec);

	for (auto &call : functionCalls)
	{
		std::cout << call.name << "Started at " << call.start_time << ", and ended at " << call.end_time << std::endl;
	}

}

void Application::AddFunctionCall(std::ifstream &stream, const std::string &address, int64_t ts_sec, int64_t ts_usec)
{
	FunctionCall call{};

	int64_t realAddress = strtoll(address.substr(2).c_str(), nullptr, 16) - mainOffset;
	std::cout << realAddress << " real address\n";
	call.name = functionsDict[realAddress];
	if (call.name.empty())
		std::cout << "function address not found : " << address << std::endl;
	call.start_time = ts_sec * 1000 + ts_usec / 1000;

	char _c;
	int64_t _ts_sec, _ts_usec;
	std::string _address;

	while (stream >> _c >> _address >> _ts_sec >> _ts_usec)
	{
		if (_c != '<')
			AddFunctionCall(stream, _address, _ts_sec, _ts_usec);
		else
		{
			call.end_time = _ts_sec * 1000 + _ts_usec / 1000;
			functionCalls.emplace_back(call);
			return ;
		}
	}
}

void Application::MainLoop()
{
	while (!window.ShouldClose())
	{
		Update();
		Render();
	}
}

void Application::Update()
{

}

void Application::Render()
{
	BeginDrawing();
	Application::window.ClearBackground(WHITE);
	EndDrawing();
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
