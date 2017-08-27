#pragma once
#include "CustomSettings.h"
#include "bucketer.h"

//-- - Converts between vectors over private hands and vectors over buckets.

//using Eigen::ArrayBase;
using namespace Eigen;

class bucket_conversion
{
public:
	bucket_conversion();
	~bucket_conversion();

	//-- - Sets the board cards for the bucketer.
	//-- @param board a non - empty vector of board cards
	void set_board(const ArrayX& board);

	//	-- - Converts a value vector over buckets to a value vector over private hands.
	//	--
	//	-- @{set_board
	//} must be called first.Used to process neural net outputs.
	//-- @param bucket_value a vector of values over buckets
	//-- @param card_value a vector in which to save the resulting vector of values
	//-- over private hands
	void bucket_value_to_card_value(const ArrayX& bucket_value, ArrayX& card_value);

	//	-- - Converts a range vector over private hands to a range vector over buckets.
	//	--
	//-- @{set_board
	//} must be called first.Used to create inputs to the neural net.
	//-- @param card_range a probability vector over private hands
	//-- @param bucket_range a vector in which to save the resulting probability
	//-- vector over buckets
	template<typename Derived, typename OtherDerived>
	void card_range_to_bucket_range(const ArrayBase<Derived>& card_range, ArrayBase<OtherDerived>& bucket_range);

	//	-- - Gives a vector of possible buckets on the the board.
	//	--
	//	-- @{set_board
	//} must be called first.
	//-- @return a mask vector over buckets where each entry is 1 if the bucket is
	//-- valid, 0 if not
	ArrayXX get_possible_bucket_mask();

private:

	size_t _bucket_count;

	ArrayXX _range_matrix;

	bucketer _bucketer;

	ArrayXX _reverse_value_matrix;
};

