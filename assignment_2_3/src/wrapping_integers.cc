#include "wrapping_integers.hh"

#include <iostream>

using namespace std;

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) { return isn + uint32_t(n); }

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {

    uint64_t upper_bound_32bit = 1UL<<32;

    uint64_t abs_seq = n.raw_value() - isn.raw_value();

    uint64_t number_wraps = checkpoint / upper_bound_32bit; // make changes to this part

    uint64_t potential_number = number_wraps*upper_bound_32bit + abs_seq; 

    if (potential_number < checkpoint) potential_number+=upper_bound_32bit;
    
    uint64_t right,left;

    right = potential_number-checkpoint;
    left = checkpoint-(potential_number-upper_bound_32bit);


    if (potential_number>=upper_bound_32bit && right > left)  potential_number =  potential_number - upper_bound_32bit;

    return static_cast<uint64_t>(potential_number);
}
