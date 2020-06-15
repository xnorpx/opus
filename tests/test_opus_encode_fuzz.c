
#include "test_opus_common.h"
#include "arch.h"
#include "opus.h"
#include <stdlib.h>
#include <stdio.h>

#define MAX_PACKET (1500)
#define SAMPLES (48000*30)
#define SSAMPLES (SAMPLES/3)
#define MAX_FRAME_SAMP (5760)
#define PI (3.141592653589793238462643f)
#define RAND_SAMPLE(a) (a[fast_rand() % sizeof(a)/sizeof(a[0])])

int test_encode(OpusEncoder *enc, int channels, int frame_size, OpusDecoder *dec)
{
   int samp_count = 0;
   opus_int16 *inbuf;
   unsigned char packet[MAX_PACKET+257];
   int len;
   opus_int16 *outbuf;
   int out_samples;
   int ret = 0;

   /* Generate input data */
   inbuf = (opus_int16*)malloc(sizeof(*inbuf)*SSAMPLES);
   generate_music(inbuf, SSAMPLES/2);

   /* Allocate memory for output data */
   outbuf = (opus_int16*)malloc(sizeof(*outbuf)*MAX_FRAME_SAMP*3);

   /* Encode data, then decode for sanity check */
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

   /* Clean up */
   free(inbuf);
   free(outbuf);
   return ret;
}

void fuzz_encoder_settings(const int num_encoders, const int num_setting_changes)
{
   OpusEncoder *enc;
   OpusDecoder *dec;
   int i,j,err;

   /* Parameters to fuzz. Some values are duplicated to increase their probability of being tested. */
   int sampling_rates[5] = {8000, 12000, 16000, 24000, 48000};
   int channels[2] = {1, 2};
   int applications[3] = {OPUS_APPLICATION_AUDIO, OPUS_APPLICATION_VOIP, OPUS_APPLICATION_RESTRICTED_LOWDELAY};
   int bitrates[11] = {6000, 12000, 16000, 24000, 32000, 48000, 64000, 96000, 510000, OPUS_AUTO, OPUS_BITRATE_MAX};
   int force_channels[4] = {OPUS_AUTO, OPUS_AUTO, 1, 2};
   int use_vbr[3] = {0, 1, 1};
   int vbr_constraints[3] = {0, 1, 1};
   int complexities[11] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
   int max_bandwidths[6] = {OPUS_BANDWIDTH_NARROWBAND, OPUS_BANDWIDTH_MEDIUMBAND,
                            OPUS_BANDWIDTH_WIDEBAND, OPUS_BANDWIDTH_SUPERWIDEBAND,
                            OPUS_BANDWIDTH_FULLBAND, OPUS_BANDWIDTH_FULLBAND};
   int signals[4] = {OPUS_AUTO, OPUS_AUTO, OPUS_SIGNAL_VOICE, OPUS_SIGNAL_MUSIC};
   int inband_fecs[3] = {0, 0, 1};
   int packet_loss_perc[4] = {0, 1, 2, 5};
   int lsb_depths[2] = {8, 24};
   int prediction_disabled[3] = {0, 0, 1};
   int use_dtx[2] = {0, 1};
   int frame_sizes_ms_x2[9] = {5, 10, 20, 40, 80, 120, 160, 200, 240};  /* x2 to avoid 2.5 ms */

   for (i=0; i<num_encoders; i++) {
      int sampling_rate = RAND_SAMPLE(sampling_rates);
      int num_channels = RAND_SAMPLE(channels);
      int application = RAND_SAMPLE(applications);

      dec = opus_decoder_create(sampling_rate, num_channels, &err);
      if(err!=OPUS_OK || dec==NULL)test_failed();

      enc = opus_encoder_create(sampling_rate, num_channels, application, &err);
      if(err!=OPUS_OK || enc==NULL)test_failed();

      for (j=0; j<num_setting_changes; j++) {
         int bitrate = RAND_SAMPLE(bitrates);
         int force_channel = RAND_SAMPLE(force_channels);
         int vbr = RAND_SAMPLE(use_vbr);
         int vbr_constraint = RAND_SAMPLE(vbr_constraints);
         int complexity = RAND_SAMPLE(complexities);
         int max_bw = RAND_SAMPLE(max_bandwidths);
         int sig = RAND_SAMPLE(signals);
         int inband_fec = RAND_SAMPLE(inband_fecs);
         int pkt_loss = RAND_SAMPLE(packet_loss_perc);
         int lsb_depth = RAND_SAMPLE(lsb_depths);
         int pred_disabled = RAND_SAMPLE(prediction_disabled);
         int dtx = RAND_SAMPLE(use_dtx);
         int frame_size_ms_x2 = RAND_SAMPLE(frame_sizes_ms_x2);
         int frame_size = frame_size_ms_x2*sampling_rate/2000;
         int frame_size_enum = get_frame_size_enum(frame_size, sampling_rate);
         force_channel = IMIN(force_channel, num_channels);

         if(opus_encoder_ctl(enc, OPUS_SET_BITRATE(bitrate)) != OPUS_OK) test_failed();
         if(opus_encoder_ctl(enc, OPUS_SET_FORCE_CHANNELS(force_channel)) != OPUS_OK) test_failed();
         if(opus_encoder_ctl(enc, OPUS_SET_VBR(vbr)) != OPUS_OK) test_failed();
         if(opus_encoder_ctl(enc, OPUS_SET_VBR_CONSTRAINT(vbr_constraint)) != OPUS_OK) test_failed();
         if(opus_encoder_ctl(enc, OPUS_SET_COMPLEXITY(complexity)) != OPUS_OK) test_failed();
         if(opus_encoder_ctl(enc, OPUS_SET_MAX_BANDWIDTH(max_bw)) != OPUS_OK) test_failed();
         if(opus_encoder_ctl(enc, OPUS_SET_SIGNAL(sig)) != OPUS_OK) test_failed();
         if(opus_encoder_ctl(enc, OPUS_SET_INBAND_FEC(inband_fec)) != OPUS_OK) test_failed();
         if(opus_encoder_ctl(enc, OPUS_SET_PACKET_LOSS_PERC(pkt_loss)) != OPUS_OK) test_failed();
         if(opus_encoder_ctl(enc, OPUS_SET_LSB_DEPTH(lsb_depth)) != OPUS_OK) test_failed();
         if(opus_encoder_ctl(enc, OPUS_SET_PREDICTION_DISABLED(pred_disabled)) != OPUS_OK) test_failed();
         if(opus_encoder_ctl(enc, OPUS_SET_DTX(dtx)) != OPUS_OK) test_failed();
         if(opus_encoder_ctl(enc, OPUS_SET_EXPERT_FRAME_DURATION(frame_size_enum)) != OPUS_OK) test_failed();

         if(test_encode(enc, num_channels, frame_size, dec)) {
            fprintf(stderr,
               "fuzz_encoder_settings: %d kHz, %d ch, application: %d, "
               "%d bps, force ch: %d, vbr: %d, vbr constraint: %d, complexity: %d, "
               "max bw: %d, signal: %d, inband fec: %d, pkt loss: %d%%, lsb depth: %d, "
               "pred disabled: %d, dtx: %d, (%d/2) ms\n",
               sampling_rate/1000, num_channels, application, bitrate,
               force_channel, vbr, vbr_constraint, complexity, max_bw, sig, inband_fec,
               pkt_loss, lsb_depth, pred_disabled, dtx, frame_size_ms_x2);
            test_failed();
         }
      }

      opus_encoder_destroy(enc);
      opus_decoder_destroy(dec);
   }
}

void print_usage(char* _argv[])
{
   fprintf(stderr,"Usage: %s [<seed>] [-fuzz <num_encoders> <num_settings_per_encoder>]\n",_argv[0]);
}

int main(int _argc, char **_argv)
{
   int args=1;
   char * strtol_str=NULL;
   int num_encoders_to_fuzz=5;
   int num_setting_changes=40;

   while(args<_argc)
   {
      if(strcmp(_argv[args], "-fuzz")==0 && _argc==(args+3)) {
         num_encoders_to_fuzz=strtol(_argv[args+1], &strtol_str, 10);
         if(strtol_str[0]!='\0' || num_encoders_to_fuzz<=0) {
            print_usage(_argv);
            return EXIT_FAILURE;
         }
         num_setting_changes=strtol(_argv[args+2], &strtol_str, 10);
         if(strtol_str[0]!='\0' || num_setting_changes<=0) {
            print_usage(_argv);
            return EXIT_FAILURE;
         }
         args+=3;
      }
      else {
        print_usage(_argv);
        return EXIT_FAILURE;
      }
   }

   /* Fuzz encoder settings online */
   if(getenv("TEST_OPUS_NOFUZZ")==NULL) {
      fprintf(stderr,"Running fuzz_encoder_settings with %d encoder(s) and %d setting change(s) each.\n",
              num_encoders_to_fuzz, num_setting_changes);
      fuzz_encoder_settings(num_encoders_to_fuzz, num_setting_changes);
   }

   fprintf(stderr,"Tests completed successfully.\n");

   return 0;
}
