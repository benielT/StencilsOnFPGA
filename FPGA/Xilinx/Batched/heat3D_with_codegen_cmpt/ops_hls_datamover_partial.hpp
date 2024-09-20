#pragma once

#include "ops_hls_defs.hpp"
// #define DEBUG_LOG

#ifndef __SYTHESIS__
#ifdef DEBUG_LOG
		#ifndef DEBUG_LOG_SIZE_OF
			#define DEBUG_LOG_SIZE_OF 4
		#endif
#endif
#endif

namespace ops {
namespace hls {

/**
 * @brief 	mem2stream reads from a memory location with to an hls stream.
 *  		This is optimized to read from AXI4 with burst and to utilize width maximum througput
 *
 * @tparam MEM_DATA_WIDTH : Data width of the AXI4 port and the hls stream port
 * @tparam BURST_SIZE : Burst length of the AXI4 (max beats < 256)
 *
 * @param mem_in : input memory port
 * @param stream_out : output hls-stream
 * @param size : Number of bytes of the data
 */
template <unsigned int MEM_DATA_WIDTH, unsigned int BURST_SIZE=32>
void mem2stream(ap_uint<MEM_DATA_WIDTH>* mem_in,
				::hls::stream<ap_uint<MEM_DATA_WIDTH>>& strm_out,
				const unsigned int num_beats)
{
//#ifndef __SYTHESIS__
//	static_assert(MEM_DATA_WIDTH >= min_mem_data_width && MEM_DATA_WIDTH <= max_mem_data_width,
//			"MEM_DATA_WIDTH failed limit check");
//	static_assert(BURST_SIZE >= min_burst_len && BURST_SIZE <= max_burst_len,
//			" BURST_SIZE has failed limit check");
//#endif

	constexpr unsigned int bytes_per_beat = MEM_DATA_WIDTH / 8;

	const unsigned int num_bursts = num_beats / BURST_SIZE;
	const unsigned int non_burst_beats = num_beats % BURST_SIZE;

#ifndef __SYTHESIS__
#ifdef DEBUG_LOG
	printf("|HLS DEBUG_LOG| %s | num_beats: %d, num_burst: %d, non_burst_beats: %d\n"
			, __func__, num_beats, num_bursts, non_burst_beats);
	printf("====================================================================================\n");
#endif
#endif

	unsigned int index = 0;

	for (unsigned int brst = 0; brst < num_bursts; brst++)
	{
//	#pragma HLS LOOP_TRIPCOUNT avg=avg_num_of_bursts max=max_num_of_bursts

		for (unsigned int beat = 0; beat < BURST_SIZE; beat++)
		{
		#pragma HLS PIPELINE II=1
//		#pragma HLS LOOP_TRIPCOUNT min=min_burst_len avg=avg_burst_len max=max_burst_len
		#pragma HLS LOOP_FLATTEN

			ap_uint<MEM_DATA_WIDTH> tmp = mem_in[index];
			strm_out << tmp;
#ifdef DEBUG_LOG
			printf("|HLS DEBUG_LOG| %s | reading index: %d\n", __func__, index);
#endif
			index++;
		}
	}

	for (unsigned int beat = 0; beat < non_burst_beats; beat++)
	{
	#pragma HLS PIPELINE II=1
		ap_uint<MEM_DATA_WIDTH> tmp = mem_in[index];
		strm_out << tmp;
#ifdef DEBUG_LOG
		printf("|HLS DEBUG_LOG| %s | reading index: %d\n", __func__, index);
#endif
		index++;
	}
}

/**
 * @brief stream2streamStepup Converts from one hls-stream to another with bigger size.
 *
 * @tparam STREAM1_DATA_WIDTH : Data width of the hls port1
 * @tparam STREAM2_DATA_WIDTH : Data width of the hls port2
 *
 * @param num_big_pkts : Number of pkts of the wider stream
 */
template <unsigned int STREAM1_DATA_WIDTH, unsigned int STREAM2_DATA_WIDTH>
void stream2streamStepup(::hls::stream<ap_uint<STREAM1_DATA_WIDTH>>& strm_in,
				::hls::stream<ap_uint<STREAM2_DATA_WIDTH>>& strm_out,
				const unsigned int num_big_pkts)
{
#ifndef __SYTHESIS__
	static_assert(STREAM1_DATA_WIDTH < STREAM2_DATA_WIDTH,
			"STREAM1_DATA_WIDTH has to be smaller than STREAM2_DATA_WIDTH");
    static_assert(STREAM2_DATA_WIDTH % STREAM1_DATA_WIDTH == 0, 
            "STREAM2_DATA_WIDTH has to be fully divisible by STREAM1_DATA_WIDTH");
    static_assert(STREAM1_DATA_WIDTH % 8 == 0, "STREAM1_DATA_WIDTH should be divisible by 8");
    static_assert(STREAM2_DATA_WIDTH % 8 == 0, "STREAM2_DATA_WIDTH should be divisible by 8");
#endif

constexpr unsigned short FACTOR = STREAM2_DATA_WIDTH / STREAM1_DATA_WIDTH;

#ifndef __SYTHESIS__
    #ifdef DEBUG_LOG
        printf("|HLS DEBUG_LOG| %s | num_big_pkts: %d\n"
                , __func__, num_big_pkts);
        printf("====================================================================================\n");
    #endif
#endif

    ap_uint<STREAM2_DATA_WIDTH> tmp2;
    for (int pkt = 0; pkt < num_big_pkts; pkt++)
    {
        for (unsigned n = 0; n < FACTOR; n++)
        {
        #pragma HLS PIPELINE II=1
        #pragma HLS LOOP_FLATTEN

            ap_uint<STREAM1_DATA_WIDTH> tmp1 = strm_in.read();
#ifndef __SYTHESIS__
#ifdef DEBUG_LOG
            printf("   |HLS DEBUG_LOG||%s| reading pkt: %d, val=(", __func__, pkt*FACTOR + n);

            for (unsigned k = 0; k < STREAM1_DATA_WIDTH/(DEBUG_LOG_SIZE_OF * 8); k++)
            {
                DataConv conv;
                conv.i = tmp1.range((k+1) * DEBUG_LOG_SIZE_OF * 8 - 1, k * DEBUG_LOG_SIZE_OF * 8);
                printf("%f,", conv.f);
            }
            printf(")\n");
#endif
#endif
            tmp2.range((n+1)* STREAM1_DATA_WIDTH -1, n * STREAM1_DATA_WIDTH) = tmp1;

            if (n == FACTOR - 1)
            {
#ifndef __SYTHESIS__
#ifdef DEBUG_LOG
            printf("   |HLS DEBUG_LOG||%s| writing pkt: %d, val=(", __func__, pkt*FACTOR + n);

            for (unsigned k = 0; k < STREAM2_DATA_WIDTH/(DEBUG_LOG_SIZE_OF * 8); k++)
            {
                DataConv conv;
                conv.i = tmp2.range((k+1) * DEBUG_LOG_SIZE_OF * 8 - 1, k * DEBUG_LOG_SIZE_OF * 8);
                printf("%f,", conv.f);
            }
            printf(")\n");
#endif
#endif

            strm_out.write(tmp2);
            }
        }
    }

#ifdef DEBUG_LOG
	printf("|HLS DEBUG_LOG|%s| exiting.\n"
			, __func__);
#endif
}

/**
 * @brief stream2streamStepdown Converts from one hls-stream to another with smaller size.
 *
 * @tparam STREAM1_DATA_WIDTH : Data width of the hls port1
 * @tparam STREAM2_DATA_WIDTH : Data width of the hls port2
 *
 * @param num_big_pkts : Number of pkts of the wider stream
 */
template <unsigned int STREAM1_DATA_WIDTH, unsigned int STREAM2_DATA_WIDTH>
void stream2streamStepdown(::hls::stream<ap_uint<STREAM1_DATA_WIDTH>>& strm_in,
				::hls::stream<ap_uint<STREAM2_DATA_WIDTH>>& strm_out,
				const unsigned int num_big_pkts)
{
#ifndef __SYTHESIS__
	static_assert(STREAM1_DATA_WIDTH > STREAM2_DATA_WIDTH,
			"STREAM1_DATA_WIDTH has to be bigger than STREAM2_DATA_WIDTH");
    static_assert(STREAM1_DATA_WIDTH % STREAM2_DATA_WIDTH == 0, 
            "STREAM1_DATA_WIDTH has to be fully divisible by STREAM2_DATA_WIDTH");
    static_assert(STREAM1_DATA_WIDTH % 8 == 0, "STREAM1_DATA_WIDTH should be divisible by 8");
    static_assert(STREAM2_DATA_WIDTH % 8 == 0, "STREAM2_DATA_WIDTH should be divisible by 8");
#endif

constexpr unsigned short FACTOR = STREAM1_DATA_WIDTH / STREAM2_DATA_WIDTH;

#ifndef __SYTHESIS__
    #ifdef DEBUG_LOG
        printf("|HLS DEBUG_LOG| %s | num_big_pkts: %d\n"
                , __func__, num_big_pkts);
        printf("====================================================================================\n");
    #endif
#endif

    ap_uint<STREAM1_DATA_WIDTH> tmp1;
    
    for (int pkt = 0; pkt < num_big_pkts; pkt++)
    {
        for (unsigned n = 0; n < FACTOR; n++)
        {
        #pragma HLS PIPELINE II=1
        #pragma HLS LOOP_FLATTEN

            if (n == 0)
            {
                tmp1 = strm_in.read();
#ifndef __SYTHESIS__
    #ifdef DEBUG_LOG
                    printf("   |HLS DEBUG_LOG||%s| receiving pkt: %d, val=(", __func__, pkt);

                    for (unsigned n = 0; n < STREAM1_DATA_WIDTH/(DEBUG_LOG_SIZE_OF * 8); n++)
                    {
                        DataConv conv;
                        conv.i = tmp1.range((n+1) * DEBUG_LOG_SIZE_OF * 8 - 1, n * DEBUG_LOG_SIZE_OF * 8);
                        printf("%f,", conv.f);
                    }
                    printf(")\n");
    #endif
#endif
            }

            ap_uint<STREAM2_DATA_WIDTH> tmp2 = tmp1.range((n+1)* STREAM2_DATA_WIDTH -1, n * STREAM2_DATA_WIDTH);
            strm_out.write(tmp2);
#ifndef __SYTHESIS__
    #ifdef DEBUG_LOG
                printf("   |HLS DEBUG_LOG||%s| writing pkt: %d, val=(", __func__, pkt*FACTOR + n);

                for (unsigned k = 0; k < STREAM2_DATA_WIDTH/(DEBUG_LOG_SIZE_OF * 8); k++)
                {
                    DataConv conv;
                    conv.i = tmp2.range((k+1) * DEBUG_LOG_SIZE_OF * 8 - 1, k * DEBUG_LOG_SIZE_OF * 8);
                    printf("%f,", conv.f);
                }
                printf(")\n");
    #endif
#endif
        }
    }

}
/**
 * @brief stream2axis Converts from one hls-stream to axis4-stream with same_size
 *
 * @tparam STREAM_DATA_WIDTH : Data width of the streams
 *
 * @param pkts : Number of pkts
 */
template <unsigned int STREAM_DATA_WIDTH>
void stream2axis(::hls::stream<ap_uint<STREAM_DATA_WIDTH>>& strm_in,
				::hls::stream<ap_axiu<STREAM_DATA_WIDTH,0,0,0>>& strm_out,
				const unsigned int pkts)
{
#ifndef __SYTHESIS__
#ifdef DEBUG_LOG
	printf("|HLS DEBUG_LOG| %s | starting. pkts: %d\n"
			, __func__, pkts);
#endif
#endif
	for (int itr = 0; itr < pkts; itr++){
		#pragma HLS PIPELINE II=1

		ap_axiu<STREAM_DATA_WIDTH,0,0,0> tmp;
		tmp.data = strm_in.read();
#ifndef __SYTHESIS__
#ifdef DEBUG_LOG
			printf("   |HLS DEBUG_LOG|%s| sending axis pkt: %d, val=(",__func__, itr);

			for (unsigned n = 0; n < STREAM_DATA_WIDTH/(DEBUG_LOG_SIZE_OF * 8); n++)
			{
				DataConv conv;
				conv.i = tmp.data.range((n+1) * DEBUG_LOG_SIZE_OF * 8 - 1, n * DEBUG_LOG_SIZE_OF * 8);
				printf("%f,", conv.f);
			}
			printf(")\n");
#endif
#endif
		strm_out << tmp;
	}
}

/**
 * @brief axis2stream Converts from one axis4-stream to hls-stream with same_size
 *
 * @tparam STREAM_DATA_WIDTH : Data width of the streams
 *
 * @param pkts : Number of pkts
 */
template <unsigned int STREAM_DATA_WIDTH>
void axis2stream(::hls::stream<ap_axiu<STREAM_DATA_WIDTH,0,0,0>>& strm_in,
		::hls::stream<ap_uint<STREAM_DATA_WIDTH>>& strm_out,
		unsigned int pkts)
{
#ifndef __SYTHESIS__
#ifdef DEBUG_LOG
	printf("|HLS DEBUG_LOG| %s | starting. pkts: %d\n"
			, __func__, pkts);
#endif
#endif
	ap_axiu<STREAM_DATA_WIDTH,0,0,0> tmp;

	for (int itr = 0; itr < pkts; itr++){
		#pragma HLS PIPELINE II=1

		tmp = strm_in.read();
		strm_out << tmp.data;
#ifdef DEBUG_LOG
			printf("   |HLS DEBUG_LOG|%s| read axis pkt: %d, val=(",__func__, itr);

			for (unsigned n = 0; n < STREAM_DATA_WIDTH/(DEBUG_LOG_SIZE_OF * 8); n++)
			{
				DataConv conv;
				conv.i = tmp.data.range((n+1) * DEBUG_LOG_SIZE_OF * 8 - 1, n * DEBUG_LOG_SIZE_OF * 8);
				printf("%f,", conv.f);
			}
			printf(")\n");
#endif
	}
}

/**
 * @brief 	stream2mem reads from a memory from hls stram and write to memory
 *  		This is optimized to write to AXI4 with burst and to utilize maximum throughput.
 *
 * @tparam MEM_DATA_WIDTH : Data width of the AXI4 port the hls stream port
 * @tparam BURST_SIZE : Burst length of the AXI4 (max beats < 256)
 *
 * @param mem_out : out memory port
 * @param stream_in : input hls-stream
 * @param size : Number of bytes of the data
 */
template <unsigned int MEM_DATA_WIDTH, unsigned int BURST_SIZE=32>
void stream2mem(ap_uint<MEM_DATA_WIDTH>* mem_out,
				::hls::stream<ap_uint<MEM_DATA_WIDTH>>& strm_in,
				const unsigned int num_beats)
{
//#ifndef __SYTHESIS__
//	static_assert(MEM_DATA_WIDTH >= min_mem_data_width && MEM_DATA_WIDTH <= max_mem_data_width,
//			"MEM_DATA_WIDTH failed limit check");
//	static_assert(BURST_SIZE >= min_burst_len && BURST_SIZE <= max_burst_len,
//			" BURST_SIZE has failed limit check");
//#endif

	constexpr unsigned int bytes_per_beat = MEM_DATA_WIDTH / 8;

	const unsigned int num_bursts = num_beats / BURST_SIZE;
	const unsigned int non_burst_beats = num_beats % BURST_SIZE;

#ifndef __SYTHESIS__
#ifdef DEBUG_LOG
	printf("|HLS DEBUG_LOG| %s | num_beats: %d, num_burst: %d, non_burst_beats: %d\n"
			, __func__, num_beats, num_bursts, non_burst_beats);
	printf("====================================================================================\n");
#endif
#endif

	unsigned int index = 0;

	for (unsigned int brst = 0; brst < num_bursts; brst++)
	{
//	#pragma HLS LOOP_TRIPCOUNT avg=avg_num_of_bursts max=max_num_of_bursts

		for (unsigned int beat = 0; beat < BURST_SIZE; beat++)
		{
		#pragma HLS PIPELINE II=1
//		#pragma HLS LOOP_TRIPCOUNT min=min_burst_len avg=avg_burst_len max=max_burst_len
		#pragma HLS LOOP_FLATTEN

			ap_uint<MEM_DATA_WIDTH> tmp = strm_in.read();
			mem_out[index] = tmp;
#ifdef DEBUG_LOG
			printf("|HLS DEBUG_LOG| %s | writing index: %d\n", __func__, index);
#endif			
            index++;
		}
	}

	for (unsigned int beat = 0; beat < non_burst_beats; beat++)
	{
	#pragma HLS PIPELINE II=1
		ap_uint<MEM_DATA_WIDTH> tmp = strm_in.read();
		mem_out[index] = tmp;
#ifdef DEBUG_LOG
		printf("|HLS DEBUG_LOG| %s | reading index: %d\n", __func__, index);
#endif
        index++;
	}
#ifdef DEBUG_LOG
	printf("|HLS DEBUG_LOG|%s| exiting.\n"
			, __func__);
#endif
}

}
}
