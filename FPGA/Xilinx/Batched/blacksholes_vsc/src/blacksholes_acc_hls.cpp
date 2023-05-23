
#include "blacksholes_acc.hpp"

// static void read_grid(
//     float * in,
//     hls::stream<t_pkt> & out_stream,
//     const unsigned int total_itr
//     )
// {
//     for (unsigned int i = 0; i < total_itr; i++)
//     {
//     #pragma HLS PIPELINE II=1
//     #pragma HLS LOOP_TRIPCOUNT min=min_grid max=max_grid avg=avg_grid
//         t_pkt pkt;
//         auto value = in[i];
//         pkt.data = value;
//         pkt.last = (i == total_itr - 1);
//         // data_conv tmp;
//         // tmp.i = pkt.data;
//         // printf("sending pktid: %d value: %f, origin_value: %f\n", i, tmp.f, value);
//         out_stream.write(pkt);
//     }
// }

static void read_grid(
    float * in,
    hls::stream<t_pkt256> & out_stream,
    const unsigned int total_itr,
    const unsigned int total_itr_256
    )
{
    for (int i = 0; i < total_itr_256; i++)
    {
        #pragma HLS PIPELINE II=1
        #pragma HLS LOOP_TRIPCOUNT min=min_grid max=max_grid avg=avg_grid

        t_pkt256 pkt;


        for (int j = 0; j < vec_factor; j++)
        {
            #pragma UNROLL
            unsigned int index = i * vec_factor + j;

            if (index < total_itr)
            {
                auto value = in[index];
                pkt.data.range((j + 1) * DATATYPE_SIZE - 1, j * DATATYPE_SIZE) = value;
                // data_conv tmp;
                // tmp.i = pkt.data.range((j + 1) * DATATYPE_SIZE - 1, j * DATATYPE_SIZE);
                // printf("sending pktid: %d value: %f, origin_value: %f\n", i, tmp.f, value);
            }
        }

        pkt.last = (i == total_itr - 1);
        out_stream.write(pkt);
    }
}

// static void write_grid(
//     float * out,
//     hls::stream<t_pkt> & in_stream,
//     const unsigned int total_itr
//     )
// {
//     for (unsigned int i = 0; i < total_itr; i++)
//     {
//     #pragma HLS PIPELINE II=1
//     #pragma HLS LOOP_TRIPCOUNT min=min_grid max=max_grid avg=avg_grid
//         t_pkt inPkt;
//         inPkt = in_stream.read();
//         // data_conv tmp;
//         // tmp.i = inPkt.data;
//         // printf("recieving pktid: %d value: %f\n", i, tmp.f);
//         out[i] = inPkt.data;
//     }
// }

static void write_grid(
    float * out,
    hls::stream<t_pkt256> & in_stream,
    const unsigned int total_itr,
    const unsigned int total_itr_256
    )
{
    for (unsigned int i = 0; i < total_itr_256; i++)
    {
        #pragma HLS PIPELINE II=1
        #pragma HLS LOOP_TRIPCOUNT min=min_grid max=max_grid avg=avg_grid

        t_pkt256 inPkt;
        inPkt = in_stream.read();

        for (unsigned int j = 0; j < vec_factor; j++)
        {
            #pragma UNROLL
            unsigned int index = i * vec_factor + j;

            if (index < total_itr)
            {
                float value = inPkt.data.range((j + 1) * DATATYPE_SIZE - 1, j * DATATYPE_SIZE);
                out[index] = value;
                
                // data_conv tmp;
                // tmp.i = value;
                // printf("recieving pktid: %d value: %f\n", i, tmp.f);
            }
        }
    }
}

static void inline get_stencil_coefficent(const float & alpha, const float & beta,
		const float & delta_t, const int & init_idx, float * a,
		float * b, float * c)
{
	for (int i = 0; i < vec_factor; i++)
	{
#pragma HLS UNROLL

		int idx = init_idx + i;
		a[i] = 0.5 * (alpha * std::pow(idx,2) - beta * idx);
		b[i] = 1 - alpha * std::pow(idx,2) - beta;
		c[i] = 0.5 * (alpha * std::pow(idx,2) + beta * idx);
	}
}

static void process_grid(hls::stream<uint256_dt> &rd_buffer, hls::stream<uint256_dt> &wr_buffer, struct data_G data_g,
		const float alpha, const float beta, const float delta_t)
{

	short end_index = data_g.end_index;

    // Registers to hold data specified by stencil
	float row_arr1[vec_factor + 2];
	float mem_wr[vec_factor];

    // partioning array into individual registers
	#pragma HLS ARRAY_PARTITION variable=row_arr1 complete dim=1
	#pragma HLS ARRAY_PARTITION variable=mem_wr complete dim=1

	//register to hold coefficients
	float a[vec_factor];
	float b[vec_factor];
	float c[vec_factor];

	//partitioning
	#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=a
	#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=b
	#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=c

    // cyclic buffers to hold larger number of elements. //No need for cyclic buffer
//	uint256_dt row1_n[max_depth_8];
//	uint256_dt row2_n[max_depth_8];
//	uint256_dt row3_n[max_depth_8];
//
//	#pragma HLS RESOURCE variable=row1_n core=XPM_MEMORY uram latency=2
//	#pragma HLS RESOURCE variable=row2_n core=XPM_MEMORY uram latency=2
//	#pragma HLS RESOURCE variable=row3_n core=XPM_MEMORY uram latency=2

	unsigned short sizex = data_g.sizex;
//	unsigned short end_row = data_g.end_row;
//	unsigned short outer_loop_limit = data_g.outer_loop_limit;
	unsigned int grid_size = data_g.gridsize;
	unsigned short end_index_minus1 = data_g.endindex_minus1;
//	unsigned short end_row_plus1 = data_g.endrow_plus1;
//	unsigned short end_row_plus2 = data_g.endrow_plus2;
//	unsigned short end_row_minus1 = data_g.endrow_minus1;
	unsigned int grid_data_size = data_g.total_itr_256;

    uint256_dt tmp1_f1, tmp1, tmp1_b1;
//    uint256_dt tmp1, tmp2, tmp3;
    uint256_dt update_j;

    unsigned int window_delay = 1;

    // flattened loop to reduce the inter loop latency
    unsigned short j;
    unsigned short j_d = -window_delay;
    unsigned int write_itr = 0;

    for(unsigned int itr = 0; itr < grid_size + window_delay; itr++) {
        #pragma HLS loop_tripcount min=min_block_x max=max_block_x avg=avg_block_x
        #pragma HLS PIPELINE II=1

    	j = j_d;

    	bool cmp_j = (j == end_index - 1);

    	if(cmp_j){
    		j_d = 0;
    	} else {
    		j_d++;
    	}

        tmp1_b1 = tmp1;
        tmp1 = tmp1_f1;
        // continuous data-flow for all the grids in the batch
        bool cond_tmp1 = (itr < grid_data_size); // && (j != end_index - 1 + window_delay);

        if(cond_tmp1){
            tmp1_f1 = rd_buffer.read();

//            printf("vector read: %d ={", itr);

            for(int k = 0; k < vec_factor; k++)
            {
				data_conv tmp;
				tmp.i = tmp1_f1.range(DATATYPE_SIZE * (k + 1) - 1, k * DATATYPE_SIZE);

//	            if (k != vec_factor - 1)
//	            	printf("%f, ", tmp.f);
//	            else
//	            	printf("%f}\n", tmp.f);
            }
        }

        vec2arr: for(int k = 0; k < vec_factor; k++){
            data_conv tmp1_u;
            tmp1_u.i = tmp1.range(DATATYPE_SIZE * (k + 1) - 1, k * DATATYPE_SIZE);
//            tmp2_u.i = tmp2.range(DATATYPE_SIZE * (k + 1) - 1, k * DATATYPE_SIZE);
//            tmp3_u.i = tmp3.range(DATATYPE_SIZE * (k + 1) - 1, k * DATATYPE_SIZE);

//            row_arr3[k] =  tmp3_u.f;
//            row_arr2[k+1] = tmp2_u.f;
            row_arr1[k+1] =  tmp1_u.f;
        }
        data_conv tmp1_o1, tmp2_o2;
        tmp1_o1.i = tmp1_b1.range(DATATYPE_SIZE * (vec_factor) - 1, (vec_factor-1) * DATATYPE_SIZE);
        tmp2_o2.i = tmp1_f1.range(DATATYPE_SIZE * (0 + 1) - 1, 0 * DATATYPE_SIZE);
        row_arr1[0] = tmp1_o1.f;
        row_arr1[vec_factor + 1] = tmp2_o2.f;

        int init_idx = (j << SHIFT_BITS);

        readCoeff: get_stencil_coefficent(alpha, beta, delta_t, init_idx, a, b, c);

        // stencil computation
        // this loop will be completely unrolled as parent loop is pipelined
        process: for(short q = 0; q < vec_factor; q++){
        	int index = (j << SHIFT_BITS) + q;
            float r1 = ( (a[q] * row_arr1[q])  + (b[q] * row_arr1[q+1]) + (c[q] * row_arr1[q+2]) );
            float result  = r1;
            bool change_cond = (index <= 0 || index > sizex);

            if (change_cond)
            	mem_wr[q] = row_arr1[q+1];
            else
            	mem_wr[q] = result;
//            mem_wr[q] = change_cond ? row_arr1[q+1] : result;
        }

        array2vec: for(int k = 0; k < vec_factor; k++){
            data_conv tmp;
            tmp.f = mem_wr[k];
            update_j.range(DATATYPE_SIZE * (k + 1) - 1, k * DATATYPE_SIZE) = tmp.i;
        }
        // conditional write to stream interface
        bool cond_wr = (itr >= 1) && write_itr < grid_data_size;

        if(cond_wr ) {
            wr_buffer << update_j;
            write_itr++;
        }

    }
}

static void axis2_fifo256(hls::stream <t_pkt256> &in, hls::stream<uint256_dt> &out,  const unsigned int total_itr)
{
	for (int itr = 0; itr < total_itr; itr++){
		#pragma HLS PIPELINE II=1
		#pragma HLS loop_tripcount min=min_grid max=max_grid avg=avg_grid
		t_pkt256 tmp = in.read();
		out << tmp.data;
	}
}

static void fifo256_2axis(hls::stream <uint256_dt> &in, hls::stream<t_pkt256> &out, const unsigned int total_itr)
{
	for (int itr = 0; itr < total_itr; itr++){
		#pragma HLS PIPELINE II=1
		#pragma HLS loop_tripcount min=min_grid max=max_grid avg=avg_grid
		t_pkt256 tmp;
		tmp.data = in.read();
		out.write(tmp);
	}
}

void blacksholes_acc::blacksholes_kernel(
        const int size0,
        const int size1,
        const int xdim0,
        const int ydim0,
        const int count,
        const int batches,
        const float spot_price,
        const float strike_price,
        const float time_to_maturity,
        const float volatility,
        const float risk_free_rate,
        const float delta_t,
        const float delta_s,
        const unsigned int N,
        const int K,
        const float SMaxFactor,
        hls::stream<t_pkt256> & in_stream,
        hls::stream<t_pkt256> & out_stream)
{

    static hls::stream<uint256_dt> streamArray[SLR0_P_STAGE + 1];
    #pragma HLS STREAM variable = streamArray depth = 10

    struct data_G data_g;

    data_g.sizex = size0;
    data_g.sizey = size1;
    data_g.xdim0 = xdim0;
	data_g.end_index = (xdim0 >> SHIFT_BITS); // number of blocks with V number of elements to be processed in a single row
	data_g.end_row = size1; // includes the boundary
	data_g.outer_loop_limit = size1; // n + D/2
	data_g.gridsize = (data_g.end_row* batches + 1) * data_g.end_index;
	data_g.endindex_minus1 = data_g.end_index -1;
	data_g.endrow_plus1 = data_g.end_row + 1;
	data_g.endrow_plus2 = data_g.end_row + 2;
	data_g.endrow_minus1 = data_g.end_row - 1;
	data_g.total_itr_256 = data_g.end_row * data_g.end_index * batches;
	data_g.total_itr_512 = (data_g.end_row * data_g.end_index * batches + 1) >> 1;

    float alpha = volatility * volatility * delta_t;
	float beta = risk_free_rate * delta_t;

	// parallel execution of following modules

    for (int j = 0; j < count; j++)
    {
        #pragma HLS dataflow
        // printf("runing iter: %d\n", j);
        axis2_fifo256(in_stream, streamArray[0], data_g.total_itr_256);

        // unrolling iterative loop
        for(int i = 0; i < SLR0_P_STAGE; i++){
            #pragma HLS unroll
            process_grid(streamArray[i], streamArray[i+1], data_g, alpha, beta, delta_t);
        }

        // sending data to kernel or mem2stream which resides in another SLR
        fifo256_2axis(streamArray[SLR0_P_STAGE], out_stream, data_g.total_itr_256); 
    }   
}

// void blacksholes_acc::ldst(
//         float * in,
//         float * out,
//         const unsigned int total_itr,
//         hls::stream<t_pkt> & in_stream,
//         hls::stream<t_pkt> & out_stream)
// {
//     // printf("num_iter: %d\n", total_itr);

//     #pragma HLS DATAFLOW
//     read_grid(in, out_stream, total_itr);
//     write_grid(out, in_stream, total_itr);
// }

void blacksholes_acc::ldst(
        float * in,
        float * out,
        const unsigned int count,
        const unsigned int total_itr,
        const unsigned int total_itr_256,
        hls::stream<t_pkt256> & in_stream,
        hls::stream<t_pkt256> & out_stream)
{
    // printf("num_iter: %d\n", total_itr);


    for (int i = 0; i < count; i++)
    {
        if (i % 2 == 0)
        {
                #pragma HLS DATAFLOW
            read_grid(in, out_stream, total_itr, total_itr_256);
            write_grid(out, in_stream, total_itr, total_itr_256);            
        }
        else
        {
                #pragma HLS DATAFLOW
            read_grid(out, out_stream, total_itr, total_itr_256);
            write_grid(in, in_stream, total_itr, total_itr_256);            
        }
    }
}

// void blacksholes_acc::blacksholes(
//         const int size0,
//         const int size1,
//         const int xdim0,
//         const int ydim0,
//         const int count,
//         const int batches,
//         const float spot_price,
//         const float strike_price,
//         const float time_to_maturity,
//         const float volatility,
//         const float risk_free_rate,
//         const float delta_t,
//         const float delta_s,
//         const unsigned int N,
//         const int K,
//         const float SMaxFactor,
//         float * in,
//         float * out)
// {
//     hls::stream<t_pkt256> in_stream;
//     hls::stream<t_pkt256> out_stream;

//     #pragma HLS STREAM variable = in_stream depth = 10
//     #pragma HLS STREAM variable = out_stream depth = 10

//     unsigned int total_itr = xdim0 * ydim0 * batches;
//     unsigned int total_itr_256 = (xdim0 >> SHIFT_BITS) * ydim0 * batches;

// 	// parallel execution of following modules

//     for (int i = 0; i < count; i++)
//     {
//         // if (i % 2 == 0)
//         // {
//             ldst(in, out, total_itr, total_itr_256, in_stream, out_stream);
//         // }
//         // else
//         // {
//         //     ldst(out, in, total_itr, total_itr_256, in_stream, out_stream);
//         // }
            
//         blacksholes_kernel(size0, 
//             size1, xdim0, ydim0, count, 
//             batches, spot_price, strike_price, 
//             time_to_maturity, volatility, risk_free_rate, 
//             delta_t, delta_s, N, K,
//             SMaxFactor, out_stream, in_stream);
//     }

    // for (int i = )
    // ldst(in, out, total_itr, total_itr_256, in_stream, out_stream);
    // blacksholes_kernel(size0, 
    //         size1, xdim0, ydim0, count, 
    //         batches, spot_price, strike_price, 
    //         time_to_maturity, volatility, risk_free_rate, 
    //         delta_t, delta_s, N, K,
    //         SMaxFactor, out_stream, in_stream);
// }
// void blacksholes_acc::simpleRepeater(
//         const int total_itr,
//         hls::stream<t_pkt> & in_stream,
//         hls::stream<t_pkt> & out_stream)
// {
//     for (unsigned int i = 0; i < total_itr; i++)
//     {
//     #pragma HLS PIPELINE II=1
//     #pragma HLS LOOP_TRIPCOUNT min=min_grid max=max_grid avg=avg_grid
//         t_pkt pkt = in_stream.read();
//         // data_conv tmp;
//         // tmp.i = pkt.data;
//         // printf("data i: %d, value: %f\n", i, tmp.f);
//         out_stream.write(pkt);
//     }
// }

// void blacksholes_acc::simpleRepeater(
//         const unsigned int count, 
//         const unsigned int total_itr,
//         const unsigned int total_itr_256,
//         hls::stream<t_pkt256> & in_stream,
//         hls::stream<t_pkt256> & out_stream)
// {
//     for (unsigned int itr = 0; itr < count; itr++)
//     {
//         for (unsigned int i = 0; i < total_itr_256; i++)
//         {
//             #pragma HLS PIPELINE II=1
//             #pragma HLS LOOP_TRIPCOUNT min=min_grid max=max_grid avg=avg_grid
//             t_pkt256 pkt = in_stream.read();

//             for (unsigned int j = 0; j < vec_factor; j++)
//             {
//                 #pragma HLS UNROLL

//                 data_conv tmp;
//                 tmp.i = pkt.data.range( (j + 1) * DATATYPE_SIZE - 1, j * DATATYPE_SIZE);
//                 tmp.f += 0.5;
//                 pkt.data.range( (j + 1) * DATATYPE_SIZE - 1, j * DATATYPE_SIZE) = tmp.i;
//             }

//             out_stream.write(pkt);
//         }
//     }
// }