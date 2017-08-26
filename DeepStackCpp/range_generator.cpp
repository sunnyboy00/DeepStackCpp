#include "range_generator.h"



range_generator::range_generator()
{
}


range_generator::~range_generator()
{
}

void range_generator::_generate_recursion(ArrayXX& cards, ArrayX& mass)
{
	size_t batch_size = cards.rows();
	assert(mass.rows() == batch_size);
	//--we terminate recursion at size of 1
	size_t card_count = cards.cols();
	if (card_count == 1)
	{
		Util::Copy(cards, mass);
	}
	else
	{

		ArrayX rand = ArrayX::Random(batch_size);
		ArrayX mass1 = mass  * rand;
		ArrayX mass2 = mass - mass1;
		float halfSize = card_count / 2;
		//--if the tensor contains an odd number of cards, randomize which way the
		//--middle card goes
		if (ceilf(halfSize) != halfSize)
		{
			halfSize -= 0.5;
			halfSize += std::rand();
		}

		int halfSizeInt = (int)halfSize;
		ArrayXX one(batch_size, halfSizeInt);
		memcpy(one.data(), cards.data(), halfSize * sizeof(float));

		int startOffset =  halfSizeInt + 1;
		int twoRows = cards.cols() - startOffset;
		ArrayXX two(batch_size, twoRows);
		memcpy(two.data(), cards.data() + startOffset, twoRows * sizeof(float));

		_generate_recursion(one, mass1);
		_generate_recursion(two, mass2);
	}
}

void range_generator::generate_sorted_range(ArrayXX& range)
{
	size_t batch_size = range.rows();
	ArrayX inputAr = ArrayX::Ones(batch_size);
	_generate_recursion(range, inputAr);
}

void range_generator::set_board(const ArrayX & board)
{
	ArrayX hand_strengths = _evaluator.batch_eval(board);
	_possible_hands_mask = _cardTools.get_possible_hand_indexes(board);
	_possible_hands_count = _possible_hands_mask.rowwise().sum()(0);

	_order = ArrayX(_possible_hands_count);
	_order = hand_strengths * _possible_hands_mask;
	Util::Sort(_order);
	_reverse_order = ArrayX(_order);
	Util::SortReverse(_reverse_order);
}

void range_generator::generate_range(ArrayXX & range)
{
	size_t batch_size = range.rows();
	_sorted_range.resize(batch_size, _possible_hands_count);
	generate_sorted_range(_sorted_range);
	//--we have to reorder the the range back to undo the sort by strength
	ArrayX A;
	ArrayXX index = Util::ExpandAs(_reverse_order, _sorted_range);
	for (size_t curRow = 0; curRow < _sorted_range.rows(); curRow++)
	{
		auto curIndex = index.row(curRow);
		for (size_t curCol = 0; curCol < curIndex.cols(); curCol++)
		{
			_reordered_range(curRow, curCol) = _sorted_range(curRow, curCol);
		}
	}

	range.setZero();
	range = Util::ExpandAs(_possible_hands_mask, range) * _reordered_range;
}
