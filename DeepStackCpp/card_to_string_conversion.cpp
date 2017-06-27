#include "card_to_string_conversion.h"
#include <assert.h>


card_to_string_conversion::card_to_string_conversion()
{
	for (size_t card = 0; card < card_count; card++)
	{
		int rank_name = rank_table[card_to_rank(card)];
		int suit_name = suit_table[card_to_suit(card)];
		card_to_string_table[card] = rank_name + suit_name;
		string_to_card_table[card_to_string_table[card]] = card;
	}
}

inline std::string card_to_string_conversion::card_to_string(int card)
{
	assert(card > 0 && card <= card_count);
	return card_to_string_table[card];
}

inline int card_to_string_conversion::card_to_suit(int card)
{
	return card % suit_count + 1;
}

inline int card_to_string_conversion::card_to_rank(int card)
{
	return floor((card - 1) / suit_count) + 1;
}

std::string card_to_string_conversion::cards_to_string(std::vector<int> cards)
{
	//if (cards.size() == 0)
	//{
	//	return "";
	//}

	std::string  out = "";
	for (int card : cards)
	{
		out += card_to_string(cards[card]);
	}

	return out;
}

inline int card_to_string_conversion::string_to_card(std::string card_string)
{
	int card = string_to_card_table[card_string];
	assert(card > 0 && card <= card_count);
	return card;
}


