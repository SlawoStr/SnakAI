#include "GeneticAlgorithm.h"
#include <stdexcept>
#include <numeric>
#include <iostream>
#include <fstream>

GeneticAlgorithm::GeneticAlgorithm(const std::vector<unsigned>& topology, bool load)
{
	modelsVec.reserve(GeneticSettings::NUMBER_OF_MODELS);

	for (int i = 0; i < GeneticSettings::NUMBER_OF_MODELS; i++)
	{
		modelsVec.push_back(NN::NeuralNetwork(topology));
	}
	// Load weight from file and update NN weights
	if (load)
	{
		loadModels();
	}
}

std::vector<double> GeneticAlgorithm::predictOne(int modelIndex, const std::vector<double>& input)
{
	if (modelIndex < 0 || modelIndex >= GeneticSettings::NUMBER_OF_MODELS)
	{
		throw std::out_of_range("Model index is out of range");
	}
	modelsVec[modelIndex].feedForward(input);

	return modelsVec[modelIndex].getPrediction();
}

std::vector<std::vector<double>> GeneticAlgorithm::predictAll(const std::vector<std::vector<double>>& input)
{
	if (modelsVec.size() != input.size())
	{
		throw std::invalid_argument("Input for NN isnt equal to number of NN ");
	}

	std::vector<std::vector<double>> result;
	result.resize(modelsVec.size());

	for (int i = 0; i < modelsVec.size(); i++)
	{
		modelsVec[i].feedForward(input[i]);
		result[i] = modelsVec[i].getPrediction();
	}
	return result;
}

void GeneticAlgorithm::saveModels()
{
	std::ofstream myFile;

	myFile.open("modelWeight.txt", std::ios::out | std::ios::trunc);

	for (int i = 0; i < GeneticSettings::NUMBER_OF_MODELS; i++)
	{
		std::vector<double> modelWeight = modelsVec[i].getWeight();

		for (int i = 0; i < modelWeight.size(); i++)
		{
			myFile << modelWeight[i] << " ";
		}
		// Last element
		if (i == GeneticSettings::NUMBER_OF_MODELS - 1)
		{
			// end of file sign
			myFile << '#' << ' ';
		}
		else
		{
			// end of weight sign
			myFile << '$' << ' ';
		}
	}
	myFile.close();
}

void GeneticAlgorithm::loadModels()
{
	std::ifstream myFile;
	myFile.open("modelWeight.txt");
	//File doesnt exist
	if (!myFile.is_open())
	{
		throw std::ios_base::failure("This file doesnt exist");
	}
	std::string input;
	int modelIndex = 0;
	int weightSize = static_cast<int>(modelsVec[0].getWeight().size());
	std::vector<double>modelWeight;


	while (myFile >> input)
	{
		if (input.size() == 1)
		{
			if (modelWeight.size() != weightSize)
			{
				throw std::invalid_argument("Weight loaded from file isnt equal to model weight");
			}
			modelsVec[modelIndex++].setWeight(modelWeight);

			// End of the file
			if (input[0] == '#')
			{
				if (modelIndex != GeneticSettings::NUMBER_OF_MODELS)
				{
					throw std::invalid_argument("Number of loaded weights isnt equal to model number. Required weights :" + std::to_string(GeneticSettings::NUMBER_OF_MODELS) + "got" + std::to_string(modelIndex));
				}
				else
				{
					break;
				}
			}
			// End of current model weight
			else if (input[0] == '$')
			{
				modelWeight.clear();
			}
		}
		else
		{
			modelWeight.push_back(std::stod(input));
		}
	}
	myFile.close();
}

std::vector<double> GeneticAlgorithm::rouletteWheel(const std::vector<Fitness>& modelsFitness)
{
	// Probabilites of each value in overall models Fitness
	std::vector<double> probabilities;
	probabilities.resize(modelsFitness.size());
	// Cumulative distribution of all models probabilites
	std::vector<double> cumulativeDistribution;
	cumulativeDistribution.resize(modelsFitness.size());

	// Get fitness sum of all models
	double fitnessSum = std::accumulate(modelsFitness.begin(), modelsFitness.end(), double(0.0f), [](double fitnessSum, const Fitness& fitness) {return fitnessSum + fitness.value; });
	// Calculate probabilites for each NN
	std::transform(modelsFitness.begin(), modelsFitness.end(), probabilities.begin(), [fitnessSum](const Fitness& fitness) {return fitness.value / fitnessSum; });

	// Calculate cumulative distibution
	double cumulative = 0.0f;
	std::partial_sum(probabilities.begin(), probabilities.end(), cumulativeDistribution.begin());

	return cumulativeDistribution;
}

int GeneticAlgorithm::getRouletteParent(const std::vector<double>& cumulativeDistribution)
{
	double randomValue = numGenerator.getRandomFloat();
	if (randomValue <= cumulativeDistribution[0])
	{
		return 0;
	}
	for (int i = 0; i < cumulativeDistribution.size() - 1; i++)
	{
		if (randomValue > cumulativeDistribution[i] && randomValue <= cumulativeDistribution[i + static_cast<int>(1)])
		{
			return i + 1;
		}
	}
	// Should never be called
	return 0;
}

void GeneticAlgorithm::crossover(int parent1, int parent2, std::vector<std::vector<double>>& newGeneration)
{
	std::vector<double> weight1 = modelsVec[parent1].getWeight();
	std::vector<double>weight2 = modelsVec[parent2].getWeight();

	// Set up generator with range of neural net connection weights
	numGenerator.setIntRange(0, int(weight1.size() - 1));

	if (GeneticSettings::CROSSOVER_TYPE == "SPC")
	{
		int dividePoint = numGenerator.getRandomInt();
		for (int i = dividePoint; i < weight1.size(); i++)
		{
			std::swap(weight1[i], weight2[i]);
		}
	}
	else if (GeneticSettings::CROSSOVER_TYPE == "MPC")
	{
		int dividePoint1 = numGenerator.getRandomInt();
		int dividePoint2 = numGenerator.getRandomInt();
		while (dividePoint2 == dividePoint1)
		{
			dividePoint2 = numGenerator.getRandomInt();
		}
		if (dividePoint1 > dividePoint2)
		{
			std::swap(dividePoint1, dividePoint2);
		}
		// Only middle part is changed
		for (int i = dividePoint1; i < dividePoint2; i++)
		{
			std::swap(weight1[i], weight2[i]);
		}
	}
	mutate(weight1);
	mutate(weight2);

	newGeneration.push_back(weight1);
	newGeneration.push_back(weight2);
}

void GeneticAlgorithm::mutate(std::vector<double>& weight)
{
	for (int i = 0; i < weight.size(); i++)
	{
		numGenerator.setFloatRange(0, 1);
		if (GeneticSettings::MUTATION_TYPE == "ADD")
		{
			if (numGenerator.getRandomFloat() < GeneticSettings::MUTATION_RATIO)
			{
				numGenerator.setFloatRange(-0.25, 0.25);
				weight[i] += numGenerator.getRandomFloat();
			}
		}
		else if (GeneticSettings::MUTATION_TYPE == "NEW")
		{
			if (numGenerator.getRandomFloat() < GeneticSettings::MUTATION_RATIO)
			{
				numGenerator.setFloatRange(NN::NEURON_MIN, NN::NEURON_MAX);
				weight[i] = numGenerator.getRandomFloat();
			}
		}
	}
}

void GeneticAlgorithm::createNewPopulation(std::vector<Fitness>& modelFitness)
{
	if (modelFitness.size() != modelsVec.size())
	{
		throw std::invalid_argument("Fitness of models size isnt equal to NN size");
	}

	// Sort Fitness 
	std::sort(modelFitness.begin(), modelFitness.end(), [](const Fitness& fit1, const Fitness& fit2) {return fit1.value > fit2.value; });

	// Vector to hold new generation weights
	std::vector<std::vector<double>> newGeneration;
	// Parent selection based on cumulative distibution of fitness
	if (GeneticSettings::PARENT_SELECTION_TYPE == "WHEEL")
	{
		std::vector<double> cumulativeDistribution = rouletteWheel(modelFitness);
		int parent1, parent2;
		while (newGeneration.size() < GeneticSettings::NUMBER_OF_MODELS - GeneticSettings::PARENT_NUMBER)
		{
			numGenerator.setFloatRange(0, 1);
			parent1 = modelFitness[getRouletteParent(cumulativeDistribution)].index;
			parent2 = modelFitness[getRouletteParent(cumulativeDistribution)].index;
			//If both parents are the same get new parent
			while (parent1 == parent2)
			{
				parent2 = modelFitness[getRouletteParent(cumulativeDistribution)].index;
			}
			crossover(parent1, parent2, newGeneration);
		}
	}
	else if (GeneticSettings::PARENT_SELECTION_TYPE == "BEST")
	{
		// Two parents with best score in whole population
		int parent1 = modelFitness[0].index;
		int parent2 = modelFitness[1].index;

		while (newGeneration.size() < GeneticSettings::NUMBER_OF_MODELS - GeneticSettings::PARENT_NUMBER)
		{
			crossover(parent1, parent2, newGeneration);
		}
	}

	int counter = 0;
	// Best Parents are kept for next generation other NN acquire new weights
	for (int i = GeneticSettings::PARENT_NUMBER; i < GeneticSettings::NUMBER_OF_MODELS; i++)
	{
		modelsVec[modelFitness[i].index].setWeight(newGeneration[counter++]);
	}
}
