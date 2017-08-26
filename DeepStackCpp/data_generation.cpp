#include "data_generation.h"



data_generation::data_generation()
{
}


data_generation::~data_generation()
{
}

void _print_time(std::chrono::duration<double> diff)
{
	std::cout << duration_cast<std::chrono::hours>(diff).count() << " h "
		<< duration_cast<std::chrono::minutes>(diff).count() << " m "
		<< diff.count() << " s";
}

void data_generation::generate_data(size_t train_data_count, size_t valid_data_count)
{
	auto start = high_resolution_clock::now();

	std::cout << "'Generating validation data ... ";


	auto end = high_resolution_clock::now();
	std::cout << "Validation gen time: ";
	_print_time(end - start);

	std::cout << "Generating data file... ";

	std::cout << "Generation data file time: ";
}

void _save_file(const char* filename, ArrayXX& dataArray)
{
	const int mode = ios::out | ios::binary | ios::trunc;
	std::ofstream out(filename, mode);
	typename ArrayXX::Index rows = dataArray.rows(), cols = dataArray.cols();
	out.write((char*)(&rows), sizeof(typename ArrayXX::Index));
	out.write((char*)(&cols), sizeof(typename ArrayXX::Index));
	out.write((char*)dataArray.data(), rows*cols * sizeof(float));
	out.close();
}

//void _read_file(const char* filename, ArrayXX& dataArray)
//{
//	std::ifstream in(filename, ios::in | std::ios::binary);
//	typename ArrayXX::Index rows = 0, cols = 0;
//	in.read((char*)(&rows), sizeof(typename ArrayXX::Index));
//	in.read((char*)(&cols), sizeof(typename ArrayXX::Index));
//	dataArray.resize(rows, cols);
//	in.read((char *)dataArray.data(), rows*cols * sizeof(float));
//	in.close();
//}

void data_generation::generate_data_file(size_t data_count, string file_name)
{
		range_generator rng_generator;
		bucket_conversion b_conversion;
		random_card_generator card_generator;

		size_t batch_size = gen_batch_size;
		assert(data_count % batch_size == 0 && "data count has to be divisible by the batch size");
		size_t batch_count = (size_t)(data_count / batch_size);
		bucketer buck;
		size_t bucket_count = buck.get_bucket_count();
		size_t target_size = bucket_count * players_count;
		ArrayXX  targets(data_count, target_size);
		size_t input_size = bucket_count * players_count + 1;
		ArrayXX inputs(data_count, input_size);
		ArrayXX mask = ArrayXX::Zero(data_count, bucket_count);

		//-- A mask of possible buckets
		const ArrayXX bucket_mask = b_conversion.get_possible_bucket_mask();

		for (size_t batch = 1; batch < batch_count; batch++)
		{
			ArrayX board = card_generator.generate_cards(board_card_count);
			rng_generator.set_board(board);
			b_conversion.set_board(board);

			//--generating ranges players_count x batch_size x card_count
			ArrayXX ranges[players_count];

			ArrayXX ranges(players_count, batch_size, card_count);
			for (int player = 1; player < players_count; player++)
			{
				rng_generator.generate_range(ranges[player]);
			}

			//--generating pot sizes between ante and stack - 0.1
			float min_pot = ante;
			float max_pot = stack - 0.1;
			float pot_range = max_pot - min_pot;

			ArrayXX random_pot_sizes = ArrayXX::Random(gen_batch_size, 1) * pot_range + min_pot;

			//--pot features are pot sizes normalized between(ante / stack, 1)
			ArrayXX pot_size_features = random_pot_sizes / stack;

			//--translating ranges to features
			size_t batchStart = (batch - 1) * batch_size + 1;
			size_t batchEnd = batch * batch_size;
			size_t pot_feature_index = input_size;

			memcpy(inputs.data() + batchStart*pot_feature_index,
				pot_size_features.data(),
				pot_size_features.size() * sizeof(float));

			for (int playerId = 0; playerId < players_count; playerId++)
			{
				AmAxx imputsMap(inputs.data() + batchStart*playerId, bucket_count);
				b_conversion.card_range_to_bucket_range(ranges[playerId], imputsMap);
			}

			//--computation of values using re - solving
			// (players_count x batch_size x card_count)
			ArrayXX values[players_count];
			for (size_t i = 0; i < batch_size; i++)
			{
				Resolving resolving;
				Node current_node;
				current_node.board = board;
				current_node.street = 2;
				current_node.current_player = P1;
				size_t pot_size = pot_size_features(i, 0) * stack;
				current_node.bets(0) = pot_size;
				current_node.bets(1) = pot_size;

				ArrayX p1_range = ranges[P1].row(i);
				ArrayX p2_range = ranges[P2].row(i);
				resolving.resolve_first_node(current_node, p1_range, p2_range);
				ArrayXX root_values = resolving.get_root_cfv_both_players();
				root_values /= pot_size;

				size_t target = players_count * i * card_count;

				//Remove3D(values, i) = root_values;
				for (int playerId = 0; playerId < players_count; playerId++)
				{
					//--translating values to nn targets
					auto cardRange = values[playerId].row(card_count);
					AmAx backetRange(targets.data(), card_count);
					b_conversion.card_range_to_bucket_range(cardRange, backetRange);

				}

				mask.row(i) = bucket_mask.replicate(batch_size, bucket_count);
			}

			_save_file("C:\\data\\inputs.bin", inputs);
			_save_file("C:\\data\\targets.bin", targets);
			_save_file("C:\\data\\mask.bin", mask);
		}
}
