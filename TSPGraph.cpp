#include "TSPGraph.h"

#include <chrono>
#include <ctime>

#include <fstream>
#include <iostream>
#include <set>

TSPGraph::TSPGraph()
	: m_mt(m_randomDevice()){
	m_mt.seed(::time(nullptr));
	m_rng = std::default_random_engine{ m_randomDevice() };
}

void TSPGraph::generateRandomCoordinates(){
	int coordsCount = -1;
	while(true){
		std::cout << "Enter a number of coordinates: ";
		std::cin >> coordsCount;
		if (coordsCount > 0)
			break;
		else std::cout << "Incorrect number of coords\n";
	}
	generateRandomCoordinates(coordsCount);
	std::cout << "Coords generated\n";
}

void TSPGraph::generateRandomCoordinates(int coordsCount){
	m_values.clear();
	std::set<std::pair<int, int>> coords;

	std::ofstream file("coords.txt");
	m_dist = std::uniform_int_distribution<int>(1, 2500);
	while(coords.size() < coordsCount){
		int x = m_dist(m_mt), y = m_dist(m_mt);
		coords.insert(std::make_pair(x, y));
	}
	
	file << coordsCount << '\n';
	int index = 1;
	for (auto it = coords.begin(); it != coords.end(); ++it) {
		file << index++ << ' ' << it->first << ' ' << it->second << '\n';
		m_values.emplace_back(std::make_pair(it->first, it->second));
	}
	file.close();
}

void TSPGraph::readFromFile(std::string fileName){
	std::ifstream file(fileName);
	if(file){
		m_values.clear();
		int coordsCount;
		file >> coordsCount;
		for(int i = 0; i < coordsCount; ++i){
			double first, second, index;
			file >> index >> first >> second;
			m_values.emplace_back(std::make_pair(first, second));
		}
	}
	file.close();
	std::cout << "Coords read from file\n";
}

void TSPGraph::printCoords(){
	for(auto &p: m_values){
		std::cout << p.first << ' ' << p.second << '\n';
	}
}

std::vector<int> TSPGraph::greedyAlgorithm(int startingCity){
	if(m_values.empty()){
		std::cerr << "Coords not generated\n";
		return std::vector<int>{};
	}
	// Start path from first vertex
	std::vector<int> path{startingCity};
	double minDistance = 0.0;
	while(path.size() < m_values.size()){
		// Get last visited city coordinates
		auto lastCoords = m_values[path.back()];
		double min = DBL_MAX;
		int minIndex = -1;
		// Find closest city to the last visited one
		for (int i = 0; i < m_values.size(); ++i) {
			double distance = sqrt(std::pow(lastCoords.first - m_values[i].first, 2) +
									  std::pow(lastCoords.second - m_values[i].second, 2));
			if (i != path.back() && distance < min && 
				std::find(path.begin(), path.end(), i) == path.end()) {
				min = distance;
				minIndex = i;
			}
		}
		if (minIndex != -1) {
			path.push_back(minIndex);
			minDistance += min;
		}
	}
	// Add distance between starting city and last visited city
	minDistance += sqrt(std::pow(m_values[0].first - m_values[path.back()].first, 2) +
		std::pow(m_values[0].second - m_values[path.back()].second, 2));
	
	std::cout << "Path: ";
	for (auto &p: path)
		std::cout << p + 1 << ' ';
	std::cout << path[0] + 1 << "\nDistance: " << minDistance << '\n';
	return path;
}

std::vector<int> TSPGraph::generateBestGreedySolution() {
	std::vector<int> bestSolution;
	double minFitnessValue = DBL_MAX;
	std::cout.setstate(std::ios_base::failbit);
	if (m_values.size() <= 500) {
		for (int i = 0; i < m_values.size(); ++i) {
			auto currentSolution = greedyAlgorithm(i);
			auto currentFitnessValue = calculateDistance(currentSolution);
			if (currentFitnessValue < minFitnessValue) {
				bestSolution = currentSolution;
				minFitnessValue = currentFitnessValue;
			}
		}
	}else{
		bestSolution = greedyAlgorithm(0);
	}
	std::cout.clear();
	return bestSolution;
}

void TSPGraph::geneticAlgorithm(){
	if (m_values.empty())
		return;
	
	GeneticAlgorithmConfig genValue;
	
	std::vector<std::pair<std::vector<int>, double>> populations;
	populations.reserve(genValue.m_populationSize * 2); populations.resize(genValue.m_populationSize, std::make_pair(std::vector<int>(m_values.size()), 0.0));

	auto bestGreedySolution = generateBestGreedySolution();
	populations[0] = std::make_pair( bestGreedySolution, calculateDistance(bestGreedySolution));
	
	for (int i = 1; i < genValue.m_populationSize; ++i) {
		std::generate(populations[i].first.begin(), populations[i].first.end(), [n = 0]() mutable  { return n++; });
		std::shuffle(populations[i].first.begin(), populations[i].first.end(), m_rng);
		
		populations[i].second = calculateDistance(populations[i].first);
	}

	while (!genValue.algorithmFinished()) {
		std::cout << genValue.m_iterationCount << ' ' << populations[0].second << ' ' << genValue.m_timePassed.count() << '\n';

		// Calculate total sum of path lengths in generation
		long double sumOfDistances = getTotalSum(populations);
		// Calculates fitness of every population
		auto distances = calculateFitness(populations, sumOfDistances);
		
		int crossoverSize = genValue.m_crossOverRate * genValue.m_populationSize;
		// Make crossovers
		for (int i = 0; i < crossoverSize; ++i) {
			int parentIndex = selectParent(distances, sumOfDistances);
			int secondParentIndex;
			m_dist = std::uniform_int_distribution<int>(0, genValue.m_populationSize - 1);
			// Make sure that two different populations are chosen
			while ((secondParentIndex = m_dist(m_mt)) == parentIndex);
			auto newPopulation = crossover(populations[parentIndex].first, populations[secondParentIndex].first);

			populations.emplace_back(std::make_pair(newPopulation, calculateDistance(newPopulation)));
			distances.push_back(populations.back().second / sumOfDistances);
		}

		// Make mutations
		int mutationsSize = genValue.m_mutationRate * genValue.m_populationSize;
		//int mutationsSize = genValue.m_populationSize * 2 - populations.size();
		for (int i = 0; i < mutationsSize; ++i) {
			m_dist = std::uniform_int_distribution<int>(0, genValue.m_populationSize - 1);
			auto newMutation = mutation(populations[m_dist(m_mt)].first);

			populations.emplace_back(std::make_pair(newMutation, calculateDistance(newMutation)));
			distances.push_back(populations.back().second / sumOfDistances);
		}
		

		auto bestDistance = populations[0].second;
		// Sort 
		std::sort(populations.begin(), populations.end(),
			[](const std::pair<std::vector<int>, double>& p1, const std::pair<std::vector<int>, double>& p2) {
			return p1.second < p2.second;
		});
		
		// Leave only the best results
		for (int i = populations.size(); i > genValue.m_populationSize; --i) {
			populations.pop_back();
			distances.pop_back();
		}
		genValue.tick(bestDistance, populations[0].second);
	}
	for(auto &p: populations[0].first){
		std::cout << p + 1 << ' ';
	}
	std::cout << populations[0].first[0] + 1 << "\nDistance: " << populations[0].second << '\n';
}

int TSPGraph::selectParent(const std::vector<double> &distances, const long double sumOfDistances){
	std::uniform_real_distribution<double> unif(0, 1);
	double random = unif(m_randomDevice), sum = 0.0;
	size_t size = distances.size();
	
	for(int i = 0; i < size; ++i){
		sum += distances[i];
		if (random < sum)
			return i;
	}
	return distances.size() - 1;
}

std::vector<int> TSPGraph::crossover(const std::vector<int>& p1, const std::vector<int>& p2){
	std::vector<int> newPopulation(p1.size());
	int newPopulationSize = 0;

	auto visited = new bool[p1.size()]();
	
	m_dist = std::uniform_int_distribution<int>(0, p1.size() / 2 - 1);
	int lowerBound = m_dist(m_mt);
	m_dist = std::uniform_int_distribution<int>(lowerBound + 1, p1.size() - 1);
	int upperBound = m_dist(m_mt);
	int index = 0;
	for(int i = lowerBound; i <= upperBound; ++i){
		newPopulation[index++] = p1[i];
		visited[p1[i]] = true;
	}

	for(auto &p: p2)
		if (!visited[p])
			newPopulation[index++] = p;
	
	delete []visited;
	return newPopulation;
}

std::vector<int> TSPGraph::mutation(const std::vector<int>& p1) {
	auto newVector = p1;
	m_dist = std::uniform_int_distribution<int>(0, p1.size() - 1);
	int a = m_dist(m_mt), b = m_dist(m_mt);
	int index1 = std::min(a, b), index2 = std::max(a,b);
	std::reverse(newVector.begin() + index1, newVector.begin() + index2);
	return newVector;
}


double TSPGraph::calculateDistance(const std::vector<int>& chromosome){
	double distance = 0.0;
	auto lastCoord = m_values[chromosome[0]];
	for(int i = 1; i < chromosome.size(); ++i){
		distance += sqrt(std::pow(lastCoord.first - m_values[chromosome[i]].first, 2) +
							std::pow(lastCoord.second - m_values[chromosome[i]].second, 2));
		lastCoord = m_values[chromosome[i]];
	}
	distance += sqrt(std::pow(lastCoord.first - m_values[chromosome[0]].first, 2) +
		std::pow(lastCoord.second - m_values[chromosome[0]].second, 2));
	return distance;
}

long double TSPGraph::getTotalSum(const std::vector<std::pair<std::vector<int>, double>>& positions){
	long double totalSum = 0.0;
	for (auto &position : positions)
		totalSum += position.second;
	return totalSum;
}

std::vector<double> TSPGraph::calculateFitness(const std::vector<std::pair<std::vector<int>, double>>& positions, long double sumOfDistances){
	std::vector<double> distances;
	distances.reserve(positions.size() * 2);
	
	int index = 0;
	for (auto &distance : positions)
		distances.push_back(positions[index++].second / sumOfDistances);
	return distances;
}

