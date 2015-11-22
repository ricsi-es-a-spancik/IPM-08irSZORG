#include "packet.h"

#include <stdexcept>

unsigned Packet::get_scheduled_at() const
{
    return scheduled_at_;
}

void Packet::set_scheduled_at(unsigned scheduled_at)
{
    if(scheduled)
        throw std::logic_error("Packet has already been scheduled!");

    scheduled_at_ = scheduled_at;
    scheduled = true;
}
