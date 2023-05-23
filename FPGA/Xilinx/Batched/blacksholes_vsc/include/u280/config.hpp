
#pragma once

#include <ap_int.h>
#include <hls_stream.h>
#include <ap_axi_sdata.h>

typedef ap_uint<512> uint512_dt;
typedef ap_uint<256> uint256_dt;
typedef ap_axiu<256,0,0,0> t_pkt256;

typedef ap_axiu<32,0,0,0> t_pkt;

// Maximum Tile Size
#define MAX_SIZE_X 8192
#define MAX_DEPTH_16 (MAX_SIZE_X/16)

// User function
#define SHIFT_BITS 3
#define VEC_FACTOR 1 << SHIFT_BITS;
#define DATATYPE_SIZE 32
#define STREAM_DEPTH MAX_DEPTH_16

// HLS limit constants
const int max_size_y = MAX_SIZE_X;
const int min_size_y = 20;
const int avg_size_y = MAX_SIZE_X;

const int max_block_x = MAX_DEPTH_16 + 1;
const int min_block_x = 20/16 + 1;
const int abg_block_x = MAX_DEPTH_16 + 1;

const int max_grid = max_block_x * max_size_y;
const int min_grid = min_block_x * min_size_y;
const int avg_grid = abg_block_x * avg_size_y;

const int vec_factor = VEC_FACTOR;
const int max_depth_16 = MAX_DEPTH_16;
const int max_depth_8 = MAX_DEPTH_16 * 2;


// union to convert int to float and vice versa 
typedef union
{
    int i;
    int f;
} data_conv;


// global memory connection to Accelerator ports
#define PORT_MAP_CURR HBM[0] // : HBM[1] : HBM[2] : HBM[3])
#define PORT_MAP_NEXT HBM[4] // : HBM[5] : HBM[6] : HBM[7])

// number of CU
#define NCU 1
#define SLR0_P_STAGE 2
