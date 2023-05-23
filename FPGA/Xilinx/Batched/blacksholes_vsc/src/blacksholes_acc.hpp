
#pragma once
#include <ap_int.h>
#include <hls_stream.h>
#include <ap_axi_sdata.h>
#include <vpp_acc.hpp>
#include <vpp_stream.hpp>

// platform specific configs
#include <config.hpp>

// strcutre to hold grid parameters to avoid recalculation in
// different process
struct data_G{
	unsigned short sizex;
	unsigned short sizey;
	unsigned short xdim0;
	unsigned short end_index;
	unsigned short end_row;
	unsigned int gridsize;
    unsigned int total_itr_512;
    unsigned int total_itr_256;
	unsigned short outer_loop_limit;
	unsigned short endrow_plus2;
	unsigned short endrow_plus1;
	unsigned short endrow_minus1;
	unsigned short endindex_minus1;
};

class blacksholes_acc : public VPP_ACC<blacksholes_acc, NCU>
{
    ACCESS_PATTERN(in, SEQUENTIAL);
    ACCESS_PATTERN(out, SEQUENTIAL);

    // Dynamic payload size determined by xdim0, ydim0 and batches for each compute() call
    DATA_COPY(in, in[(xdim0 >> SHIFT_BITS) * ydim0 * batches]);
    DATA_COPY(out, out[(xdim0 >> SHIFT_BITS) * ydim0 * batches]);

    // Kernel global memory connection for generic platform
    SYS_PORT(in, PORT_MAP_CURR);
    SYS_PORT(out, PORT_MAP_NEXT);

    // // Kernel global memory connection for u280
    // SYS_PORT_PFM(u280, in, HBM[0]);
    // SYS_PORT_PFM(u280, out, HBM[1]);

public:

    static void compute(
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
        float * in,
        float * out);

    // static void blacksholes(
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
    //     float * out);

    // static void compute(
    //     const int xdim0,
    //     const int ydim0,
    //     const int batches,
    //     float * in,
    //     float * out);

    // static void compute(
    //     const int xdim0,
    //     const int ydim0,
    //     const int batches,
    //     uint512_dt * in,
    //     uint512_dt * out        
    // )

    static void blacksholes_kernel(
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
        hls::stream<t_pkt256> & out_stream);
    
    // static void ldst(
    //     float * in,
    //     float * out,
    //     const unsigned int total_itr,
    //     hls::stream<t_pkt> & in_stream,
    //     hls::stream<t_pkt> & out_stream);

    static void ldst(
        float * in,
        float * out,
        const unsigned int count,
        const unsigned int total_itr,
        const unsigned int total_itr_256,
        hls::stream<t_pkt256> & in_stream,
        hls::stream<t_pkt256> & out_stream);

    // static void ldst(
    //     uint512_dt * in,
    //     uint512_dt * out,
    //     const int total_itr,
    //     hls::stream<t_pkt> & in_stream,
    //     hls::stream<t_pkt> & out_stream);

    // static void simpleRepeater(
    //     const int total_itr,
    //     hls::stream<t_pkt> & in_stream,
    //     hls::stream<t_pkt> & out_stream);

    // static void simpleRepeater(
    // const unsigned int count, 
    // const unsigned int total_itr,
    // const unsigned int total_itr_256,
    // hls::stream<t_pkt256> & in_stream,
    // hls::stream<t_pkt256> & out_stream);


}; 