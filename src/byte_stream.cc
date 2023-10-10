

#include "byte_stream.hh"
#include <bits/stdc++.h>
#include <algorithm>

// You will need to add private members to the class declaration in `byte_stream.hh`

//Replace all the dummy definitions inside the methods in this file. 


using namespace std;

ByteStream::ByteStream(const size_t capa){

    capacity = capa;
    stream={};
    total_write = 0;
    total_read = 0;
    fin = false;
    _error= false;
}

size_t ByteStream::write(const string &data) {
  size_t bytes_to_write=min(data.size(),remaining_capacity());
  for(size_t i=0;i<bytes_to_write;i++){
    stream.push_back(data[i]);
    total_write++;
  }
  return bytes_to_write;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
  size_t bytes_to_peek=min(len,stream.size());
  string peeked="";
  for(size_t i=0;i<bytes_to_peek;i++){
    peeked+=stream[i];
  }
  return peeked;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
  if(len>capacity){
    _error=true;
    return;
  }
  size_t bytes_to_pop=min(len,stream.size());

  for(size_t i=0;i<bytes_to_pop;i++){
    stream.pop_front();
    total_read++;
  }
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
  if(len>capacity){
    _error=true;
    return"";
  }
  string readed=peek_output(len);
  pop_output(len);
  return readed;
}

void ByteStream::end_input() {
  fin=true;
}

bool ByteStream::input_ended() const { 
  return fin;
}

size_t ByteStream::buffer_size() const {
  return stream.size();
}

bool ByteStream::buffer_empty() const {
  return stream.empty();
}

bool ByteStream::eof() const { 
  return fin && stream.empty();
}

size_t ByteStream::bytes_written() const { 
  return total_write;
}

size_t ByteStream::bytes_read() const { 
  return total_read;
}

size_t ByteStream::remaining_capacity() const {
  return capacity-stream.size();
}
