#include "LeducEvaluator.h"



LeducEvaluator::LeducEvaluator()
{
}

long long LeducEvaluator::evaluate_two_card_hand(Array2f hand_ranks)
{
	////--check for the pair
	long long hand_value;

	if (hand_ranks(0) == hand_ranks(1))
	{
		////--hand is a pair
		hand_value = (long long)hand_ranks(0);
	}
	else
	{
		////--hand is a high card
		hand_value = (long long)(hand_ranks(0) * rank_count + hand_ranks(1));
	}

	return hand_value;
}

long long LeducEvaluator::evaluate_three_card_hand(Array2f hand_ranks)
{
	long long hand_value;
	// //--check for the pair
	if (hand_ranks(0) == hand_ranks(1))
	{
		//--paired hand, value of the pair goes first, value of the kicker goes second
		hand_value = (long long)(hand_ranks(0) * rank_count + hand_ranks(2));
	}
	else if (hand_ranks(1) == hand_ranks(2))
	{
		//--paired hand, value of the pair goes first, value of the kicker goes second
		hand_value = (long long)(hand_ranks(1) * rank_count + hand_ranks(0));
	}
	else
	{
		//--hand is a high card
		hand_value = (long long)(hand_ranks(0) * rank_count * rank_count + hand_ranks(1) * rank_count + hand_ranks(2));
	}

	return hand_value;
}

ArrayXf LeducEvaluator::evaluate(ArrayXf hand, ArrayXf impossible_hand_value)
{
	assert(hand.maxCoeff() <= card_count && hand.minCoeff() >= 0 && "hand does not correspond to any cards");

	if (!_card_tools.hand_is_possible(hand))
	{
		return impossible_hand_value;
	}
	//--we are not interested in the hand suit - we will use ranks instead of cards
	ArrayXf hand_ranks = ArrayXf(hand.size());
	for (int i = 0; i < hand_ranks.size(); i++)
	{
		hand_ranks(i) = (float)_card_to_string.card_to_rank((int)hand(i));
	}

	sort(hand_ranks.data(), hand_ranks.data() + hand_ranks.size());
		//hand_ranks = hand_ranks.so : sort()

	ArrayXf res(1);

	if (hand.size() == 2)
	{
		res << (float)evaluate_two_card_hand(hand_ranks);
		return res;
	}
	else if (hand.size() == 3)
	{
		res << (float)evaluate_three_card_hand(hand_ranks);
		return res;
	}
	else
	{
		assert(false && "unsupported size of hand!");
		throw std::exception("unsupported size of hand");
	}
}

ArrayXf LeducEvaluator::batch_eval(ArrayXf board, ArrayXf impossible_hand_value)
{
	CardArray hand_values = CardArray::Constant(-1);
	size_t board_size = board.size();
	if (board_size == 0)
	{
		for (int hand = 0; hand < card_count; hand++)
		{
			hand_values(hand) = (float)((hand - 1) / suit_count + 1);
		}
	}
	else
	{
		assert(board_size == 1 || board_size == 2 && "Incorrect board size for Leduc");
		ArrayXf whole_hand = ArrayXf(board_size + 1);
		memcpy(whole_hand.data(), board.data(), board.size() * sizeof(float));  //copy data to the beginning

		for (int card = 0; card < card_count; card++)
		{
			whole_hand(board_size) = (float)card;
			hand_values.row(card) = evaluate(whole_hand, impossible_hand_value);
		}
	}

	return hand_values;
}

// Warning is it ok to use -1 as DEFAULT_IMPOSSIBLE_HAND_VALUE?
ArrayXf LeducEvaluator::batch_eval(ArrayXf board)
{
	ArrayXf impossible_hand_value(1);
	impossible_hand_value << DEFAULT_IMPOSSIBLE_HAND_VALUE;
	return batch_eval(board, impossible_hand_value);
}
