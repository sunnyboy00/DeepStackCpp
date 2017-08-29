#include "range_generator.h"



range_generator::range_generator()
{
}


range_generator::~range_generator()
{
}

void range_generator::_generate_recursion(ArrayXX& cards, ArrayX& mass, size_t cardsOffset, size_t cardsCount)
{
	assert(cardsCount > 0 && cardsOffset < cards.cols());
	size_t batch_size = cards.rows();
	assert(mass.rows() == batch_size);
	//--we terminate recursion at size of 1
	//size_t card_count = cards.cols();
	if (cardsCount == 1)
	{
		cards.col(cardsOffset) = mass; //ToDo: make cards column wise array for better speed.
	}
	else
	{
		ArrayX rand = (ArrayX::Random(batch_size) + 1) /2; //ToDo: optimize
		ArrayX mass1 = mass  * rand;
		ArrayX mass2 = mass - mass1;
		float halfSize = cardsCount / 2.0f;
		//--if the tensor contains an odd number of cards, randomize which way the
		//--middle card goes
		if (ceilf(halfSize) != halfSize)
		{
			if (std::rand() % 2 == 0)
			{
				halfSize = floorf(halfSize);
			}
			else
			{
				halfSize = ceilf(halfSize);
			}
			//halfSize -= 0.5;
			//halfSize += (std::rand() / double(RAND_MAX));
			//halfSize = halfSize;
		}

		int halfSizeInt = (int)halfSize;
		_generate_recursion(cards, mass1, cardsOffset, halfSizeInt);
		const int secondPartCardsCount = cardsCount - halfSizeInt;
		_generate_recursion(cards, mass2, cardsOffset + halfSizeInt, secondPartCardsCount);
		assert(halfSizeInt + secondPartCardsCount == cardsCount);
	}
}

void range_generator::generate_sorted_range(ArrayXX& range)
{
	size_t batch_size = range.rows();
	ArrayX inputAr = ArrayX::Ones(batch_size);
	_generate_recursion(range, inputAr, 0, range.cols());
}

void range_generator::set_board(const ArrayX & board)
{
	ArrayX hand_strengths = _evaluator.batch_eval(board);
	_possible_hands_mask = _cardTools.get_possible_hand_indexes(board);
	_possible_hands_count = _possible_hands_mask.colwise().sum()(0);

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
	_reordered_range.resize(batch_size, _possible_hands_count);

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
	size_t missedCols = 0;
	for (size_t curCard = 0; curCard < card_count; curCard++)
	{
		if (_possible_hands_mask(curCard))
		{
			range.col(curCard) = _reordered_range.col(curCard - missedCols);
		}
		else
		{
			missedCols++;
		}
	}
}
