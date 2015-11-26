#ifndef FRAME_H
#define FRAME_H

#include <string>
#include <list>
#include <queue>
#include <utility>
#include <memory>
#include "common.h"
#include "packet.h"

enum frame_type { TYPE_ONE, TYPE_TWO };

class Frame
{
private:
    unsigned packet_excepted_arrival(unsigned packet_idx) const;

    unsigned packets_added_ = 0;
    unsigned packets_transmitted_ = 0;
    std::queue<std::shared_ptr<Packet>, std::list<std::shared_ptr<Packet>>> packets_to_transmit;

    static unsigned pending_packets_overall_;
public:
    static std::shared_ptr<Frame> create_virtual_frame(frame_type type);

    Frame(const std::string& id, frame_type type, unsigned packet_num, unsigned value, unsigned periodicity, unsigned jitter, unsigned slack, unsigned arrival);
    ~Frame();

    const std::string id_;
    const frame_type type_;
    const unsigned packet_num_;
    const unsigned value_;
    const unsigned periodicity_;
    const unsigned jitter_;
    const unsigned slack_;
    const unsigned arrival_;

    double density();
    slot_interval slack_interval() const;
    unsigned next_packet_deadline() const;
    unsigned packet_deadline(unsigned packet_idx) const;
    slot_interval next_packet_designated_interval() const;
    slot_interval packet_designated_interval(unsigned packet_idx) const;

    bool has_packet() const;
    bool is_last_packet() const;
    void add_packet(std::shared_ptr<Packet> packet);
    void transmit_next_packet();
    std::shared_ptr<Packet> next_packet_to_transmit() const;
    static unsigned pending_packets_overall();

    class Compare
    {
    public:
        bool operator() (const Frame& fst, const Frame& snd) const;
        bool operator() (const std::shared_ptr<Frame>& fst, const std::shared_ptr<Frame>& snd) const;
    };
};

#endif // FRAME_H
