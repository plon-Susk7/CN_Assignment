#include "tcp_receiver.hh"

#include <algorithm>


using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    const TCPHeader head = seg.header();

    string data = seg.payload().copy();

    bool eof = false;

    if(!head.syn && !_synReceived) return;

    if(head.syn && !_synReceived){
        _synReceived = true;
        _isn = head.seqno;
        if(head.fin){
            _finReceived = true;
            eof = true;
        }

        _reassembler.push_substring(data, 0, eof);
        return;
    }

    if(_synReceived && head.fin){
        _finReceived = true;
        eof = true;
    }
    // ...

    // --- Hint ------
        // convert the seqno into absolute seqno
    uint64_t checkpoint = _reassembler.ack_index();
    uint64_t abs_seqno = unwrap(head.seqno, _isn, checkpoint);
    uint64_t stream_idx = abs_seqno - _synReceived;
    // --- Hint ------  

    // ... 

    _reassembler.push_substring(data, stream_idx, eof);

}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (!_synReceived) {
        return nullopt;
    }

    if(_reassembler.empty() && _finReceived) {
        return wrap(_reassembler.ack_index() + 2 , _isn);
    }
    
    return wrap(_reassembler.ack_index() + 1, _isn);

}

size_t TCPReceiver::window_size() const {
   return _capacity - _reassembler.stream_out().buffer_size();
}
