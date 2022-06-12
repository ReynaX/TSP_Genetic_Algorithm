#pragma once
#include <vector>
#include <random>
#include <chrono>

class TSPGraph
{
public:
	TSPGraph();
	void generateRandomCoordinates();
	void generateRandomCoordinates(int coordsCount);
	void readFromFile(std::string fileName);
	void printCoords();
	std::vector<int> greedyAlgorithm(int startingCity = 0);
	std::vector<int> generateBestGreedySolution();
	
	void geneticAlgorithm();
private:
	std::vector<std::pair<double, double>> m_values;
	// Used for generating random numbers
	std::mt19937 m_mt;
	std::random_device m_randomDevice;
	std::uniform_int_distribution<int> m_dist;

	struct GeneticAlgorithmConfig
	{
		typedef std::chrono::duration<double, std::milli> ms;
		double m_crossOverRate = 0.45;
		double m_mutationRate = 0.05;
		int m_populationSize = 200;
		
		int m_noImprovementCount = 0;
		int m_maxNoImprovement = 5000;

		int m_iterationCount = 1;
		// Time passed in seconds
		ms m_timePassed{0};
		double m_maxPossibleTimePassed = 300.0;

		std::chrono::time_point<std::chrono::high_resolution_clock> m_lastTime = std::chrono::high_resolution_clock::now();
		
		void tick(double lastDistance, double currentDistance){
			if (std::abs(currentDistance - lastDistance) <= std::numeric_limits<double>::epsilon())
				++m_noImprovementCount;
			else m_noImprovementCount = 0;
			++m_iterationCount;
			
			auto end = std::chrono::high_resolution_clock::now();
			ms duration = end - m_lastTime;
			m_timePassed += (duration / 1000);
			m_lastTime = end;
		}

		
		bool algorithmFinished(){
			if (m_noImprovementCount >= m_maxNoImprovement || m_timePassed.count() >= m_maxPossibleTimePassed)
				return true;
			return false;
		}
	};
	
	// Select population that will be used for breeding using Roulette Wheel Selection
	// returns index of that population
	int selectParent(const std::vector<double> &distances, const long double sumOfDistances);
	// Perform crossover on two populations using PMX algorithm
	// Returns new population
	std::vector<int> crossover(const std::vector<int>& p1, const std::vector<int>& p2);
	// Perform mutation on a given population using Reverse Sequence Mutation
	std::vector<int> mutation(const std::vector<int>& p1);
	// Calculates path distance in a given chromosone
	double calculateDistance(const std::vector<int>& chromosome);
	// Calculates and returns sum of distances
	long double getTotalSum(const std::vector<std::pair<std::vector<int>, double>>& positions);
	
	// Modifies given vector by dividing its value by a total sum
	std::vector<double> calculateFitness(const std::vector<std::pair<std::vector<int>, double>>& positions, long double sumOfDistances);
	std::default_random_engine m_rng;
};

