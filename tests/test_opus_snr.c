/* Copyright (c) Microsoft Corporation.
   Written by Marcus Asteborg and Warren Lam */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef _WIN32
#define _USE_MATH_DEFINES /* M_PI */
#endif

#define MAX_PACKET 1500
#define MAX_FRAME_SAMP 5760
#define SAMPLES (48000*30)
#define SSAMPLES (SAMPLES/3)

//sampling_rates 8, 12, 16, 24, 32, 48
//Bitrates 6 -> 510
//Mono -> Stereo
//Frame size 2.5 -> 60ms?
//fixed point
//floating point

#include <math.h> /* M_PI */
#include <stdlib.h> /* malloc */
#include <stdio.h> /* stderr */
#include <string.h> /* memset */
#include "opus.h"
#include "opus_types.h"
#include "test_opus_common.h"

void project_on_tone(opus_int16* buffer, int num_samples, int tone_frequency, int sampling_frequency,
                            float* signal_power, float* noise_power, float* signal_buffer,
                            float* noise_buffer)
{
    // inner products
    float ss = 0;
    float cc = 0;
    float sx = 0;
    float cx = 0;

    for (int i = 0; i < num_samples; i++) {
        float s = (float)sin(2 * (float)M_PI * i * tone_frequency / sampling_frequency);
        float c = (float)cos(2 * (float)M_PI * i * tone_frequency / sampling_frequency);
        ss += s * s;
        cc += c * c;
        sx += s * ((float)buffer[i]);
        cx += c * ((float)buffer[i]);
    }

    *signal_power = 0.0f;
    *noise_power = 0.0f;
    for (int i = 0; i < num_samples; i++) {
        float s = (float)sin(2 * M_PI * i * tone_frequency / sampling_frequency);
        float c = (float)cos(2 * M_PI * i * tone_frequency / sampling_frequency);
        float signal = sx * s / ss + cx * c / cc;
        float noise = signal - (float)buffer[i];
        *signal_power += signal * signal;
        *noise_power += noise * noise;
        if (signal_buffer) {
            signal_buffer[i] = signal;
        }
        if (noise_buffer) {
            noise_buffer[i] = noise;
        }
    }
}

int test_encode(OpusEncoder *enc, int channels, int frame_size, int duration, OpusDecoder *dec)
{
   int samp_count = 0;
   opus_int16 *inbuf;
   unsigned char packet[4000];
   int len;
   opus_int16 *outbuf;
   int out_samples;
   int ret = 0;

   //frame_size_samples = x

   /* Generate input data */
   inbuf = (opus_int16*)malloc(sizeof(*inbuf)*4096);

   /* Allocate memory for output data */
   outbuf = (opus_int16*)malloc(sizeof(*outbuf)*4096);

   do {
      len = opus_encode(enc, &inbuf[samp_count*channels], frame_size, packet, MAX_PACKET);
      if(len<0 || len>MAX_PACKET) {
         fprintf(stderr,"opus_encode() returned %d\n",len);
         ret = -1;
         break;
      }

      out_samples = opus_decode(dec, packet, len, outbuf, MAX_FRAME_SAMP, 0);
      if(out_samples!=frame_size) {
         fprintf(stderr,"opus_decode() returned %d\n",out_samples);
         ret = -1;
         break;
      }

      samp_count += frame_size;
   } while (samp_count < ((SSAMPLES/2)-MAX_FRAME_SAMP));

  free(inbuf);
  free(outbuf);
  return ret;
}

void test_encode_decode() {
    const int bitrate[] = {6000}; /* bps */
    const size_t bitrate_len = sizeof(bitrate)/sizeof(bitrate[0]);
    int bitrate_index;

    const int fs[] = {48000}; /* hz*/
    const size_t fs_len = sizeof(fs)/sizeof(fs[0]);
    int fs_index;

    const int frame_size[] = {10}; /* ms */
    const size_t frame_size_len = sizeof(frame_size)/sizeof(frame_size[0]);
    int frame_size_index = 0;

    for(bitrate_index = 0; bitrate_index < bitrate_len; bitrate_index++) {
        for(fs_index = 0; fs_index < fs_len; fs_index++) {
            for(frame_size_index = 0; frame_size_index < frame_size_len; frame_size_index++) {
                
            }
        }
    }

}

int main(int _argc, char **_argv)
{
    (void)_argc;
    (void)_argv;
    test_encode_decode();
    fprintf(stderr, "All encode decode tests passed.\n");
    return 0;
}
