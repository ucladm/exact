#include "LVDAQHeader.hh"

#include <sstream>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

using namespace std;


extern const int filename_size = 300;
extern const int int_type_size = 4;
extern const int double_type_size = 8;
extern const int uint8_type_size = 1;
extern const int uint16_type_size = 2;

void LVDAQHeader::load_file(std::string file)
{
  sprintf(filename, "%s", file.c_str());
  try {if (binary_file.is_open()) throw std::runtime_error("ERROR: There is already an open file in LVDAQHeader!");}
  catch (const std::exception & e) { cerr << e.what()<<endl; throw; }
  binary_file.open(filename, ios::in|ios::binary);
}

void LVDAQHeader::close_file()
{  
  if (binary_file.is_open()) binary_file.close();
}

bool LVDAQHeader::format_test()
{
  int local_int_variable;
  double local_double_variable;

  if (sizeof(local_int_variable) != int_type_size) {
    cout << "ERROR: LVDAQHeader: int type variable size is not compatible!"<<endl;
    return false;
  }
  else if (sizeof(local_double_variable) != double_type_size) {
    cout << "ERROR: LVDAQHeader: double type variable size is not compatible!"<<endl;
    return false;
  }
  else
    return true;
}

void LVDAQHeader::read_header_content()
{
  header_size = 152;

  bool print_header = false;
  string header(header_size,' ');
  if (print_header) {
    cout << "\nHEADER"<<endl;
    binary_file.read(&header[0],header.size());
    cout << header << endl;
  }
  

  binary_file.seekg(0,std::ios::beg);
  
  // Extract all the header data as strings first, since that's how they're written into the raw data.
  string s_UCLA(4,' ');       binary_file.read(&s_UCLA[0],s_UCLA.size()); 
  string s_version(6,' ');    binary_file.read(&s_version[0],s_version.size());
  string s_year(4,' ');       binary_file.read(&s_year[0], s_year.size());
  string s_month(2,' ');      binary_file.read(&s_month[0],s_month.size());
  string s_day(2,' ');        binary_file.read(&s_day[0], s_day.size());
  string s_hour(2,' ');       binary_file.read(&s_hour[0], s_hour.size());
  string s_minute(2,' ');     binary_file.read(&s_minute[0], s_minute.size());
  string s_sec(6,' ');        binary_file.read(&s_sec[0], s_sec.size());
  string s_nchannels(2,' ');  binary_file.read(&s_nchannels[0], s_nchannels.size());
  string s_run_id(6,' ');     binary_file.read(&s_run_id[0], s_run_id.size());
  string s_ntriggers(6, ' '); binary_file.read(&s_ntriggers[0], s_ntriggers.size());
  vector<string> s_vertical_offset;
  int tmp_nchans; istringstream(s_nchannels) >> tmp_nchans;
  for (int i=0; i<tmp_nchans; ++i) {
    string chOffset(6,' '); binary_file.read(&chOffset[0], chOffset.size());
    s_vertical_offset.push_back(chOffset);
  }
  vector<string> s_full_scale;
  for (int i=0; i<tmp_nchans; ++i) {
    string chFullScale(4,' '); binary_file.read(&chFullScale[0], chFullScale.size());
    s_full_scale.push_back(chFullScale);
  }
  string s_trigger_sample(7,' ');     binary_file.read(&s_trigger_sample[0], s_trigger_sample.size());
  string s_sample_interval(3,' ');    binary_file.read(&s_sample_interval[0], s_sample_interval.size());
  string s_nsamps(7,' ');             binary_file.read(&s_nsamps[0], s_nsamps.size());
  string s_trigger_level(6,' ');      binary_file.read(&s_trigger_level[0], s_trigger_level.size());
  string s_trigger_source(1,' ');     binary_file.read(&s_trigger_source[0], s_trigger_source.size());
  string s_trigger_channel(2,' ');    binary_file.read(&s_trigger_channel[0], s_trigger_channel.size());
  string s_trigger_coupling(1,' ');   binary_file.read(&s_trigger_coupling[0], s_trigger_coupling.size());
  string s_trigger_slope(1,' ');      binary_file.read(&s_trigger_slope[0], s_trigger_slope.size());
  string s_nbits(2,' ');              binary_file.read(&s_nbits[0], s_nbits.size());

  
  // Convert all the strings to offline-friendly formats.
  UCLA = s_UCLA;
  version = s_version;
  
  istringstream(s_year) >> year;
  istringstream(s_month) >> month;
  istringstream(s_day) >> day;
  istringstream(s_hour) >> hour;
  istringstream(s_minute) >> minute;
  istringstream(s_sec) >> sec;
  istringstream(s_nchannels) >> nchannels;
  istringstream(s_run_id) >> run_id;
  istringstream(s_ntriggers) >> ntriggers;
  vertical_offset.clear();
  for (int i=0; i<(int)s_vertical_offset.size(); ++i) {
    // first character is sign
    int sign = 1;
    if (s_vertical_offset[i].compare(0,1,"+")==0) sign = 1;
    else if (s_vertical_offset[i].compare(0,1,"-")==0) sign = -1;
    int offset;
    istringstream(s_vertical_offset[i].substr(1)) >> offset;
    vertical_offset.push_back(sign*offset);
  }
  vertical_full_scale.clear();
  for (int i=0; i<(int)s_full_scale.size(); ++i) {
    int scale; istringstream(s_full_scale[i]) >> scale;
    vertical_full_scale.push_back(scale);
  }
  istringstream(s_trigger_sample) >> trigger_sample;
  istringstream(s_sample_interval) >> sample_interval;
  istringstream(s_nsamps) >> nsamps;
  int sign = 1;
  if (s_trigger_level.compare(0,1,"+")==0) sign = 1;
  else if (s_trigger_level.compare(0,1,"-")==0) sign = -1;
  int temp; istringstream(s_trigger_level.substr(1)) >> temp; trigger_level = temp*sign;
  istringstream(s_trigger_source) >> trigger_source;
  istringstream(s_trigger_channel) >> trigger_channel;
  istringstream(s_trigger_coupling) >> trigger_coupling;
  istringstream(s_trigger_slope) >> trigger_slope;
  istringstream(s_nbits) >> nbits;

  // hard code for now because DAQ is broken
  //nchannels=8;
  if (print_header) {
    cout << "\nPARSED HEADER"<<endl;
    cout << "UCLA: "<<UCLA<<endl;
    cout << "version: "<<version<<endl;
    cout << "date: "<<year<<"-"<<month<<"-"<<day<<endl;
    cout << "time: "<<hour<<":"<<minute<<":"<<sec<<endl;
    cout << "nchannels: "<<nchannels<<endl;
    cout << "run ID: "<<run_id<<endl;
    cout << "n triggers: "<<ntriggers<<endl;
    cout << "v. offsets: "; for (int i=0; i<(int)vertical_offset.size(); ++i) cout << vertical_offset[i]<<" "; cout << endl;
    cout << "full scales: "; for (int i=0; i<(int)vertical_full_scale.size(); ++i) cout << vertical_full_scale[i]<<" "; cout << endl;
    cout << "trigger sample: "<<trigger_sample<<endl;
    cout << "sample interval: "<<sample_interval<<endl;
    cout << "nsamps: "<<nsamps<<endl;
    cout << "trigger level: "<<trigger_level<<endl;
    cout << "trigger source: "<<trigger_source<<endl;
    cout << "trigger channel: "<<trigger_channel<<endl;
    cout << "trigger coupling: "<<trigger_coupling<<endl;
    cout << "trigger slope: "<<trigger_slope<<endl;
    cout << "nbits: "<<nbits<<endl;
  }
}

void LVDAQHeader::read_event(int event_id, vector< vector<double> > & data_array)
{
  data_array.clear();

  if (event_id < 0 || event_id >= ntriggers) {
    cout << "ERROR: LVDAQHeader: event_id outside of expected range."<<endl;
    abort();
  }

  // Determine which point of file to navigate to:
  // must be careful of first event, which does not have a timestamp associated to it
  // header size + event_id * waveform data size + (event_id-1) * timestamp size;
  const int channel_data_length = nsamps*uint8_type_size;
  const long int start_point = (header_size +
                                event_id*channel_data_length*nchannels +
                                (event_id>0?event_id-1:0)*2*uint16_type_size);

  // Navigate to target position in file and extract event data
  binary_file.seekg(start_point, std::ios::beg);
  
  // First event has no timestamp associated to it.
  if (event_id == 0) {
    event_sec = 0;
    event_millisec = 0;
  }
  else { //event_id > 0
    binary_file.read((char*)&event_sec, sizeof(event_sec));
    binary_file.read((char*)&event_millisec, sizeof(event_millisec));
  }

  // Load waveform
  vector<double> single_waveform;
  for (int ch=0; ch<nchannels; ++ch) {
    single_waveform.clear();
    for (int i=0; i<nsamps; ++i) {
      int8_t value;
      binary_file.read((char*)&value, sizeof(char));
      single_waveform.push_back(value);
    }
    data_array.push_back(single_waveform);
  }

}

void LVDAQHeader::read_event_channel(int event_id, int channel_id, vector<double> & array)
{
  array.clear();
  if (event_id < 0 || event_id >= ntriggers) {
    cout << "ERROR: LVDAQHeader: event_id outside of expected range."<<endl;
    abort();
  }

  // Determine which point of file to navigate to:
  // header size + event_id * waveform data size + (event_id-1) * timestamp size;
  const int channel_data_length = nsamps*uint8_type_size;
  const long int start_point = (header_size +
                                event_id*2*uint16_type_size + 
                                event_id*channel_data_length*nchannels +
                                channel_id*nsamps);
                                

  // Navigate to target position in file and extract event data
  binary_file.seekg(start_point, std::ios::beg);

  // Load waveform
  for (int i=0; i<nsamps; ++i) {
    int8_t value;
    binary_file.read((char*)&value, sizeof(char));
    //single_waveform.push_back(value);
    array.push_back(value);
  }
}

void LVDAQHeader::read_event_timestamp(int event_id)
{
  if (event_id < 0 || event_id >= ntriggers) {
    cout << "ERROR: LVDAQHeader: event_id outside of expected range."<<endl;
    abort();
  }

  // Determine which point of file to navigate to:
  // header size + event_id * waveform data size + (event_id-1) * timestamp size;
  const int channel_data_length = nsamps*uint8_type_size;
  const long int start_point = (header_size +
                                event_id*2*uint16_type_size + 
                                event_id*channel_data_length*nchannels);

  // Navigate to target position in file and extract event data
  binary_file.seekg(start_point, std::ios::beg);

  binary_file.read((char*)&event_sec, sizeof(event_sec));
  binary_file.read((char*)&event_millisec, sizeof(event_millisec));



}
