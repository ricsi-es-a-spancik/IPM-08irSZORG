#ifndef PACKET_H
#define PACKET_H

#include <string>
#include "common.h"

class Packet
{
public:
    Packet(const std::string& id, unsigned arrival) : frame_id_(id), arrival_(arrival) {}

    const std::string frame_id_;
    const unsigned arrival_;

    unsigned get_scheduled_at() const;
    void set_scheduled_at(unsigned scheduled_at);
private:
    bool scheduled = false;
    unsigned scheduled_at_;
};

#endif // PACKET_H
