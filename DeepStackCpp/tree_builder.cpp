#include "tree_builder.h"


tree_builder::tree_builder()
{
	_card_tools = card_tools();
	_card_to_string = card_to_string_conversion();

}

vector<Node*> tree_builder::_get_children_nodes_transition_call(Node& parent_node)
{
	Node* chance_node = new Node(parent_node);
	chance_node->current_player = chance;
	vector<Node*> out = { chance_node };
	return out;
}

vector<Node*> tree_builder::_get_children_nodes_chance_node(Node& parent_node)
{
	assert(parent_node.current_player == chance);

	if (_limit_to_street)
	{
		return vector<Node*>();
	}

	MatrixXf next_boards = _card_tools.get_second_round_boards();
	long long next_boards_count = next_boards.rows();

	long long subtree_height = -1;
	auto children = vector<Node*>();
	//1.0 iterate over the next possible boards to build the corresponding subtrees
	for (unsigned long long i = 0; next_boards_count; i++)
	{
		CardArray next_board = next_boards.row(i);
		std::string next_board_string = _card_to_string.cards_to_string(next_board);
		
		Node* child = new Node();
		child->node_type = inner_node;
		child->parent = &parent_node;
		child->current_player = P1;
		child->street = parent_node.street + 1;
		child->board = next_board;
		child->board_string = next_board_string;
		child->bets = parent_node.bets;

		children.push_back(child);
	}

	return children;
}

void tree_builder::_fill_additional_attributes(Node& node)
{
	node.pot =(long long)node.bets.minCoeff();
}

vector<Node*> tree_builder::_get_children_player_node(Node & parent_node)
{
	assert(parent_node.current_player != chance);

	auto children = vector<Node*>();
	int current_player = 3 - parent_node.current_player;

	//1.0 fold action
	Node* fold_node = new Node();
	fold_node->node_type = terminal_fold;
	fold_node->terminal = true;
	fold_node->current_player = current_player;
	fold_node->street = parent_node.street;
	fold_node->board = parent_node.board;
	fold_node->board_string = parent_node.board_string;
	fold_node->bets = parent_node.bets;

	children.push_back(fold_node);


	//2.0 check action
	//Warning: why only if cur_player == P1. What about check for p2?
	if (parent_node.current_player == P1 && (parent_node.bets(0) == parent_node.bets(1)))
	{
		Node* check_node = new Node();
		check_node->node_type = check;
		check_node->terminal = false;
		check_node->current_player = current_player;
		check_node->street = parent_node.street;
		check_node->board = parent_node.board;
		check_node->board_string = parent_node.board_string;
		check_node->bets = parent_node.bets;

		children.push_back(check_node);
	}

	//transition call
	else if (
		(parent_node.street == 1) &&
		(
			(parent_node.current_player == P2 && parent_node.bets(0) == parent_node.bets(1)) &&
			(parent_node.bets(0) != parent_node.bets(1) && parent_node.bets.maxCoeff() < stack))
		)
	{
		Node* chnce_node = new Node();
		chnce_node->node_type = chance_node;
		chnce_node->street = parent_node.street;
		chnce_node->board = parent_node.board;
		chnce_node->board_string = parent_node.board_string;
		chnce_node->current_player = chance;
		chnce_node->bets = parent_node.bets;
		chnce_node->bets.fill(parent_node.bets.maxCoeff());
		children.push_back(chnce_node);
	}
	else
	{
		//2.0 terminal call - either last street or allin
		Node* terminal_call_node = new Node();
		terminal_call_node->node_type = terminal_call;
		terminal_call_node->terminal = true;
		terminal_call_node->current_player = current_player;
		terminal_call_node->street = parent_node.street;
		terminal_call_node->board = parent_node.board;
		terminal_call_node->board_string = parent_node.board_string;
		terminal_call_node->bets = parent_node.bets;
		terminal_call_node->bets.fill(parent_node.bets.maxCoeff());
		children.push_back(terminal_call_node);
	}
	
	//3.0 bet actions  
	ArrayX2f possible_bets = _bet_sizing_manager->get_possible_bets(parent_node);

	if (possible_bets.rows() != 0)
	{
		assert(possible_bets.cols() == players_count);

		for (int i = 0; i < possible_bets.rows(); i++)
		{
				Node* child = new Node();
				child->parent = &parent_node;
				child->current_player = current_player;
				child->street = parent_node.street;
				child->board = parent_node.board;
				child->board_string = parent_node.board_string;
				child->bets = possible_bets.row(i);
				children.push_back(child);
		}
				
	}

	return children;
}

vector<Node*> tree_builder::_get_children_nodes(Node& parent_node)
{
	//is this a transition call node(leading to a chance node) ? A bug?
	//bool call_is_transit = parent_node.current_player == P2 && parent_node.bets(1) == parent_node.bets(2) && parent_node.street < streets_count;

	//transition call->create a chance node
	if (parent_node.terminal)
	{
		return vector<Node*>();
	}
	//chance node
	else if (parent_node.current_player == chance)
	{
		return _get_children_nodes_chance_node(parent_node);
	}
	// inner nodes->handle bet sizes
	else
	{
		return _get_children_player_node(parent_node);
	}

	assert(false);
}

Node & tree_builder::_build_tree_dfs(Node & current_node)
{
	_fill_additional_attributes(current_node);
	vector<Node*> children = _get_children_nodes(current_node);
	current_node.children = children;

	long long depth = 0;

	int ss = children.size();
	current_node.actions = ArrayXf(children.size());
	for (unsigned long long i = 0; i < children.size(); i++)
	{
		Node* cur_children = children[i];
		cur_children->parent = &current_node;
		_build_tree_dfs(*cur_children);
		depth = max(depth, cur_children->depth);

		if (i == 0) // First child is always fold, if exists
		{
			current_node.actions(i) = fold;
		}
		else if (i == 1) // Second child is always call, if exists
		{
			current_node.actions(i) = ccall;
		}
		else // All others children are possible bets
		{
			current_node.actions(i) = cur_children->bets.maxCoeff(); // Max possible bet is the max bet off child?
		}
	}

	current_node.depth = depth + 1;

	return current_node;
}

Node& tree_builder::build_tree(TreeBuilderParams& params)
{
	Node* root = new Node();

	//copy necessary stuff from the root_node not to touch the input
	root->street = params.root_node->street;
	root->bets = params.root_node->bets;
	root->current_player = params.root_node->current_player;
	root->board = params.root_node->board;

	if (params.bet_sizing.size() == 0)
	{
		params.bet_sizing = bet_sizing;
	}
	else
	{
		params.bet_sizing = params.bet_sizing;
	}

	_bet_sizing_manager = new bet_sizing_manager(params.bet_sizing);

	assert(params.bet_sizing.size() != 0);
	_bet_sizing = params.bet_sizing;
	_limit_to_street = params.limit_to_street;
	_build_tree_dfs(*root);

	strategy_filling filler;
	filler.fill_uniform(*root);
	return *root;
}


