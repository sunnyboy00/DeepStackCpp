#pragma once
#include "Node.h"
#include "card_tools.h"
#include "game_settings.h"
#include "constants.h"
#include <Eigen/Dense>
#include <assert.h>

using Eigen::MatrixXf;

//
//-- - Fills a game's public tree with a uniform strategy. In particular, fills
//--the chance nodes with the probability of each outcome.
//--
//--A strategy is represented at each public node by a NxK tensor where:
//--
//-- * N is the number of possible child nodes.
//--
//-- * K is the number of information sets for the active player in the public
//--node.For the Leduc Hold'em variants we implement, there is one for each
//-- private card that the player could hold.
//--
//--For a player node, `strategy[i][j]` gives the probability of taking the
//-- action that leads to the `i`th child when the player holds the `j`th card.
//--
//--For a chance node, `strategy[i][j]` gives the probability of reaching the
//-- `i`th child for either player when that player holds the `j`th card.
//-- @classmod strategy_filling.

class strategy_filling
{
public:

	strategy_filling();

	//-- - Fills a public tree with a uniform strategy.
	//-- @param tree a public tree for Leduc Hold'em or variant
	void fill_uniform(Node& tree);

private:

	//-- - Fills a chance node with the probability of each outcome.
	//-- @param node the chance node
	//-- @local
	void _fill_chance(Node& node);

	//-- - Fills a player node with a uniform strategy.
	//-- @param node the player node
	//-- @local
	void _fill_uniformly(Node& node);

	//-- - Fills a node with a uniform strategy and recurses on the children.
	//-- @param node the node
	//-- @local
	void _fill_uniform_dfs(Node& node);

	card_tools _card_tools;
};

