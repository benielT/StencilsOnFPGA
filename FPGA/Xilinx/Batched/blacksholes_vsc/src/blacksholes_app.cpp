
#include "common.h"
#include "blacksholes_acc.hpp"
#include "blacksholes_cpu.h"
#include "config.hpp"

// #define FPGA_FLOAT_NAIVE_BATCHING
#define FPGA_FLOAT_VSC_BATCHING

int main(int argc, const char**argv)
{
    GridParameter gridProp;

    gridProp.logical_size_x = 200;
	gridProp.logical_size_y = 1;
	gridProp.batch = 2;
	gridProp.num_iter = 2040;

	unsigned int vectorization_factor = 8;

	// setting grid parameters given by user
	const char* pch;

	for ( int n = 1; n < argc; n++ )
	{
		pch = strstr(argv[n], "-size=");

		if(pch != NULL)
		{
			gridProp.logical_size_x = atoi ( argv[n] + 7 ); continue;
		}

		pch = strstr(argv[n], "-iters=");

		if(pch != NULL)
		{
			gridProp.num_iter = atoi ( argv[n] + 7 ); continue;
		}
		pch = strstr(argv[n], "-batch=");

		if(pch != NULL)
		{
			gridProp.batch = atoi ( argv[n] + 7 ); continue;
		}
	}

	printf("Grid: %dx1 , %d iterations, %d batches\n", gridProp.logical_size_x, gridProp.num_iter, gridProp.batch);

    	//adding halo
	gridProp.act_size_x = gridProp.logical_size_x+2;
	gridProp.act_size_y = 1;

	//padding each row as multiple of vectorization factor
	gridProp.grid_size_x = (gridProp.act_size_x % vectorization_factor) != 0 ?
			(gridProp.act_size_x/vectorization_factor + 1) * vectorization_factor :
			gridProp.act_size_x;
	gridProp.grid_size_y = gridProp.act_size_y;

	//for allocating memory buffer
	unsigned int data_size_bytes = gridProp.grid_size_x * gridProp.grid_size_y * sizeof(float) * gridProp.batch;

	if(data_size_bytes >= 4000000000)
	{
		std::cerr << "Maximum buffer size is exceeded!" << std::endl;
		return -1;
	}

    BlacksholesParameter calcParam;

//	calcParam.spot_price = 16;
//	calcParam.strike_price = 10;
//	calcParam.time_to_maturity = 0.25;
//	calcParam.volatility = 0.4;
//	calcParam.risk_free_rate = 0.1;s
	calcParam.spot_price = 16;
	calcParam.strike_price = 10;
	calcParam.time_to_maturity = 0.25;
	calcParam.volatility = 0.4;
	calcParam.risk_free_rate = 0.1;
	calcParam.N = gridProp.num_iter;
	calcParam.K = gridProp.logical_size_x;
	calcParam.SMaxFactor = 3;
	calcParam.delta_t = calcParam.time_to_maturity / calcParam.N;
	calcParam.delta_S = calcParam.strike_price * calcParam.SMaxFactor/ (calcParam.K);
 

    // // **************************************************************
    // // *                     Stability checkup                      *
    // // **************************************************************

    // if (stencil_stability(calcParam))
    // {
    //     std::cout << "stencil calculation is stable" << std::endl << std::endl;
    // }
    // else
    // {
    //     std::cerr << "stencil calculation stability check failed" << std::endl << std::endl;
    //     return -1;
    // }


	// **************************************************************
    // *                  Blacksholes FPGA setup                    *
    // **************************************************************

	auto currBufPool = blacksholes_acc::create_bufpool(vpp::bidirectional);
    auto nextBufPool = blacksholes_acc::create_bufpool(vpp::bidirectional);

    //to track out-of-order transfer with handle
    int run = 0;

#ifdef FPGA_FLOAT_NAIVE_BATCHING
	// **************************************************************
    // *          Blacksholes FPGA float - Naive batching           *
    // **************************************************************

	float * grid_u1 = (float*) aligned_alloc(4096, data_size_bytes);
	float * grid_u2 = (float*) aligned_alloc(4096, data_size_bytes);

	initialize_grid(grid_u1, gridProp, calcParam);

    // send thread
    blacksholes_acc::send_while([&]() -> bool {
        blacksholes_acc::set_handle(run);
        float * currBuf = (float*) blacksholes_acc::alloc_buf(currBufPool, data_size_bytes);
        float * nextBuf = (float*) blacksholes_acc::alloc_buf(nextBufPool, data_size_bytes);
        
		std::memcpy(currBuf, grid_u1, data_size_bytes);

// compute(
//     const int size0,
//     const int size1,
//     const int xdim0,
//     const int ydim0,
//     const int count,
//     const int batches,
//     const float spot_price,
//     const float strike_price,
//     const float time_to_maturity,
//     const float volatility,
//     const float risk_free_rate,
//     const float delta_t,
//     const float delta_s,
//     const unsigned int N,
//     const int K,
//     const float SMaxFactor,
//     float * in,
//     float * out)

        blacksholes_acc::compute(
			gridProp.logical_size_x,
			gridProp.logical_size_y,
            gridProp.grid_size_x,
            gridProp.grid_size_y,
			gridProp.num_iter,
            gridProp.batch,
			calcParam.spot_price,
			calcParam.strike_price,
			calcParam.time_to_maturity,
			calcParam.volatility,
			calcParam.risk_free_rate,
			calcParam.delta_t,
			calcParam.delta_s,
			calcParam.N,
			calcParam.K,
			calcParam.SMaxFactor,
            currBuf,
            nextBuf);

        // For 1 iteration.
        return (++run < 1);
    });

    // recieve thread
    blacksholes_acc::receive_all_in_order([&](){
        int run = blacksholes_acc::get_handle();
        float * nextBuf = (float*) blacksholes_acc::get_buf(nextBufPool);

		std::memcpy(grid_u2, nextBuf, data_size_bytes);
    });

    blacksholes_acc::join();
#endif

#ifdef FPGA_FLOAT_VSC_BATCHING
	// **************************************************************
    // *           Blacksholes FPGA float - VSC batching            *
    // **************************************************************

	run = 0;

	float * grid_u3 = (float*) aligned_alloc(4096, data_size_bytes);
	float * grid_u4 = (float*) aligned_alloc(4096, data_size_bytes);

	initialize_grid(grid_u3, gridProp, calcParam);

	unsigned int data_size_bytes_per_batch = gridProp.grid_size_x * gridProp.grid_size_y * sizeof(float);
	GridParameter gridPropTmp = gridProp;
	gridPropTmp.batch = 1;

	//send thread for number of batch size
	    blacksholes_acc::send_while([&]() -> bool {
        blacksholes_acc::set_handle(run);
        float * currBuf = (float*) blacksholes_acc::alloc_buf(currBufPool, data_size_bytes_per_batch);
        float * nextBuf = (float*) blacksholes_acc::alloc_buf(nextBufPool, data_size_bytes_per_batch);
        
		// print_grid(grid_u3, gridProp, "grid_u3");

		// std::cout << "sending : " << run << std::endl;

		std::memcpy(currBuf, ((void*) grid_u3) + run * data_size_bytes_per_batch, data_size_bytes_per_batch);

		// print_grid(currBuf, gridPropTmp, "currBuf with grid_u3");

        blacksholes_acc::compute(
			gridProp.logical_size_x,
			gridProp.logical_size_y,
            gridProp.grid_size_x,
            gridProp.grid_size_y,
			gridProp.num_iter,
            1,
			calcParam.spot_price,
			calcParam.strike_price,
			calcParam.time_to_maturity,
			calcParam.volatility,
			calcParam.risk_free_rate,
			calcParam.delta_t,
			calcParam.delta_S,
			calcParam.N,
			calcParam.K,
			calcParam.SMaxFactor,
            currBuf,
            nextBuf);

        // For 1 iteration.
        return (++run < gridProp.batch);
    });

	// recieve thread
    blacksholes_acc::receive_all_in_order([&](){
        int run = blacksholes_acc::get_handle();
        float * nextBuf = (float*) blacksholes_acc::get_buf(nextBufPool);

		std::memcpy(((void*) grid_u4) + run * data_size_bytes_per_batch, nextBuf, data_size_bytes_per_batch);
    });

	blacksholes_acc::join();
#endif

	// **************************************************************
    // *                       Verification                         *
    // **************************************************************

#ifdef FPGA_FLOAT_NAIVE_BATCHING
	std::cout << "VERIFICAION FPGA-float naïve" << std::endl;

	if (verify_grid(grid_u1, grid_u2, gridProp))
	{
		std::cout << "VERIFICATION PASSED" << std::endl;
	}
	else
	{
		std::cerr << "VERIFICATION FAILED" << std::endl;
	}
#endif

#ifdef FPGA_FLOAT_VSC_BATCHING
	std::cout << "VERIFICAION FPGA-float VSC batching" << std::endl;

	if (verify_grid(grid_u3, grid_u4, gridProp))
	{
		std::cout << "VERIFICATION PASSED" << std::endl;
	}
	else
	{
		std::cerr << "VERIFICATION FAILED" << std::endl;
	}
#endif 

	// **************************************************************
    // *                         Cleaning                           *
    // **************************************************************

#ifdef FPGA_FLOAT_NAIVE_BATCHING
	free(grid_u1);
	free(grid_u2);
#endif

#ifdef FPGA_FLOAT_VSC_BATCHING
	free(grid_u3);
	free(grid_u4);
#endif

    return 0;
}