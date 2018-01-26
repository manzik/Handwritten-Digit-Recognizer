//#include "stdafx.h"

#include <cmath>
#include <vector>

using namespace std;

class GeneralNeuralNetwork;
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
	double V = NAN;
	double Value = NAN;
	double J = NAN;
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

class GeneralNeuralNetwork
{
public:
	Neuron *Neurons;
	int NeuronsSize = 0;
	vector<Connection*> Connections;
	int ConnectionsSize = 0;
	GeneralNeuralNetwork()
	{
	}

	void CreateNeurons(int Count)
	{
		Neurons = new Neuron[Count];
		for (int i = 0; i < Count; i++)
		{

		}
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
		//return in > 0 ? 1 : 0;
		double sig = Sigmoid(in);

		return sig*(1 - sig);
	}
	double Sigmoid(double in)
	{
		//return in > 0 ? in : 0;
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
				ThisNeuron.V = 0;
				for (int i = 0; i < ThisNeuron.InputsSize; i++)
				{
					ThisNeuron.V += ThisNeuron.Inputs[i]->From->Value*ThisNeuron.Inputs[i]->Weight;
				}

				ThisNeuron.Value = Sigmoid(ThisNeuron.V);
			}

		}
	}
	void ClearUp()
	{
		for (int i = 0; i < NeuronsSize; i++)
		{
			Neurons[i].Value = NAN;
			Neurons[i].J = NAN;
			Neurons[i].V = NAN;
		}
	}
	double LearningRate = 0.2;
	void BackPropagate(DesiredOutput **DesiredOutputs, int DesiredOutputsLength,bool Softmax)
	{
double TotalSum=0;
if(Softmax)
{
		for (int i = 0; i < DesiredOutputsLength; i++)
		{
			DesiredOutput &DO = *(DesiredOutputs[i]);
			Neuron &TargetNeuron = Neurons[DO.index];
			TotalSum+=TargetNeuron.Value;
		}
}
		for (int i = 0; i < DesiredOutputsLength; i++)
		{
			DesiredOutput &DO = *(DesiredOutputs[i]);
			Neuron &TargetNeuron = Neurons[DO.index];
if(Softmax)
TargetNeuron.J = (DO.value - TargetNeuron.Value/TotalSum)/TotalSum;
else
			TargetNeuron.J = SigmoidDerivative(TargetNeuron.V)*(DO.value - TargetNeuron.Value);
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
					double Sa = TargetNeuron.Value*TargetNeuron.Outputs[j]->To->J*LearningRate;

					TargetNeuron.Outputs[j]->ShouldAdd += Sa;

					AvgWj += TargetNeuron.Outputs[j]->To->J*TargetNeuron.Outputs[j]->Weight;
				}

				if (i > 0)
				{
					TargetNeuron.J = SigmoidDerivative(TargetNeuron.V)*(AvgWj / TargetNeuron.OutputsSize);
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
