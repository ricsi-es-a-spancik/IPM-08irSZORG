#include "pipeline.h"

#include <stdexcept>
#include <algorithm>

PipeLine::PipeLine()
{
}

void PipeLine::check_reservations_length(unsigned slot)
{
    if(reservations_.size() <= slot)
    {
        if(reservations_.size() <= slot / 2)
            reservations_.resize(slot+1, std::make_pair(false, std::shared_ptr<Frame>(nullptr)));
        else
            reservations_.resize(reservations_.size()*2+1, std::make_pair(false, std::shared_ptr<Frame>(nullptr)));
    }
}

void PipeLine::make_reservation(unsigned slot, std::shared_ptr<Frame> frame)
{
    check_reservations_length(slot);

    if(reservations_[slot].first)
    {
        throw std::logic_error("The given slot has already been reserved.");
    }
    else
    {
        reservations_[slot].second = frame;
        reservations_[slot].first = true;
    }
}

void PipeLine::cancel_reservation(unsigned slot)
{
    check_reservations_length(slot);

    if(!reservations_[slot].first)
    {
        throw std::logic_error("The given slot reservation cannot be canceled because it is not a reserved slot.");
    }
    else
    {
        reservations_[slot].second = nullptr;
        reservations_[slot].first = false;
    }
}

void PipeLine::cancel_last_packet_reservation(std::shared_ptr<Frame> frame)
{
    auto slack = frame->slack_interval();
    auto interval_res = reservations_in_interval(slack);
    auto result = std::find_if(interval_res.rbegin(), interval_res.rend(), [frame](const pipeline_datatype& pair) -> bool {return frame == pair.second;} );
    cancel_reservation(slack.second - 1 - std::distance(interval_res.rbegin(), result));
}

void PipeLine::make_last_packet_reservation(std::shared_ptr<Frame> frame)
{
    auto slack = frame->slack_interval();
    auto interval_res = reservations_in_interval(slack);
    auto result = std::find_if(interval_res.begin(), interval_res.end(), [](const pipeline_datatype& pair) -> bool {return !pair.first;} );
    make_reservation(slack.first + std::distance(interval_res.begin(), result), frame);
}

bool PipeLine::is_slot_reserved(unsigned slot)
{
    check_reservations_length(slot);

    return reservations_[slot].first;
}

bool PipeLine::all_slots_reserved(const slot_interval& interval)
{
    auto interval_res = reservations_in_interval(interval);
    return std::all_of(interval_res.begin(), interval_res.end(),
                [](const pipeline_datatype& pair) -> bool {return pair.first;});
}

std::shared_ptr<Frame> PipeLine::slot_reserved_by(unsigned slot)
{
    return reservations_[slot].second;
}

std::shared_ptr<Frame> PipeLine::smallest_frame_with_reserved_slot(const slot_interval& interval)
{
    auto interval_res = reservations_in_interval(interval);
    return std::min_element(interval_res.begin(), interval_res.end(), [](const pipeline_datatype& fst, const pipeline_datatype& snd) -> bool
        {return fst.second->packet_num_ < snd.second->packet_num_;})->second;
}

std::vector<pipeline_datatype> PipeLine::reservations_in_interval(const slot_interval& interval)
{
    check_reservations_length(interval.second);

    return std::vector<pipeline_datatype>(reservations_.begin()+interval.first, reservations_.begin()+interval.second);
}

