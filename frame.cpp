#include "frame.h"

#include <stdexcept>
#include <climits>

std::shared_ptr<Frame> Frame::create_virtual_frame(frame_type type)
{
    return std::make_shared<Frame>("", type, 0, 0, 0, 0, 0, 0);
}

Frame::Frame(const std::string& id, frame_type type, unsigned packet_num, unsigned value, unsigned periodicity, unsigned jitter, unsigned slack, unsigned arrival) :
    id_(id), type_(type), packet_num_(packet_num), value_(value), periodicity_(periodicity), jitter_(jitter), slack_(slack), arrival_(arrival)
{

}

double Frame::density()
{
    return value_ / static_cast<double>(packet_num_);
}

unsigned Frame::packet_excepted_arrival(unsigned packet_idx) const
{
    return arrival_ + packet_idx * periodicity_;
}

slot_interval Frame::slack_interval() const
{
    auto tmp = packet_excepted_arrival(packet_num_ - 1) + jitter_;
    return slot_interval(tmp, tmp + slack_);
}

unsigned Frame::packet_deadline(unsigned packet_idx) const
{
    return packet_excepted_arrival(packet_idx) + jitter_ + periodicity_;
}

unsigned Frame::next_packet_deadline() const
{
    if(packets_to_transmit.size() == 0)
        return UINT_MAX;
    else
        return packet_deadline(packets_transmitted_);
}

slot_interval Frame::packet_designated_interval(unsigned packet_idx) const
{
    return slot_interval(packet_excepted_arrival(packet_idx) + jitter_, packet_deadline(packet_idx));
}

bool Frame::is_last_packet() const
{
    return packets_transmitted_ + 1 == packet_num_;
}

void Frame::add_packet(std::shared_ptr<Packet> packet)
{
    if(packets_added_++ < packet_num_)
        packets_to_transmit.push(packet);
    else
        throw std::length_error("Expected packet number is exceeded (frame already contains all of its packets).");
}

void Frame::transmit_next_packet()
{
    if(packets_to_transmit.size() != 0)
    {
        packets_to_transmit.pop();
        ++packets_transmitted_;
    }
}

std::shared_ptr<Packet> Frame::next_packet_to_transmit() const
{
    if(packets_to_transmit.size() == 0)
        return std::shared_ptr<Packet>(nullptr);
    else
        return packets_to_transmit.front();
}

bool Frame::Compare::operator() (const Frame& fst, const Frame& snd) const
{
    return fst.id_ < snd.id_;
}

bool Frame::Compare::operator() (const std::shared_ptr<Frame>& fst, const std::shared_ptr<Frame>& snd) const
{
    return this->operator ()(*fst, *snd);
}

