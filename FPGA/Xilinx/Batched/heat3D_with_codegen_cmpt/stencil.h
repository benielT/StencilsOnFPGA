
#pragma once

#include <ap_int.h>
#include <hls_stream.h>
#include <ap_axi_sdata.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "heat3D_common.h"
// #include "ops_hls_defs.hpp"
#include "ops_hls_datamover_partial.hpp"
// #include "ops_hls_utils.hpp"
#include "ops_hls_stencil_core_v2.hpp"

typedef float stencil_type;
constexpr unsigned int data_width = sizeof(stencil_type) * 8;
constexpr unsigned int max_depth = max_depth_bytes / data_width;
constexpr unsigned int vector_factor = 8;
constexpr unsigned int mem_vector_factor = 8;
constexpr unsigned int iter_par_factor = 7;
constexpr unsigned short mem_data_width = data_width * mem_vector_factor;
constexpr unsigned short shift_bits = 3; 
constexpr unsigned short axis_data_width = data_width * vector_factor;

typedef ap_uint<512> uint512_dt;
typedef ap_uint<256> uint256_dt;
typedef ap_axiu<256,0,0,0> t_pkt;
typedef ap_axiu<32,0,0,0> t_pkt_32;

#define SLR_P_STAGE NUM_OF_PROCESS_GRID_PER_SLR

//Maximum Tile Size
#define MAX_SIZE_X 304
#define MAX_DEPTH_16 (MAX_SIZE_X/16)

//user function
#define VEC_FACTOR 8
#define SHIFT_BITS 3
#define DATATYPE_SIZE 32  // single precision operations


const int max_size_y = MAX_SIZE_X;
const int min_size_y = 20;
const int avg_size_y = MAX_SIZE_X;

const int max_block_x = MAX_SIZE_X/VEC_FACTOR + 1;
const int min_block_x = 20/VEC_FACTOR + 1;
const int avg_block_x = MAX_SIZE_X/VEC_FACTOR + 1;

const int max_grid = max_block_x * max_size_y * max_size_y;
const int min_grid = min_block_x * min_size_y * min_size_y;
const int avg_grid = avg_block_x * avg_size_y * avg_size_y;

const int vec_factor = VEC_FACTOR;
const int max_depth_16 = MAX_DEPTH_16;
const int max_depth_8 = MAX_DEPTH_16 *2;
const int max_depth_xy = max_block_x * max_size_y;


typedef ap_uint<axis_data_width> widen_dt;
typedef ::hls::stream<widen_dt> widen_stream_dt;

constexpr unsigned short s_stencil_half_span_x = 1;
constexpr unsigned short s_stencil_span_x = 2;
constexpr unsigned short s_datatype_size = 32;

// strcutre to hold grid parameters to avoid recalculation in
// different process
//struct data_G{
//	unsigned short sizex;
//	unsigned short sizey;
//	unsigned short xdim0;
//	unsigned short end_index;
//	unsigned short end_row;
//	unsigned int gridsize;
//    unsigned int total_itr_512;
//    unsigned int total_itr_256;
//	unsigned short outer_loop_limit;
//	unsigned short endrow_plus2;
//	unsigned short endrow_plus1;
//	unsigned short endrow_minus1;
//	unsigned short endindex_minus1;
//};

// struct data_G{
// 	unsigned short sizex;
// 	unsigned short sizey;
// 	unsigned short sizez;
// 	unsigned short xblocks;
// 	unsigned short grid_size_x;
// 	unsigned short grid_size_y;
// 	unsigned short grid_size_z;
// 	unsigned short limit_z;
// 	unsigned short offset_x;
// 	unsigned short offset_y;
// 	unsigned short offset_z;
// 	unsigned int plane_size;
// 	unsigned int gridsize_pr;
// 	unsigned int gridsize_da;
// 	unsigned int plane_diff;
// 	unsigned int line_diff;
// 	unsigned short outer_loop_limit;
// 	unsigned int total_itr;
// 	bool last_half;
// 	unsigned short batches;
// };


// static constexpr unsigned short read_num_points = 7;
// static constexpr unsigned short read_stencil_size = 3;
// static constexpr unsigned short read_stencil_dim = 3;

// static constexpr unsigned short write_num_points = 1;
// static constexpr unsigned short write_stencil_size = 1;
// static constexpr unsigned short write_stencil_dim = 3;

// inline void kernel_ops_krnl_heat3D_core(
//         stencil_type& reg_0_0,
//         const stencil_type& reg_1_0,
//         const stencil_type& reg_1_1,
//         const stencil_type& reg_1_2,
//         const stencil_type& reg_1_3,
//         const stencil_type& reg_1_4,
//         const stencil_type& reg_1_5,
//         const stencil_type& reg_1_6,
//         const float* K,
//         const short idx[3]
// )
// {
// #ifdef DEBUG_LOG
//     printf("[KERNEL_INTERNAL_CORE]|%s| starting kernel core: kernel_ops_krnl_heat3D_core\n",__func__);
// #endif

// 	reg_0_0 = (1 - 6 * (*K)) * reg_1_3
// 			+ (*K) * (reg_1_4 + reg_1_2 + reg_1_5 + reg_1_1 + reg_1_6 + reg_1_0);

// #ifdef DEBUG_LOG
//     printf("[KERNEL_INTERNAL_CORE]|%s| write_val - reg_0_0: %f \n", __func__, reg_0_0);
//     printf("[KERNEL_INTERNAL_CORE]|%s| read_val - reg_1_0: %f \n", __func__, reg_1_0);
//     printf("[KERNEL_INTERNAL_CORE]|%s| read_val - reg_1_1: %f \n", __func__, reg_1_1);
//     printf("[KERNEL_INTERNAL_CORE]|%s| read_val - reg_1_2: %f \n", __func__, reg_1_2);
//     printf("[KERNEL_INTERNAL_CORE]|%s| read_val - reg_1_3: %f \n", __func__, reg_1_3);
//     printf("[KERNEL_INTERNAL_CORE]|%s| read_val - reg_1_4: %f \n", __func__, reg_1_4);
//     printf("[KERNEL_INTERNAL_CORE]|%s| read_val - reg_1_5: %f \n", __func__, reg_1_5);
//     printf("[KERNEL_INTERNAL_CORE]|%s| read_val - reg_1_6: %f \n", __func__, reg_1_6);
//     printf("[KERNEL_INTERNAL_CORE]|%s| index_val: (%d, %d, %d) \n", __func__, idx[0], idx[1], idx[2]);
// #endif

// #ifdef DEBUG_LOG
//     printf("[KERNEL_INTERNAL_CORE]|%s| exiting: kernel_ops_krnl_heat3D_core\n",__func__);
// #endif
// }

// class Stencil_ops_krnl_heat3D : public ops::hls::StencilCoreV2<stencil_type, read_num_points, vector_factor, ops::hls::CoefTypes::CONST_COEF,
//         read_stencil_size, read_stencil_dim>
// {
//     using ops::hls::StencilCoreV2<stencil_type, read_num_points, vector_factor, ops::hls::CoefTypes::CONST_COEF,
//             read_stencil_size, read_stencil_dim>::m_stencilConfig;
// public:

//     void stencilRun(
//             widen_stream_dt& arg0_wr_buffer,
//             widen_stream_dt& arg1_rd_buffer,
//             const float& param_k
//         )
//     {
//         ::ops::hls::StencilConfigCore stencilConfig = m_stencilConfig;
//     //read_origin_wide_diff_x: 0, read_origin_wide_diff: (0,0,1)

//         short i = -1;
//         short j = 0;
//         short k = -s_stencil_half_span_x;
//         unsigned short plane_diff = stencilConfig.grid_size[0] * stencilConfig.grid_size[1] - 1;

//         // point: (1,1,0)
//         // point: (1,0,1)
//         // point: (0,1,1)
//         // point: (1,1,1)
//         // point: (2,1,1)
//         // point: (1,2,1)
//         // point: (1,1,2)

//         unsigned short stencil3D_7pt_buf_p0_1_rd;

//         if (-1 * stencilConfig.grid_size[0] + 0 > 0)
//             stencil3D_7pt_buf_p0_1_rd = -1 * stencilConfig.grid_size[0] + 0;
//         else
//             stencil3D_7pt_buf_p0_1_rd = 0;

//         unsigned short stencil3D_7pt_buf_p0_1_wr;

//         if (1 * stencilConfig.grid_size[0] + 0 > 0)
//             stencil3D_7pt_buf_p0_1_wr = 1 * stencilConfig.grid_size[0] + 0;
//         else
//             stencil3D_7pt_buf_p0_1_wr = 0;
//         // read point: (0,1,1), write point: (1,0,1)
//         unsigned short stencil3D_7pt_buf_r0_1_p1_rd = 0;
//         unsigned short stencil3D_7pt_buf_r0_1_p1_wr = 1;
//         // read point: (1,2,1), write point: (2,1,1)
//         unsigned short stencil3D_7pt_buf_r1_2_p1_rd = 0;
//         unsigned short stencil3D_7pt_buf_r1_2_p1_wr = 1;

//         unsigned short stencil3D_7pt_buf_p1_2_rd;

//         if (-1 * stencilConfig.grid_size[0] + 0 > 0)
//             stencil3D_7pt_buf_p1_2_rd = -1 * stencilConfig.grid_size[0] + 0;
//         else
//             stencil3D_7pt_buf_p1_2_rd = 0;

//         unsigned short stencil3D_7pt_buf_p1_2_wr;

//         if (1 * stencilConfig.grid_size[0] + 0 > 0)
//             stencil3D_7pt_buf_p1_2_wr = 1 * stencilConfig.grid_size[0] + 0;
//         else
//             stencil3D_7pt_buf_p1_2_wr = 0;

//         #pragma HLS ARRAY_PARTITION variable = stencilConfig.lower_limit dim = 1 complete
//         #pragma HLS ARRAY_PARTITION variable = stencilConfig.upper_limit dim = 1 complete

//         unsigned short iter_limit = stencilConfig.outer_loop_limit *
//                 stencilConfig.grid_size[1] * stencilConfig.grid_size[0] ;

//         unsigned short arg1_read_lb_itr = 0 * stencilConfig.grid_size[0] * stencilConfig.grid_size[1];
//         unsigned short arg1_read_ub_itr = (0 + stencilConfig.grid_size[2]) * stencilConfig.grid_size[1] * stencilConfig.grid_size[0];

//         widen_dt arg0_update_val;
//         widen_dt arg1_read_val = 0;

//         widen_dt arg1_widenStencilValues[read_num_points];
//         #pragma HLS ARRAY_PARTITION variable = arg1_widenStencilValues dim = 1 complete

//         widen_dt arg1_buf_p0_1[max_depth];
//         #pragma HLS BIND_STORAGE variable = arg1_buf_p0_1 type = ram_t2p impl=uram latency=2
//         widen_dt arg1_buf_r0_1_p1[max_depth];
//         #pragma HLS BIND_STORAGE variable = arg1_buf_r0_1_p1 type = ram_t2p impl=uram latency=2
//         widen_dt arg1_buf_r1_2_p1[max_depth];
//         #pragma HLS BIND_STORAGE variable = arg1_buf_r1_2_p1 type = ram_t2p impl=uram latency=2
//         widen_dt arg1_buf_p1_2[max_depth];
//         #pragma HLS BIND_STORAGE variable = arg1_buf_p1_2 type = ram_t2p impl=uram latency=2

//         stencil_type arg1_rowArr_1_0[vector_factor + s_stencil_span_x];
//         #pragma HLS ARRAY_PARTITION variable = arg1_rowArr_1_0 dim=1 complete
//         stencil_type arg1_rowArr_0_1[vector_factor + s_stencil_span_x];
//         #pragma HLS ARRAY_PARTITION variable = arg1_rowArr_0_1 dim=1 complete
//         stencil_type arg1_rowArr_1_1[vector_factor + s_stencil_span_x];
//         #pragma HLS ARRAY_PARTITION variable = arg1_rowArr_1_1 dim=1 complete
//         stencil_type arg1_rowArr_2_1[vector_factor + s_stencil_span_x];
//         #pragma HLS ARRAY_PARTITION variable = arg1_rowArr_2_1 dim=1 complete
//         stencil_type arg1_rowArr_1_2[vector_factor + s_stencil_span_x];
//         #pragma HLS ARRAY_PARTITION variable = arg1_rowArr_1_2 dim=1 complete

//         const short cond_x_val = stencilConfig.grid_size[0] - 1;
//         const short cond_y_val = stencilConfig.grid_size[1] - 1;
//         const short cond_z_val = stencilConfig.outer_loop_limit - 1;

//         for (unsigned short itr = 0; itr < iter_limit; itr++)
//         {
//         #pragma HLS PIPELINE II=1

//             spc_temp_blocking_read:
//             {
//                 bool cond_x_terminate = (i == cond_x_val ? true : false);
//                 bool cond_y_terminate = (j == cond_y_val ? true : false);
//                 bool cond_z_terminate = (k == cond_z_val ? true : false);

// #ifdef DEBUG_LOG
//                 printf("[DEBUG][INTERNAL][ops_krnl_heat3D_PE_%d] loop params before update i(%d), "\
//                     "j(%d), "\
//                     "k(%d), "\
//                     "stencil3D_7pt_buf_p0_1_rd: %d, "\
//                     "stencil3D_7pt_buf_p0_1_wr: %d, "\
//                     "stencil3D_7pt_buf_r0_1_p1_rd: %d, "\
//                     "stencil3D_7pt_buf_r0_1_p1_wr: %d, "\
//                     "stencil3D_7pt_buf_r1_2_p1_rd: %d, "\
//                     "stencil3D_7pt_buf_r1_2_p1_wr: %d, "\
//                     "stencil3D_7pt_buf_p1_2_rd: %d, "\
//                     "stencil3D_7pt_buf_p1_2_wr: %d, "\
//                     "itr(%d)\n", m_PEId, i,
//                     j,
//                     k,
//                     stencil3D_7pt_buf_p0_1_rd,
//                     stencil3D_7pt_buf_p0_1_wr,
//                     stencil3D_7pt_buf_r0_1_p1_rd,
//                     stencil3D_7pt_buf_r0_1_p1_wr,
//                     stencil3D_7pt_buf_r1_2_p1_rd,
//                     stencil3D_7pt_buf_r1_2_p1_wr,
//                     stencil3D_7pt_buf_p1_2_rd,
//                     stencil3D_7pt_buf_p1_2_wr,
//                     itr);
// #endif
//                 if (cond_x_terminate)
//                     i = 0;
//                 else
//                     i++;
//                 if (cond_x_terminate && cond_y_terminate)
//                     j = 0;
//                 else if  (cond_x_terminate)
//                     j++;
//                 if (cond_x_terminate && cond_y_terminate && cond_z_terminate)
//                     k = 0;
//                 else if (cond_x_terminate && cond_y_terminate)
//                     k++;

//                 bool arg1_read_cond = (itr < arg1_read_ub_itr) and (itr >= arg1_read_lb_itr);

//                 if (arg1_read_cond)
//                     arg1_read_val = arg1_rd_buffer.read();

//                 arg1_widenStencilValues[0] = arg1_buf_p0_1[stencil3D_7pt_buf_p0_1_wr];
//                 arg1_widenStencilValues[1] = arg1_buf_r0_1_p1[stencil3D_7pt_buf_r0_1_p1_wr];
//                 arg1_buf_p0_1[stencil3D_7pt_buf_p0_1_rd] = arg1_widenStencilValues[1];
//                 arg1_widenStencilValues[2] = arg1_widenStencilValues[3];
//                 arg1_widenStencilValues[3] = arg1_widenStencilValues[4];
//                 arg1_widenStencilValues[4] = arg1_buf_r1_2_p1[stencil3D_7pt_buf_r1_2_p1_wr];
//                 arg1_buf_r0_1_p1[stencil3D_7pt_buf_r0_1_p1_rd] = arg1_widenStencilValues[2];
//                 arg1_widenStencilValues[5] = arg1_buf_p1_2[stencil3D_7pt_buf_p1_2_wr];
//                 arg1_buf_r1_2_p1[stencil3D_7pt_buf_r1_2_p1_rd] = arg1_widenStencilValues[5];
//                 arg1_widenStencilValues[6] = arg1_read_val;
//                 arg1_buf_p1_2[stencil3D_7pt_buf_p1_2_rd] = arg1_widenStencilValues[6];

//                 bool cond_end_of_line_buff_stencil3D_7pt_buf_p0_1_rd = stencil3D_7pt_buf_p0_1_rd >= (plane_diff);
//                 bool cond_end_of_line_buff_stencil3D_7pt_buf_p0_1_wr = stencil3D_7pt_buf_p0_1_wr >= (plane_diff);

//                 if (cond_end_of_line_buff_stencil3D_7pt_buf_p0_1_rd)
//                     stencil3D_7pt_buf_p0_1_rd = 0;
//                 else
//                     stencil3D_7pt_buf_p0_1_rd++;

//                 if (cond_end_of_line_buff_stencil3D_7pt_buf_p0_1_wr)
//                     stencil3D_7pt_buf_p0_1_wr = 0;
//                 else
//                     stencil3D_7pt_buf_p0_1_wr++;
//                 bool cond_end_of_line_buff_stencil3D_7pt_buf_r0_1_p1_rd = stencil3D_7pt_buf_r0_1_p1_rd >= (stencilConfig.grid_size[0] - 1);
//                 bool cond_end_of_line_buff_stencil3D_7pt_buf_r0_1_p1_wr = stencil3D_7pt_buf_r0_1_p1_wr >= (stencilConfig.grid_size[0] - 1);

//                 if (cond_end_of_line_buff_stencil3D_7pt_buf_r0_1_p1_rd)
//                     stencil3D_7pt_buf_r0_1_p1_rd = 0;
//                 else
//                     stencil3D_7pt_buf_r0_1_p1_rd++;

//                 if (cond_end_of_line_buff_stencil3D_7pt_buf_r0_1_p1_wr)
//                     stencil3D_7pt_buf_r0_1_p1_wr = 0;
//                 else
//                     stencil3D_7pt_buf_r0_1_p1_wr++;
//                 bool cond_end_of_line_buff_stencil3D_7pt_buf_r1_2_p1_rd = stencil3D_7pt_buf_r1_2_p1_rd >= (stencilConfig.grid_size[0] - 1);
//                 bool cond_end_of_line_buff_stencil3D_7pt_buf_r1_2_p1_wr = stencil3D_7pt_buf_r1_2_p1_wr >= (stencilConfig.grid_size[0] - 1);

//                 if (cond_end_of_line_buff_stencil3D_7pt_buf_r1_2_p1_rd)
//                     stencil3D_7pt_buf_r1_2_p1_rd = 0;
//                 else
//                     stencil3D_7pt_buf_r1_2_p1_rd++;

//                 if (cond_end_of_line_buff_stencil3D_7pt_buf_r1_2_p1_wr)
//                     stencil3D_7pt_buf_r1_2_p1_wr = 0;
//                 else
//                     stencil3D_7pt_buf_r1_2_p1_wr++;
//                 bool cond_end_of_line_buff_stencil3D_7pt_buf_p1_2_rd = stencil3D_7pt_buf_p1_2_rd >= (plane_diff);
//                 bool cond_end_of_line_buff_stencil3D_7pt_buf_p1_2_wr = stencil3D_7pt_buf_p1_2_wr >= (plane_diff);

//                 if (cond_end_of_line_buff_stencil3D_7pt_buf_p1_2_rd)
//                     stencil3D_7pt_buf_p1_2_rd = 0;
//                 else
//                     stencil3D_7pt_buf_p1_2_rd++;

//                 if (cond_end_of_line_buff_stencil3D_7pt_buf_p1_2_wr)
//                     stencil3D_7pt_buf_p1_2_wr = 0;
//                 else
//                     stencil3D_7pt_buf_p1_2_wr++;

// #ifdef DEBUG_LOG
//                 printf("[DEBUG][INTERNAL][ops_krnl_heat3D_PE_%d] loop params after update i(%d), "\
//                                 "j(%d), "\
//                                 "k(%d), "\
//                                 "stencil3D_7pt_buf_p0_1_rd(%d), "\
//                                 "stencil3D_7pt_buf_p0_1_wr(%d), "\
//                                 "stencil3D_7pt_buf_r0_1_p1_rd(%d), "\
//                                 "stencil3D_7pt_buf_r0_1_p1_wr(%d), "\
//                                 "stencil3D_7pt_buf_r1_2_p1_rd(%d), "\
//                                 "stencil3D_7pt_buf_r1_2_p1_wr(%d), "\
//                                 "stencil3D_7pt_buf_p1_2_rd(%d), "\
//                                 "stencil3D_7pt_buf_p1_2_wr(%d), "\
//                                 "itr(%d)\n", m_PEId, i,
//                                 j,
//                                 k,
//                                 stencil3D_7pt_buf_p0_1_rd,
//                                 stencil3D_7pt_buf_p0_1_wr,
//                                 stencil3D_7pt_buf_r0_1_p1_rd,
//                                 stencil3D_7pt_buf_r0_1_p1_wr,
//                                 stencil3D_7pt_buf_r1_2_p1_rd,
//                                 stencil3D_7pt_buf_r1_2_p1_wr,
//                                 stencil3D_7pt_buf_p1_2_rd,
//                                 stencil3D_7pt_buf_p1_2_wr,
//                                 itr);

//                 printf("[DEBUG][INTERNAL][ops_krnl_heat3D_PE_%d] --------------------------------------------------------\n\n", m_PEId);

//                 printf("[DEBUG][INTERNAL][ops_krnl_heat3D_PE_%d] read values arg1: (", m_PEId);
//                 for (int ri = 0; ri < vector_factor; ri++)
//                 {
//                     ops::hls::DataConv tmpConverter;
//                     tmpConverter.i = arg1_read_val.range((ri + 1)*s_datatype_size - 1, ri * s_datatype_size);
//                     printf("%f ", tmpConverter.f);
//                 }
//                 printf(")\n");
// #endif
//             }

//             vec2arr: for (unsigned short x = 0; x < vector_factor; x++)
//             {
// #pragma HLS UNROLL factor=vector_factor
//                 ops::hls::DataConv arg1_tmpConverter_1_0;
//                 arg1_tmpConverter_1_0.i = arg1_widenStencilValues[0].range(s_datatype_size * (x + 1) - 1, x * s_datatype_size);
//                 arg1_rowArr_1_0[x + s_stencil_half_span_x] = arg1_tmpConverter_1_0.f;
//                 ops::hls::DataConv arg1_tmpConverter_0_1;
//                 arg1_tmpConverter_0_1.i = arg1_widenStencilValues[1].range(s_datatype_size * (x + 1) - 1, x * s_datatype_size);
//                 arg1_rowArr_0_1[x + s_stencil_half_span_x] = arg1_tmpConverter_0_1.f;
//                 ops::hls::DataConv arg1_tmpConverter_1_1;
//                 arg1_tmpConverter_1_1.i = arg1_widenStencilValues[3].range(s_datatype_size * (x + 1) - 1, x * s_datatype_size);
//                 arg1_rowArr_1_1[x + s_stencil_half_span_x] = arg1_tmpConverter_1_1.f;
//                 ops::hls::DataConv arg1_tmpConverter_2_1;
//                 arg1_tmpConverter_2_1.i = arg1_widenStencilValues[5].range(s_datatype_size * (x + 1) - 1, x * s_datatype_size);
//                 arg1_rowArr_2_1[x + s_stencil_half_span_x] = arg1_tmpConverter_2_1.f;
//                 ops::hls::DataConv arg1_tmpConverter_1_2;
//                 arg1_tmpConverter_1_2.i = arg1_widenStencilValues[6].range(s_datatype_size * (x + 1) - 1, x * s_datatype_size);
//                 arg1_rowArr_1_2[x + s_stencil_half_span_x] = arg1_tmpConverter_1_2.f;

//             }
//             vec2arr_rest:
//             {
//                 //diff = -1
//                 // access_idx = 0
//                 ops::hls::DataConv arg1_tmpConverter_0_1_1_7;
//                 arg1_tmpConverter_0_1_1_7.i = arg1_widenStencilValues[2].range(s_datatype_size * (7 + 1) - 1, s_datatype_size * 7);
//                 arg1_rowArr_1_1[0] = arg1_tmpConverter_0_1_1_7.f;
//                 //diff = 1
//                 // access_idx = 9
//                 ops::hls::DataConv arg1_tmpConverter_2_1_1_0;
//                 arg1_tmpConverter_2_1_1_0.i = arg1_widenStencilValues[4].range(s_datatype_size * (0 + 1) - 1, s_datatype_size * 0);
//                 arg1_rowArr_1_1[9] = arg1_tmpConverter_2_1_1_0.f;
//             }

//             process: for (unsigned short x = 0; x < vector_factor; x++)
//             {
// #pragma HLS UNROLL factor=vector_factor
//                 short index = (i << shift_bits) + x;
//                 bool neg_cond = register_it(
//                         (index < stencilConfig.lower_limit[0])
//                         || (index >= stencilConfig.upper_limit[0])
//                         || (j < stencilConfig.lower_limit[1])
//                         || (j >= stencilConfig.upper_limit[1])
//                         || (k < stencilConfig.lower_limit[2])
//                         || (k >= stencilConfig.upper_limit[2])
//                 );

// #ifdef DEBUG_LOG
//                 printf("[DEBUG][INTERNAL][ops_krnl_heat3D_PE_%d] index=(%d, %d, %d), lowerbound=(%d, %d, %d), upperbound=(%d, %d, %d), neg_cond=%d\n", m_PEId, index, j, k,
//                             stencilConfig.lower_limit[0], stencilConfig.lower_limit[1], stencilConfig.lower_limit[2],
//                             stencilConfig.upper_limit[0], stencilConfig.upper_limit[1], stencilConfig.upper_limit[2], neg_cond);

// #endif
//                 stencil_type arg0_result;

//                 short idx[] = {index, j, k};

//                 kernel_ops_krnl_heat3D_core(
//                         arg0_result,
//                         arg1_rowArr_1_0[x + 1],
//                         arg1_rowArr_0_1[x + 1],
//                         arg1_rowArr_1_1[x + 0],
//                         arg1_rowArr_1_1[x + 1],
//                         arg1_rowArr_1_1[x + 2],
//                         arg1_rowArr_2_1[x + 1],
//                         arg1_rowArr_1_2[x + 1],
//                         &param_k,
//                         idx
//                 );

//                 ops::hls::DataConv arg0_tmpConvWrite;

//                 if (not neg_cond)
//                 {
//                     arg0_tmpConvWrite.f = arg0_result;
//                 }
//                 else
//                 {

//                     arg0_tmpConvWrite.f = arg1_rowArr_1_1[x + 1];
//                 }

//                 arg0_update_val.range(s_datatype_size * (x + 1) - 1, x * s_datatype_size) = arg0_tmpConvWrite.i;

//             }

//             write:
//             {
//                 bool cond_write = (k >= 0);

//                 if (cond_write)
//                 {
//                     arg0_wr_buffer <<  arg0_update_val;
//                 }
//             }
//         }
//     }
// };

// void kernel_ops_krnl_heat3D_PE(const short& PEId, const ops::hls::StencilConfigCore& stencilConfig,
//             Stencil_ops_krnl_heat3D::widen_stream_dt& arg0_wr_buffer,
//             Stencil_ops_krnl_heat3D::widen_stream_dt& arg1_rd_buffer,
//             const float& param_k
// )
// {
//     Stencil_ops_krnl_heat3D stencil;

// #ifdef DEBUG_LOG
//     printf("[KERNEL_DEBUG]|%s| stencil config gridSize: %d (xblocks), %d, %d\n", __func__, stencilConfig.grid_size[0], stencilConfig.grid_size[1], stencilConfig.grid_size[2]);
// #endif
//     stencil.setConfig(PEId, stencilConfig);

// #ifdef DEBUG_LOG
//     printf("[KERNEL_DEBUG]|%s| starting stencil kernel PE\n", __func__);
// #endif

//     stencil.stencilRun(
//             arg0_wr_buffer,
//             arg1_rd_buffer,
//             param_k

// );

// #ifdef DEBUG_LOG
//     printf("[KERNEL_DEBUG]|%s| Ending stencil kernel PE\n", __func__);
// #endif
// }
