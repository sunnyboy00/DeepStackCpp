//#include "terminal_equity.h"
//#include <iostream>
//#include <string>
//#include "Util.h"
//
//terminal_equity::terminal_equity() 
//{
//	_equity_matrix = Tf2(card_count, card_count);
//	Tf1 board;
//	set_board(board);
//}
//
//void terminal_equity::set_board(const Tf1 & board)
//{
//	_set_call_matrix(board);
//	_set_fold_matrix(board);
//}
//
//
//void terminal_equity::_set_call_matrix(const Tf1 & board)
//{
//	int street = _cardTools.board_to_street(board);
//	_equity_matrix.setConstant(0);
//
//	if (street == 1)
//	{
//		// --iterate through all possible next round streets
//		Tf2 next_round_boards = _cardTools.get_second_round_boards();
//		size_t boards_count = next_round_boards.size();
//		Tf2 next_round_equity_matrix = Tf2(card_count, card_count);
//		for (size_t board = 0; board < boards_count; board++)
//		{
//			Tf1 nextBoard = next_round_boards.chip(board, 0);
//			get_last_round_call_matrix(nextBoard, next_round_equity_matrix);
//			_equity_matrix += next_round_equity_matrix;
//		}
//
//		//--averaging the values in the call matrix
//		float weight_constant = board_card_count == 1 ? 1.0f / (card_count - 2) : 2.0f / ((card_count - 2) * (card_count - 3)); // Warning: Why '2' is in nominator for two cards board?
//		_equity_matrix *= weight_constant;
//	}
//	else if (street == 2)
//	{
//		// --for last round we just return the matrix
//		get_last_round_call_matrix(board, _equity_matrix);
//	}
//	else
//	{
//		//--impossible street
//		assert(false && "impossible street");
//	}
//}
//
//void terminal_equity::_set_fold_matrix(const Tf1 & board)
//{
//	_fold_matrix.resize(card_count, card_count);
//	_fold_matrix.setConstant(1);
//	// --setting cards that block each other to zero - exactly elements on diagonal in leduc variants
//	_fold_matrix -= MatrixXf::Identity(card_count, card_count).array();
//	_handle_blocking_cards(_fold_matrix, board);
//}
//
//void terminal_equity::tree_node_fold_value(const Tf2& ranges, Tf2& result, int folding_player)
//{
//	Tf2 tempResult(result.dimension(0), result.dimension(1));
//	fold_value(ranges, tempResult);
//	result.chip(0, 0) = tempResult.chip(1, 0);
//	result.chip(1, 0) = tempResult.chip(0, 0);
//
//	result.chip(folding_player, 0) *= -1;
//}
//
//Tf2 terminal_equity::get_call_matrix()
//{
//	return _equity_matrix;
//}
//
//void terminal_equity::tree_node_call_value(const Tf2& ranges, Tf2& result)
//{
//	//ToDo: performance warning maybe just call_value and swap rows? Or Maps as below commented out will be faster?
//	Tf2 tempResult(result.dimension(0), result.dimension(1)); 
//	call_value(ranges, tempResult);
//	result.chip(0, 0) = tempResult.chip(1, 0);
//	result.chip(1, 0) = tempResult.chip(0, 0);
//	//result.chip(0, 0) = Map<Tf2>(tempResult.chip(1, 0).data(), 1, ranges.dimension(1));
//	//result.chip(1, 0) = Map<Tf2>(tempResult.chip(0, 0).data(), 1, ranges.dimension(1));
//}
//
//void terminal_equity::_handle_blocking_cards(Tf2& equity_matrix, const Tf1& board)
//{
//	RowTf1 possible_hand_indexes_1(_cardTools.get_possible_hand_indexes(board));
//	//Tf2 possible_hand_matrix = possible_hand_indexes.replicate(card_count, 1); remove
//	equity_matrix *= possible_hand_indexes_1.replicate(card_count, 1).array();
//
//	Tf1 possible_hand_indexes_2(possible_hand_indexes_1);
//	equity_matrix *= possible_hand_indexes_2.replicate(1, card_count).array();
//}
//
//void terminal_equity::get_last_round_call_matrix(const Tf1& board_cards, Tf2& call_matrix)
//{
//	assert(board_cards.size() == 1 || board_cards.size() == 2 && "Only Leduc and extended Leduc are now supported.");
//
//	Tf1 strength = _evaluator.batch_eval(board_cards);
//
//	Tf1 v1(strength);
//	std::array<DenseIndex, 2> dims = { 1, card_count };
//	Tf2 strength_view_1 = v1.broadcast(dims);
//
//	Tf1 v2(strength);
//	dims = { card_count, 1 };
//	Tf2 strength_view_2 = v2.broadcast(dims);
//	call_matrix = (strength_view_1 > strength_view_2).cast<float>() - (strength_view_1 < strength_view_2).cast<float>();
//	_handle_blocking_cards(call_matrix, board_cards);
//}
