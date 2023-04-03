#pragma once

#include <string>
#include <vector>
#include "RandomNumberGenerator.h"
#include "NeuralNetwork.h"

namespace GeneticSettings
{
	// Number of models in each generation
	const int NUMBER_OF_MODELS = 1000;
	// Number of parent that will be kept for next generation
	const int PARENT_NUMBER = 100;
	// Mutation Ratio  (probability that neuron connection weight will be mutated)
	const float MUTATION_RATIO = 0.05f;
	// Type of crossover operation ()
	const std::string CROSSOVER_TYPE = "MPC";   // (SPC,MPC)
	// Parent Selection Type
	const std::string PARENT_SELECTION_TYPE = "WHEEL"; // (WHEEL,BEST)
	//Mutation Type
	const std::string MUTATION_TYPE = "ADD";// (ADD,NEW)
}

struct Fitness
{
	double value;
	int index;
	Fitness(double val, int index) :value(val), index(index) {}
};

class GeneticAlgorithm
{
public:
	GeneticAlgorithm(const std::vector<unsigned>& topology, bool load);

	void createNewPopulation(std::vector<Fitness>& modelFitness);

	// Get Prediction results
	std::vector<double> predictOne(int modelIndex, const std::vector<double>& input);
	std::vector<std::vector<double>>predictAll(const std::vector<std::vector<double>>& input);

	// Load Save to file
	void saveModels();
	void loadModels();
private:
	// Genetic algorithm main functions
	std::vector<double> rouletteWheel(const std::vector<Fitness>& modelsFitness);
	int getRouletteParent(const std::vector<double>& cumulativeDistribution);
	void crossover(int parent1, int parent2, std::vector<std::vector<double>>& newGeneration);
	void mutate(std::vector<double>& weight);

private:
	RandomNumberGenerator numGenerator;
	std::vector<NN::NeuralNetwork> modelsVec;
};