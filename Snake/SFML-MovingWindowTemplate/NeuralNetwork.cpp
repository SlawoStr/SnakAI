#include "NeuralNetwork.h"
#include <numeric>
#include <iostream>
#include <exception>

NN::Neuron::Neuron(unsigned numOutputs, unsigned neuronIndex) : myIndex(neuronIndex), outputValue(0.0f)
{
	// Create and initialize with random values connection Weights
	for (unsigned i = 0; i < numOutputs; ++i)
	{
		connectionWeights.push_back(Connection({randGenerator->getRandomFloat()}));
	}
}

void NN::Neuron::feedForward(const Layer& prevLayer, double(*activationFunction)(double))
{
	// Multiply neuron output value by connection weight
	auto sum = [this](double currentSum,const Neuron& neuron) {return currentSum + neuron.getOutputValue() * neuron.connectionWeights[myIndex].weight; };
	// Sum all connection values to this neuron
	double result = std::accumulate(prevLayer.begin(), prevLayer.end(), double(0.0f), sum);
	// Use activation function to get output value
	outputValue = activationFunction(result);
}

// Initialize generator with maximum range of NN connection weights
RandomNumberGenerator* NN::Neuron::randGenerator = new RandomNumberGenerator(0, 0, NEURON_MIN, NEURON_MAX);

NN::NeuralNetwork::NeuralNetwork(const std::vector<unsigned>& topolgy)
{
	/// <summary>
	/// Topology should contain number of neurons for each layer. Topolgy size is equal to number of layers in neural net.
	/// Each net size will be determined by value of certain layer.
	/// </summary>
	/// <param name="topolgy">Vector containing number of neurons per layer</param>
	for (unsigned layersNum = 0; layersNum < topolgy.size(); ++layersNum)
	{
		// Create new Layer
		layers.push_back(Layer());
		// Each neuron will have number of connection equal to number of neurons in next layer except last layer which have 0 connections
		unsigned numOutputs = (layersNum == topolgy.size() - 1 ? 0 : topolgy[layersNum + 1]);

		// Each layer have extra neuron(BIAS)
		for (unsigned neuronNum = 0; neuronNum <= topolgy[layersNum]; ++neuronNum)
		{
			layers.back().push_back(Neuron(numOutputs, neuronNum));
		}
		// Bias values is contant and equal to 1
		layers.back().back().setOutputValue(1.0);
	}
}

void NN::NeuralNetwork::feedForward(const std::vector<double>& inputVals)
{
	if (inputVals.size() != layers[0].size() - 1)
	{
		throw std::invalid_argument("Input value size must be equal to input layer of neural network");
	}

	// Set Input layer
	for (unsigned i = 0; i < inputVals.size(); ++i)
	{
		layers[0][i].setOutputValue(inputVals[i]);
	}

	for (unsigned layerNum = 1; layerNum < layers.size(); ++layerNum)
	{
		Layer& prevLayer = layers[layerNum - 1];
		for (unsigned n = 0; n < layers[layerNum].size() - 1; ++n)
		{
			// Activation Function for hidden layers
			if (layerNum != layers.size() - 1)
			{
				layers[layerNum][n].feedForward(prevLayer, &reluFunction);
			}
			else
			{
				layers[layerNum][n].feedForward(prevLayer, &sigmoidFunction);
			}
		}
	}
}

std::vector<double> NN::NeuralNetwork::getPrediction() const
{
	// Get reference to last layer(output layer)
	const Layer& outputLayer = layers.back();

	std::vector<double> results;
	results.resize(outputLayer.size()-1);
	// Get all values from output neurons
	std::transform(outputLayer.begin(), outputLayer.end() - 1, results.begin(), [](const Neuron& neuron) {return neuron.getOutputValue(); });

	return results;
}

std::vector<double> NN::NeuralNetwork::getWeight(bool bias) const
{
	std::vector<double> weights;
	int lastNeuron;
	// Ignore output layer
	for (int i = 0; i < layers.size() - 1; ++i)
	{
		// if bias is false ignore last neuron in each layer
		lastNeuron = int((bias == true ? layers[i].size() : layers[i].size() - 1));
		for (int j = 0; j < lastNeuron; ++j)
		{
			for (const auto& connection : layers[i][j].connectionWeights)
			{
				weights.push_back(connection.weight);
			}
		}
	}
	return weights;
}

std::vector<double> NN::NeuralNetwork::getBias() const
{
	std::vector<double> weights;

	// Skip last layer
	for (int layerNum = 0; layerNum < layers.size() - 1; ++layerNum)
	{
		for (int n = 0; n < layers[layerNum].back().connectionWeights.size(); ++n)
		{
			weights.push_back(layers[layerNum].back().connectionWeights[n].weight);
		}
	}
	return weights;
}

void NN::NeuralNetwork::setWeight(const std::vector<double>& weight,bool bias)
{
	int index = 0;
	int lastNeuron;
	for (int i = 0; i < layers.size() - 1; ++i)
	{
		// if bias is false ignore last neuron in each layer
		lastNeuron = int((bias == true ? layers[i].size() : layers[i].size() - 1));
		for (int j = 0; j < lastNeuron; ++j)
		{
			for (int weightNum = 0; weightNum < layers[i][j].connectionWeights.size(); weightNum++)
			{
				layers[i][j].connectionWeights[weightNum].weight = weight[index++];
			}
		}
	}
}

void NN::NeuralNetwork::setBias(const std::vector<double>& bias)
{
	int index = 0;
	// Skip last layer
	for (int layerNum = 0; layerNum < layers.size() - 1; ++layerNum)
	{
		for (int n = 0; n < layers[layerNum].back().connectionWeights.size(); n++)
		{
			layers[layerNum].back().connectionWeights[n].weight = bias[index++];
		}
	}
}