#include "random_card_generator.h"


random_card_generator::random_card_generator()
{
}


random_card_generator::~random_card_generator()
{
}

ArrayX random_card_generator::generate_cards(size_t count)
{
	//--marking all used cards
	size_t used_cards[card_count];
	ArrayX out = ArrayX(count);

	//--counter for generated cards
	size_t generated_cards_count = 0;
	while (generated_cards_count < count)
	{
		int card = (int)(rand() % card_count);

		if (used_cards[card] == 0)
		{
			generated_cards_count++;
			out(generated_cards_count) = card;
			used_cards[card] = 1;
		}
	}

	return out;
}
