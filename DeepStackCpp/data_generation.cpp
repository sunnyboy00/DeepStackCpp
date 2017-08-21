#include "data_generation.h"



data_generation::data_generation()
{
}


data_generation::~data_generation()
{
}

void data_generation::generate_data(size_t train_data_count, size_t valid_data_count)
{
	clock_t begin = clock();
	clock_t end = clock();
	string pr = ctime(end - begin);
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
}

void data_generation::generate_data_file(size_t data_count, string file_name)
{

}
