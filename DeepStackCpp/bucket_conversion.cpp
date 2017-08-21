#include "bucket_conversion.h"



bucket_conversion::bucket_conversion()
{
}


bucket_conversion::~bucket_conversion()
{
}

void bucket_conversion::set_board(const ArrayX& board)
{
	_bucket_count = _bucketer.get_bucket_count();
	_range_matrix = ArrayXX::Zero(card_count, _bucket_count);

	ArrayX buckets = _bucketer.compute_buckets(board);
	ArrayXX class_ids = ArrayX::LinSpaced(1, 0, _bucket_count).replicate(card_count, _bucket_count);
	ArrayXX card_buckets = buckets.replicate(card_count, _bucket_count);

	//--finding all strength classes
	//--matrix for transformation from card ranges to strength class ranges
	for (size_t row = 0; row < class_ids.rows(); row++)
	{
		for (size_t col = 0; col < class_ids.cols(); col++)
		{
			if (class_ids(row, col) == card_buckets(row, col))
			{
				_range_matrix == 1;
			}
		}
	}

	//--matrix for transformation form class values to card values
	_reverse_value_matrix = _range_matrix.transpose();
}

void bucket_conversion::bucket_value_to_card_value(const ArrayX& bucket_value, ArrayX& card_value)
{
	card_value = bucket_value.matrix() * _reverse_value_matrix.matrix();
}

void bucket_conversion::card_range_to_bucket_range(const ArrayX& card_range, ArrayX& bucket_range)
{
	bucket_range = card_range.matrix() * _range_matrix.matrix();
}

ArrayXX bucket_conversion::get_possible_bucket_mask()
{
	ArrayXX mask = ArrayXX(1, _bucket_count);
	ArrayXX card_indicator = ArrayXX::Ones(1, card_count);
	mask = card_indicator.matrix() *  _range_matrix.matrix();
	return mask;
}
