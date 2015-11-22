#ifndef PIPELINE_H
#define PIPELINE_H

#include <vector>
#include <utility>
#include <memory>
#include "common.h"
#include "frame.h"

typedef std::pair<bool, std::shared_ptr<Frame>> pipeline_datatype;

class PipeLine
{
private:
    std::vector<pipeline_datatype> reservations_;

    void check_reservations_length(unsigned slot);
public:
    PipeLine();

    void make_reservation(unsigned slot, std::shared_ptr<Frame> frame);
    void cancel_reservation(unsigned slot);
    void cancel_last_packet_reservation(std::shared_ptr<Frame> frame); //for type1 only
    void make_last_packet_reservation(std::shared_ptr<Frame> frame); //for type1 only
    bool is_slot_reserved(unsigned slot);
    bool all_slots_reserved(const slot_interval& interval);
    std::shared_ptr<Frame> slot_reserved_by(unsigned slot);
    std::shared_ptr<Frame> smallest_frame_with_reserved_slot(const slot_interval& interval);

    std::vector<pipeline_datatype> reservations_in_interval(const slot_interval& interval);
};

#endif // PIPELINE_H
