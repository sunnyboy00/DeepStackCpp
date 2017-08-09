#include "lookahead.h"



lookahead::lookahead()
{
}


lookahead::~lookahead()
{
}

void lookahead::resolve_first_node(Tf1& player_range, Tf1& opponent_range)
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
		/*int gp_layer_terminal_actions_count = terminal_actions_count[d - 2];
		int ggp_layer_nonallin_bets_count = nonallinbets_count[d - 3];

			cfvs_data[d][{ {}, {}, {}, { 1 }, {}}]:cmul(self.empty_action_mask[d])
			self.cfvs_data[d][{ {}, {}, {}, { 2 }, {}}] : cmul(self.empty_action_mask[d])

			self.placeholder_data[d] : copy(self.cfvs_data[d])

			--player indexing is swapped for cfvs
			self.placeholder_data[d][{ {}, {}, {}, self.acting_player[d], {}}] : cmul(self.current_strategy_data[d])

			torch.sum(self.regrets_sum[d], self.placeholder_data[d], 1)

			--use a swap placeholder to change{ { 1,2,3 },{ 4,5,6 } } into{ { 1,2 },{ 3,4 },{ 5,6 } }
			local swap = self.swap_data[d - 1]
			swap : copy(self.regrets_sum[d])
			self.cfvs_data[d - 1][{ {gp_layer_terminal_actions_count + 1, -1}, { 1, ggp_layer_nonallin_bets_count }, {}, {}, {}}] : copy(swap : transpose(2, 3))*/
	}
}

void lookahead::_compute()
{
	//--1.0 main loop
	for (size_t iter = 0; iter < cfr_iters; iter++)
	{
		//_set_opponent_starting_range(iter);
		//_compute_current_strategies();
		//_compute_ranges();
		//_compute_update_average_strategies(iter);
		//_compute_terminal_equities();
		//_compute_cfvs();
		//_compute_regrets();
		//_compute_cumulate_average_cfvs(iter);
	}

	//--2.0 at the end normalize average strategy
	//_compute_normalize_average_strategies();
	//--2.1 normalize root's CFVs
	//_compute_normalize_average_cfvs()
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
