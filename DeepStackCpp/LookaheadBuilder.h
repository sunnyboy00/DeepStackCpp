#pragma once
#include "Node.h"
#include "assert.h"
#include "Util.h"
#include "ValueNn.h"
#include "lookahead.h"

#include <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>
#include <vector>

class LookaheadBuilder
{
public:
	LookaheadBuilder(lookahead& ahead);
	~LookaheadBuilder();

	//-- - Constructs the lookahead from a game's public tree.
	//--
	//--Must be called to initialize the lookahead.
	//-- @param tree a public tree
	//void build_lookahead(Node& tree);

	//-- - Builds the lookahead's internal data structures using the public tree.
	//-- @param tree the public tree used to construct the lookahead
	void build_from_tree(Node& tree);

	//-- - Computes the maximum number of actions at each depth of the tree.
	//--
	//--Used to find the size for the tensors which store lookahead data.The
	//-- maximum number of actions is used so that every node at that depth can
	//-- fit in the same tensor.
	//-- @param current_layer a list of tree nodes at the current depth
	//-- @param current_depth the depth of the current tree nodes
	void _compute_tree_structures(const vector<Node*>& current_layer, int current_depth);

	//-- - Traverses the tree to fill in lookahead data structures that summarize data
	//	-- contained in the tree.
	//	--
	//	--For example, saves pot sizes and numbers of actions at each lookahead state.
	//	--
	//	-- @param node the current node of the public tree
	//	-- @param layer the depth of the current node
	//	-- @param action_id the index of the action that led to this node
	//	-- @param parent_id the index of the current node's parent
	//	-- @param gp_id the index of the current node's grandparent
	void set_datastructures_from_tree_dfs(Node& node, int layer, int action_id, int parent_id, int gp_id);

	void ProcessNodeWithEmptyActions(Node &node, int layer, int action_id, int next_parent_id, int next_gp_id);

	//-- - Computes the number of nodes at each depth of the tree.
	//--Used to find the size for the tensors which store lookahead data.
	void _compute_structure();

	//-- - Builds the tensors that store lookahead data during re - solving.
	void construct_data_structures();

	//-- - Builds the neural net query boxes which estimate counterfactual values
	//-- at depth - limited states of the lookahead.
	void _construct_transition_boxes();

private:

	// NN that is used to get approximate values
	ValueNn _neural_net;

	// Lookahead instance associated with the builder
	lookahead* _lookahead;
};

