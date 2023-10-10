#include "stream_reassembler.hh"
#include <iostream>
// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
    :_output(capacity),acknowledgement(0),unassembled(0),buffer_map()
{}



//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    size_t remaining_size=min(data.size(),stream_out().remaining_capacity());

    if(index>acknowledgement){
        for(size_t i=0;i<remaining_size;i++){
            if(buffer_map.count(index+i)==0){
                if(i==remaining_size-1){
                    pair<string,bool> insert={data.substr(i,1),eof};
                    buffer_map[index+i]=insert;
                    unassembled++;   
                }else{
                    pair<string,bool> insert={data.substr(i,1),false};
                    buffer_map[index+i]=insert;
                    unassembled++;
                }
            }
        }
        return;
    }

    if(index<acknowledgement){
        if(index+data.size()>acknowledgement&&stream_out().remaining_capacity()!=0){
            for(size_t i=acknowledgement-index-1;i<data.size();i++){
                if(buffer_map.count(index+i)==0){
                    if(i==data.size()-1){
                        buffer_map[index+i]=pair<string,bool>(data.substr(i,1),eof);
                        unassembled++;
                    }
                    else{
                        buffer_map[index+i]=pair<string,bool>(data.substr(i,1),false);
                        unassembled++;
                    }
                }
            }
        }
        else{
            return;
        }
    }

    
    if(index==acknowledgement){
        for(size_t i=0;i<remaining_size;i++){
            _output.write(data.substr(i,1));
            acknowledgement++;
            if(buffer_map.count(index+i)==0){
                continue;
            }
            buffer_map.erase(index+i);
            unassembled--;
        
        }
        if(eof){
            _output.end_input();
        }
    }
    while(buffer_map.count(acknowledgement)!=0&&stream_out().remaining_capacity()!=0){
        pair<string,bool> pair = buffer_map[acknowledgement];
        buffer_map.erase(acknowledgement);
        unassembled--;
        _output.write(pair.first);
        acknowledgement++;
        if(pair.second){
            _output.end_input();
        }
        
    }
}

size_t StreamReassembler::unassembled_bytes() const { return unassembled; }

bool StreamReassembler::empty() const { return stream_out().buffer_empty(); }

size_t StreamReassembler::ack_index() const { return acknowledgement; }