#include "cfrd_gadget.h"

cfrd_gadget::cfrd_gadget(ArrayXf& board, Tf1& player_range, Tf1& opponent_cfvs) 
{
	_input_opponent_value = opponent_cfvs;

	_play_current_strategy.resize(card_count);
	_play_current_strategy.setConstant(0);

	_terminate_regrets.resize(card_count);
	_terminate_regrets.setConstant(0);

	_terminate_current_strategy.resize(card_count);
	_terminate_current_strategy.setConstant(1.0);

	//--holds achieved CFVs at each iteration so that we can compute regret
	_total_values.resize(card_count);
	_total_values.setConstant(0);

	_play_regrets.resize(card_count);
	_play_regrets.setConstant(0);

	_regret_sum.resize(card_count);
	_regret_sum.setConstant(0);

	_terminate_current_regret.resize(card_count);
	_terminate_current_regret.setZero();

	_input_opponent_range.resize(card_count);
	_input_opponent_range.setZero();

	//--init range mask for masking out impossible hands
	CardArray posibleHaands = _card_tools.get_possible_hand_indexes(board);
	_range_mask = Util::CardArrayToTensor(posibleHaands);
}

Tf1 cfrd_gadget::compute_opponent_range(const Tf1& current_opponent_cfvs)
{
	Tf1 play_values = current_opponent_cfvs;
	Tf1& terminate_values = _input_opponent_value;

	//--1.0 compute current regrets
	_total_values = play_values * _play_current_strategy; /* In the beginning we assume that we never Follow 
														  and always terminate. For this _play_current_strategy is
														  zero in the beginning.*/

	//				 (_total_values_p2                             )
	_total_values += terminate_values * _terminate_current_strategy; /* In the beginning we assume that we are
																	 ending up(Terminate) with initial opponent
																	 CFVS. _terminate_current_strategy is one.*/
	
	/* = current_opponent_cfvs - current_opponent_cfvs * _play_current_strategy - opponent_cfvs * _terminate_current_strategy
	current_opponent_cfvs * (1 - _play_current_strategy) - opponent_cfvs * _terminate_current_strategy
	*/


	_play_current_regret = play_values - _total_values; // Remove this extra variable

	_terminate_current_regret = terminate_values - _total_values;
	/*= opponent_cfvs - current_opponent_cfvs * (1 - _play_current_strategy) - opponent_cfvs * _terminate_current_strategy =
	opponent_cfvs * (1 - _terminate_current_strategy) - current_opponent_cfvs * (1 - _play_current_strategy)
	*/

	//--1.1 cumulate regrets
	_play_regrets += _play_current_regret;
	_terminate_regrets += _terminate_current_regret;

	//--2.0 we use cfr+ in reconstruction
	Util::Clip(_terminate_regrets, regret_epsilon, max_number);
	Util::Clip(_play_regrets, regret_epsilon, max_number);

	//--3.0 regret matching
	_regret_sum += _terminate_regrets;

	_play_current_strategy = _play_regrets /  _regret_sum;
	_terminate_current_strategy = _terminate_regrets / _regret_sum;

	//--4.0 for poker, the range size is larger than the allowed hands
	//--we need to make sure reconstruction does not choose a range
	//--that is not allowed
	_play_current_strategy *= _range_mask;
	_terminate_current_strategy *= _range_mask;

	_input_opponent_range = Tf1(_play_current_strategy);

	return _input_opponent_range;
}

