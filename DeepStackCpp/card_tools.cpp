#include "card_tools.h"
#include <assert.h>
#include <unordered_map>
#include <stdlib.h>
#include <time.h>

card_tools::card_tools()
{
	_init_board_index_table();
}

bool card_tools::hand_is_possible(ArrayXf hand)
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

CardArray card_tools::get_possible_hand_indexes(ArrayXf board)
{
	CardArray out;
	out.setZero();

	if (board.size() == 0)
	{
		out.setOnes();
		return out;
	}

	ArrayXf whole_hand(board.size() + 1);
	
	memcpy(whole_hand.data(), board.data(), board.size() * sizeof(mainDataType)); 
	for (int card = 0; card < card_count; card++)
	{
		whole_hand[whole_hand.size()] = (float)card;
		if (hand_is_possible(whole_hand))
		{
			out[card] = 1;
		}
	}

	return out;
}

CardArray card_tools::get_impossible_hand_indexes(ArrayXf board)
{
	CardArray out = get_possible_hand_indexes(board);
	out -= 1;
	out *= -1;
	return out;
}

CardArray card_tools::get_uniform_range(ArrayXf board)
{
	CardArray out = get_possible_hand_indexes(board);
	out /= out.sum();
	return out;
}

CardArray card_tools::get_random_range(ArrayXf board, int seed = -1)
{
	if (seed == -1)
		srand((unsigned int)time(NULL));
	else
		srand(seed);

	CardArray out;
	out.Random();

	CardArray possibleHands = get_possible_hand_indexes(board);
	out *= possibleHands;
	out /= out.sum();

	return out;
}

bool card_tools::is_valid_range(CardArray range, ArrayXf board)
{
	CardArray impossibleCards = get_impossible_hand_indexes(board);
	CardArray check = range * impossibleCards;
	bool only_possible_hands = check.sum() == 0;
	bool sums_to_one = abs(1.0 - range.sum()) < 0.0001;
	return only_possible_hands && sums_to_one;
}

int card_tools::board_to_street(ArrayXf board)
{
	if (board.size() == 0)
		return 1;
	else
		return 2;
}

long long card_tools::get_boards_count()
{
	if (board_card_count == 1)
	{
		return card_count;
	}
	else if (board_card_count == 2)
	{
		return ((long long)card_count * (card_count - 1)) / 2;
	}
	else
	{
		assert(false && "unsupported board size");
	}
}

MatrixXf card_tools::get_second_round_boards()
{
	long long boards_count = get_boards_count();
	if (board_card_count == 1)
	{
		MatrixXf out(boards_count, 1);
		for (int card = 1; card < card_count; card++)
		{
			out(card, 0) = (float)card;
		}

		return out;
	}
	else if (board_card_count == 2)
	{
		MatrixXf out(boards_count, 2);
		long long board_idx = 0;
		for (int card_1 = 1; card_1 < card_count; card_1++)
		{
			for (int card_2 = card_1 + 1; card_2 < card_count; card_2++)
			{
				board_idx++;
				out(board_idx, 0) = (float)card_1;
				out(board_idx, 1) = (float)card_2;
			}
		}

		assert(board_idx == boards_count && "wrong boards count!");
		return out;
	}
	else
	{
		assert(false && "unsupported board size");
	}
}

auto ar = ArrayXf::LinSpaced(5, 1, 5);

void card_tools::_init_board_index_table()
{
	if (board_card_count == 1)
	{
		_board_index_table = MatrixXf(1, card_count);
		_board_index_table.row(0) = ArrayXf::LinSpaced(card_count, 1.0, (float)card_count);
	}
	else if (board_card_count == 2)
	{
		_board_index_table = ArrayXXf::Constant(1, card_count, -1);
		float board_idx = 0;
		for (int card_1 = 1; card_1 < card_count; card_1++)
		{
			for (int card_2 = card_1 + 1; card_2 < card_count; card_2++)
			{
				board_idx++;
				_board_index_table(card_1, card_2) = board_idx;
				_board_index_table(card_2, card_1) = board_idx;
			}
		}
	}
	else
	{
		assert(false && "unsupported board size");
	}
}