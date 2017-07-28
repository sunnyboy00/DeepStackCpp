#pragma once
#include "Node.h"
#include "assert.h"
#include "Util.h"
#include <Eigen/Dense>
#include "ValueNn.h"

class LookaheadBuilder
{
public:
	LookaheadBuilder();
	~LookaheadBuilder();

private:
	ValueNn _neural_net;
};

