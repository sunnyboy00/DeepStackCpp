#pragma once
#include "Node.h"
#include "assert.h"
#include "Util.h"
#include <Eigen/Dense>
#include "tree_builder.h"

//-- - Implements depth - limited re - solving at a node of the game tree.
//--Internally uses @{cfrd_gadget | CFRDGadget} TODO SOLVER
class Resolving
{
public:
	Resolving();
	~Resolving();

	//-- - Builds a depth - limited public tree rooted at a given game node.
	//-- @param node the root of the tree
	void _create_lookahead_tree(Node& node);

	//-- - Re - solves a depth - limited lookahead using input ranges.
	//--
	//--Uses the input range for the opponent instead of a gadget range, so only
	//-- appropriate for re - solving the root node of the game tree(where ranges
	//		-- are fixed).
	//--
	//-- @param node the public node at which to re - solve
	//-- @param player_range a range vector for the re - solving player
	//-- @param opponent_range a range vector for the opponent
	void resolve_first_node(Node& node, CardArray player_range, CardArray opponent_range);

private:
	tree_builder builder;
};

