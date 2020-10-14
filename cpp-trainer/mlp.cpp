//#include "stdafx.h"

#include <cmath>
#include <vector>

using namespace std;

class MultilayerPerceptron;
class Neuron;
class Connection;

class Connection
{
public:
	Neuron *from;
	Neuron *to;
	double weight;
	double shouldAdd = 0;
};
class Neuron
{
public:
	Neuron() { }
	Neuron(int nIndex)
	{
		index = nIndex;
	}
	int index;
	double netValue = NAN; // Net value: value of neuron before the activation function
	double value = NAN; // value: value of the neuron after the activation function
	double j = NAN; // Current neuron's dN/dE
	vector<Connection*> inputs;
	vector<Connection*> outputs;
};

class DesiredOutput
{
public:
	int index = 0;
	double value = 0;
	DesiredOutput(int _index, double _value)
	{
		index = _index;
		value = _value;
	}
};

class MultilayerPerceptron
{
public:
	double learningRate;

	Neuron *neurons;
	int neuronsSize = 0;

	vector<Connection*> connections;

	MultilayerPerceptron(double learningRate = 0.2)
	{
		this->learningRate = learningRate;
	}

	void createNeurons(int count)
	{
		neurons = new Neuron[count];
		neuronsSize = count;
	}

	void connectNeurons(int Index1, int Index2, double weight)
	{
		Connection *newConnection = new Connection();

		newConnection->weight = weight;


		if (Index2 > Index1)
		{
			newConnection->from = &(neurons[Index1]);
			newConnection->to = &(neurons[Index2]);

			addToNeuronOutputs(&neurons[Index1], newConnection);
			addToNeuronInputs(&neurons[Index2], newConnection);
		}
		else
		{
			newConnection->from = &(neurons[Index2]);
			newConnection->to = &(neurons[Index1]);

			addToNeuronOutputs(&neurons[Index2], newConnection);
			addToNeuronInputs(&neurons[Index1], newConnection);
		}

		addToConnections(newConnection);
	}
	double getNeuronValue(int index)
	{
		return neurons[index].value;
	}
	void setNeuronValue(int index, double value)
	{
		neurons[index].value = value;
	}
	double sigmoidDerivative(double in)
	{
		//return in > 0 ? 1 : 0; // ReLU?
		double sig = sigmoid(in);

		return sig*(1 - sig);
	}
	double sigmoid(double in)
	{
		//return in > 0 ? in : 0; // ReLU?
		return 1 / (1 + exp(-in));
	}
	void computeOutput()
	{

		for (int i = 0; i < neuronsSize; i++)
		{

			Neuron &thisNeuron = neurons[i];
			double TV = thisNeuron.value;
			if (TV != TV)
			{
				thisNeuron.netValue = 0;
				for (int i = 0; i < thisNeuron.inputs.size(); i++)
				{
					thisNeuron.netValue += thisNeuron.inputs[i]->from->value*thisNeuron.inputs[i]->weight;
				}

				thisNeuron.value = sigmoid(thisNeuron.netValue);
			}

		}
	}
	void cleanUp()
	{
		for (int i = 0; i < neuronsSize; i++)
		{
			neurons[i].value = NAN;
			neurons[i].j = NAN;
			neurons[i].netValue = NAN;
		}
	}
	void backPropagate(DesiredOutput **desiredOutputs, int desiredOutputsLength)
	{
		// We compute last layer of the neural network separately.
		for (int i = 0; i < desiredOutputsLength; i++)
		{
			DesiredOutput &desiredOutput = *(desiredOutputs[i]);
			Neuron &targetNeuron = neurons[desiredOutput.index];

			/*
				W: Target weight we want to calculate the derivates for
				V0: value of the neuron weight connection starts from (after activation)
				V1: value of second neuron after the activation function
				A: Activation function (sigmoid in our case)
				netValue: value of the neuron V1 before passing to the activation function
				T: Output neuron's desired output value
				
				netValue = V0 * W
				V1 = A(netValue)
				E = 0.5 * (T - V1) ^ 2
				
				dE/dV1 = 2 * 0.5 * (T - V1) * (-1) = (V1 - T)
				dV1/dN = A'(netValue)
				dN/dW = V0

				j: A variable for keeping track of dE/dN up until that neuron (moving backwards) in the network so we don't recalculate.
				For the last layer we have:
				j = dE/dN = dE/dV1 * dV1/dN = (T - V1) * A'(netValue)

				dE/dW = dE/dV1 * dV1/dN * dN/dW =  j * dN/dW = j * V0

				So we calculate j for the last (output) layer's neurons.
				
				Then for each layer's neurons calculate dE/dW from j and V0 (then do += -dE/dW to minimize E). BUT we need to apply
				the value changes later so we save them at shouldAdd and adjust weights in the end.
				And Also calculate current j from avg of output connections' weight and j and current netValue
			*/

			targetNeuron.j = sigmoidDerivative(targetNeuron.netValue) * (targetNeuron.value - desiredOutput.value);
		}
		for (int i = neuronsSize - 1; i >= 0; i--)
		{
			Neuron &targetNeuron = neurons[i];
			double TJ = targetNeuron.j;
			if (TJ != TJ)
			{
				// let j be the j for current neuron and i be set if indexes for neurons current is connected to in the next layer.
				// Calculating for specific connection i: dEi/dV1 = dEi/dNi * dNi/dV1 = Ji * wi
				// But we need to consider all outgoing weights the change affects for minimzing error so:
				// dE/dV1 = avg(dE1/dV1, dE/dV1, dE2/dV1, ..., dEn-1/dV1, dEn/dV1) 
				// avgWj is dE/dV1
				double avgWj = 0;
				for (int j = 0; j < targetNeuron.outputs.size(); j++)
				{
					// dE/dW = dE/dV1 * dV1/dN * dN/dW =  j * dN/dW = j * V0
					// deltaW = -dE/dW * learningRate; to minimize E
					targetNeuron.outputs[j]->shouldAdd += -targetNeuron.value * targetNeuron.outputs[j]->to->j * learningRate;

					
					avgWj += targetNeuron.outputs[j]->to->j * targetNeuron.outputs[j]->weight;
				}
				avgWj /= targetNeuron.outputs.size();

				// j =  dE/dN = dV1/dN  * dE/dV1 = A'(netValue) * avg(dE1/dV1, dE/dV1, dE2/dV1, ..., dEn-1/dV1, dEn/dV1) 
				if (i > 0) // Don't really need to calculate j for the first input layer
				{
					// j =  dE/dN = dV1/dN  * dE/dV1 = A'(netValue) * avg(dE1/dV1, dE/dV1, dE2/dV1, ..., dEn-1/dV1, dEn/dV1) 
					targetNeuron.j = sigmoidDerivative(targetNeuron.netValue) * avgWj;
				}

			}
		}
		for (int i = 0; i < connections.size(); i++)
		{
			connections[i]->weight += connections[i]->shouldAdd;
			connections[i]->shouldAdd = 0;
		}
	}

public:
	void addToNeuronInputs(Neuron *targetNeuron, Connection *connection)
	{
		targetNeuron->inputs.push_back(connection);
	}

	void addToNeuronOutputs(Neuron *targetNeuron, Connection *connection)
	{
		targetNeuron->outputs.push_back(connection);
	}

	void addToConnections(Connection *item)
	{
		connections.push_back(item);
	}
};
