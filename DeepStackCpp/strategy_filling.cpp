#include "strategy_filling.h"


strategy_filling::strategy_filling() 
{
	_card_tools = card_tools();
}

void strategy_filling::fill_uniform(Node & tree)
{
	_fill_uniform_dfs(tree);
}

void strategy_filling::_fill_chance(Node& node)
{
	//WARNING: test this method. There is a lot of changes from original.

	assert(!node.terminal);

	//--filling strategy
	//--we will fill strategy with an uniform probability, but it has to be zero for hands that are not possible on
	//--corresponding board
	float cardsProbability = 1.0 / (card_count - 2); //--remove 2 because each player holds one card
	node.strategy = MatrixXf(node.children.size(), card_count);
	node.strategy.fill(cardsProbability);

	//--setting probability of impossible hands to 0
	for (long long i = 0; i < node.children.size(); i++)
	{
		Node child_node = node.children[i];
		CardArray mask = _card_tools.get_possible_hand_indexes(child_node.board);
		//node.strategy[i] : fill(0)
		node.strategy.row(0) *= mask; // [i][mask] = 1.0 / (game_settings.card_count - 2)
	}
}

void strategy_filling::_fill_uniformly(Node & node)
{
	assert(node.current_player == P1 || node.current_player == P2);

	if (node.terminal)
	{
		return;
	}

	node.strategy = MatrixXf(node.children.size(), card_count);
	node.strategy.fill(1.0 / node.children.size());
}

void strategy_filling::_fill_uniform_dfs(Node& node)
{
	if (node.current_player == chance)
	{
		_fill_chance(node);
	}
	else
	{
		_fill_uniformly(node);
	}


	for (long long i = 0; node.children.size(); i++)
	{
		_fill_uniform_dfs(node.children[i]);
	}
	
}

