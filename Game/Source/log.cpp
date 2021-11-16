#pragma once
#include "Globals.h"
#include <vector>

std::vector<std::string> outputLog;

std::string GetOutputText(int index)
{
	return outputLog[index];
}

bool SameString(std::string a, std::string b)
{
	bool ret = true;

	if (a.size() != b.size()) return false;

	for (int i = 0; i < a.size(); i++)
	{
		ret = (a[i] == b[i]);
		if (!ret) return false;
	}

	return true;
}

int GetOutputSize()
{
	return outputLog.size();
}

void log(const char file[], int line, const char* format, ...)
{
	static char tmp_string[4096];
	static char tmp_string2[4096];
	static va_list  ap;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(tmp_string, 4096, format, ap);
	va_end(ap);
	sprintf_s(tmp_string2, 4096, "\n%s(%d) : %s", file, line, tmp_string);
	OutputDebugString(tmp_string2);

	outputLog.push_back(tmp_string2);
	outputLog.back().erase(outputLog.back().begin());
	if (outputLog.size() > OUTPUT_LOG_LIMIT) outputLog.erase(outputLog.begin());
}