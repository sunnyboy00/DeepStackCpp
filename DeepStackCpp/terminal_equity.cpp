#include "terminal_equity.h"
#include <iostream>
#include <string>



terminal_equity::terminal_equity() 
{
	_equity_matrix = ArrayXXf(card_count, card_count);
	ArrayXf board;
	set_board(board);
}

void terminal_equity::set_board(const ArrayXf & board)
{
	_set_call_matrix(board);
	_set_fold_matrix(board);
}


void terminal_equity::_set_call_matrix(const ArrayXf & board)
{
	int street = _cardTools.board_to_street(board);
	_equity_matrix.fill(0);

	if (street == 1)
	{
		// --iterate through all possible next round streets
		ArrayXXf next_round_boards = _cardTools.get_second_round_boards();
		size_t boards_count = next_round_boards.size();
		ArrayXXf next_round_equity_matrix = ArrayXXf(card_count, card_count);
		for (size_t board = 0; board < boards_count; board++)
		{
			ArrayXf nextBoard = next_round_boards.row(board);
			get_last_round_call_matrix(nextBoard, next_round_equity_matrix);
			_equity_matrix += next_round_equity_matrix;
			cout << _equity_matrix << "\n" << "\n";
		}

		//--averaging the values in the call matrix
		float weight_constant = board_card_count == 1 ? 1.0f / (card_count - 2) : 2.0f / ((card_count - 2) * (card_count - 3)); // Warning: Why '2' is in nominator for two cards board?
		_equity_matrix *= weight_constant;
	}
	else if (street == 2)
	{
		// --for last round we just return the matrix
		get_last_round_call_matrix(board, _equity_matrix);
	}
	else
	{
		//--impossible street
		assert(false && "impossible street");
	}
}

void terminal_equity::_set_fold_matrix(const ArrayXf & board)
{
	_fold_matrix = ArrayXXf(card_count, card_count);
	_fold_matrix.fill(1);
	// --setting cards that block each other to zero - exactly elements on diagonal in leduc variants
	_fold_matrix -= (ArrayXXf)MatrixXf::Identity(card_count, card_count);
	_handle_blocking_cards(_fold_matrix, board);
}

void terminal_equity::_handle_blocking_cards(ArrayXXf& equity_matrix, const ArrayXf& board)
{
	RowVectorXf possible_hand_indexes_1(_cardTools.get_possible_hand_indexes(board));
	//ArrayXXf possible_hand_matrix = possible_hand_indexes.replicate(card_count, 1); remove
	equity_matrix *= (ArrayXXf)possible_hand_indexes_1.replicate(card_count, 1);

	VectorXf possible_hand_indexes_2(possible_hand_indexes_1);
	equity_matrix *= (ArrayXXf)possible_hand_indexes_2.replicate(1, card_count);
}

void terminal_equity::get_last_round_call_matrix(const ArrayXf& board_cards, ArrayXXf& call_matrix)
{
	assert(board_cards.size() == 1 || board_cards.size() == 2 && "Only Leduc and extended Leduc are now supported.");

	VectorXf strength = _evaluator.batch_eval(board_cards);

	VectorXf v1(strength);
	ArrayXXf strength_view_1 = v1.replicate(1, card_count);

	RowVectorXf v2(strength);
	ArrayXXf strength_view_2 = v2.replicate(card_count, 1);
	call_matrix = (strength_view_1 > strength_view_2).cast<float>() - (strength_view_1 < strength_view_2).cast<float>();
	_handle_blocking_cards(call_matrix, board_cards);
}

