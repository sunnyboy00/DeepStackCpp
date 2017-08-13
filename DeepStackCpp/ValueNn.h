#pragma once
#include "CustomSettings.h"
#include <Eigen/Dense>

using namespace Eigen;

//-- - Wraps the calls to the final neural net.
class ValueNn
{
public:
	ValueNn();
	~ValueNn();

	// Constructor. Loads the neural net from disk.
	void LoadNNfromDick();

	//	-- - Gives the neural net output for a batch of inputs.
	//	-- @param inputs An NxI tensor containing N instances of neural net inputs.
	//	--See @{net_builder
	//} for details of each input.
	//-- @param output An NxO tensor in which to store N sets of neural net outputs.
	//--See @{net_builder} for details of each output.
	void get_value(Tf2 inputs, Tf2 output);
};

