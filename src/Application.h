//
// Created by Nathan Jennes on 5/25/22.
//

#ifndef APPLICATION_H
#define APPLICATION_H

#include "raylib-cpp.hpp"
#include <string>
#include <map>

struct FunctionCall
{
	int64_t start_time;
	int64_t end_time;
	std::string name;
};

class Application
{
public:
	inline static int width, height;

	Application() = delete;

	static void Start(int _width, int _height, std::string _programToProfile);

private:
	inline static std::string title;
	inline static raylib::Window window;
	inline static std::string programToProfile;

	inline static std::map<int64_t, std::string> functionsDict;
	inline static std::vector<FunctionCall> functionCalls;
	inline static int64_t mainOffset;

	static std::vector<std::string> GetCommandOutput(std::string &command);

	static void InitializeFunctionDict();
	static void InitializeFunctionCalls();
	static void AddFunctionCall(std::ifstream &stream, const std::string &address, int64_t ts_sec, int64_t ts_usec);
	static void MainLoop();
	static void Update();
	static void Render();
};

#endif //APPLICATION_H
