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

			local player_indexes = { { 1, bucket_count },{ bucket_count + 1, bucket_count * 2 } }
			for (int player = 0; player < players_count; player++)
			{
				local player_index = player_indexes[player];
				b_conversion.card_range_to_bucket_range(ranges[player], inputs[{batch_index, player_index}]);
			}

			//--computation of values using re - solving
			ArrayXX values(players_count, batch_size, card_count)
				for (size_t i = 0; i < batch_size; i++)
				{
					local resolving = Resolving()
						local current_node = {}

						current_node.board = board
						current_node.street = 2
						current_node.current_player = constants.players.P1
						local pot_size = pot_size_features[i][1] * arguments.stack
						current_node.bets = arguments.Tensor{ pot_size, pot_size }
						local p1_range = ranges[1][i]
						local p2_range = ranges[2][i]
						resolving:resolve_first_node(current_node, p1_range, p2_range)
						local root_values = resolving : get_root_cfv_both_players()
						root_values : mul(1 / pot_size)
						values[{ {}, i, {}}] : copy(root_values)
				}


			//--translating values to nn targets
			for (int player = 0; player < players_count; player++)
			{
				local player_index = player_indexes[player];
				b_conversion.card_range_to_bucket_range(values[player], targets[{batch_index, player_index}]);
			}
			//--computing a mask of possible buckets
			ArrayXX bucket_mask = b_conversion.get_possible_bucket_mask();
			mask[{batch_index, {}}] : copy(bucket_mask : expand(batch_size, bucket_count))
		}

		torch.save(file_name .. '.inputs', inputs:float())
		torch.save(file_name .. '.targets', targets:float())
		torch.save(file_name .. '.mask', mask:float())
}
