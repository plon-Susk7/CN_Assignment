#include "byte_stream.hh"
#include <algorithm>
#include <iostream>

// You will need to add private members to the class declaration in `byte_stream.hh`

/* Replace all the dummy definitions inside the methods in this file. */


using namespace std;

ByteStream::ByteStream(const size_t capa) : capcity(capa),total_bytes_buffered(0),total_bytes_read(0),total_bytes_written(0),_ended(false),_error(false),buffer() {}

size_t ByteStream::write(const string &data) {
    // writing as much is possible

    size_t data_cap = min(data.size(),remaining_capacity());
    for (size_t i=0; i<data_cap;i++) {
        buffer.push_back(data[i]);
    }
    total_bytes_written+=data_cap;
    total_bytes_buffered+=data_cap;
    return data_cap;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
  //peeking as much is possible
  return string(buffer.begin(),buffer.begin()+min(len,buffer_size()));
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
  size_t data_cap = min(len,buffer_size());

  // setting error if len is greater than buffer size
  if(len > buffer_size()) {
      set_error();
  }

  for (size_t i = 0; i < data_cap; i++) {
      buffer.pop_front();
  }
  total_bytes_buffered-=data_cap;
  total_bytes_read+=data_cap;
    
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
  if(len > buffer_size()) {
      set_error();
  }

  string output = peek_output(len);
  pop_output(len);
  return output;
}

void ByteStream::end_input() { 
  _ended = true; 
}

bool ByteStream::input_ended() const {  return _ended;}

size_t ByteStream::buffer_size() const {
  return buffer.size();
}

bool ByteStream::buffer_empty() const {
 return buffer_size() == 0 ? true : false; 
}

bool ByteStream::eof() const {
  return (buffer_empty() && input_ended()) ? true : false;
}

size_t ByteStream::bytes_written() const {
   return total_bytes_written; 
}

size_t ByteStream::bytes_read() const {
    return total_bytes_read; 
 }

size_t ByteStream::remaining_capacity() const { 
  
  return capcity - buffer_size(); 
}
