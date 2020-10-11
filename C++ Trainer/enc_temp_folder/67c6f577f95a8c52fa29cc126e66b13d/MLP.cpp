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
	Neuron *From;
	Neuron *To;
	double Weight;
	double ShouldAdd = 0;
};
class Neuron
{
public:
	Neuron()
	{
	}
	Neuron(int nIndex)
	{
		Index = nIndex;
	}
	int Index;
	double N = NAN; // Net value: Value of neuron before the activation function
	double Value = NAN; // Value: Value of the neuron after the activation function
	double J = NAN; // Current neuron's dN/dE
	int InputsSize = 0;
	vector<Connection*> Inputs;
	int OutputsSize = 0;
	vector<Connection*> Outputs;
};

class DesiredOutput
{
public:
	DesiredOutput()
	{

	}
	DesiredOutput(int _index, double _value)
	{
		index = _index;
		value = _value;
	}
	int index = 0;
	double value = 0;
};

class MultilayerPerceptron
{
public:
	double LearningRate;

	Neuron *Neurons;
	int NeuronsSize = 0;
	vector<Connection*> Connections;
	int ConnectionsSize = 0;
	MultilayerPerceptron(double LearningRate = 0.2)
	{
		this->LearningRate = LearningRate;
	}

	void CreateNeurons(int Count)
	{
		Neurons = new Neuron[Count];
		NeuronsSize = Count;
	}

	void ConnectNeurons(int Index1, int Index2, double Weight)
	{
		Connection *NewConnection = new Connection();

		NewConnection->Weight = Weight;


		if (Index2 > Index1)
		{
			NewConnection->From = &(Neurons[Index1]);
			NewConnection->To = &(Neurons[Index2]);

			AddToNeuronOutputs(&Neurons[Index1], NewConnection);
			AddToNeuronInputs(&Neurons[Index2], NewConnection);
		}
		else
		{
			NewConnection->From = &(Neurons[Index2]);
			NewConnection->To = &(Neurons[Index1]);

			AddToNeuronOutputs(&Neurons[Index2], NewConnection);
			AddToNeuronInputs(&Neurons[Index1], NewConnection);
		}

		AddToConnections(NewConnection);
	}
	double GetNeuronValue(int Index)
	{
		return Neurons[Index].Value;
	}
	void SetNeuronValue(int Index, double Value)
	{
		Neurons[Index].Value = Value;
	}
	double SigmoidDerivative(double in)
	{
		//return in > 0 ? 1 : 0; // ReLU?
		double sig = Sigmoid(in);

		return sig*(1 - sig);
	}
	double Sigmoid(double in)
	{
		//return in > 0 ? in : 0; // ReLU?
		return 1 / (1 + exp(-in));
	}
	void ComputeOutput()
	{

		for (int i = 0; i < NeuronsSize; i++)
		{

			Neuron &ThisNeuron = Neurons[i];
			double TV = ThisNeuron.Value;
			if (TV != TV)
			{
				ThisNeuron.N = 0;
				for (int i = 0; i < ThisNeuron.InputsSize; i++)
				{
					ThisNeuron.N += ThisNeuron.Inputs[i]->From->Value*ThisNeuron.Inputs[i]->Weight;
				}

				ThisNeuron.Value = Sigmoid(ThisNeuron.N);
			}

		}
	}
	void ClearUp()
	{
		for (int i = 0; i < NeuronsSize; i++)
		{
			Neurons[i].Value = NAN;
			Neurons[i].J = NAN;
			Neurons[i].N = NAN;
		}
	}
	void BackPropagate(DesiredOutput **DesiredOutputs, int DesiredOutputsLength)
	{
		double TotalSum = 0;

		// We compute last layer of the neural network separately.
		for (int i = 0; i < DesiredOutputsLength; i++)
		{
			DesiredOutput &DesiredOutput = *(DesiredOutputs[i]);
			Neuron &TargetNeuron = Neurons[DesiredOutput.index];

			/*
				W: Target weight we want to calculate the derivates for
				V0: Value of the neuron weight connection starts from (after activation)
				V1: Value of second neuron after the activation function
				A: Activation function (sigmoid in our case)
				N: Value of the neuron V1 before passing to the activation function
				T: Output neuron's desired output value
				
				N = V0 * W
				V1 = A(N)
				E = 0.5 * (T - V1) ^ 2
				
				dE/dV1 = 2 * 0.5 * (T - V1) * (-1) = (V1 - T)
				dV1/dN = A'(N)
				dN/dW = V0

				J: A variable for keeping track of dE/dN up until that neuron (moving backwards) in the network so we don't recalculate.
				J = dE/dN = dE/dV1 * dV1/dN = (T - V1) * A'(N)

				dE/dW = dE/dV1 * dV1/dN * dN/dW =  J * dN/dW = J * V0

				deltaW = 

				So we calculate J for the last layer.
				Then for each layer calculate 
			*/

			TargetNeuron.J = SigmoidDerivative(TargetNeuron.N) * (DesiredOutput.value - TargetNeuron.Value);
		}
		for (int i = NeuronsSize - 1; i >= 0; i--)
		{
			Neuron &TargetNeuron = Neurons[i];
			double TJ = TargetNeuron.J;
			if (TJ != TJ)
			{
				double AvgWj = 0;
				for (int j = 0; j < TargetNeuron.OutputsSize; j++)
				{
					// dE/dW = J * dN/dW = J * V0;
					TargetNeuron.Outputs[j]->ShouldAdd += -TargetNeuron.Value * TargetNeuron.Outputs[j]->To->J * LearningRate;

					AvgWj += TargetNeuron.Outputs[j]->To->J * TargetNeuron.Outputs[j]->Weight;
				}
				AvgWj /= TargetNeuron.OutputsSize;

				if (i > 0)
				{
					TargetNeuron.J = SigmoidDerivative(TargetNeuron.N) * AvgWj;
				}

			}
		}
		for (int i = 0; i < ConnectionsSize; i++)
		{
			Connections[i]->Weight += Connections[i]->ShouldAdd;
			Connections[i]->ShouldAdd = 0;
		}
	}

public:
	void AddToNeuronInputs(Neuron *TargetNeuron, Connection *connection)
	{
		TargetNeuron->Inputs.push_back(connection);
		TargetNeuron->InputsSize++;
	}

	void AddToNeuronOutputs(Neuron *TargetNeuron, Connection *connection)
	{
		TargetNeuron->Outputs.push_back(connection);
		TargetNeuron->OutputsSize++;
	}

	void AddToConnections(Connection *Item)
	{
		Connections.push_back(Item);
		ConnectionsSize++;
	}
};
