#include "card_to_string_conversion.h"
#include <assert.h>

//string const card_to_string_conversion::suit_table[] = { "h", "s", "c", "d" };
string const card_to_string_conversion::suit_table[] = { "s", "h", "c", "d" }; // Suits are calculated out of order in the original implimentation. Changing the order to get the same results.
string const card_to_string_conversion::rank_table[] = { "A", "K", "Q", "J", "T", "9", "8", "7", "6", "5", "4", "3", "2" };

card_to_string_conversion::card_to_string_conversion()
{
	for (int card = 0; card < card_count; card++)
	{
		int cardRank = card_to_rank(card);
		string rank_name = rank_table[cardRank];
		int cardSuit = card_to_suit(card);
		string suit_name = suit_table[cardSuit];
		string cardName = rank_name + suit_name;
		card_to_string_table[card] = cardName;
		string_to_card_table[cardName] = card;
	}
}

inline int card_to_string_conversion::card_to_rank(int card)
{
	assert(card >= 0 && card < card_count);
	return card / suit_count;
}

inline string card_to_string_conversion::card_to_string(int card)
{
	assert(card >= 0 && card < card_count);
	return card_to_string_table[card];
}

inline int card_to_string_conversion::card_to_suit(int card)
{
	assert(card >= 0 && card < card_count);
	return card % suit_count;
}

string card_to_string_conversion::cards_to_string(Tf1 cards)
{
	std::string  out = "";
	for (int card = 0; card < cards.size(); card++)
	{
		int curCard = (int)cards(card);
		out += card_to_string(curCard);
	}

	return out;
}

inline int card_to_string_conversion::string_to_card(string card_string)
{
	int card = string_to_card_table[card_string];
	assert(card >= 0 && card < card_count);
	return card;
}

//Warning: return by value? Perf degradation? ToDo:Review
Tf1 card_to_string_conversion::string_to_board(string card_string)
{
	if (card_string == "")
	{
		Tf1 out;
		return out;
	}
	
	Tf1 out(1);
	out(0) = (float)string_to_card(card_string);
	return out;
}


