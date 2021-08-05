#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "linux_parser.h"
#include <unistd.h>

class Processor {
 public:
  Processor();
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  float PrevIdle_;
  float PrevNonIdle_;
};

#endif
