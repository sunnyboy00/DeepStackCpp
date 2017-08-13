#include "card_tools.h"
#include "CustomSettings.h"
#include <assert.h>
#include <unordered_map>
#include <stdlib.h>
#include <time.h>

card_tools::card_tools()
{
	_init_board_index_table();
}

bool card_tools::hand_is_possible(const Tf1& hand) // Perfomance warning: Cant we change all Tf1 to vectors?
{
	assert(MinSc(hand) >= 0 && MaxSc(hand) < card_count   && "Illegal cards in hand");
	vector<bool> cards_table(card_count);


	for (int i = 0; i < hand.size(); i++)
	{
		int card = (int)hand(i);
		if (cards_table[card]) // If this card already exists in hand
		{
			return false;
		}

		cards_table[card] = true;
	}

	return true;
}

CardArray card_tools::get_possible_hand_indexes(const Tf1& board)
{
	CardArray out = CardArray();

	out.setZero();

	if (board.size() == 0)
	{
		out.setConstant(1.0f);
		return out;
	}

	int newSize = (int)board.size() + 1; //some extra space for one more element
	Tf1 whole_hand(newSize);
	
	memcpy(whole_hand.data(), board.data(), board.size() * sizeof(float));  //copy data to the beginning
	for (int card = 0; card < card_count; card++)
	{
		whole_hand[newSize - 1] = (float)card; // setting last element
		if (hand_is_possible(whole_hand))
		{
			out(card) = 1;
		}
	}

	return out;
}

CardArray card_tools::get_impossible_hand_indexes(const Tf1& board)
{
	CardArray out = get_possible_hand_indexes(board);
	out -= 1;
	out *= -1;
	return out;
}

CardArray card_tools::get_uniform_range(const Tf1& board)
{
	CardArray out = get_possible_hand_indexes(board);
	out /= out.sum();
	return out;
}

CardArray card_tools::get_random_range(const Tf1& board, int seed = -1)
{
	if (seed == -1)
		srand((unsigned int)time(NULL));
	else
		srand(seed);

	CardArray out;
	out.setRandom();

	CardArray possibleHands = get_possible_hand_indexes(board);
	out *= possibleHands;
	out /= out.sum();

	return out;
}

bool card_tools::is_valid_range(const CardArray& range, const  Tf1& board)
{
	CardArray impossibleCards = get_impossible_hand_indexes(board);
	CardArray check = range * impossibleCards;
	bool only_possible_hands = SumSc(check) == 0;
	bool sums_to_one = abs(1.0 - SumSc(range)) < 0.0001;
	return only_possible_hands && sums_to_one;
}

int card_tools::board_to_street(const Tf1& board)
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

Tf2 card_tools::get_second_round_boards()
{
	long long boards_count = get_boards_count();
	if (board_card_count == 1)
	{
		Tf2 out(boards_count, 1);
		for (int card = 0; card < card_count; card++)
		{
			out(card, 0) = (float)card;
		}

		return out;
	}
	else if (board_card_count == 2)
	{
		Tf2 out(boards_count, 2);
		long long board_idx = 0;
		for (int card_1 = 0; card_1 < card_count; card_1++)
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


void card_tools::_init_board_index_table()
{
	if (board_card_count == 1)
	{
		_board_index_table = Tf2(1, card_count);
		for (size_t card = 0; card < card_count; card++)
		{
			_board_index_table(0, card) = card;
		}
	}
	else if (board_card_count == 2)
	{
		_board_index_table = Tf2(1, card_count);
		_board_index_table.setConstant(-1);
		float board_idx = 0;
		for (int card_1 = 0; card_1 < card_count; card_1++)
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

int card_tools::get_board_index(const Tf1& board)
{
	int bordsSize = (int)board.size();

	assert(bordsSize > 0 && bordsSize <= 2 && "unsupported board size");
	if (bordsSize == 0)
	{
		return 0;
	}
	else if (bordsSize == 1)
	{
		return (int)_board_index_table(0, (int)board(0));
	}
	else
	{
		return (int)_board_index_table((int)board(0), (int)board(1));
	}
}

CardArray card_tools::normalize_range(const Tf1& board, CardArray& range)
{
	Tf1 rangeM = Tf1(range);
	Tf1 mask = get_possible_hand_indexes(board);
	CardArray out = rangeM * mask;

	float curSum = SumSc(out);

	//return zero range if it all collides with board(avoid div by zero)
	if (curSum == 0)
	{
		return range;
	}

	out /= curSum;

	return out;
}