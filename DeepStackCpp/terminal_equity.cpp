#include "terminal_equity.h"
//#include <iostream>
//#include <string>



terminal_equity::terminal_equity()
{
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
	assert(board_cards.size() == 1 || board_cards.size() == 2 && "Only Leduc and extended Leduc are now supported");

	VectorXf strength = _evaluator.batch_eval(board_cards);

	VectorXf v1(strength);
	ArrayXXf strength_view_1 = v1.replicate(1, card_count);

	RowVectorXf v2(strength);
	ArrayXXf strength_view_2 = v2.replicate(card_count, 1);
	call_matrix = (strength_view_1 > strength_view_2).cast<float>() - (strength_view_1 < strength_view_2).cast<float>();
	_handle_blocking_cards(call_matrix, board_cards);
}


