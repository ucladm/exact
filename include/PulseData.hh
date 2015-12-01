/*

  Pulse-level information

  v0.1 2015-01-19 AFan

 */


#ifndef PulseData_hh
#define PulseData_hh

class PulseData
{
public:
  PulseData();

  int pulse_id;
  double start_time;
  double end_time;
  double peak_time;
  double peak_amp;
  bool saturated;

  double integral;
  double prompt;
  double fixed_int1;
  double fixed_int2;

  double f90;
};


#endif
