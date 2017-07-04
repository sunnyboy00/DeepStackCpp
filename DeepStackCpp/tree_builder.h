#pragma once

#include "Node.h"
#include "assert.h"
#include "card_tools.h"
#include "card_to_string_conversion.h"

#include <vector>
#include <memory>

using namespace std;
using Eigen::Array2f;

//-- - Builds a public tree for Leduc Hold'em or variants.
//--
//--Each node of the tree contains the following fields :
//--
//-- * `node_type`: an element of @{constants.node_types} (if applicable)
//--
//-- * `street`: the current betting round
//--
//-- * `board`: a possibly empty vector of board cards
//--
//-- * `board_string`: a string representation of the board cards
//--
//-- * `current_player`: the player acting at the node
//--
//-- * `bets`: the number of chips that each player has committed to the pot
//--
//-- * `pot`: half the pot size, equal to the smaller number in `bets`
//--
//-- * `children`: a list of children nodes

class tree_builder
{
public:
	tree_builder();

private:

	// if `true`, only build the current betting round
	bool _limit_to_street;

	// object which gives the allowed bets for each player
	long long _bet_sizing;

	card_tools _card_tools;

	card_to_string_conversion _card_to_string;

	//-- - Creates the child node after a call which transitions between betting
	//-- rounds.
	//-- @param parent_node the node at which the transition call happens
	//-- @return a list containing the child node
	vector<unique_ptr<Node>>_get_children_nodes_transition_call(Node& parent_node);

	//-- - Creates the children nodes after a chance node.
	//-- @param parent_node the chance node
	//-- @return a list of children nodes
	vector<unique_ptr<Node>>_get_children_nodes_chance_node(Node& parent_node);
};

