#include "Node.h"
#include <Windows.h>
#include <iostream>
#include <sstream>
#include <string>

//Node::Node(const Node & node)
//{
//	node_type = node.node_type;
//	street = node.street;
//	board = node.board;
//	board_string = node.board_string;
//	current_player = node.current_player;
//	bets = Eigen::Array2f(node.bets);
//	pot = node.pot;
//	terminal = terminal;
//}

Node::Node() : street(1), current_player(chance), bets(2), terminal(false), depth(-1), type(uninitialized), board_string(""), pot(0) //ToDo: Perfomance Warning: do we realy need default constructor to save perfomance?
{
}

Node::~Node()
{
	for (size_t i = 0; i < children.size(); i++)
	{
		Node* cur_node = children[i];
		if (cur_node != nullptr)
		{
			delete(cur_node);
		}
	}
}

string GetNodeTypeString(const node_types value) {
	static map<node_types, string> strings;
	if (strings.size() == 0) {
#define INSERT_ELEMENT(p) strings[p] = #p
		INSERT_ELEMENT(uninitialized);
		INSERT_ELEMENT(terminal_fold);
		INSERT_ELEMENT(terminal_call);
		INSERT_ELEMENT(check);
		INSERT_ELEMENT(chance_node);
		INSERT_ELEMENT(inner_node);
#undef INSERT_ELEMENT
	}

	return strings[value];
}

void Node::ToString()
{
	wostringstream ss;
	ss << "Type: " << GetNodeTypeString(type).c_str();
	ss << ". Player: " << current_player;
	ss << ". Street: " << street;
	ss << ". Depth: " << depth;
	ss << ". board_string: " << board_string.c_str();
	ss << ". IsTerminal: " << terminal << L"\n";
	ss << " Bets: [" << bets(0) << " , " << bets(1) << "] ";
	OutputDebugString(ss.str().c_str());
}
