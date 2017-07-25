#include "terminal_equity.h"
#include <iostream>
#include <string>
#include "Util.h"

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
	_fold_matrix.resize(card_count, card_count);
	_fold_matrix.fill(1);
	// --setting cards that block each other to zero - exactly elements on diagonal in leduc variants
	_fold_matrix -= MatrixXf::Identity(card_count, card_count).array();
	_handle_blocking_cards(_fold_matrix, board);
}

void terminal_equity::call_value(const ArrayXXf & ranges, ArrayXXf & result)
{
	result = ranges.matrix() * _equity_matrix.matrix();
}

void terminal_equity::fold_value(const ArrayXXf & ranges, ArrayXXf& result)
{
	assert(_fold_matrix.size() > 0);
	result = (ranges.matrix() * _fold_matrix.matrix()).array();
}

ArrayXXf terminal_equity::get_call_matrix()
{
	return _equity_matrix;
}

void terminal_equity::tree_node_call_value(const ArrayXXf& ranges, ArrayXXf& result)
{
	//ToDo: performance warning maybe just call_value and swap rows? Or Maps as below commented out will be faster?
	ArrayXXf tempResult(result.rows(), result.cols());
	call_value(ranges, tempResult);
	result.row(0) = tempResult.row(1);
	result.row(1) = tempResult.row(0);
	//result.row(0) = Map<ArrayXXf>(tempResult.row(1).data(), 1, ranges.cols());
	//result.row(1) = Map<ArrayXXf>(tempResult.row(0).data(), 1, ranges.cols());
}

void terminal_equity::tree_node_fold_value(const ArrayXXf& ranges, ArrayXXf& result, int folding_player)
{
	ArrayXXf tempResult(result.rows(), result.cols());
	fold_value(ranges, tempResult);
	result.row(0) = tempResult.row(1);
	result.row(1) = tempResult.row(0);

	result.row(folding_player) *= -1;
}

void terminal_equity::_handle_blocking_cards(ArrayXXf& equity_matrix, const ArrayXf& board)
{
	RowVectorXf possible_hand_indexes_1(_cardTools.get_possible_hand_indexes(board));
	//ArrayXXf possible_hand_matrix = possible_hand_indexes.replicate(card_count, 1); remove
	equity_matrix *= possible_hand_indexes_1.replicate(card_count, 1).array();

	VectorXf possible_hand_indexes_2(possible_hand_indexes_1);
	equity_matrix *= possible_hand_indexes_2.replicate(1, card_count).array();
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
