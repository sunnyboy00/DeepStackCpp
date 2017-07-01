#include "bet_sizing.h"

bet_sizing::bet_sizing(VectorXf* pot_fractions = nullptr)
{
	if (pot_fractions == nullptr)
	{
		_pot_fractions = new VectorXf(1);
		_pot_fractions->setOnes();
	}
	else
	{
		_pot_fractions = pot_fractions;
	}
}

bet_sizing::~bet_sizing()
{
	delete(_pot_fractions);
}

ArrayX2f bet_sizing::get_possible_bets(GameState& node)
{
	int current_player = node.current_player;
	assert(current_player == 1 || current_player == 2 && "Wrong player for bet size computation");
	int opponent = 3 - node.current_player;
	long long opponent_bet = node.bets[opponent];

	assert(node.bets[current_player] <= opponent_bet);

	//--compute min possible raise size
	long long max_raise_size = stack - opponent_bet;
	long long min_raise_size = opponent_bet - node.bets[current_player];
	long long min_raise_size = std::max(min_raise_size, ante);
	long long min_raise_size = std::min(max_raise_size, min_raise_size);
	if (min_raise_size == 0)
	{
		return 
	}
}


