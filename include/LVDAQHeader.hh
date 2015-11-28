/*

  2015-06-02 AFan

  Header file for new LabView DAQ.


 */

#ifndef LVDAQHeader_hh
#define LVDAQHeader_hh

#include <fstream>
#include <iostream>
#include <time.h>
#include <string>
#include <vector>
#include <stdint.h>


using namespace std;


class LVDAQHeader
{
public:

  LVDAQHeader() {};
  
  char filename[300];   // binary file name
  int header_size;      // size of header [characters]


  fstream binary_file;

  string UCLA;
  string version;
  int year;
  int month;
  int day;
  int hour;
  int minute;
  float sec;
  int nchannels;
  int run_id;
  int ntriggers;
  vector<int> vertical_offset;
  vector<float> vertical_full_scale;
  int trigger_sample;
  int sample_interval;
  int nsamps;
  int trigger_level;
  int trigger_source;
  int trigger_channel;
  int trigger_coupling;
  int trigger_slope;
  int nbits;

  uint16_t event_sec;
  uint16_t event_millisec;
  
  bool format_test();
  void load_file(std::string file);
  void read_header_content();
  void close_file();

  void read_event(int event_id, vector< vector<double> > & data_array);
  void read_event_channel(int event_id, int channel_id, vector<double> & array);

};


#endif
