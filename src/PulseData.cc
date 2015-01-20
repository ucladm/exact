#include "PulseData.hh"

PulseData::PulseData():
  pulse_id(-1),
  start_time(-999),
  end_time(-999),
  peak_time(-1),
  peak_amp(-1),
  saturated(false),
  integral(-1),
  fixed_int1(-1),
  fixed_int2(-1),
  f90(-1)
{ }
