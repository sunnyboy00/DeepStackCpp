#pragma once
#include "CustomSettings.h"
#include "bucketer.h"
#include "bucket_conversion.h"
#include "random_card_generator.h"
#include  "range_generator.h"
#include "arguments.h"
#include <iostream>
#include <chrono>
#include <vector>

using namespace std;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;

class data_generation
{
public:
	data_generation();
	~data_generation();

	//	-- - Generates training and validation files by sampling random poker
	//	-- situations and solving them.
	//	--
	//	--Makes two calls to @{generate_data_file
	//}.The files are saved to
	//-- @{arguments.data_path}, respectively appended with `valid` and `train`.
	//--
	//-- @param train_data_count the number of training examples to generate
	//-- @param valid_data_count the number of validation examples to generate
	void generate_data(size_t train_data_count, size_t valid_data_count);

	//	-- - Generates data files containing examples of random poker situations with
	//	-- counterfactual values from an associated solution.
	//	--
	//	--Each poker situation is randomly generated using @{range_generator
	//} and
	//-- @{random_card_generator}.For description of neural net input and target
	//-- type, see @{net_builder}.
	//--
	//-- @param data_count the number of examples to generate
	//-- @param file_name the prefix of the files where the data is saved(appended
	//	-- with `.inputs`, `.targets`, and `.mask`).
	void generate_data_file(size_t data_count, string file_name);

private:

	void _print_time(std::chrono::duration<double> diff);
};

