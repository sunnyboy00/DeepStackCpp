#include "bet_sizing.h"


bet_sizing::bet_sizing(VectorXf pot_fractions = VectorXf())
{
	_pot_fractions = pot_fractions;

	if (pot_fractions.size() == 0)
	{
		_pot_fractions.resize(1);
		_pot_fractions.setOnes();
	}
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
	min_raise_size = std::max(min_raise_size, ante);
	min_raise_size = std::min(max_raise_size, min_raise_size);
	if (min_raise_size == 0)
	{
		ArrayX2f ar(0, 0);
		return ar;
	}
	else if (min_raise_size == max_raise_size)
	{
		ArrayX2f out(1, 2);
		out.fill((float)opponent_bet);
		out(1, current_player) = (float)(opponent_bet + min_raise_size);
		return out;
	}
	else
	{
		// --iterate through all bets and check if they are possible
		long long max_possible_bets_count = _pot_fractions.size() + 1; // --we can always go allin
		ArrayX2f out(max_possible_bets_count, 2);
		out.fill((float)opponent_bet); //Perf: don't need to fill all array that will be overwrited

		// --take pot size after opponent bet is called
		long long pot = opponent_bet * 2;
		long long used_bets_count = 0;
		// --try all pot fractions bet and see if we can use them
		for (int i = 0; i < _pot_fractions.size(); i++)
		{
			long long raise_size = pot * (long long)_pot_fractions(i);
			if (raise_size >= min_raise_size && raise_size < max_raise_size)
			{
				used_bets_count++;
				out(used_bets_count, current_player) = (float)(opponent_bet + raise_size);
			}
		}

		// --adding allin
		used_bets_count++;
		assert(used_bets_count <= max_possible_bets_count);
		out(used_bets_count, current_player) = (float)(opponent_bet + max_raise_size);
		out.conservativeResize(1, used_bets_count);
		return out;
	}
}


