#include <ap_int.h>
#include <hls_stream.h>
#include <ap_axi_sdata.h>
#include <math.h>
#include "stencil.h"
#include "stencil.cpp"

void process_SLR(hls::stream <t_pkt> &in, hls::stream<t_pkt> &out, const int xdim0, const unsigned short size_x,
		const unsigned int size_y, const unsigned int size_z, const unsigned short batches, const float calcParam_K)
{
	hls::stream<uint256_dt> streamArray[SLR_P_STAGE + 1];
#pragma HLS STREAM variable = streamArray depth = 10

	// data_G data_g;
	// data_g.sizex = size_x;
	// data_g.sizey = size_y;
	// data_g.sizez = size_z;
	// data_g.offset_x = 0;
	// data_g.grid_size_x = xdim0;
	// data_g.xblocks = (data_g.grid_size_x >> SHIFT_BITS);
	// data_g.offset_y = 0;
	// data_g.grid_size_y = size_y + 2;
	// data_g.offset_z = 0;
	// data_g.grid_size_z = size_z + 2;
	// data_g.batches = batches;
	// data_g.limit_z = size_z + 3;

// struct StencilConfigCore
// {
//     SizeType grid_size; //{xblocks, y, z, ...}
//     SizeType lower_limit;
//     SizeType upper_limit;
//     unsigned short dim;
//     unsigned short outer_loop_limit;
//     unsigned int total_itr;
// };

    ops::hls::StencilConfigCore stencilConfig;
    stencilConfig.dim = 3;
    stencilConfig.grid_size[0] = xdim0 >> SHIFT_BITS;
    stencilConfig.grid_size[1] = size_y + 2;
    stencilConfig.grid_size[2] = size_z + 2;
    stencilConfig.lower_limit[0] = 1;
    stencilConfig.lower_limit[1] = 1;
    stencilConfig.lower_limit[2] = 1;
    stencilConfig.upper_limit[0] = size_x + 1;
    stencilConfig.upper_limit[1] = size_y + 1;
    stencilConfig.upper_limit[2] = size_z + 1;
    stencilConfig.total_itr = register_it(stencilConfig.grid_size[0] * stencilConfig.grid_size[1]) * stencilConfig.grid_size[2];
    stencilConfig.outer_loop_limit = size_z + 3;


	// unsigned short tile_y_1 = data_g.grid_size_y - 1;
	// unsigned int plane_size = data_g.xblocks * data_g.grid_size_y;

	// data_g.plane_diff = data_g.xblocks * tile_y_1;
	// data_g.line_diff = data_g.xblocks - 1;
	// data_g.gridsize_pr = plane_size * register_it(data_g.grid_size_z * batches + 1);
	// data_g.gridsize_da = register_it(plane_size * data_g.grid_size_z) * batches;

	// const float coefficients[7] = {calcParam_K, calcParam_K, calcParam_K, 1-6*calcParam_K, calcParam_K, calcParam_K, calcParam_K};
//#pragma HLS ARRAY_PARTITION variable=coefficients complete dim=1

#pragma HLS DATAFLOW
	{
//		axis2_fifo256(in, streamArray[0], stencilConfig.total_itr);
		ops::hls::axis2stream<256>(in, streamArray[0], stencilConfig.total_itr);
		for (int i = 0; i < SLR_P_STAGE; i++)
		{
#pragma HLS unroll
			kernel_ops_krnl_heat3D_PE(0, stencilConfig, streamArray[i+1], streamArray[i], calcParam_K);
		}

		ops::hls::stream2axis<256>(streamArray[SLR_P_STAGE], out, stencilConfig.total_itr);

	}

}

extern "C"
{
	void stencil_SLR(
			const int sizex,
			const int sizey,
			const int sizez,
			const int xdim0,
			const int batches,
			const int count,
			const float calcParam_K,
			hls::stream <t_pkt> &in,
			hls::stream <t_pkt> &out)
	{
#pragma HLS INTERFACE axis port = in register
#pragma HLS INTERFACE axis port = out register

#pragma HLS INTERFACE s_axilite port = sizex bundle = control
#pragma HLS INTERFACE s_axilite port = sizey bundle = control
#pragma HLS INTERFACE s_axilite port = sizez bundle = control
#pragma HLS INTERFACE s_axilite port = xdim0 bundle = control
#pragma HLS INTERFACE s_axilite port = batches bundle = control
#pragma HLS INTERFACE s_axilite port = count bundle = control
#pragma HLS INTERFACE s_axilite port = calcParam_K bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

		for (unsigned int i = 0; i < count * 2; i++)
		{
			process_SLR(in, out, xdim0, sizex, sizey, sizez, batches, calcParam_K);
		}

	}
}
