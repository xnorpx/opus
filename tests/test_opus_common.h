/* Copyright (c) 2011 Xiph.Org Foundation
   Written by Gregory Maxwell */
/*
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef TEST_OPUS_COMMON_H
#define TEST_OPUS_COMMON_H

#include "opus_defines.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static OPUS_INLINE void deb2_impl(unsigned char *_t,unsigned char **_p,int _k,int _x,int _y)
{
  int i;
  if(_x>2){
     if(_y<3)for(i=0;i<_y;i++)*(--*_p)=_t[i+1];
  }else{
     _t[_x]=_t[_x-_y];
     deb2_impl(_t,_p,_k,_x+1,_y);
     for(i=_t[_x-_y]+1;i<_k;i++){
       _t[_x]=i;
       deb2_impl(_t,_p,_k,_x+1,_x);
     }
  }
}

/*Generates a De Bruijn sequence (k,2) with length k^2*/
static OPUS_INLINE void debruijn2(int _k, unsigned char *_res)
{
   unsigned char *p;
   unsigned char *t;
   t=malloc(sizeof(unsigned char)*_k*2);
   memset(t,0,sizeof(unsigned char)*_k*2);
   p=&_res[_k*_k];
   deb2_impl(t,&p,_k,1,1);
   free(t);
}

/*MWC RNG of George Marsaglia*/
static opus_uint32 Rz, Rw;
static OPUS_INLINE opus_uint32 fast_rand(void)
{
  Rz=36969*(Rz&65535)+(Rz>>16);
  Rw=18000*(Rw&65535)+(Rw>>16);
  return (Rz<<16)+Rw;
}
static opus_uint32 iseed;

#ifdef __GNUC__
__attribute__((noreturn))
#elif defined(_MSC_VER)
__declspec(noreturn)
#endif
static OPUS_INLINE void _test_failed(const char *file, int line)
{
  fprintf(stderr,"\n ***************************************************\n");
  fprintf(stderr," ***         A fatal error was detected.         ***\n");
  fprintf(stderr," ***************************************************\n");
  fprintf(stderr,"Please report this failure and include\n");
  fprintf(stderr,"'make check SEED=%u fails %s at line %d for %s'\n",iseed,file,line,opus_get_version_string());
  fprintf(stderr,"and any relevant details about your system.\n\n");
#if defined(_MSC_VER)
   _set_abort_behavior( 0, _WRITE_ABORT_MSG);
#endif
  abort();
}
#define test_failed() _test_failed(__FILE__, __LINE__);

void regression_test(void);

static void generate_music(short *buf, opus_int32 len)
{
   opus_int32 a1,b1,a2,b2;
   opus_int32 c1,c2,d1,d2;
   opus_int32 i,j;
   a1=b1=a2=b2=0;
   c1=c2=d1=d2=0;
   j=0;
   /*60ms silence*/
   for(i=0;i<2880;i++)buf[i*2]=buf[i*2+1]=0;
   for(i=2880;i<len;i++)
   {
     opus_uint32 r;
     opus_int32 v1,v2;
     v1=v2=(((j*((j>>12)^((j>>10|j>>12)&26&j>>7)))&128)+128)<<15;
     r=fast_rand();v1+=r&65535;v1-=r>>16;
     r=fast_rand();v2+=r&65535;v2-=r>>16;
     b1=v1-a1+((b1*61+32)>>6);a1=v1;
     b2=v2-a2+((b2*61+32)>>6);a2=v2;
     c1=(30*(c1+b1+d1)+32)>>6;d1=b1;
     c2=(30*(c2+b2+d2)+32)>>6;d2=b2;
     v1=(c1+128)>>8;
     v2=(c2+128)>>8;
     buf[i*2]=v1>32767?32767:(v1<-32768?-32768:v1);
     buf[i*2+1]=v2>32767?32767:(v2<-32768?-32768:v2);
     if(i%6==0)j++;
   }
}

static int get_frame_size_enum(int frame_size, int sampling_rate)
{
   int frame_size_enum;

   if(frame_size==sampling_rate/400)
      frame_size_enum = OPUS_FRAMESIZE_2_5_MS;
   else if(frame_size==sampling_rate/200)
      frame_size_enum = OPUS_FRAMESIZE_5_MS;
   else if(frame_size==sampling_rate/100)
      frame_size_enum = OPUS_FRAMESIZE_10_MS;
   else if(frame_size==sampling_rate/50)
      frame_size_enum = OPUS_FRAMESIZE_20_MS;
   else if(frame_size==sampling_rate/25)
      frame_size_enum = OPUS_FRAMESIZE_40_MS;
   else if(frame_size==3*sampling_rate/50)
      frame_size_enum = OPUS_FRAMESIZE_60_MS;
   else if(frame_size==4*sampling_rate/50)
      frame_size_enum = OPUS_FRAMESIZE_80_MS;
   else if(frame_size==5*sampling_rate/50)
      frame_size_enum = OPUS_FRAMESIZE_100_MS;
   else if(frame_size==6*sampling_rate/50)
      frame_size_enum = OPUS_FRAMESIZE_120_MS;
   else
      test_failed();

   return frame_size_enum;
}

#endif
