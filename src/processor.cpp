#include "processor.h"
#include "linux_parser.h"


Processor::Processor() : PrevIdle_(0), PrevNonIdle_(0) {}

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
    auto c = LinuxParser::CpuUtilization();

    float Idle = stof(c[LinuxParser::CPUStates::kIdle_]) + stof(c[LinuxParser::CPUStates::kIOwait_]);
    float NonIdle = stof(c[LinuxParser::CPUStates::kUser_]) + stof(c[LinuxParser::CPUStates::kNice_])
        + stof(c[LinuxParser::CPUStates::kSystem_]) + stof(c[LinuxParser::CPUStates::kIRQ_])
        + stof(c[LinuxParser::CPUStates::kSoftIRQ_]) + stof(c[LinuxParser::CPUStates::kSteal_]);

    float PrevTotal = PrevIdle_ + PrevNonIdle_;
    float Total = Idle + NonIdle;

    float totald = Total - PrevTotal;
    float idled = Idle - PrevIdle_;
    float ret = (totald - idled) / totald;

    PrevIdle_ = Idle;
    PrevNonIdle_ = NonIdle;
    return ret;
}
