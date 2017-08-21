#pragma once
#include "CustomSettings.h"
#include "LeducEvaluator.h"
#include "card_tools.h"
#include "Util.h"
#include <stdlib.h>  

class range_generator
{
public:
	range_generator();
	~range_generator();

	//-- - Samples a batch of ranges with hands sorted by strength on the board.
	//-- @param range a NxK tensor in which to store the sampled ranges, where N is
	//-- the number of ranges to sample and K is the range size
	//-- @see generate_range
	//-- @local
	void generate_sorted_range(ArrayXX& range);

	//-- - Sets the(possibly empty) board cards to sample ranges with.
	//--
	//--The sampled ranges will assign 0 probability to any private hands that
	//-- share any cards with the board.
	//--
	//-- @param board a possibly empty vector of board cards
	void set_board(const ArrayX& board);

	//	-- - Samples a batch of random range vectors.
	//	--
	//	--Each vector is sampled independently by randomly splitting the probability
	//	-- mass between the bottom half and the top half of the range, and then
	//	-- recursing on the two half.
	//	--
	//	-- @{set_board
	//} must be called first.
	//--
	//-- @param range a NxK tensor in which to store the sampled ranges, where N is
	//-- the number of ranges to sample and K is the range size
	void generate_range(ArrayXX& range);

private:

	card_tools _cardTools;

	LeducEvaluator _evaluator;

	size_t _possible_hands_count;

	ArrayX _possible_hands_mask;

	//-- - Recursively samples a section of the range vector.
	//-- @param cards an NxJ section of the range tensor, where N is the batch size
	//-- and J is the length of the range sub - vector
	//-- @param mass a vector of remaining probability mass for each batch member
	//-- @see generate_range
	void _generate_recursion(ArrayXX& cards, ArrayX& mass);

	ArrayX _order;

	ArrayX _reverse_order;

	ArrayX _reordered_range;

	ArrayXX _sorted_range;
};

