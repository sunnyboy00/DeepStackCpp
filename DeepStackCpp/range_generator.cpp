#include "range_generator.h"



range_generator::range_generator()
{
}


range_generator::~range_generator()
{
}

void range_generator::_generate_recursion(const ArrayXX& cards, const ArrayX& mass)
{

}

void range_generator::generate_sorted_range(ArrayXX& range)
{

}

void range_generator::set_board(const ArrayX & board)
{
	ArrayX hand_strengths = _evaluator.batch_eval(board);
	_possible_hands_mask = _cardTools.get_possible_hand_indexes(board);
	_possible_hands_count = _possible_hands_mask.rowwise().sum()(0);

	ArrayX non_coliding_strengths = ArrayX(_possible_hands_count);
	non_coliding_strengths = hand_strengths * _possible_hands_mask;
	Util::Sort()

	_, order = non_coliding_strengths : sort()
	_, self.reverse_order = order : sort()
	self.reverse_order = self.reverse_order : view(1, -1) : long()
	self.reordered_range = arguments.Tensor()
	self.sorted_range = arguments.Tensor()
}

void range_generator::generate_range(ArrayXX & range)
{
	size_t batch_size = range.rows();
	_sorted_range.resize(batch_size, _possible_hands_count);
	generate_sorted_range(_sorted_range)
	//--we have to reorder the the range back to undo the sort by strength
		local index = self.reverse_order : expandAs(self.sorted_range)
		if arguments.gpu then
			index = index : cuda()
			end
			self.reordered_range = self.sorted_range : gather(2, index)

			range : zero()
			range : maskedCopy(self.possible_hands_mask:expandAs(range), self.reordered_range)
}
