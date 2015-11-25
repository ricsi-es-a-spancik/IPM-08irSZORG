#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "common.h"
#include "frame.h"
#include "pipeline.h"

class Scheduler
{
public:
    Scheduler(const unsigned min_preiod, const unsigned max_preiod);

    void schedule(const std::vector<std::string>& raw_packets);

    unsigned get_time();
    unsigned get_total_frames_delivered();
    unsigned get_total_frames_rejected();
    const PipeLine& get_pipeline();
    const std::string act_frames() const;
    const std::string act1_frames() const;
    const std::string get_last_transfered_packet() const;
private:
    unsigned time_ = 0;

    void subroutine_a(std::shared_ptr<Frame> frame);
    void subroutine_s();

    bool is_first_packet(const std::string& raw_packet) const;
    std::shared_ptr<Frame> parse_first_packet(const std::string& raw_packet) const;
    std::shared_ptr<Packet> parse_packet(const std::string& raw_packet) const;

    const unsigned min_period_;
    const unsigned max_period_;

    std::map<std::string, std::shared_ptr<Frame>> act;
    std::map<std::string, std::pair<std::shared_ptr<Frame>, int>> act1;
    PipeLine pipeline_; //last packet reservations

    unsigned total_frames_delivered_ = 0;
    unsigned total_frames_rejected_ = 0;
    std::string last_transfered_packet_;
};

#endif // SCHEDULER_H
