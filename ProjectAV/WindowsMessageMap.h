#pragma once
#include <unordered_map>
#include "Win.h"
#include <string>

class WindowsMessageMap
{
public:
	WindowsMessageMap() noexcept;
	std::string operator()( DWORD msg,LPARAM lp,WPARAM wp ) const noexcept;
private:
	std::unordered_map<DWORD,std::string> map;
};