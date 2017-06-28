#include "card_tools.h"
#include <assert.h>
#include <unordered_map>



card_tools::card_tools()
{
}

bool card_tools::hand_is_possible(ArrayXi hand)
{
	assert(hand.minCoeff() > 0 && hand.maxCoeff() <= card_count   && "Illegal cards in hand");
	std::unordered_map<int, bool> suit_table;

	for (int card  = 0; card < hand.cols(); card++)
	{
		if (suit_table[card])
			return false;

		suit_table[card] = true;
	}

	return true;
}

ArrayXi card_tools::get_possible_hand_indexes(ArrayXi board)
{
	Matrix<mainDataType, card_count, 1> out;
	out.setZero();

	if (board.size() == 0)
	{
		out.setOnes();
		return out;
	}

	ArrayXi whole_hand(board.size() + 1);
	
	memcpy(whole_hand.data(), board.data(), board.size() * sizeof(mainDataType)); //Warning! sizeof(..) Should be the same type as VectorXi
	for (int card = 0; card < card_count; card++)
	{
		whole_hand[whole_hand.size()] = card;
		if (hand_is_possible(whole_hand))
		{
			out[card] = 1;
		}
	}

	return out;
}

ArrayXi card_tools::get_impossible_hand_indexes(ArrayXi board)
{
	ArrayXi out = get_possible_hand_indexes(board);
	out -= 1;
	out *= -1;
	return out;
}
