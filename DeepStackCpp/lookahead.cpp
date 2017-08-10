#include "lookahead.h"



lookahead::lookahead()
{
}


lookahead::~lookahead()
{
}

void lookahead::resolve_first_node(const Tf1& player_range, const Tf1& opponent_range)
{
	RemoveF4D(ranges_data[1], 0, 0, 0, 0) = player_range;
	RemoveF4D(ranges_data[1], 0, 0, 0, 1) = opponent_range;
	_compute();
}

void lookahead::resolve(Tf1& player_range, Tf1& opponent_cfvs)
{
	_reconstruction_gadget = new cfrd_gadget(tree.board, player_range, opponent_cfvs);
	RemoveF4D(ranges_data[1], 0, 0, 0, 0) = player_range;
	_reconstruction_opponent_cfvs = opponent_cfvs;
	_compute();
}

Tf1 lookahead::get_chance_action_cfv(int action_index, Tf2& board)
{
	Tf3 box_outputs;
	Tf3 next_street_box;
	int batch_index = 0;
	Tf4 pot_mult;

	assert(!((action_index == 0) && (first_call_terminal)));

	//--check if we should not use the first layer for transition call
	if (action_index == 0 && first_call_transition)
	{
		box_outputs = _next_street_boxes_inputs[1];
		box_outputs.setZero();
		assert(box_outputs.dimension(0) == 1);
		batch_index = 0;
		next_street_box = _next_street_boxes[1];
		pot_mult = RemoveF1D(pot_size[1], 1);
	}
	else
	{
		batch_index = action_index - 1; //--remove fold
		if (first_call_transition)
		{
			batch_index--;
		}

		box_outputs = _next_street_boxes_inputs[2];
		next_street_box = _next_street_boxes[2];
		pot_mult = RemoveF1D(pot_size[2], 1);;
	}

#ifdef DEBUG
	if (box_outputs.size() == 0)
	{
		assert(false);
	}
#endif // DEBUG


	//ToDo: uncomment when NN will be ready!
	//next_street_box.get_value_on_board(board, box_outputs)

	box_outputs *= pot_mult;

	Tf1 out = RemoveF2D(box_outputs, batch_index, 2 - tree.current_player);
	return out;
}

LookaheadResult lookahead::get_results()
{
	LookaheadResult out;

	const int actions_count = (int)average_strategies_data[1].dimension(0);
	 
	//--1.0 average strategy
	//--[actions x range]
	//--lookahead already computes the average strategy we just convert the dimensions
	const std::array<DenseIndex, 2> action_cards_dims = { actions_count, card_count};
	out.strategy = average_strategies_data[1].reshape(action_cards_dims);

	//--2.0 achieved opponent's CFVs at the starting node 
	const std::array<DenseIndex, 2> players_cards_dims = { players_count, card_count };
	Tf2 resized_average_cfvs = average_cfvs_data[0].reshape(players_cards_dims); //ToDo: copy

	out.achieved_cfvs = RemoveF1D(resized_average_cfvs, P1);

	//--3.0 CFVs for the acting player only when resolving first node
	if (_reconstruction_opponent_cfvs.size() > 0)
	{
		out.root_cfvs = RemoveF1D(resized_average_cfvs, P2);

		//--swap cfvs indexing
		out.root_cfvs_both_players = resized_average_cfvs; // !Copy Do we need this? Looks like we are overwriting below
		RemoveF1D(out.root_cfvs_both_players, P2) = out.achieved_cfvs;
		RemoveF1D(out.root_cfvs_both_players, P1) = out.root_cfvs;
	}

	//--4.0 children CFVs
	//--[actions x range]
	out.children_cfvs = Remove4D(average_cfvs_data[1], P1).reshape(action_cards_dims);

	//--IMPORTANT divide average CFVs by average strategy in here
	Tf2 scaler = average_strategies_data[1].reshape(action_cards_dims);

	const Eigen::array<DenseIndex, 2> s_dims = { 1, card_count };

	Tf2 range_mul = Remove4D(ranges_data[0], P1).reshape(s_dims);
	range_mul = Util::ExpandAs(range_mul, scaler);

	scaler *= range_mul;
	Tf2 scalerSum = Util::NoReductionSum(scaler, 1);

	scaler = Util::ExpandAs(scalerSum, range_mul);
	scaler *= scaler.constant(cfr_iters - cfr_skip_iters);

	out.children_cfvs /= scaler;

	assert(out.strategy.size() > 0);
	assert(out.achieved_cfvs.size() > 0);
	assert(out.children_cfvs.size() > 0);

	return out;
}

void lookahead::_compute_terminal_equities_next_street_box()
{
	assert(tree.street == 1);

	for (int d = 1; d <= depth; d++)
	{
		if (d > 1 || first_call_transition)
		{
			if (_next_street_boxes_inputs[d].size() == 0)
			{
				size_t totalLen = ranges_data[d].dimension(1) + ranges_data[d].dimension(2) + ranges_data[d].dimension(3) + ranges_data[d].dimension(4);
				_next_street_boxes_inputs[d] = Tf3(totalLen - players_count - card_count, players_count, card_count);
				_next_street_boxes_inputs[d].setZero();
			}

			if (_next_street_boxes_outputs[d].size() == 0)
			{
				_next_street_boxes_outputs[d] = _next_street_boxes_inputs[d];
			}

			//--now the neural net accepts the input for P1 and P2 respectively, so we need to swap the ranges if necessary
			Tf4 dataCopy = RemoveF1D(ranges_data[d], 1);// ToDo: copy

			CopyDif(_next_street_boxes_outputs[d], dataCopy);

			if (tree.current_player == P1)
			{
				Util::Copy(_next_street_boxes_inputs[d], _next_street_boxes_outputs[d]);
			}
			else
			{
				Tf2 p2Ranges = Remove2D(_next_street_boxes_outputs[d], P2);
				Remove2D(_next_street_boxes_inputs[d], P1) = Remove2D(_next_street_boxes_outputs[d], P2);
				Remove2D(_next_street_boxes_inputs[d], P2) = Remove2D(_next_street_boxes_outputs[d], P1);
			}

			//!!!!!!!!!!!!!!!!!!!!! ToDo: Uncomment after adding NN 
			//_next_street_boxes[d] : get_value(_next_street_boxes_inputs[d], self.next_street_boxes_outputs[d])

			//--now the neural net outputs for P1 and P2 respectively, so we need to swap the output values if necessary
			if (tree.current_player == 1)
			{
				Util::Copy(_next_street_boxes_inputs[d], _next_street_boxes_outputs[d]);

				Remove2D(_next_street_boxes_outputs[d], P1) = Remove2D(_next_street_boxes_inputs[d], P2);
				Remove2D(_next_street_boxes_outputs[d], P2) = Remove2D(_next_street_boxes_inputs[d], P1);
			}

			Tf4 cfvsCopy = RemoveF1D(cfvs_data[d], 1);
			CopyDif(cfvsCopy, _next_street_boxes_outputs[d]);
		}
	}
}

void lookahead::_compute_update_average_strategies(size_t iter)
{
	if (iter > cfr_skip_iters)
	{
		//--no need to go through layers since we care for the average strategy only in the first node anyway
		//--note that if you wanted to average strategy on lower layers, you would need to weight the current strategy by the current reach probability
		average_strategies_data[1] += current_strategy_data[1];
	}
}

void lookahead::_compute_terminal_equities_terminal_equity()
{
	for (int d = 1; d <= depth; d++)
	{
		//--call term eq evaluation
		if (tree.street == 1)
		{
			if (d > 1 || first_call_terminal)
			{
				//Eigen::array<Eigen::DenseIndex, 2> rangesDims = { players_count, card_count };
				Tf3 ranges = RemoveF2D(ranges_data[d], 1, -1);//.reshape(rangesDims); // ToDo: Extra copy
				Tf3 cfvs = RemoveF2D(cfvs_data[d], 1, -1);// .reshape(rangesDims);

				_terminal_equity.call_value(ToTmxx(ranges, players_count, card_count), ToTmxx(cfvs, players_count, card_count));
			}
		}
		else
		{
			assert(tree.street == 2);
					//--on river, any call is terminal
			if (d > 1 || first_call_terminal)
			{
				Tf4 ranges = RemoveF1D(ranges_data[d], 1); // ToDo: Extra copy
				Tf4 cfvs = RemoveF1D(cfvs_data[d], 1);
				_terminal_equity.call_value(ToTmxx(ranges, players_count, card_count), ToTmxx(cfvs, players_count, card_count));
			}
			

			//--folds
			Tf4 ranges = RemoveF1D(ranges_data[d], 0); // ToDo: Extra copy
			Tf4 cfvs = RemoveF1D(cfvs_data[d], 0);

			_terminal_equity.fold_value(ToTmxx(ranges, players_count, card_count), ToTmxx(cfvs, players_count, card_count));

			//--correctly set the folded player by multiplying by - 1
			float fold_mutliplier = (acting_player[d] * 2 - 3);// ?

			auto chip1 = cfvs_data[d].chip(0, 0);
			chip1 *= chip1.constant(fold_mutliplier);

			auto chip2 = cfvs_data[d].chip(0, 0);
			chip2 *= chip2.constant(-fold_mutliplier);
		}
	}
}

void lookahead::_compute_terminal_equities()
{
	if (tree.street == 1)
	{
		_compute_terminal_equities_next_street_box();
	}

	_compute_terminal_equities_terminal_equity();

		//--multiply by pot scale factor
	for (int d = 1; d <= _depth; d++)
	{
		cfvs_data[d] *= pot_size[d];
	}
}

void lookahead::_compute_cfvs()
{
	for (int d = _depth; d > 1; d--)
	{
		int gp_layer_terminal_actions_count = terminal_actions_count[d - 2];
		int ggp_layer_nonallin_bets_count = nonallinbets_count[d - 3];
		Remove4D(cfvs_data[d], P1) *= empty_action_mask[d];
		Remove4D(cfvs_data[d], P2) *= empty_action_mask[d];
		Util::Copy(placeholder_data[d], cfvs_data[d]);

		//--player indexing is swapped for cfvs
		Remove4D(placeholder_data[d], (int)acting_player[d]) *= current_strategy_data[d];
		std::array<int, 1> dims = { 0 };
		regrets_sum[d] = placeholder_data[d].sum(dims);

		//--use a swap placeholder to change{ { 1,2,3 },{ 4,5,6 } } into{ { 1,2 },{ 3,4 },{ 5,6 } }
		auto swap = swap_data[d - 1];
		CopyDif(swap, regrets_sum[d]);

		std::array<std::array<DenseIndex, 2>, 5> const slices =
		{ { { gp_layer_terminal_actions_count, -1 }, { 0, ggp_layer_nonallin_bets_count - 1 },{ 0 , -1 },{ 0 , -1 },{ 0, -1 } } };

		Util::CopyToSlice(cfvs_data[d - 1], slices, Util::Transpose(swap, { 1, 2 }));
	}
}

void lookahead::_compute_cumulate_average_cfvs(size_t iter)
{
	if (iter > cfr_skip_iters)
	{
		average_cfvs_data[P1] += cfvs_data[P1];
		average_cfvs_data[P2] += cfvs_data[P2];
	}
}

void lookahead::_compute_normalize_average_strategies()
{
	Tensor<float, 2> input(7, 11);
	std::array<DenseIndex, 3> three_dims{ { 7, 11, 1 } };
	Tensor<float, 3> result1 = input.reshape(three_dims);

	// Decrease the rank of the input tensor by merging 2 dimensions;
	std::array<DenseIndex, 1> one_dim{ { 7 * 11 } };
	Tensor<float, 1> result2 = input.reshape(one_dim);

	//--using regrets_sum as a placeholder container
	auto player_avg_strategy = average_strategies_data[1];
	auto player_avg_strategy_sum = regrets_sum[1];

	player_avg_strategy_sum = Util::NoReductionSum(player_avg_strategy, 0);
	player_avg_strategy /= Util::ExpandAs(player_avg_strategy_sum, player_avg_strategy);

	//--if the strategy is 'empty' (zero reach), strategy does not matter but we need to make sure
	//--it sums to one->now we set to always fold
	//ToDo: BuG! Fix!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//player_avg_strategy[1][player_avg_strategy[1]:ne(player_avg_strategy[1])] = 1
	//player_avg_strategy[player_avg_strategy:ne(player_avg_strategy)] = 0
}

void lookahead::_compute_regrets()
{
	for (int d = _depth; d > 1; d--)
	{
		int gp_layer_terminal_actions_count = terminal_actions_count[d - 2];
		int gp_layer_bets_count = bets_count[d - 2];
		int ggp_layer_nonallin_bets_count = nonallinbets_count[d - 3];

		Tf4 current_regrets = current_regrets_data[d];
		DenseIndex cur_acting_player = (DenseIndex)acting_player[d];
		Tf4 dataToCopy = Remove4D(cfvs_data[d], cur_acting_player);
		Util::Copy(current_regrets, dataToCopy);
		Tf5 next_level_cfvs = cfvs_data[d - 1];

		auto parent_inner_nodes = inner_nodes_p1[d - 1];
		std::array<std::array<DenseIndex, 2>, 5> const slices =
		{ {{ gp_layer_terminal_actions_count, -1 },{ 0, ggp_layer_nonallin_bets_count - 1 },{ 0 , -1 }, { cur_acting_player, cur_acting_player}, { 0 , -1 }} };
		Tf5 sliceData = Util::Slice(next_level_cfvs, slices);
		Util::Copy(parent_inner_nodes, Util::Transpose(next_level_cfvs, { 1, 2 }));

		std::array<int, 4> sizes = { 1, gp_layer_bets_count, -1, card_count };
		Util::ProcessSizes((int)parent_inner_nodes.size(), sizes);
		Tm4 parrentMap(parent_inner_nodes.data(), sizes[0], sizes[1], sizes[2], sizes[3]);

		Tf4 ex_parent_inner_nodes = Util::ExpandAs(parrentMap, current_regrets);

		current_regrets /= ex_parent_inner_nodes;

		regrets_data[d] += current_regrets;

		//--(CFR + )
		Util::ClipLow(regrets_data[d], 0);
	}
}

void lookahead::_set_opponent_starting_range(size_t iteration)
{
	if (_reconstruction_opponent_cfvs.size() > 0)
	{
		//--note that CFVs indexing is swapped, thus the CFVs for the reconstruction player are for player '1'
		const std::array<DenseIndex, 1> dims = { 1 };
		Tf1 opponent_range = _reconstruction_gadget->compute_opponent_range(Remove4D(cfvs_data[0], P1).reshape(dims), iteration);
		Remove4D(ranges_data[0], P2) = opponent_range;
	}
}

void lookahead::_compute_normalize_average_cfvs()
{
	average_cfvs_data[0] /= average_cfvs_data[0].constant(cfr_iters - cfr_skip_iters);
}

void lookahead::_compute()
{
	//--1.0 main loop
	for (size_t iter = 0; iter < cfr_iters; iter++)
	{
		_set_opponent_starting_range(iter);
		_compute_current_strategies();
		_compute_ranges();
		_compute_update_average_strategies(iter);
		_compute_terminal_equities();
		_compute_cfvs();
		_compute_regrets();
		_compute_cumulate_average_cfvs(iter);
	}

	//--2.0 at the end normalize average strategy
	_compute_normalize_average_strategies();
	//--2.1 normalize root's CFVs
	_compute_normalize_average_cfvs();
}

void lookahead::_compute_current_strategies()
{
	for (int d = 0; d < _depth; d++)
	{
		positive_regrets_data[d] = regrets_data[d];
		Util::ClipLow(positive_regrets_data[d], regret_epsilon);

		//--1.0 set regret of empty actions to 0
		positive_regrets_data[d] *= empty_action_mask[d]; //ToDo: how can we get non zero regret of impossible actions? Remove?

		//--1.1  regret matching
		//--note that the regrets as well as the CFVs have switched player indexing
		std::array<int, 1> dims = { 0 };
		regrets_sum[d].chip(0, 0) = positive_regrets_data[d].sum(dims);

		Tf4 player_current_strategy = current_strategy_data[d];
		Tf4 player_regrets = positive_regrets_data[d];
		Tf4 player_regrets_sum = regrets_sum[d];

		player_current_strategy = player_regrets / Util::ExpandAs(player_regrets_sum, player_regrets);
	}
}

void lookahead::_compute_ranges()
{
	for (int d = 0; d < _depth; d++)
	{
		Tf5& current_level_ranges = ranges_data[d];
		Tf5& next_level_ranges = ranges_data[d + 1]; // ToDo: check Is this a copy or a ref to original tensor?

		int prev_layer_terminal_actions_count = terminal_actions_count[d - 1];
		int prev_layer_actions_count = actions_count[d - 1];
		int prev_layer_bets_count = bets_count[d - 1];
		int gp_layer_nonallin_bets_count = nonallinbets_count[d - 2];
		int gp_layer_terminal_actions_count = terminal_actions_count[d - 2];


		//for (size_t action = prev_layer_terminal_actions_count; action < current_level_ranges.dimension(0); action++)
		//{
		//	for (size_t parentAction = 0; parentAction < gp_layer_nonallin_bets_count; parentAction++)
		//	{

		//	}
		//}

		//--copy the ranges of inner nodes and transpose
		Tf5& betActionsRanges = Util::Slice(current_level_ranges,
		{ { { prev_layer_terminal_actions_count, -1 }, { 1, gp_layer_nonallin_bets_count }, {0, -1}, {0 , -1}, {0, -1} } });
		inner_nodes[d] = Util::Transpose(betActionsRanges, { 1 , 2 }); // Shuffle parent and gp axis;
		std::array<int, 5> sizes = { { 1, prev_layer_bets_count, -1, players_count, card_count } };
		Tm5& super_view = Util::View(inner_nodes[d], sizes);
		Tf5& super_view_t = Util::ExpandAs(super_view, betActionsRanges); // ToDo: Extra copy perf hit
		Util::Copy(next_level_ranges, super_view_t);
		Tf4& next_level_strategies = current_strategy_data[d + 1];

		//--multiply the ranges of the acting player by his strategy
		DenseIndex curPlayer = (DenseIndex)acting_player[d];

		Util::Slice(next_level_ranges,
		{ { { 0, -1 },{ 0, -1 },{ 0, -1 },{ curPlayer, curPlayer },{ 0, -1 } } }) *= next_level_strategies;
	}
}
