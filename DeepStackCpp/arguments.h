#pragma once
#include <assert.h>
#include <Eigen/Dense>

using Eigen::VectorXf;

//-- - Parameters for DeepStack.
//--@module arguments


//-- - whether to run on GPU
static const int gpu = false;
//-- - list of pot - scaled bet sizes to use in tree
//-- @field bet_sizing.  i. e. a list of fractions of the pot which are allowed 
//-- as bets, sorted in ascending order. Note: Should be integer fraction with current code.
static const VectorXf bet_sizing = VectorXf::Ones(1); 
//-- - server running the ACPC dealer
//static const char acpc_server[] = "localhost";
//-- - server port running the ACPC dealer
//static const int acpc_server_port = 20000;
//-- - the number of betting rounds in the game
static const int streets_count = 2;
//-- - the tensor datatype used for storing DeepStack"s internal data
//Tensor = torch.FloatTensor
//-- - the directory for data files
static const char data_directory[] = "../Data/";
//-- - the size of the game"s ante, in chips
static const long long ante = 100;
//-- - the size of each player"s stack, in chips
static const long long stack = 1200;
//-- - the number of iterations that DeepStack runs CFR for
static const int cfr_iters = 1000;
//-- - the number of preliminary CFR iterations which DeepStack doesn"t factor into the average strategy (included in cfr_iters)
static const int cfr_skip_iters = 500;
//-- - how many poker situations are solved simultaneously during data generation
static const int gen_batch_size = 10;
//-- - how many poker situations are used in each neural net training batch
static const int train_batch_size = 100;
//-- - path to the solved poker situation data used to train the neural net
static const char data_path[] = "../Data/TrainSamples/PotBet/";
//-- - path to the neural net model
static const char model_path[] = "../Data/Models/PotBet/";
//-- - the name of the neural net file
static const char value_net_name[] = "final";
//-- - the neural net architecture
//static const char net = "{nn.Linear(input_size, 50), nn.PReLU(), nn.Linear(50, output_size)}";
//-- - how often to save the model during training
static const int save_epoch = 2;
//-- - how many epochs to train for
static const int epoch_count = 10;
//-- - how many solved poker situations are generated for use as training examples
static const int train_data_count = 100;
//-- - how many solved poker situations are generated for use as validation examples
static const int valid_data_count = 100;
//-- - learning rate for neural net training
static const double learning_rate = 0.001;

//assert(cfr_iters > cfr_skip_iters);
//if gpu then
//require "cutorch"
//Tensor = torch.CudaTensor
//end