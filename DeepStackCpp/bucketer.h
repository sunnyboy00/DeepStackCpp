#pragma once
#include "CustomSettings.h"
#include "card_tools.h"

//-- - Assigns hands to buckets on the given board.
//--
//--For the Leduc implementation, we simply assign every possible set of
//-- private and board cards to a unique bucket.
class bucketer
{
public:
	bucketer();
	~bucketer();

	//-- - Gives the total number of buckets across all boards.
	//-- @return the number of buckets
	size_t get_bucket_count();

	//-- - Gives a vector which maps private hands to buckets on a given board.
	//-- @param board a non - empty vector of board cards
	//-- @return a vector which maps each private hand to a bucket index
	ArrayX compute_buckets(const ArrayX& board);

private:

	card_tools _cardTools;
};

