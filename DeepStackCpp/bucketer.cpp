#include "bucketer.h"



bucketer::bucketer()
{
}


bucketer::~bucketer()
{
}

size_t bucketer::get_bucket_count()
{
	return card_count * _cardTools.get_boards_count();
}

ArrayX bucketer::compute_buckets(const ArrayX& board)
{
	size_t shift = (_cardTools.get_board_index(board) - 1) * card_count; // -1 ?
	ArrayX buckets = ArrayX::LinSpaced(1, 0, card_count) + shift;
	//--impossible hands will have bucket number - 1
	for (size_t i = 0; i < board.rows(); i++)
	{
		buckets[board[i]] = -1;
	}

	return buckets;
}
