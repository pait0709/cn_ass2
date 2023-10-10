#include "tcp_receiver.hh"

#include <algorithm>


using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    const TCPHeader head = seg.header();
    // --- Hint ------
        // convert the seqno into absolute seqno
    uint64_t checkpoint = _reassembler.ack_index();
    uint64_t abs_seqno = unwrap(head.seqno, _isn, checkpoint);
    uint64_t stream_idx = abs_seqno - _synReceived;
    // --- Hint ------  


    if(head.syn||_synReceived){
    // ...
        string data=seg.payload().copy();
        bool eof=false;
        if(head.syn&&!_synReceived){
            _synReceived=true;
            _isn=head.seqno;
            _finReceived=head.fin;
            eof=head.fin;
            _reassembler.push_substring(data,0,eof);
            return;
        }
        
        _finReceived=head.fin;
        eof=head.fin;
        
        _reassembler.push_substring(data,stream_idx,eof);
    }
    // ... 
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if(_synReceived){
        if(_finReceived){
            return wrap(_reassembler.ack_index()+2, _isn);
        }
        return wrap(_reassembler.ack_index()+1, _isn);
    }
    return nullopt;
}

size_t TCPReceiver::window_size() const { return _capacity -_reassembler.stream_out().buffer_size(); }
