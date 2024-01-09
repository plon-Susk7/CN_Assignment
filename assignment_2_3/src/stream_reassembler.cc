#include "stream_reassembler.hh"

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
    :_output(capacity),_capacity(capacity),_unassembled_bytes(0),_ack_index(0)
{}


//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {

        int data_cap = min(data.size(),stream_out().remaining_capacity());
        if(stream_out().remaining_capacity() == 0 || stream_out().eof()) return;
        if(index < _ack_index) {
            
            if(index+data.size() <= _ack_index){
                //do nothing
            }else{
                for(size_t i = _ack_index - index-1; i <data.size();i++){
                    
                    if(buffer.count(index+i)!=0) continue;

                    if(i!=data.size()-1)
                        buffer[index+i] = make_pair(data[i],false);
                    else
                        buffer[index+i] = make_pair(data[i],eof);

                    _unassembled_bytes++;
                }
            }


        }else if(index > _ack_index) {

            /*Bytes that fit within the stream’s available capacity but can’t yet be written, because
earlier bytes remain unknown. These should be stored internally in the Reassembler
(buffered basically).*/

            for(size_t i = 0;i<data_cap;i++) {
                if(buffer.count(index+i)!=0) continue;

                if(i!=data_cap-1)
                    buffer[index+i] = make_pair(data[i],false);
                else
                    buffer[index+i] = make_pair(data[i],eof);

                _unassembled_bytes++;
            }

            

        }else{
            /*Bytes that are the next bytes in the stream. The Reassembler should push these to
the Writer as soon as they are known.*/
            // _output.write(data.substr(0,data_cap));
            // _ack_index+=data_cap;

            for(size_t i=0;i<data_cap;i++){
                _output.write(string(1,data[i]));
                _ack_index++;
                if(buffer.count(index+i)!=0){
                    _unassembled_bytes--;
                    buffer.erase(index+i);
                }
            }
            if(eof) {
                _output.end_input();
            }
        }

        // Need to check if next byte is in stream or not

        while(buffer.count(_ack_index)!=0){

            if(_output.remaining_capacity() == 0 || _output.eof()) return;
            
            _output.write(string(1,buffer[_ack_index].first));
            if(buffer[_ack_index].second) {
                _output.end_input();
            }
            buffer.erase(_ack_index);
            _ack_index++;
            _unassembled_bytes--;
        }
}

size_t StreamReassembler::unassembled_bytes() const {
    return _unassembled_bytes;

}

bool StreamReassembler::empty() const {
    return _unassembled_bytes == 0;

}

size_t StreamReassembler::ack_index() const {

    return _ack_index;
}