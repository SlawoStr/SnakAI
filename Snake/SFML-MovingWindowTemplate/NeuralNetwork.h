#pragma once
#include "RandomNumberGenerator.h"
#include <vector>

namespace NN
{
	// Activation Functions
	inline double tahnFunction(double x) { return tanh(x); }
	inline double sigmoidFunction(double x) { return 1.0f / (1.0f + exp(-x)); }
	inline double reluFunction(double x) { return x <= 0 ? 0 : x; }
	// Neuron Range values
	const int NEURON_MIN = -1;
	const int NEURON_MAX = 1;

	struct Connection
	{
		double weight;
	};

	class Neuron
	{
		using Layer = std::vector<Neuron>;
	public:
		Neuron(unsigned numOutputs, unsigned neuronIndex);
		double getOutputValue()const { return this->outputValue; }
		void setOutputValue(double value) { this->outputValue = value; }
		// Calculate output for neuron
		void feedForward(const Layer& prevLayer, double(*activationFunction)(double));
	public:
		std::vector<Connection> connectionWeights;
	private:
		// Random number generator for connection weight initialization
		static RandomNumberGenerator* randGenerator;
		unsigned myIndex;
		double outputValue;
	};

	class NeuralNetwork
	{
		using Layer = std::vector<Neuron>;
	public:
		NeuralNetwork(const std::vector<unsigned>& topolgy);
		// Feed neural network with input data
		void feedForward(const std::vector<double>& inputVals);
		// Get prediction from neural net(output layer values)
		std::vector<double> getPrediction()const;
		// Getters for weights control
		std::vector<double> getWeight(bool bias=true) const;
		std::vector<double> getBias() const;
		// Setters for weights control
		void setWeight(const std::vector<double>& weight, bool bias = true);
		void setBias(const std::vector<double>& bias);
	private:
		std::vector<Layer> layers;
	};
}

