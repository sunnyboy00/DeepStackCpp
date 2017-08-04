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
	for (size_t d = 0; d < _depth; d++)
	{
		positive_regrets_data[d] = regrets_data[d];
		Util::ClipLow(positive_regrets_data[d], regret_epsilon);

		//--1.0 set regret of empty actions to 0
		positive_regrets_data[d] *= empty_action_mask[d]; //ToDo: how can we get non zero regret of impossible actions? Remove?

		//--1.1  regret matching
		//--note that the regrets as well as the CFVs have switched player indexing
		regrets_sum[d] = positive_regrets_data[d].sum(0);
		Tf4 player_current_strategy = current_strategy_data[d];
		Tf4 player_regrets = positive_regrets_data[d];
		Tf4 player_regrets_sum = regrets_sum[d];

		player_current_strategy = player_regrets / player_regrets_sum.broadcast(player_regrets);
	}
}

void lookahead::_compute_ranges()
{
	for (int d = 0; d < _depth; d++)
	{
		Tf5 current_level_ranges = ranges_data[d];
		Tf5 next_level_ranges = ranges_data[d + 1];

		int prev_layer_terminal_actions_count = terminal_actions_count[d - 1];
		int prev_layer_actions_count = actions_count[d - 1];
		int prev_layer_bets_count = bets_count[d - 1];
		int gp_layer_nonallin_bets_count = nonallinbets_count[d - 2];
		int gp_layer_terminal_actions_count = terminal_actions_count[d - 2];


		//--copy the ranges of inner nodes and transpose
		for (size_t action = prev_layer_terminal_actions_count; action < current_level_ranges.dimension(0); action++)
		{
			for (size_t parentAction = 0; parentAction < gp_layer_nonallin_bets_count; parentAction++)
			{

			}
		}



		//inner_nodes[d]:copy(current_level_ranges[{ {prev_layer_terminal_actions_count + 1, -1}, { 1, gp_layer_nonallin_bets_count }, {}, {}, {}}]:transpose(2, 3))

		//	local super_view = inner_nodes[d]
		//	super_view = super_view : view(1, prev_layer_bets_count, -1, constants.players_count, game_settings.card_count)

		//	super_view = super_view : expandAs(next_level_ranges)
		//	local next_level_strategies = current_strategy_data[d + 1]

		//	next_level_ranges : copy(super_view)

		//	//--multiply the ranges of the acting player by his strategy
		//	next_level_ranges[{ {}, {}, {}, acting_player[d], {}}] : cmul(next_level_strategies)
	}
}
