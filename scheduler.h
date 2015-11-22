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
    PipeLine pipeline; //last packet reservations
};

#endif // SCHEDULER_H
