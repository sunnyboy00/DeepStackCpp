#pragma once
#include "bet_sizing.h"
#include "Node.h"
#include "assert.h"
#include "card_tools.h"
#include "card_to_string_conversion.h"
#include "arguments.h"
#include "TreeBuilderParams.h"
#include "strategy_filling.h"

#include <vector>
#include <memory>
#include <algorithm> 

using namespace std;

// Builds a public tree for Leduc Hold'em or variants.
//
//Each node of the tree contains the following fields :
//
// * `node_type`: an element of @{constants.node_types} (if applicable)
//
// * `street`: the current betting round
//
// * `board`: a possibly empty vector of board cards
//
// * `board_string`: a string representation of the board cards
//
// * `current_player`: the player acting at the node
//
// * `bets`: the number of chips that each player has committed to the pot
//
// * `pot`: half the pot size, equal to the smaller number in `bets`
//
// * `children`: a list of children nodes

class tree_builder
{
public:
	tree_builder();

	//	-- - Builds the tree.
	//		-- @param params table of tree parameters, containing the following fields :
	//	--
	//		-- * `street`: the betting round of the root node
	//		--
	//		-- * `bets`: the number of chips committed at the root node by each player
	//		--
	//		-- * `current_player`: the acting player at the root node
	//		--
	//		-- * `board`: a possibly empty vector of board cards at the root node
	//		--
	//		-- * `limit_to_street`: if `true`, only build the current betting round
	//		--
	//		-- * `bet_sizing` (optional) : a @{bet_sizing} object which gives the allowed
	//	    -- bets for each player
	//      -- @return the root node of the built tree
	Node* build_tree(TreeBuilderParams& params);

//private: Remove after testing
	bet_sizing_manager _bet_sizing_manager;

	// if `true`, only build the current betting round
	bool _limit_to_street;

	// object which gives the allowed bets for each player
	VectorX _bet_sizing;


	card_tools _card_tools;

	card_to_string_conversion _card_to_string;

	// Creates the child node after a call which transitions between betting
	// rounds.
	// @param parent_node the node at which the transition call happens
	// @return a list containing the child node
	vector<Node*>_get_children_nodes_transition_call(const Node& parent_node);

	// Creates the children nodes after a chance node.
	// @param parent_node the chance node
	// @return a list of children nodes
	vector<Node*>_get_children_nodes_chance_node(Node& parent_node);

	// Fills in additional convenience attributes which only depend on existing
	// node attributes.
	//c-- @param node the node
	void _fill_additional_attributes(Node& node);

	// Creates the children nodes after a player node.
	// @param parent_node the chance node
	// @return a list of children nodes
	vector<Node*> _get_children_player_node(Node& parent_node);

	// Creates the children after a node.
	// @param parent_node the node to create children for
	// @return a list of children nodes
	vector<Node*> _get_children_nodes(Node& parent_node);

	// Recursively build the(sub)tree rooted at the current node.
	// @param current_node the root to build the(sub)tree from
	// @return `current_node` after the(sub)tree has been built
	Node& _build_tree_dfs(Node& current_node);
};

