

#include "blacksholes_acc.hpp"

// void blacksholes_acc::compute(
//         const int xdim0,
//         const int ydim0,
//         const int batches,
//         float * in,
//         float * out)
//     {
//         static vpp::stream<t_pkt, STREAM_DEPTH> in_stream;
//         static vpp::stream<t_pkt, STREAM_DEPTH> out_stream;

//         unsigned int total_itr = xdim0 * ydim0 * batches;

//         ldst(in, out, total_itr, in_stream, out_stream);
//         simpleRepeater(total_itr, out_stream, in_stream);
//     }

void blacksholes_acc::compute(
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
        float * out)
{
        static vpp::stream<t_pkt256, STREAM_DEPTH> in_stream;
        static vpp::stream<t_pkt256, STREAM_DEPTH> out_stream;

        unsigned int total_itr = xdim0 * ydim0 * batches;
        unsigned int total_itr_256 = (xdim0 >> SHIFT_BITS) * ydim0 * batches;

        ldst(in, out, count, total_itr, total_itr_256, in_stream, out_stream);
        blacksholes_kernel(size0, 
            size1, 
            xdim0, 
            ydim0, 
            count,
            batches, 
            spot_price, 
            strike_price, 
            time_to_maturity, 
            volatility,
            risk_free_rate,
            delta_t,
            delta_s,
            N,
            K,
            SMaxFactor,
            out_stream,
            in_stream);
        // simpleRepeater(count, total_itr, total_itr_256, out_stream, in_stream);
}