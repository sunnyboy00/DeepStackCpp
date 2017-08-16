#include "terminal_equity.h"
#include <iostream>
#include <string>
#include "Util.h"

terminal_equity::terminal_equity() 
{
	_equity_matrix = ArrayXX(card_count, card_count);
	ArrayX board;
	set_board(board);
}

void terminal_equity::set_board(const ArrayX & board)
{
	_set_call_matrix(board);
	_set_fold_matrix(board);
}


void terminal_equity::_set_call_matrix(const ArrayX & board)
{
	int street = _cardTools.board_to_street(board);
	_equity_matrix.fill(0);

	if (street == 1)
	{
		// --iterate through all possible next round streets
		ArrayXX next_round_boards = _cardTools.get_second_round_boards();
		size_t boards_count = next_round_boards.size();
		ArrayXX next_round_equity_matrix = ArrayXX(card_count, card_count);
		for (size_t board = 0; board < boards_count; board++)
		{
			ArrayX nextBoard = next_round_boards.row(board);
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

void terminal_equity::_set_fold_matrix(const ArrayX & board)
{
	_fold_matrix.resize(card_count, card_count);
	_fold_matrix.fill(1);
	// --setting cards that block each other to zero - exactly elements on diagonal in leduc variants
	_fold_matrix -= MatrixX::Identity(card_count, card_count).array();
	_handle_blocking_cards(_fold_matrix, board);
}

void terminal_equity::tree_node_fold_value(const ArrayXX& ranges, ArrayXX& result, int folding_player)
{
	ArrayXX tempResult(result.rows(), result.cols());
	fold_value(ranges, tempResult);
	result.row(0) = tempResult.row(1);
	result.row(1) = tempResult.row(0);

	result.row(folding_player) *= -1;
}

ArrayXX terminal_equity::get_call_matrix()
{
	return _equity_matrix;
}

void terminal_equity::tree_node_call_value(const ArrayXX& ranges, ArrayXX& result)
{
	//ToDo: performance warning maybe just call_value and swap rows? Or Maps as below commented out will be faster?
	ArrayXX tempResult(result.rows(), result.cols());
	call_value(ranges, tempResult);
	result.row(0) = tempResult.row(1);
	result.row(1) = tempResult.row(0);
	//result.row(0) = Map<ArrayXX>(tempResult.row(1).data(), 1, ranges.cols());
	//result.row(1) = Map<ArrayXX>(tempResult.row(0).data(), 1, ranges.cols());
}

void terminal_equity::_handle_blocking_cards(ArrayXX& equity_matrix, const ArrayX& board)
{
	RowVectorX possible_hand_indexes_1(_cardTools.get_possible_hand_indexes(board));
	//ArrayXX possible_hand_matrix = possible_hand_indexes.replicate(card_count, 1); remove
	equity_matrix *= possible_hand_indexes_1.replicate(card_count, 1).array();

	VectorX possible_hand_indexes_2(possible_hand_indexes_1);
	equity_matrix *= possible_hand_indexes_2.replicate(1, card_count).array();
}

void terminal_equity::get_last_round_call_matrix(const ArrayX& board_cards, ArrayXX& call_matrix)
{
	assert(board_cards.size() == 1 || board_cards.size() == 2 && "Only Leduc and extended Leduc are now supported.");

	VectorX strength = _evaluator.batch_eval(board_cards);

	VectorX v1(strength);
	ArrayXX strength_view_1 = v1.replicate(1, card_count);

	RowVectorX v2(strength);
	ArrayXX strength_view_2 = v2.replicate(card_count, 1);
	call_matrix = (strength_view_1 > strength_view_2).cast<float>() - (strength_view_1 < strength_view_2).cast<float>();
	_handle_blocking_cards(call_matrix, board_cards);
}
