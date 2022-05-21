#include "../common.h"
#include <OpenAL/al.h>
#include <OpenAL/alc.h>

// AUDIO ENABLE 0x4015
#define ENABLE_SQ1        0x01
#define ENABLE_SQ2        0x02
#define ENABLE_TRI        0x04
#define ENABLE_NOISE      0x08
#define ENABLE_DPCM       0x10

// status
#define MASK_DECAY        0x0f
#define MASK_FLAG_DECAY   0x10
#define MASK_FLAG_TIME    0x20
#define MASK_DUTY         0xc0

#define MASK_AMOUNT_SWEEP 0x07
#define MASK_VEC_SWEEP    0x08
#define MASK_RATE_SWEEP   0x70
#define MASK_FLAG_SWEEP   0x80

#define VEC_SWEEP_DOWN    0 
#define VEC_SWEEP_UP      1

#define MASK_FREQ_UPPER   0x07
#define MASK_TIMER        0xf8

#define SEQUENCE_MODE0    0
#define SEQUENCE_MODE1    1

#define DEFAULT_FREQ      440
#define CPU_CLOCK         1790000

enum {
	PULSE_12_5,
	PULSE_25,
	PULSE_50,
	PULSE_75,
	TRIANGLE, 
	MAX_WAVE,
};

enum {
  CHANNEL_SQ1,
  CHANNEL_SQ2,
  CHANNEL_TRI,
  MAX_CHANNEL,
};

const uint8_t count_table[32] = {
  0x0a, 0xfe, 0x14, 0x02, 0x28, 0x04, 0x50, 0x06,
  0xa0, 0x08, 0x3c, 0x0a, 0x0e, 0x0c, 0x1a, 0x0e,
  0x0c, 0x10, 0x18, 0x12, 0x30, 0x14, 0x60, 0x16, 
  0xc0, 0x18, 0x48, 0x1a, 0x10, 0x1c, 0x20, 0x1e
};

const uint8_t pulse[MAX_WAVE-1][8] = {
	{0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00},
	{0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00},
	{0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00},
};
	
const uint8_t triangle[4*8] = {
	0xff,0xee,0xdd,0xcc,0xbb,0xaa,0x99,0x88,
	0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,
	0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,
	0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff
};

struct apu emu_apu;
struct channel {
  // ctrl2
  uint8_t duty;
  bool flag_loop;
  bool flag_decay;
  uint8_t decay;
  uint8_t decay_volume;
  uint64_t decay_cnt;
  // ctrl2
  bool flag_sweep;
  uint8_t rate_sweep;
  uint8_t vec_sweep;
  uint8_t amount_sweep;
  uint8_t sweep_cnt;
  //
  uint16_t freq;
  uint8_t timer;

  bool swt;
  ALuint sid;
  ALCdevice* device;
  ALCcontext* context;
};

ALuint buffers[MAX_WAVE];

struct channel channels[MAX_CHANNEL];

const float global_gain = 1.f;
uint8_t mode = SEQUENCE_MODE0;

float reg_to_freq(uint16_t val)
{
  return ((float)CPU_CLOCK)/((val+1)<<5);
}

void audio_init()
{
  int i;
  emu_apu.reg_sound = 0;
  for(i = 0; i < MAX_CHANNEL; i++){
	  channels[i].device = alcOpenDevice(NULL);
	  if(channels[i].device == NULL){
      ERROR("error in alcOpenDevice");
	  }
	  channels[i].context = alcCreateContext(channels[i].device, NULL);
	  if(channels[i].context== NULL){
      ERROR("error in alcCreateContext");
	  }
	  alcMakeContextCurrent(channels[i].context);
  }

	alGenBuffers(MAX_WAVE, buffers);

	for(i=0; i<TRIANGLE; i++){
		ALsizei size = sizeof(pulse[0]);
		alBufferData(buffers[i], AL_FORMAT_MONO8, pulse[i], size, size*DEFAULT_FREQ);
	}
  alBufferData(buffers[TRIANGLE], AL_FORMAT_MONO8, triangle, sizeof(triangle), sizeof(triangle)*DEFAULT_FREQ);

  for(i=0; i<MAX_CHANNEL; i++){
    alGenSources(1, &(channels[i].sid));
    alSourcei(channels[i].sid, AL_LOOPING, AL_TRUE);
    alSourcef(channels[i].sid, AL_GAIN, global_gain);
  }

  // default setting
  alSourcei(channels[CHANNEL_TRI].sid, AL_BUFFER, buffers[TRIANGLE]);
  alSourcei(channels[CHANNEL_SQ1].sid, AL_BUFFER, buffers[0]);
  alSourcei(channels[CHANNEL_SQ2].sid, AL_BUFFER, buffers[1]);
  return;
}

void audio_set_mode(uint8_t val)
{
  mode = (val >> 7);
}

void audio_play(uint8_t ch)
{
  if(ch < MAX_CHANNEL)
	  alSourcePlay(channels[ch].sid);
  else
    ERROR("Invalid channel num");  
}

void audio_stop(uint8_t ch)
{
  if(ch < MAX_CHANNEL)
	  alSourceStop(channels[ch].sid);
  else
    ERROR("Invalid channel num");  
}

void audio_set_ctrl1(uint8_t ch, uint8_t val)
{
  channels[ch].decay = val & MASK_DECAY;
  channels[ch].flag_decay = (val & MASK_FLAG_DECAY) ? false : true;
  channels[ch].flag_loop = (val & MASK_FLAG_TIME) ? false : true;
  channels[ch].duty = (val & MASK_DECAY) >> 6;
  alSourcei(channels[ch].sid, AL_BUFFER, buffers[channels[ch].duty]);
  if(channels[ch].flag_decay == false)
    alSourcef(channels[ch].sid, AL_GAIN, channels[ch].decay / (0xf/global_gain));
  return;
}

void audio_set_ctrl2(uint8_t ch, uint8_t val)
{
  channels[ch].flag_sweep = (val & MASK_FLAG_SWEEP) ? true : false;
  channels[ch].amount_sweep = (val & MASK_AMOUNT_SWEEP);
  channels[ch].vec_sweep = (val & MASK_VEC_SWEEP) ? VEC_SWEEP_UP : VEC_SWEEP_DOWN;
  channels[ch].rate_sweep = (val & MASK_RATE_SWEEP) >> 4;
  return;
}

void audio_set_freq1(uint8_t ch, uint8_t val)
{
  channels[ch].freq = (channels[ch].freq & 0xff00) | val;
  alSourcef(channels[ch].sid, AL_PITCH, reg_to_freq(channels[ch].freq)/DEFAULT_FREQ);
  return;
}

void audio_set_freq2(uint8_t ch, uint8_t val)
{
  channels[ch].freq = (channels[ch].freq & 0x00ff) | ((val & MASK_FREQ_UPPER) << 8);
  channels[ch].timer = count_table[(val & MASK_TIMER) >> 3];
  alSourcef(channels[ch].sid, AL_PITCH, reg_to_freq(channels[ch].freq)/DEFAULT_FREQ);
  channels[ch].decay_volume = 0xf;
  channels[ch].decay_cnt = 0;
  channels[ch].sweep_cnt = 0;
  return;
}

void update_timer_sweep()
{
  int i;
  for(i = 0; i < MAX_CHANNEL; i++){
    /* timer */
    if(channels[i].flag_loop){
      channels[i].timer--;
      if(channels[i].timer == 0){
        channels[i].flag_loop = false;
        audio_stop(i);
      }
    }

    /* sweep  */
    if(channels[i].flag_sweep && channels[i].timer != 0 && channels[i].amount_sweep != 0) {
      channels[i].sweep_cnt++;
      if((channels[i].sweep_cnt % (channels[i].rate_sweep+1))==0){
        if(channels[i].vec_sweep == VEC_SWEEP_DOWN)
          channels[i].freq += (channels[i].freq >> channels[i].amount_sweep);
        else 
          channels[i].freq -= (channels[i].freq >> channels[i].amount_sweep);
        alSourcef(channels[i].sid, AL_PITCH, reg_to_freq(channels[i].freq)/DEFAULT_FREQ);
      }
    }

    if(channels[i].freq > 0x7ff || channels[i].freq < 8){
      channels[i].flag_sweep = false;
      channels[i].swt = false;
      audio_stop(i);
    }
  }
}

void update_envelope()
{
  int i;
  for(i = 0; i < MAX_CHANNEL; i++){
    /* decay */
    if(channels[i].flag_decay){
      channels[i].decay_cnt++;
      if(channels[i].decay_cnt % (channels[i].decay+1) == 0) {
        if(channels[i].decay_volume == 0 && channels[i].flag_loop) 
          channels[i].decay_volume = 0xf;
        else if(channels[i].decay_volume > 0)
          channels[i].decay_volume--;
        alSourcef(channels[i].sid, AL_GAIN, channels[i].decay_volume / (0xf/global_gain));
      }
    }
  }
}

void audio_update()
{
  int i;
  uint8_t step;
  while(1){
    switch(mode) {
      case SEQUENCE_MODE0:
        step = (step+1)%4;
        if((step % 2) == 1)
          update_timer_sweep();
        update_envelope();
        break;

      case SEQUENCE_MODE1:
        step = (step+1)%5;
        if((step % 2) == 1)
          update_timer_sweep();
        if(step != 4)
          update_envelope();
        break;

      default:
        ERROR("Invslid audio mode");
        break;
    } 

    for(i = 0; i < MAX_CHANNEL; i++) {
      if(channels[i].swt) {
        audio_stop(i);
        audio_play(i);
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(4));
  }
}

void audio_switch(uint8_t val)
{
  int i;
  for(i = 0; i < MAX_CHANNEL; i++){
    if(val & 1){
      channels[i].swt = true;
    }
    else {
      channels[i].swt = false;
    }
    val >>= 1;    
  }
  return;
}