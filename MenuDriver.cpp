#include "MenuDriver.h"

#include <iostream>
#include <string>

MenuDriver::MenuDriver(){
	m_menuOptions = {
		{1, "Generate random coordinates", [tspGraph = &m_tspGraph] {tspGraph->generateRandomCoordinates(); }},
		{2, "Read coordinates from file", [this] { chooseFileName(); } },
		{3, "Greedy algorithm", [tspGraph = &m_tspGraph] {tspGraph->greedyAlgorithm(); }},
		{4, "Genetic algorithm", [tspGraph = &m_tspGraph] {tspGraph->geneticAlgorithm(); }}
	};
	std::sort(m_menuOptions.begin(), m_menuOptions.end(), [](const MenuOption &op1, const MenuOption &op2) { return op1.m_choice < op2.m_choice; });
	m_menuOptions.push_back({ 0, "Exit", ([] {exit(0); }) });
	showMenu();
}

void MenuDriver::showMenu() {
	if (m_menuOptions.empty())
		return;
	while (true) {
		for (auto &option : m_menuOptions) {
			std::cout << option.m_choice << ". " << option.m_text << '\n';
		}

		int currentChoice = -1;

		do {
			std::cin >> currentChoice;
			auto it = std::find_if(m_menuOptions.begin(), m_menuOptions.end(), [&currentChoice](const MenuOption &option) {
				return currentChoice == option.m_choice;
			});
			// Element clicked by a user is correct
			if (it != m_menuOptions.end()) {
				it->m_function();
				break;
			}
			else
				currentChoice = -1;
		} while (currentChoice == -1);
		getchar(); getchar();
		system("cls");
	}
}

void MenuDriver::chooseFileName(){
	std::vector<MenuOption> fileNames = {
		{1, "berlin52.txt", [tspGraph = &m_tspGraph] {tspGraph->readFromFile("berlin52.txt"); }},
		{2, "bier127.txt", [tspGraph = &m_tspGraph] {tspGraph->readFromFile("bier127.txt"); }},
		{3, "tsp250.txt", [tspGraph = &m_tspGraph] {tspGraph->readFromFile("tsp250.txt"); }},
		{4, "tsp500.txt", [tspGraph = &m_tspGraph] {tspGraph->readFromFile("tsp500.txt"); }},
		{5, "tsp1000.txt", [tspGraph = &m_tspGraph] {tspGraph->readFromFile("tsp1000.txt"); }},
		{6, "coords.txt", [tspGraph = &m_tspGraph] {tspGraph->readFromFile("coords.txt"); }},
	};
	while (true) {
		for (auto &option : fileNames) {
			std::cout << option.m_choice << ". " << option.m_text << '\n';
		}

		int currentChoice = -1;

		do {
			std::cin >> currentChoice;
			auto it = std::find_if(fileNames.begin(), fileNames.end(), [&currentChoice](const MenuOption &option) {
				return currentChoice == option.m_choice;
			});
			// Element clicked by a user is correct
			if (it != fileNames.end()) {
				it->m_function();
				return;
			}
			else
				currentChoice = -1;
		} while (currentChoice == -1);
		getchar(); getchar();
		system("cls");
	}
}
