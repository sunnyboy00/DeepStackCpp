#include "bet_sizing.h"

bet_sizing_manager::bet_sizing_manager(const VectorXf& pot_fractions)
{
	_pot_fractions = pot_fractions;

	if (pot_fractions.size() == 0)
	{
		_pot_fractions.resize(1);
		_pot_fractions.setOnes();
	}
}

bet_sizing_manager::bet_sizing_manager() : bet_sizing_manager(bet_sizing) {}

ArrayX2f bet_sizing_manager::get_possible_bets(const Node& node)
{
	assert(node.current_player == 1 || node.current_player == 2 && "Wrong player for bet size computation");

	int current_player_z = node.current_player - 1; // -1 because in tourch index is 1 based.
	int opponent = 2 - node.current_player;
	float opponent_bet = node.bets(opponent);

	assert(node.bets(current_player_z) <= opponent_bet);

	//compute min possible raise size
	float max_raise_size = stack - opponent_bet;
	float min_raise_size = opponent_bet - node.bets(current_player_z);
	min_raise_size = max(min_raise_size, ante);
	min_raise_size = min(max_raise_size, min_raise_size);
	if (min_raise_size == 0)
	{
		//DebugBreak();
		ArrayX2f ar;
		return ar;
	}
	else if (min_raise_size == max_raise_size)
	{
		ArrayX2f out(1, 2);
		out.fill((float)opponent_bet);
		out(0, current_player_z) = (float)(opponent_bet + min_raise_size);
		return out;
	}
	else
	{
		// iterate through all bets and check if they are possible
		long long max_possible_bets_count = _pot_fractions.size() + 1; // we can always go allin
		ArrayX2f out(max_possible_bets_count, player_count);
		out.fill((float)opponent_bet); // Warning: Perf: don't need to fill all array that will be overwrited

		// take pot size after opponent bet is called
		float pot = opponent_bet * 2;
		long long used_bets_count = 0;
		// try all pot fractions bet and see if we can use them
		for (int i = 0; i < _pot_fractions.size(); i++)
		{
			float raise_size = pot * _pot_fractions(i);
			if (raise_size >= min_raise_size && raise_size < max_raise_size)
			{
				//used_bets_count++;
				out(used_bets_count, current_player_z) = opponent_bet + raise_size;
				used_bets_count++;
			}
		}

		out(used_bets_count, current_player_z) = (float)(opponent_bet + max_raise_size);

		// adding allin
		used_bets_count++;
		assert(used_bets_count <= max_possible_bets_count);
		out.conservativeResize(used_bets_count, player_count);
		return out;
	}
}

void bet_sizing_manager::SetPotFraction(const VectorXf& potFractions)
{
	_pot_fractions = potFractions;
}


