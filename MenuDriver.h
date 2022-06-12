#pragma once
#include <functional>

#include "TSPGraph.h"

class MenuDriver
{
public:
	MenuDriver();
private:
	struct MenuOption
	{
		int m_choice;
		std::string m_text;
		std::function<void()> m_function;
	};
	std::vector<MenuOption> m_menuOptions;
	
	void showMenu();
	void chooseFileName();
	TSPGraph m_tspGraph;
};

