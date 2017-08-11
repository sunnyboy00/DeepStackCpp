#pragma once
#include "game_settings.h"
#include "CustomSettings.h"
#include "constants.h"
#include <Eigen/Dense>
#include <memory>

using Eigen::ArrayXf;
using Eigen::Array;
using Eigen::MatrixXf;
using Eigen::ArrayXXf;
using namespace std;

class card_tools
{
public:
	card_tools();

	//  - Gives whether a set of cards is valid.
	//  @param hand a vector of cards
	//  @return `true` if the tensor contains valid cards and no card is repeated
	bool hand_is_possible(const ArrayXf& hand);

	// Gives the private hands which are valid with a given board.
	// @param board a possibly empty vector of board cards
	// @return a vector with an entry for every possible hand(private card), which
	//  is `1` if the hand shares no cards with the board and `0` otherwise
	CardArray get_possible_hand_indexes(const ArrayXf& board);

	//  Gives the private hands which are invalid with a given board.
	// @param board a possibly empty vector of board cards
	// @return a vector with an entry for every possible hand(private card), which
	// is `1` if the hand shares at least one card with the board and `0` otherwise
	CardArray get_impossible_hand_indexes(const ArrayXf& board);

	// Gives a range vector that has uniform probability on each hand which is
	// valid with a given board.
	// @param board a possibly empty vector of board cards
	// @return a range vector where invalid hands have 0 probability and valid
	// hands have uniform probability
	CardArray get_uniform_range(const ArrayXf& board);

	// Randomly samples a range vector which is valid with a given board.
	// @param board a possibly empty vector of board cards
	// @param[opt] seed a seed for the random number generator
	// @return a range vector where invalid hands are given 0 probability, each
	// valid hand is given a probability randomly sampled from the uniform
	// distribution on[0, 1), and the resulting range is normalized
	CardArray get_random_range(const ArrayXf& board, int seed);

	// Checks if a range vector is valid with a given board.
	// @param range a range vector to check
	// @param board a possibly empty vector of board cards
	// @return `true` if the range puts 0 probability on invalid hands and has
	// total probability 1
	template <typename Derived>
	bool is_valid_range(Eigen::ArrayBase<Derived> & range, ArrayXf & board)
	{
		CardArray impossibleCards = get_impossible_hand_indexes(board);
		CardArray check = range * impossibleCards;
		bool only_possible_hands = check.sum() == 0;
		bool sums_to_one = abs(1.0 - range.sum()) < 0.0001;
		return only_possible_hands && sums_to_one;
	}

	// Gives the current betting round based on a board vector.
	// @param board a possibly empty vector of board cards
	// @return the current betting round
	int board_to_street(const ArrayXf& board);

	// Gives the number of possible boards.
	// @return the number of possible boards
	long long get_boards_count();

	// Gives all possible sets of board cards for the game.
	// @return an NxK tensor, where N is the number of possible boards, and K is
	// the number of cards on each board
	ArrayXXf get_second_round_boards();

	// Initializes the board index table.
	// @local
	void _init_board_index_table();

	// Gives a numerical index for a set of board cards.
	// @param board a non - empty vector of board cards
	// @return the numerical index for the board
	int card_tools::get_board_index(const ArrayXf& board);

	// Normalizes a range vector over hands which are valid with a given board.
	// @param board a possibly empty vector of board cards
	// @param range a range vector
	// @return a modified version of `range` where each invalid hand is given 0
	//probability and the vector is normalized
	CardArray normalize_range(const ArrayXf& board, CardArray& range);

private:

	// Id's of all possible boards
	MatrixXf _board_index_table;
};

