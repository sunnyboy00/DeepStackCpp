#pragma once
#pragma warning(disable:4554) 

#include "Constants.h"
#include "CustomSettings.h"
#include <Eigen/Dense>
#include <vector>
#include <memory>
#include <unsupported/Eigen/CXX11/Tensor>

using namespace std;
using namespace Eigen;

struct Node
{
	Node();

	~Node();

	// Type of the node. An element of @{constants.node_types} (if applicable)
	node_types type;

	// The current betting round
	int street;

	// A possibly empty vector of board cards
	ArrayX board;

	// A string representation of the board cards
	string board_string;

	// The player acting at the node
	int current_player;

	// The number of chips that each player has committed to the pot
	Array2 bets;

	// Half the pot size, equaled to the smaller number in `bets`
	float pot;

	// Parent of the node
	Node* parent = nullptr;

	// Is node a terminal node
	bool terminal;

	//A strategy is represented at each public node by a NxK tensor where:
	//
	// * N is the number of possible child nodes.
	//
	// * K is the number of information sets for the active player in the public
	//node.For the Leduc Hold'em variants we implement, there is one for each
	// private card that the player could hold.
	//
	//For a player node, `strategy[i][j]` gives the probability of taking the
	// action that leads to the `i`th child when the player holds the `j`th card.
	//
	//For a chance node, `strategy[i][j]` gives the probability of reaching the
	// `i`th child for either player when that player holds the `j`th card.
	ArrayXX strategy;

	// A list of children nodes
	vector<Node*> children;

	// Actions
	ArrayX actions;

	// Depth of the node inside the tree
	long long depth;

	// Player regrets. A tensor of [actions_count x card_count] size.
	ArrayXX regrets;

	// A 2xK tensor containing the probabilities of each
	// player reaching the current node with each private hand
	ArrayXX ranges_absolute;

	//-- Recursively calculated counterfactual values for each player 
	//-- using the saved strategy profile when playing against
	//--each other. [players_count X card_count]
	ArrayXX cf_values;

	//-- The cfvs for a best response against each player in the profile
	ArrayXX cf_values_br;

	// Counterfactual values weighted by the reach prob
	ArrayXX cfv_infset;

	// CFV-BR values weighted by the reach prob
	ArrayXX cfv_br_infset;

	//Weighted sum of card ranges
	ArrayX iter_weight_sum;

	//Weighted current card ranges contribution;
	ArrayX iter_weight_contribution;

	// Difference between CFV-BR and Counterfactual values = node.cfv_br_infset - node.cfv_infset
	ArrayXX epsilon;

	// <action_id, parent_id, gp_id>
	// action_id - the index of the action that led to this node
	// parent_id - the index of the current node's parent
	// gp_id - he index of the current node's grandparent.
	Tensor<float, 3> lookahead_coordinates;

	// Node exploitability mean of node.epsilon.
	float exploitability;

	void ToString();
	//Node(const Node &node);
};

