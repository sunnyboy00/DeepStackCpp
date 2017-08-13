#include "cfrd_gadget.h"

cfrd_gadget::cfrd_gadget(Tf1& board, Tf1& player_range, Tf1& opponent_cfvs) 
{
	_input_opponent_range = player_range;
	_input_opponent_value = opponent_cfvs;
	_opponent_reconstruction_regret.resize(players_count, card_count);
	_play_current_strategy.setConstant(0);
	_terminate_current_strategy.setConstant(1.0);

	//--holds achieved CFVs at each iteration so that we can compute regret
	_total_values.resize(card_count);
	_total_values.setConstant(0);
	_play_regrets.setConstant(0);

	//--init range mask for masking out impossible hands
	CardArray posibleHaands = _card_tools.get_possible_hand_indexes(board);
	_range_mask = Util::CardArrayToTensor(posibleHaands);
}

Tf1 cfrd_gadget::compute_opponent_range(const Tf1& current_opponent_cfvs, size_t iteration)
{
	Tf1 play_values = current_opponent_cfvs;
	Tf1 terminate_values = _input_opponent_value;

	//--1.0 compute current regrets
	_total_values = play_values * _play_current_strategy;

	if (_total_values_p2.size() == 0)
	{
		_total_values_p2 = Tf1(_total_values);
	}
	
	_total_values_p2 = terminate_values * _terminate_current_strategy;
	_total_values += _total_values_p2;

	if (_play_current_regret.size() == 0) 
	{
		_play_current_regret = Tf1(play_values);
	}

	if (_terminate_current_regret.size() == 0) // ToDo: move to constructor
	{
		_terminate_current_regret.resize(card_count);
		_terminate_current_regret.setZero();
	}

	_play_current_regret = Tf1(play_values);
	_play_current_regret -= _total_values;

	_terminate_current_regret = Tf1(terminate_values);
	_terminate_current_regret -= _total_values;

	//--1.1 cumulate regrets
	_play_regrets += _play_current_regret;
	_terminate_regrets += _terminate_current_regret;

	//--2.0 we use cfr + in reconstruction
	Util::Clip(_terminate_regrets, regret_epsilon, max_number);
	Util::Clip(_play_regrets, regret_epsilon, max_number);

	_play_possitive_regrets = _play_regrets;
	_terminate_possitive_regrets = _terminate_regrets;

	//--3.0 regret matching
	_regret_sum = Tf1(_play_possitive_regrets);
	_regret_sum += _terminate_possitive_regrets;


	_play_current_strategy = Tf1(_play_possitive_regrets);
	_terminate_current_strategy = Tf1(_terminate_possitive_regrets);

	_play_current_strategy /=  _regret_sum;
	_terminate_current_strategy = _regret_sum;

	//--4.0 for poker, the range size is larger than the allowed hands
	//--we need to make sure reconstruction does not choose a range
	//--that is not allowed
	_play_current_strategy *= _range_mask;
	_terminate_current_strategy *= _range_mask;

	if (_input_opponent_range.size() == 0)
	{
		_input_opponent_range = Tf1(card_count);
		_input_opponent_range.setZero();
	}

	_input_opponent_range = Tf1(card_count);
	return _input_opponent_range;
}

