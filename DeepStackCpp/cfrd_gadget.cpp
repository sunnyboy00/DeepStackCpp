#include "cfrd_gadget.h"

cfrd_gadget::cfrd_gadget(const ArrayX& board, const Range& player_range, const Range& opponent_cfvs)
{
	_input_opponent_value = opponent_cfvs;

	_play_current_strategy.setConstant(0);
	_terminate_regrets.setConstant(0);
	_terminate_current_strategy.setConstant(1.0);

	//--holds achieved CFVs at each iteration so that we can compute regret
	_total_values.setConstant(0);

	_play_regrets.setConstant(0);
	_regret_sum.setConstant(0);
	_terminate_current_regret.setZero();
	_input_opponent_range.setZero();

	//--init range mask for masking out impossible hands
	_range_mask = _card_tools.get_possible_hand_indexes(board);
}

Range cfrd_gadget::compute_opponent_range(const Range& current_opponent_cfvs)
{
	Range play_values = current_opponent_cfvs;
	Range& terminate_values = _input_opponent_value;

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
	_regret_sum = _play_regrets + _terminate_regrets;

	_play_current_strategy = _play_regrets / _regret_sum;
	_terminate_current_strategy = _terminate_regrets / _regret_sum;

	//--4.0 for poker, the range size is larger than the allowed hands
	//--we need to make sure reconstruction does not choose a range
	//--that is not allowed
	_play_current_strategy *= _range_mask;
	_terminate_current_strategy *= _range_mask;

	_input_opponent_range = _play_current_strategy;

	return _input_opponent_range; // Only in this range opponent have reason to play(F) and not to fall-back to trunk strategy(T) 
}

