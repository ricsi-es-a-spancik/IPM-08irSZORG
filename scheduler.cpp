#include "scheduler.h"

#include <stdexcept>
#include <algorithm>
#include <string.h>
#include <sstream>

std::vector<std::string> split(const std::string &s, char delim)
{
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> tokens;
    while (std::getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

unsigned string_to_unsigned(const std::string& str)
{
    std::stringstream ss(str);
    unsigned result;
    if(ss >> result)
        return result;
    else
        throw std::runtime_error("Cannot convert string to unsigned int:" + str);
}

Scheduler::Scheduler(const unsigned min_period, const unsigned max_period) : min_period_(min_period), max_period_(max_period)
{

}

void Scheduler::schedule(const std::vector<std::string>& raw_packets)
{
    for(const auto& raw_packet : raw_packets)
    {
        if(is_first_packet(raw_packet))
        {
            std::shared_ptr<Frame> frame = parse_first_packet(raw_packet);
            subroutine_a(frame);
        }
        else
        {
            auto packet = parse_packet(raw_packet);
            auto result = act.find(packet->frame_id_);
            if(result != act.end())
                result->second->add_packet(packet);
        }
    }

    subroutine_s();

    for( auto it = act1.begin(); it != act1.end(); )
    {
        if( it->second.second ==  static_cast<int>(this->time_)) it = act1.erase(it);
        else ++it;
    }

    ++time_;
}

void Scheduler::subroutine_a(std::shared_ptr<Frame> frame)
{
    std::shared_ptr<Frame> frame_tick;

    if(frame->type_ == TYPE_ONE && frame->packet_num_ == 1 && act1.size() >= min_period_ / 2)
        return; //rejected
    else if(frame->type_ == TYPE_ONE && frame->packet_num_ > 1 && pipeline.all_slots_reserved(frame->slack_interval()))
        frame_tick = pipeline.smallest_frame_with_reserved_slot(frame->slack_interval());
    else
        frame_tick = Frame::create_virtual_frame(TYPE_TWO);

    if(frame_tick->packet_num_ == 0 && act.size() >= min_period_ / 2)
        frame_tick = std::min_element(act.begin(), act.end(), [](const std::pair<std::string, std::shared_ptr<Frame>>& fst,
                                       const std::pair<std::string, std::shared_ptr<Frame>>& snd)
                                       -> bool {return fst.second->packet_num_ < snd.second->packet_num_;})->second;

    if(frame->packet_num_ < 2 * frame_tick->packet_num_)
        return; //rejected

    if(frame_tick->type_ == TYPE_ONE)
    {
        act1.erase(frame_tick->id_);
        pipeline.cancel_last_packet_reservation(frame_tick);
    }

    if(frame->type_ == TYPE_ONE)
    {
        act1[frame->id_] = std::make_pair(frame, -1);
        pipeline.make_last_packet_reservation(frame);
    }

    if(frame_tick->packet_num_ != 0)
        act.erase(frame_tick->id_);

    act[frame->id_] = frame;
}

void Scheduler::subroutine_s()
{
    if(pipeline.is_slot_reserved(time_))
    {
        auto frame = pipeline.slot_reserved_by(time_);
        act.erase(frame->id_);
        act1[frame->id_].second = time_ + min_period_;

        //auto packet = frame->next_packet_to_transmit(); //query last packet
        frame->transmit_next_packet(); //transmitting last packet
    }
    else
    {
        auto result = std::min_element(act.begin(), act.end(),[](const std::pair<std::string, std::shared_ptr<Frame>>& fst,
                                       const std::pair<std::string, std::shared_ptr<Frame>>& snd)
                                      -> bool {return fst.second->next_packet_deadline() < snd.second->next_packet_deadline();});

        if(result != act.end())
        {
            auto frame = result->second;
            if(frame->is_last_packet())
                act.erase(frame->id_);

            //auto packet = frame->next_packet_to_transmit(); //query last packet
            frame->transmit_next_packet(); //transmitting last packet
        }
    }
}

bool Scheduler::is_first_packet(const std::string& raw_packet) const
{
    return split(raw_packet, ' ').size() > 1;
}

std::shared_ptr<Frame> Scheduler::parse_first_packet(const std::string& raw_packet) const
{
    //format: "frame_id packet_num value periodicity jitter slack"
    auto splitted = split(raw_packet, ' ');
    unsigned slack = string_to_unsigned(splitted[5]);

    auto frame = std::make_shared<Frame>(
            splitted[0],
            slack < min_period_ ? TYPE_ONE : TYPE_TWO,
            string_to_unsigned(splitted[1]),
            string_to_unsigned(splitted[2]),
            string_to_unsigned(splitted[3]),
            string_to_unsigned(splitted[4]),
            slack,
            time_);

    frame->add_packet(std::make_shared<Packet>(splitted[0], time_));
    return frame;
}

std::shared_ptr<Packet> Scheduler::parse_packet(const std::string& raw_packet) const
{
    //format: "frame_id"
    auto splitted = split(raw_packet, ' ');
    return std::make_shared<Packet>(splitted[0], time_);
}
