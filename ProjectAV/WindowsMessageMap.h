#pragma once
#include <unordered_map>
#include <string>
#include <Windows.h>

class WindowsMessageMap
{
public:
	WindowsMessageMap();
	std::wstring operator()( DWORD msg,LPARAM lp,WPARAM wp ) const;
private:
	std::unordered_map<DWORD,std::wstring> map;
};