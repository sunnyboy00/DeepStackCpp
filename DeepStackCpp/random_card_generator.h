#pragma once
#include "CustomSettings.h"
#include <stdlib.h> 

class random_card_generator
{
public:
	random_card_generator();
	~random_card_generator();


	//-- - Samples a random set of cards.
	//--
	//--Each subset of the deck of the correct size is sampled with
	//-- uniform probability.
	//--
	//-- @param count the number of cards to sample
	//-- @return a vector of cards, represented numerically
	ArrayX generate_cards(size_t count);
};

