#include "stencil.h"
//#include "common_config.hpp"
#include <ops_hls_stencil_core_v2.hpp>

#define OPTIMIZED_REDUCTION
#define DEBUG_VERBOSE

// template <typename T>
// static T register_it(T x)
// {
// #pragma HLS inline off
// 	T temp = x;
// 	return temp;
// }

// static void axis2_fifo256(hls::stream <t_pkt> &in, hls::stream<uint256_dt> &out,  const unsigned int total_itr)
// {
// 	for (int itr = 0; itr < total_itr; itr++){
// 		#pragma HLS PIPELINE II=1
// 		#pragma HLS loop_tripcount min=min_grid max=max_grid avg=avg_grid
// 		t_pkt tmp = in.read();
// //		printf("reading 256 bit input from SLR. iter: %d\n", itr);
// 		out << tmp.data;
// 	}
// }

// static void fifo256_2axis(hls::stream <uint256_dt> &in, hls::stream<t_pkt> &out, const unsigned int total_itr)
// {
// 	for (int itr = 0; itr < total_itr; itr++){
// 		#pragma HLS PIPELINE II=1
// 		#pragma HLS loop_tripcount min=min_grid max=max_grid avg=avg_grid
// 		t_pkt tmp;
// 		tmp.data = in.read();
// 		out.write(tmp);
// 	}
// }

// static void inline get_stencil_coefficent(const float & alpha, const float & beta,
// 		const float & delta_t, const int & init_idx, float * a,
// 		float * b, float * c)
// {
// 	for (int i = 0; i < VEC_FACTOR; i++)
// 	{
// #pragma HLS UNROLL

// 		int idx = init_idx + i;
// 		a[i] = 0.5 * (alpha * std::pow(idx,2) - beta * idx);
// 		b[i] = 1 - alpha * std::pow(idx,2) - beta;
// 		c[i] = 0.5 * (alpha * std::pow(idx,2) + beta * idx);
// 	}
// }

// static void process_grid(hls::stream<uint256_dt> &rd_buffer, hls::stream<uint256_dt> &wr_buffer, struct data_G data_g,
// 		const float coefficients[7])
// {
// 	float s_1_1_2_arr[VEC_FACTOR];
// 	float s_1_2_1_arr[VEC_FACTOR];
// 	float s_1_1_1_arr[VEC_FACTOR+2];
// 	float s_1_0_1_arr[VEC_FACTOR];
// 	float s_1_1_0_arr[VEC_FACTOR];

// 	float mem_wr[VEC_FACTOR];

// #pragma HLS ARRAY_PARTITION variable=s_1_1_2_arr complete dim=1
// #pragma HLS ARRAY_PARTITION variable=s_1_2_1_arr complete dim=1
// #pragma HLS ARRAY_PARTITION variable=s_1_1_1_arr complete dim=1
// #pragma HLS ARRAY_PARTITION variable=s_1_0_1_arr complete dim=1
// #pragma HLS ARRAY_PARTITION variable=s_1_1_0_arr complete dim=1
// #pragma HLS ARRAY_PARTITION variable=mem_wr complete dim=1

// 	uint256_dt windowBuff_1[max_depth_xy];
// 	uint256_dt windowBuff_2[max_depth_8];
// 	uint256_dt windowBuff_3[max_depth_8];
// 	uint256_dt windowBuff_4[max_depth_xy];

// #pragma HLS BIND_STORAGE variable=windowBuff_1 type=ram_t2p impl=uram latency=1
// #pragma HLS BIND_STORAGE variable=windowBuff_2 type=ram_t2p impl=uram latency=1
// #pragma HLS BIND_STORAGE variable=windowBuff_3 type=ram_t2p impl=uram latency=1
// #pragma HLS BIND_STORAGE variable=windowBuff_4 type=ram_t2p impl=uram latency=1

// 	uint256_dt s_1_1_2, s_1_2_1, s_1_1_1, s_1_1_1b, s_1_1_1f, s_1_0_1, s_1_1_0;
// 	uint256_dt update_j;

// 	unsigned short i = 0, j = 0, k = 0;
// 	unsigned short j_p = 0, j_l = 0;

// 	for(unsigned int itr = 0; itr < data_g.gridsize_pr; itr++)
// 	{
// #pragma HLS loop_tripcount min=min_grid max=max_grid avg=avg_grid
// #pragma HLS PIPELINE II=1

// 		spc_temp_blocking_read:
// 		{
// 			bool cond_x = (i == data_g.xblocks);
// 			bool cond_y = (j == data_g.grid_size_y - 1);
// 			bool cond_z = (k == data_g.limit_z - 1);

// 			if (cond_x)
// 				i = 0;

// 			if (cond_y && cond_x)
// 				j = 0;
// 			else if(cond_x)
// 				j++;

// 			if (cond_x && cond_y && cond_z)
// 				k = 1;
// 			else if(cond_y && cond_x)
// 				k++;

// 			s_1_1_0 = windowBuff_4[j_p];

// 			s_1_0_1 = windowBuff_3[j_l];
// 			windowBuff_4[j_p] = s_1_0_1;

// 			s_1_1_1b = s_1_1_1;
// 			windowBuff_3[j_l] = s_1_1_1b;

// 			s_1_1_1 = s_1_1_1f;
// 			s_1_1_1f = windowBuff_2[j_l];

// 			s_1_2_1 = windowBuff_1[j_p];
// 			windowBuff_2[j_l] = s_1_2_1;

// 			bool cond_read = (itr < data_g.gridsize_da);

// 			if (cond_read)
// 			{
// //				printf("reading data for iteration %d\n", itr);
// 				s_1_1_2 = rd_buffer.read();
// 			}

// 			windowBuff_1[j_p] = s_1_1_2;

// 			bool cond_eo_plane = (j_p == data_g.plane_diff);
// 			bool cond_eo_line = (j_l == data_g.line_diff);

// 			if (cond_eo_plane)
// 				j_p = 0;
// 			else
// 				j_p++;

// 			if(cond_eo_line)
// 				j_l = 0;
// 			else
// 				j_l++;
// 		}

// 		vec2arr:
// 		{
// 			for (int id = 0; id < VEC_FACTOR; id++)

// 			{
// #pragma HLS UNROLL

// 				data_conv s_1_1_2_u, s_1_2_1_u, s_1_1_1_u, s_1_0_1_u, s_1_1_0_u;

// 				s_1_1_2_u.i = s_1_1_2.range(DATATYPE_SIZE * (id + 1) - 1, id * DATATYPE_SIZE);
// 				s_1_2_1_u.i = s_1_2_1.range(DATATYPE_SIZE * (id + 1) - 1, id * DATATYPE_SIZE);
// 				s_1_1_1_u.i = s_1_1_1.range(DATATYPE_SIZE * (id + 1) - 1, id * DATATYPE_SIZE);
// 				s_1_0_1_u.i = s_1_0_1.range(DATATYPE_SIZE * (id + 1) - 1, id * DATATYPE_SIZE);
// 				s_1_1_0_u.i = s_1_1_0.range(DATATYPE_SIZE * (id + 1) - 1, id * DATATYPE_SIZE);

// 				s_1_1_2_arr[id]   =  s_1_1_2_u.f;
// 				s_1_2_1_arr[id]   =  s_1_2_1_u.f;
// 				s_1_1_1_arr[id+1] =  s_1_1_1_u.f;
// 				s_1_0_1_arr[id]   =  s_1_0_1_u.f;
// 				s_1_1_0_arr[id]   =  s_1_1_0_u.f;
// 			}

// 			data_conv tmp1_o1, tmp2_o2;
// 			tmp1_o1.i = s_1_1_1b.range(DATATYPE_SIZE * (VEC_FACTOR) - 1, (VEC_FACTOR-1) * DATATYPE_SIZE);
// 			tmp2_o2.i = s_1_1_1f.range(DATATYPE_SIZE * (0 + 1) - 1, 0 * DATATYPE_SIZE);
// 			s_1_1_1_arr[0] = tmp1_o1.f;
// 			s_1_1_1_arr[VEC_FACTOR + 1] = tmp2_o2.f;
// 		}

// 		process:
// 		{
// 			unsigned short y_index = j + data_g.offset_y;

// 			for (short q = 0; q < VEC_FACTOR; q++)
// 			{
// #pragma HLS UNROLL

// 				short index = (i << SHIFT_BITS) + q + data_g.offset_x;

// 				float r1_1_2 =  s_1_1_2_arr[q] * coefficients[0];
// 				float r1_2_1 =  s_1_2_1_arr[q] * coefficients[1];
// 				float r0_1_1 =  s_1_1_1_arr[q] * coefficients[2];
// 				float r1_1_1 =  s_1_1_1_arr[q+1] * coefficients[3];
// 				float r2_1_1 =  s_1_1_1_arr[q+2] * coefficients[4];
// 				float r1_0_1 =  s_1_0_1_arr[q] * coefficients[5];
// 				float r1_1_0 =  s_1_1_0_arr[q] * coefficients[6];

// #ifdef OPTIMIZED_REDUCTION
// 				float f1 = r1_1_2 + r1_2_1;
// 				float f2 = r0_1_1 + r1_1_1;
// 				float f3 = r2_1_1 + r1_0_1;

// #pragma HLS BIND_OP variable=f1 op=fadd
// #pragma HLS BIND_OP variable=f2 op=fadd

// 				float r1 = f1 + f2;
// 				float r2 = f3 + r1_1_0;

// 				float result = r1 + r2;
// #else
// 				float result = r1_1_2 + r1_2_1 + r0_1_1 + r1_1_1 + r2_1_1 + r1_0_1 + r1_1_0;
// #endif

// 				bool cond_change = register_it <bool> (index <= data_g.offset_x || index > data_g.sizex
// 						|| (k <= 1) || (k >= data_g.limit_z -1) || (y_index <= 0) || (y_index >= data_g.grid_size_y - 1));

// 				mem_wr[q] = cond_change ? s_1_1_1[q+1] : result;
// 			}
// 		}

// 		array2vec: for (int q = 0; q < VEC_FACTOR; q++)
// 		{
// #pragma HLS UNROLL
// 			data_conv tmp;
// 			tmp.f = mem_wr[q];
// 			update_j.range(DATATYPE_SIZE * (q + 1) - 1, q * DATATYPE_SIZE) = tmp.i;
// 		}

// 		write:
// 		{
// 			bool cond_wr = (k >= 1) && (k < data_g.limit_z);

// 			if (cond_wr)
// 				wr_buffer << update_j;
// 		}

// 		// move cell block
// 		i++;

// 	}

// }


static constexpr unsigned short read_num_points = 7;
static constexpr unsigned short read_stencil_size = 3;
static constexpr unsigned short read_stencil_dim = 3;

static constexpr unsigned short write_num_points = 1;
static constexpr unsigned short write_stencil_size = 1;
static constexpr unsigned short write_stencil_dim = 3;

inline void kernel_ops_krnl_heat3D_core(
        stencil_type& reg_0_0,
        const stencil_type& reg_1_0,
        const stencil_type& reg_1_1,
        const stencil_type& reg_1_2,
        const stencil_type& reg_1_3,
        const stencil_type& reg_1_4,
        const stencil_type& reg_1_5,
        const stencil_type& reg_1_6,
        const float* K,
        const short idx[3]
)
{
#ifdef DEBUG_LOG
    printf("[KERNEL_INTERNAL_CORE]|%s| starting kernel core: kernel_ops_krnl_heat3D_core\n",__func__);
#endif

	reg_0_0 = (1 - 6 * (*K)) * reg_1_3
			+ (*K) * (reg_1_4 + reg_1_2 + reg_1_5 + reg_1_1 + reg_1_6 + reg_1_0);

#ifdef DEBUG_LOG
    printf("[KERNEL_INTERNAL_CORE]|%s| write_val - reg_0_0: %f \n", __func__, reg_0_0);
    printf("[KERNEL_INTERNAL_CORE]|%s| read_val - reg_1_0: %f \n", __func__, reg_1_0);
    printf("[KERNEL_INTERNAL_CORE]|%s| read_val - reg_1_1: %f \n", __func__, reg_1_1);
    printf("[KERNEL_INTERNAL_CORE]|%s| read_val - reg_1_2: %f \n", __func__, reg_1_2);
    printf("[KERNEL_INTERNAL_CORE]|%s| read_val - reg_1_3: %f \n", __func__, reg_1_3);
    printf("[KERNEL_INTERNAL_CORE]|%s| read_val - reg_1_4: %f \n", __func__, reg_1_4);
    printf("[KERNEL_INTERNAL_CORE]|%s| read_val - reg_1_5: %f \n", __func__, reg_1_5);
    printf("[KERNEL_INTERNAL_CORE]|%s| read_val - reg_1_6: %f \n", __func__, reg_1_6);
    printf("[KERNEL_INTERNAL_CORE]|%s| index_val: (%d, %d, %d) \n", __func__, idx[0], idx[1], idx[2]);
#endif

#ifdef DEBUG_LOG
    printf("[KERNEL_INTERNAL_CORE]|%s| exiting: kernel_ops_krnl_heat3D_core\n",__func__);
#endif
}

class Stencil_ops_krnl_heat3D : public ops::hls::StencilCoreV2<stencil_type, read_num_points, vector_factor, ops::hls::CoefTypes::CONST_COEF,
        read_stencil_size, read_stencil_dim>
{
    using ops::hls::StencilCoreV2<stencil_type, read_num_points, vector_factor, ops::hls::CoefTypes::CONST_COEF,
            read_stencil_size, read_stencil_dim>::m_stencilConfig;
public:

    void stencilRun(
            widen_stream_dt& arg0_wr_buffer,
            widen_stream_dt& arg1_rd_buffer,
            const float& param_k
        )
    {
        ::ops::hls::StencilConfigCore stencilConfig = m_stencilConfig;
    //read_origin_wide_diff_x: 0, read_origin_wide_diff: (0,0,1)

        short i = -1;
        short j = 0; 
        short k = -s_stencil_half_span_x;
        unsigned short plane_diff = stencilConfig.grid_size[0] * stencilConfig.grid_size[1] - 1;

        // point: (1,1,0)
        // point: (1,0,1)
        // point: (0,1,1)
        // point: (1,1,1)
        // point: (2,1,1)
        // point: (1,2,1)
        // point: (1,1,2)

        unsigned short stencil3D_7pt_buf_p0_1_rd;

        if (-1 * stencilConfig.grid_size[0] + 0 > 0)
            stencil3D_7pt_buf_p0_1_rd = -1 * stencilConfig.grid_size[0] + 0;
        else
            stencil3D_7pt_buf_p0_1_rd = 0;

        unsigned short stencil3D_7pt_buf_p0_1_wr;

        if (1 * stencilConfig.grid_size[0] + 0 > 0)
            stencil3D_7pt_buf_p0_1_wr = 1 * stencilConfig.grid_size[0] + 0;
        else
            stencil3D_7pt_buf_p0_1_wr = 0;
        // read point: (0,1,1), write point: (1,0,1)
        unsigned short stencil3D_7pt_buf_r0_1_p1_rd = 0;
        unsigned short stencil3D_7pt_buf_r0_1_p1_wr = 1;
        // read point: (1,2,1), write point: (2,1,1)
        unsigned short stencil3D_7pt_buf_r1_2_p1_rd = 0;
        unsigned short stencil3D_7pt_buf_r1_2_p1_wr = 1;

        unsigned short stencil3D_7pt_buf_p1_2_rd;

        if (-1 * stencilConfig.grid_size[0] + 0 > 0)
            stencil3D_7pt_buf_p1_2_rd = -1 * stencilConfig.grid_size[0] + 0;
        else
            stencil3D_7pt_buf_p1_2_rd = 0;

        unsigned short stencil3D_7pt_buf_p1_2_wr;

        if (1 * stencilConfig.grid_size[0] + 0 > 0)
            stencil3D_7pt_buf_p1_2_wr = 1 * stencilConfig.grid_size[0] + 0;
        else
            stencil3D_7pt_buf_p1_2_wr = 0;

        #pragma HLS ARRAY_PARTITION variable = stencilConfig.lower_limit dim = 1 complete
        #pragma HLS ARRAY_PARTITION variable = stencilConfig.upper_limit dim = 1 complete

        unsigned short iter_limit = stencilConfig.outer_loop_limit * 
                stencilConfig.grid_size[1] * stencilConfig.grid_size[0] ;

        unsigned short arg1_read_lb_itr = 0 * stencilConfig.grid_size[0] * stencilConfig.grid_size[1];
        unsigned short arg1_read_ub_itr = (0 + stencilConfig.grid_size[2]) * stencilConfig.grid_size[1] * stencilConfig.grid_size[0];

        widen_dt arg0_update_val;
        widen_dt arg1_read_val = 0;

        widen_dt arg1_widenStencilValues[read_num_points];
        #pragma HLS ARRAY_PARTITION variable = arg1_widenStencilValues dim = 1 complete

        widen_dt arg1_buf_p0_1[max_depth];
        #pragma HLS BIND_STORAGE variable = arg1_buf_p0_1 type = ram_t2p impl=uram latency=2
        widen_dt arg1_buf_r0_1_p1[max_depth];
        #pragma HLS BIND_STORAGE variable = arg1_buf_r0_1_p1 type = ram_t2p impl=uram latency=2
        widen_dt arg1_buf_r1_2_p1[max_depth];
        #pragma HLS BIND_STORAGE variable = arg1_buf_r1_2_p1 type = ram_t2p impl=uram latency=2
        widen_dt arg1_buf_p1_2[max_depth];
        #pragma HLS BIND_STORAGE variable = arg1_buf_p1_2 type = ram_t2p impl=uram latency=2

        stencil_type arg1_rowArr_1_0[vector_factor + s_stencil_span_x];
        #pragma HLS ARRAY_PARTITION variable = arg1_rowArr_1_0 dim=1 complete
        stencil_type arg1_rowArr_0_1[vector_factor + s_stencil_span_x];
        #pragma HLS ARRAY_PARTITION variable = arg1_rowArr_0_1 dim=1 complete
        stencil_type arg1_rowArr_1_1[vector_factor + s_stencil_span_x];
        #pragma HLS ARRAY_PARTITION variable = arg1_rowArr_1_1 dim=1 complete
        stencil_type arg1_rowArr_2_1[vector_factor + s_stencil_span_x];
        #pragma HLS ARRAY_PARTITION variable = arg1_rowArr_2_1 dim=1 complete
        stencil_type arg1_rowArr_1_2[vector_factor + s_stencil_span_x];
        #pragma HLS ARRAY_PARTITION variable = arg1_rowArr_1_2 dim=1 complete

        const short cond_x_val = stencilConfig.grid_size[0] - 1; 
        const short cond_y_val = stencilConfig.grid_size[1] - 1;
        const short cond_z_val = stencilConfig.outer_loop_limit - 1;

        for (unsigned short itr = 0; itr < iter_limit; itr++)
        {
        #pragma HLS PIPELINE II=1

            spc_temp_blocking_read:
            {
                bool cond_x_terminate = (i == cond_x_val ? true : false); 
                bool cond_y_terminate = (j == cond_y_val ? true : false);
                bool cond_z_terminate = (k == cond_z_val ? true : false);

#ifdef DEBUG_LOG
                printf("[DEBUG][INTERNAL][ops_krnl_heat3D_PE_%d] loop params before update i(%d), "\
                    "j(%d), "\
                    "k(%d), "\
                    "stencil3D_7pt_buf_p0_1_rd: %d, "\
                    "stencil3D_7pt_buf_p0_1_wr: %d, "\
                    "stencil3D_7pt_buf_r0_1_p1_rd: %d, "\
                    "stencil3D_7pt_buf_r0_1_p1_wr: %d, "\
                    "stencil3D_7pt_buf_r1_2_p1_rd: %d, "\
                    "stencil3D_7pt_buf_r1_2_p1_wr: %d, "\
                    "stencil3D_7pt_buf_p1_2_rd: %d, "\
                    "stencil3D_7pt_buf_p1_2_wr: %d, "\
                    "itr(%d)\n", m_PEId, i, 
                    j,
                    k,
                    stencil3D_7pt_buf_p0_1_rd, 
                    stencil3D_7pt_buf_p0_1_wr, 
                    stencil3D_7pt_buf_r0_1_p1_rd, 
                    stencil3D_7pt_buf_r0_1_p1_wr, 
                    stencil3D_7pt_buf_r1_2_p1_rd, 
                    stencil3D_7pt_buf_r1_2_p1_wr, 
                    stencil3D_7pt_buf_p1_2_rd, 
                    stencil3D_7pt_buf_p1_2_wr, 
                    itr);
#endif
                if (cond_x_terminate)
                    i = 0;
                else
                    i++;
                if (cond_x_terminate && cond_y_terminate)
                    j = 0;
                else if  (cond_x_terminate)
                    j++;
                if (cond_x_terminate && cond_y_terminate && cond_z_terminate)
                    k = 0;
                else if (cond_x_terminate && cond_y_terminate)
                    k++;

                bool arg1_read_cond = (itr < arg1_read_ub_itr) and (itr >= arg1_read_lb_itr);

                if (arg1_read_cond)
                    arg1_read_val = arg1_rd_buffer.read();

                arg1_widenStencilValues[0] = arg1_buf_p0_1[stencil3D_7pt_buf_p0_1_wr];                
                arg1_widenStencilValues[1] = arg1_buf_r0_1_p1[stencil3D_7pt_buf_r0_1_p1_wr];                
                arg1_buf_p0_1[stencil3D_7pt_buf_p0_1_rd] = arg1_widenStencilValues[1];                
                arg1_widenStencilValues[2] = arg1_widenStencilValues[3];                
                arg1_widenStencilValues[3] = arg1_widenStencilValues[4];                
                arg1_widenStencilValues[4] = arg1_buf_r1_2_p1[stencil3D_7pt_buf_r1_2_p1_wr];                
                arg1_buf_r0_1_p1[stencil3D_7pt_buf_r0_1_p1_rd] = arg1_widenStencilValues[2];                
                arg1_widenStencilValues[5] = arg1_buf_p1_2[stencil3D_7pt_buf_p1_2_wr];                
                arg1_buf_r1_2_p1[stencil3D_7pt_buf_r1_2_p1_rd] = arg1_widenStencilValues[5];                
                arg1_widenStencilValues[6] = arg1_read_val;                
                arg1_buf_p1_2[stencil3D_7pt_buf_p1_2_rd] = arg1_widenStencilValues[6];

                bool cond_end_of_line_buff_stencil3D_7pt_buf_p0_1_rd = stencil3D_7pt_buf_p0_1_rd >= (plane_diff);
                bool cond_end_of_line_buff_stencil3D_7pt_buf_p0_1_wr = stencil3D_7pt_buf_p0_1_wr >= (plane_diff);

                if (cond_end_of_line_buff_stencil3D_7pt_buf_p0_1_rd)
                    stencil3D_7pt_buf_p0_1_rd = 0;
                else
                    stencil3D_7pt_buf_p0_1_rd++;

                if (cond_end_of_line_buff_stencil3D_7pt_buf_p0_1_wr)
                    stencil3D_7pt_buf_p0_1_wr = 0;
                else
                    stencil3D_7pt_buf_p0_1_wr++;
                bool cond_end_of_line_buff_stencil3D_7pt_buf_r0_1_p1_rd = stencil3D_7pt_buf_r0_1_p1_rd >= (stencilConfig.grid_size[0] - 1);
                bool cond_end_of_line_buff_stencil3D_7pt_buf_r0_1_p1_wr = stencil3D_7pt_buf_r0_1_p1_wr >= (stencilConfig.grid_size[0] - 1);

                if (cond_end_of_line_buff_stencil3D_7pt_buf_r0_1_p1_rd)
                    stencil3D_7pt_buf_r0_1_p1_rd = 0;
                else
                    stencil3D_7pt_buf_r0_1_p1_rd++;

                if (cond_end_of_line_buff_stencil3D_7pt_buf_r0_1_p1_wr)
                    stencil3D_7pt_buf_r0_1_p1_wr = 0;
                else
                    stencil3D_7pt_buf_r0_1_p1_wr++;
                bool cond_end_of_line_buff_stencil3D_7pt_buf_r1_2_p1_rd = stencil3D_7pt_buf_r1_2_p1_rd >= (stencilConfig.grid_size[0] - 1);
                bool cond_end_of_line_buff_stencil3D_7pt_buf_r1_2_p1_wr = stencil3D_7pt_buf_r1_2_p1_wr >= (stencilConfig.grid_size[0] - 1);

                if (cond_end_of_line_buff_stencil3D_7pt_buf_r1_2_p1_rd)
                    stencil3D_7pt_buf_r1_2_p1_rd = 0;
                else
                    stencil3D_7pt_buf_r1_2_p1_rd++;

                if (cond_end_of_line_buff_stencil3D_7pt_buf_r1_2_p1_wr)
                    stencil3D_7pt_buf_r1_2_p1_wr = 0;
                else
                    stencil3D_7pt_buf_r1_2_p1_wr++;
                bool cond_end_of_line_buff_stencil3D_7pt_buf_p1_2_rd = stencil3D_7pt_buf_p1_2_rd >= (plane_diff);
                bool cond_end_of_line_buff_stencil3D_7pt_buf_p1_2_wr = stencil3D_7pt_buf_p1_2_wr >= (plane_diff);

                if (cond_end_of_line_buff_stencil3D_7pt_buf_p1_2_rd)
                    stencil3D_7pt_buf_p1_2_rd = 0;
                else
                    stencil3D_7pt_buf_p1_2_rd++;

                if (cond_end_of_line_buff_stencil3D_7pt_buf_p1_2_wr)
                    stencil3D_7pt_buf_p1_2_wr = 0;
                else
                    stencil3D_7pt_buf_p1_2_wr++;

#ifdef DEBUG_LOG
                printf("[DEBUG][INTERNAL][ops_krnl_heat3D_PE_%d] loop params after update i(%d), "\
                                "j(%d), "\
                                "k(%d), "\
                                "stencil3D_7pt_buf_p0_1_rd(%d), "\
                                "stencil3D_7pt_buf_p0_1_wr(%d), "\
                                "stencil3D_7pt_buf_r0_1_p1_rd(%d), "\
                                "stencil3D_7pt_buf_r0_1_p1_wr(%d), "\
                                "stencil3D_7pt_buf_r1_2_p1_rd(%d), "\
                                "stencil3D_7pt_buf_r1_2_p1_wr(%d), "\
                                "stencil3D_7pt_buf_p1_2_rd(%d), "\
                                "stencil3D_7pt_buf_p1_2_wr(%d), "\
                                "itr(%d)\n", m_PEId, i, 
                                j,
                                k,
                                stencil3D_7pt_buf_p0_1_rd, 
                                stencil3D_7pt_buf_p0_1_wr, 
                                stencil3D_7pt_buf_r0_1_p1_rd, 
                                stencil3D_7pt_buf_r0_1_p1_wr, 
                                stencil3D_7pt_buf_r1_2_p1_rd, 
                                stencil3D_7pt_buf_r1_2_p1_wr, 
                                stencil3D_7pt_buf_p1_2_rd, 
                                stencil3D_7pt_buf_p1_2_wr, 
                                itr);

                printf("[DEBUG][INTERNAL][ops_krnl_heat3D_PE_%d] --------------------------------------------------------\n\n", m_PEId);

                printf("[DEBUG][INTERNAL][ops_krnl_heat3D_PE_%d] read values arg1: (", m_PEId);
                for (int ri = 0; ri < vector_factor; ri++)
                {
                    ops::hls::DataConv tmpConverter;
                    tmpConverter.i = arg1_read_val.range((ri + 1)*s_datatype_size - 1, ri * s_datatype_size);
                    printf("%f ", tmpConverter.f);
                }
                printf(")\n");
#endif      
            }

            vec2arr: for (unsigned short x = 0; x < vector_factor; x++)
            {
#pragma HLS UNROLL factor=vector_factor
                ops::hls::DataConv arg1_tmpConverter_1_0;
                arg1_tmpConverter_1_0.i = arg1_widenStencilValues[0].range(s_datatype_size * (x + 1) - 1, x * s_datatype_size);
                arg1_rowArr_1_0[x + s_stencil_half_span_x] = arg1_tmpConverter_1_0.f; 
                ops::hls::DataConv arg1_tmpConverter_0_1;
                arg1_tmpConverter_0_1.i = arg1_widenStencilValues[1].range(s_datatype_size * (x + 1) - 1, x * s_datatype_size);
                arg1_rowArr_0_1[x + s_stencil_half_span_x] = arg1_tmpConverter_0_1.f; 
                ops::hls::DataConv arg1_tmpConverter_1_1;
                arg1_tmpConverter_1_1.i = arg1_widenStencilValues[3].range(s_datatype_size * (x + 1) - 1, x * s_datatype_size);
                arg1_rowArr_1_1[x + s_stencil_half_span_x] = arg1_tmpConverter_1_1.f; 
                ops::hls::DataConv arg1_tmpConverter_2_1;
                arg1_tmpConverter_2_1.i = arg1_widenStencilValues[5].range(s_datatype_size * (x + 1) - 1, x * s_datatype_size);
                arg1_rowArr_2_1[x + s_stencil_half_span_x] = arg1_tmpConverter_2_1.f; 
                ops::hls::DataConv arg1_tmpConverter_1_2;
                arg1_tmpConverter_1_2.i = arg1_widenStencilValues[6].range(s_datatype_size * (x + 1) - 1, x * s_datatype_size);
                arg1_rowArr_1_2[x + s_stencil_half_span_x] = arg1_tmpConverter_1_2.f; 

            }
            vec2arr_rest:
            {
                //diff = -1
                // access_idx = 0
                ops::hls::DataConv arg1_tmpConverter_0_1_1_7;
                arg1_tmpConverter_0_1_1_7.i = arg1_widenStencilValues[2].range(s_datatype_size * (7 + 1) - 1, s_datatype_size * 7);
                arg1_rowArr_1_1[0] = arg1_tmpConverter_0_1_1_7.f;
                //diff = 1
                // access_idx = 9
                ops::hls::DataConv arg1_tmpConverter_2_1_1_0;
                arg1_tmpConverter_2_1_1_0.i = arg1_widenStencilValues[4].range(s_datatype_size * (0 + 1) - 1, s_datatype_size * 0);
                arg1_rowArr_1_1[9] = arg1_tmpConverter_2_1_1_0.f;
            }

            process: for (unsigned short x = 0; x < vector_factor; x++)
            {
#pragma HLS UNROLL factor=vector_factor
                short index = (i << shift_bits) + x;
                bool neg_cond = register_it(
                        (index < stencilConfig.lower_limit[0])
                        || (index >= stencilConfig.upper_limit[0])
                        || (j < stencilConfig.lower_limit[1])
                        || (j >= stencilConfig.upper_limit[1])
                        || (k < stencilConfig.lower_limit[2])
                        || (k >= stencilConfig.upper_limit[2])
                );

#ifdef DEBUG_LOG
                printf("[DEBUG][INTERNAL][ops_krnl_heat3D_PE_%d] index=(%d, %d, %d), lowerbound=(%d, %d, %d), upperbound=(%d, %d, %d), neg_cond=%d\n", m_PEId, index, j, k,
                            stencilConfig.lower_limit[0], stencilConfig.lower_limit[1], stencilConfig.lower_limit[2], 
                            stencilConfig.upper_limit[0], stencilConfig.upper_limit[1], stencilConfig.upper_limit[2], neg_cond);

#endif
                stencil_type arg0_result;

                short idx[] = {index, j, k};

                kernel_ops_krnl_heat3D_core(
                        arg0_result,
                        arg1_rowArr_1_0[x + 1],
                        arg1_rowArr_0_1[x + 1],
                        arg1_rowArr_1_1[x + 0],
                        arg1_rowArr_1_1[x + 1],
                        arg1_rowArr_1_1[x + 2],
                        arg1_rowArr_2_1[x + 1],
                        arg1_rowArr_1_2[x + 1],
                        &param_k,
                        idx
                );

                ops::hls::DataConv arg0_tmpConvWrite;

                if (not neg_cond)
                {
                    arg0_tmpConvWrite.f = arg0_result;
                }
                else
                {

                    arg0_tmpConvWrite.f = arg1_rowArr_1_1[x + 1];
                }

                arg0_update_val.range(s_datatype_size * (x + 1) - 1, x * s_datatype_size) = arg0_tmpConvWrite.i;

            }

            write:
            {
                bool cond_write = (k >= 0);

                if (cond_write)
                {
                    arg0_wr_buffer <<  arg0_update_val;
                }
            }
        }
    } 
};

void kernel_ops_krnl_heat3D_PE(const short& PEId, const ops::hls::StencilConfigCore& stencilConfig,
            Stencil_ops_krnl_heat3D::widen_stream_dt& arg0_wr_buffer,
            Stencil_ops_krnl_heat3D::widen_stream_dt& arg1_rd_buffer,
            const float& param_k
)
{
    Stencil_ops_krnl_heat3D stencil;

#ifdef DEBUG_LOG
    printf("[KERNEL_DEBUG]|%s| stencil config gridSize: %d (xblocks), %d, %d\n", __func__, stencilConfig.grid_size[0], stencilConfig.grid_size[1], stencilConfig.grid_size[2]);
#endif
    stencil.setConfig(PEId, stencilConfig);

#ifdef DEBUG_LOG
    printf("[KERNEL_DEBUG]|%s| starting stencil kernel PE\n", __func__);
#endif

    stencil.stencilRun(
            arg0_wr_buffer,
            arg1_rd_buffer,
            param_k

);

#ifdef DEBUG_LOG
    printf("[KERNEL_DEBUG]|%s| Ending stencil kernel PE\n", __func__);
#endif
}
