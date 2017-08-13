#pragma once
#include "Node.h"
#include "assert.h"
#include "Util.h"
#include <Eigen/Dense>
#include "tree_builder.h"
#include "lookahead.h"
#include "LookaheadBuilder.h"
#include "card_tools.h"

//-- - Implements depth - limited re - solving at a node of the game tree.
//--Internally uses @{cfrd_gadget | CFRDGadget} TODO SOLVER
class Resolving
{
public:
	Resolving();
	~Resolving();

	//---- - Re - solves a depth - limited lookahead using input ranges.
	//----
	//----Uses the input range for the opponent instead of a gadget range, so only
	//---- appropriate for re - solving the root node of the game tree(where ranges
	//---- are fixed).
	//----
	//---- @param node the public node at which to re - solve
	//---- @param player_range a range vector for the re - solving player
	//---- @param opponent_range a range vector for the opponent
	LookaheadResult resolve_first_node(Node& node, const Tf1& player_range, const Tf1& opponent_range);

	//---- - Re - solves a depth - limited lookahead using an input range for the player and
	//----the @{cfrd_gadget | CFRDGadget
	//} to generate ranges for the opponent.
	//----
	//---- @param node the public node at which to re - solve
	//---- @param player_range a range vector for the re - solving player
	//---- @param opponent_cfvs a vector of cfvs achieved by the opponent
	//---- before re - solving
	void resolve(const Node& node, const Tf1& player_range, const Tf1& opponent_cfvs);

	//---- - Gives the index of the given action at the node being re - solved.
	//----
	//----The node must first be re - solved with @{resolve} or @{resolve_first_node}.
	//---- @param action a legal action at the node
	//---- @return the index of the action
	//---- @local
	void _action_to_action_id(int action);

	//---- - Gives a list of possible actions at the node being re - solved.
	//----
	//----The node must first be re - solved with @{resolve} or @{resolve_first_node}.
	//---- @return a list of legal actions
	void get_possible_actions();

	//---- - Gives the average counterfactual values that the re - solve player received
	//---- at the node during re - solving.
	//----
	//----The node must first be re - solved with @{resolve_first_node}.
	//----
	//---- @return a vector of cfvs
	Tf1 get_root_cfv();

	//---- - Gives the average counterfactual values that each player received
	//---- at the node during re - solving.
	//----
	//----Useful for data generation for neural net training
	//----
	//----The node must first be re - solved with @{resolve_first_node}.
	//----
	//---- @return a 2xK tensor of cfvs, where K is the range size
	Tf2 get_root_cfv_both_players();

	//---- - Gives the average counterfactual values that the opponent received
	//---- during re - solving after the re - solve player took a given action.
	//----
	//----Used during continual re - solving to track opponent cfvs.The node must
	//---- first be re - solved with @{resolve} or @{resolve_first_node}.
	//----
	//---- @param action the action taken by the re - solve player at the node being
	//---- re - solved
	//---- @return a vector of cfvs
	Tf1 get_action_cfv(int action);

	//---- - Gives the average counterfactual values that the opponent received
	//---- during re - solving after a chance event(the betting round changes and
	//----more cards are dealt).
	//----
	//----Used during continual re - solving to track opponent cfvs.The node must
	//---- first be re - solved with @{resolve} or @{resolve_first_node}.
	//----
	//---- @param action the action taken by the re - solve player at the node being
	//---- re - solved
	//---- @param board a vector of board cards which were updated by the chance event
	//---- @return a vector of cfvs
	void get_chance_action_cfv(int action, Tf1 board);

	//---- - Gives the probability that the re - solved strategy takes a given action.
	//----
	//----The node must first be re - solved with @{resolve} or @{resolve_first_node}.
	//----
	//---- @param action a legal action at the re - solve node
	//---- @return a vector giving the probability of taking the action with each
	//---- private hand
	void get_action_strategy(int action);

private:
	LookaheadResult _resolve_results;

	tree_builder builder;

	lookahead _lookahead;

	LookaheadBuilder* _lookBuilder;

	card_tools _card_tools;

	//-- - Builds a depth - limited public tree rooted at a given game node.
	//-- @param node the root of the tree
	void _create_lookahead_tree(Node& node);
};

