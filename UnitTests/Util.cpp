#include "catch.hpp"
#include "Util.h"
#include <string>
#include <iostream>

TEST_CASE("ConvertOffset")
{
	Tf2 tensor(2, 3);
	tensor.setRandom();

	DenseIndex offset = Util::ConvertOffset(tensor, -1, 0);
	REQUIRE(offset == 1);
	offset = Util::ConvertOffset(tensor, -2, 0);
	REQUIRE(offset == 0);

	offset = Util::ConvertOffset(tensor, -1, 1);
	REQUIRE(offset == 2);

	offset = Util::ConvertOffset(tensor, -2, 1);
	REQUIRE(offset == 1);
}

void FillTensor(Tf2 &tensor)
{
	int value = 0;
	for (DenseIndex x = 0; x < tensor.dimension(0); x++)
	{
		for (size_t y = 0; y < tensor.dimension(1); y++)
		{
			tensor(x, y) = (float)value;
			value++;
		}
	}
}
TEST_CASE("Slice")
{
	Tf2 tensor(2, 3);
	FillTensor(tensor);
	std::array<std::array<DenseIndex, 2>, 2> offsets;
	offsets[0] = { 0 , 0 };
	offsets[1] = { 1 , 1 };
	Tf2 res = Util::Slice(tensor, offsets);

	REQUIRE(res.size() == 1);
	REQUIRE(res(0, 0) == 1);

	res = Util::Slice(tensor, { { { 0 , 0 }, { 1 , -1 } } });

	REQUIRE(res.dimension(0) == 1);
	REQUIRE(res.dimension(1) == 2);

	REQUIRE(res(0, 0) == 1);
	REQUIRE(res(0, 1) == 2);

	res = Util::Slice(tensor, { { { 0 , 0 },{ 1 , -2 } } });

	REQUIRE(res.dimension(0) == 1);
	REQUIRE(res.dimension(1) == 1);

	REQUIRE(res(0, 0) == 1);

	res = Util::Slice(tensor, { { {0, -1},{ 1, -1 } } });

	REQUIRE(res.dimension(0) == 2);
	REQUIRE(res.dimension(1) == 2);

	REQUIRE(res(0, 0) == 1);
	REQUIRE(res(1, 0) == 4);

	REQUIRE(res(0, 1) == 2);
	REQUIRE(res(1, 1) == 5);

	res = Util::Slice(tensor, { { { 0, -1 },{ 0, 0 } } });
	REQUIRE(res.dimension(0) == 2);
	REQUIRE(res.dimension(1) == 1);

	REQUIRE(res(0, 0) == 0);
	REQUIRE(res(1, 0) == 3);
}

TEST_CASE("View")
{
	
	Tf2 tensor(2, 3);

	FillTensor(tensor);
	std::array<int, 2> sizes = { 3, 2 };
	TensorMap<Tf2, 2> res = Util::View(tensor, sizes);

	REQUIRE(res.dimension(0) == 3);
	REQUIRE(res.dimension(1) == 2);

	REQUIRE(res(0, 0) == 0);
	REQUIRE(res(0, 1) == 1);

	REQUIRE(res(1, 0) == 2);
	REQUIRE(res(1, 1) == 3);

	REQUIRE(res(2, 0) == 4);
	REQUIRE(res(2, 1) == 5);

	Tf3 tensor2(1, 2, 3);
	std::array<int, 3> sizes2 = { 2, 1, -1 };
	TensorMap<Tf3, 3> res2 = Util::View(tensor2, sizes2);
	REQUIRE(res2.dimension(0) == 2);
	REQUIRE(res2.dimension(1) == 1);
	REQUIRE(res2.dimension(2) == 3);
}