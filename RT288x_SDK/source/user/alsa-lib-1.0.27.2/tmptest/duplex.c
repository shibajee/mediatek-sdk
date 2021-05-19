/*
 * dulplex.c
 *
 *  Created on: 2014/8/8
 *      Author: mtk04880
 */
#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>

#define DULPLEX_ENABLE 1
#define I2S_PAGE_SIZE		(3*4096)//(1152*2*2*2)
#define PCM_NAME	"plughw:0,0"
#define DEFAULT_SAMP_RATE (44100)
#define DBG_PRINT(fmt, args...) \
	printf("%s[%d]:",__func__,__LINE__);\
	printf(fmt, ## args);\
	fflush(stdout)


snd_pcm_t* playback_init(void){
	snd_pcm_hw_params_t *hw_params;
	snd_pcm_t *handle = NULL;
	int err;
	int rate = DEFAULT_SAMP_RATE;
	if ((err = snd_pcm_open (&handle, PCM_NAME, SND_PCM_STREAM_PLAYBACK,0)) < 0) {
			fprintf (stdout, "cannot open audio device %s (%s)\n", PCM_NAME,snd_strerror (err));
			return NULL;
	}
	if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
		fprintf (stdout, "cannot allocate hardware parameter structure (%s)\n",
			 snd_strerror (err));
		return NULL;
	}
	if ((err = snd_pcm_hw_params_any (handle, hw_params)) < 0) {
		fprintf (stdout, "cannot initialize hardware parameter structure (%s)\n",
			 snd_strerror (err));
		return NULL;
	}
	if ((err = snd_pcm_hw_params_set_access (handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		fprintf (stdout, "cannot set access type (%s)\n",
			 snd_strerror (err));
		return NULL;
	}
	if ((err = snd_pcm_hw_params_set_format (handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
		fprintf (stdout, "cannot set sample format (%s)\n",
			 snd_strerror (err));
		return NULL;
	}
	if ((err = snd_pcm_hw_params_set_rate_near (handle, hw_params, &rate, 0)) < 0) {
		fprintf (stdout, "cannot set sample rate (%s)\n",
			 snd_strerror (err));
		return NULL;
	}
	if ((err = snd_pcm_hw_params_set_channels (handle, hw_params, 1)) < 0) {
		fprintf (stdout, "cannot set channel count (%s)\n",
			 snd_strerror (err));
		return NULL;
	}
	if ((err = snd_pcm_hw_params (handle, hw_params)) < 0) {
		fprintf (stdout, "cannot set parameters (%s)\n",
			 snd_strerror (err));
		return NULL;
	}
	snd_pcm_hw_params_free (hw_params);
#if 1
	if ((err = snd_pcm_prepare (handle)) < 0) {
		fprintf (stdout, "cannot prepare audio interface for use (%s)\n",
			 snd_strerror (err));
		return NULL;
	}
#endif
	return handle;
}

#ifdef DULPLEX_ENABLE
snd_pcm_t* capture_init(void){
	snd_pcm_t *handle = NULL;
	snd_pcm_hw_params_t *hw_params;

	int err;
	int rate = DEFAULT_SAMP_RATE;

	if ((err = snd_pcm_open (&handle, PCM_NAME, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
		fprintf (stderr, "cannot open audio device %s (%s)\n",
				PCM_NAME,
			 snd_strerror (err));
		return NULL;
	}

	if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
		fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n",
			 snd_strerror (err));
		return NULL;
	}

	if ((err = snd_pcm_hw_params_any (handle, hw_params)) < 0) {
		fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",
			 snd_strerror (err));
		return NULL;
	}
	if ((err = snd_pcm_hw_params_set_access (handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		fprintf (stderr, "cannot set access type (%s)\n",
			 snd_strerror (err));
		return NULL;
	}
	if ((err = snd_pcm_hw_params_set_format (handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
		fprintf (stderr, "cannot set sample format (%s)\n",
			 snd_strerror (err));
		return NULL;
	}
	if ((err = snd_pcm_hw_params_set_rate_near (handle, hw_params, &rate, 0)) < 0) {
		fprintf (stderr, "cannot set sample rate (%s)\n",
			 snd_strerror (err));
		return NULL;
	}
	if ((err = snd_pcm_hw_params_set_channels (handle, hw_params, 1)) < 0) {
		fprintf (stderr, "cannot set channel count (%s)\n",
			 snd_strerror (err));
		return NULL;
	}
#if 1
	if ((err = snd_pcm_hw_params (handle, hw_params)) < 0) {
		fprintf (stderr, "cannot set parameters (%s)\n",
			 snd_strerror (err));
		return NULL;
	}
	snd_pcm_hw_params_free (hw_params);


	if ((err = snd_pcm_prepare (handle)) < 0) {
		fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
			 snd_strerror (err));
		return NULL;
	}
#endif
	return handle;
}
#endif

int main (int argc, char *argv[])
{
	int i;
	int err;
	int rate = 44100;
	short buf[(I2S_PAGE_SIZE/2)];
	FILE *pb_filp,*cp_filp;
	snd_pcm_t *playback_handle,*capture_handle;
	snd_pcm_hw_params_t *hw_params;
	if(argc > 1){
		pb_filp = fopen(argv[1],"r");
		if(!pb_filp){
			fprintf (stdout, "cannot allocate hardware parameter structure \n");
			exit (1);
		}
	}
	else{
		fprintf (stdout, "no params\n");
		exit(1);
	}
	DBG_PRINT("<<<<>>>> 3\n");
#ifdef DULPLEX_ENABLE
	if(argc > 2){
		cp_filp = fopen(argv[2],"wb+");
		if(!cp_filp){
			fprintf (stderr, "file open fail\n");
			exit (1);
		}
	}
#endif

	if(!(playback_handle = playback_init())){
		fprintf (stdout, "playback init fail\n");
		exit(1);
	}
#ifdef DULPLEX_ENABLE
	if(!(capture_handle = capture_init())){
		fprintf (stdout, "cap init fail\n");
		exit(1);
	}
#endif


	while(!feof(pb_filp)){
		err = fread(buf,1,sizeof(buf),pb_filp);
		if(err != sizeof(buf)){
			if(err == 0){
				DBG_PRINT("file eof \n");
				exit(1);
			}
			else{
				fprintf (stdout, "cannot set parameters\n");
				exit(1);
			}
		}
		if ((err = snd_pcm_writei (playback_handle, buf, 6144)) < 0) {
			//fprintf (stdout, "write to audio interface failed (%s) - err:%d\n",
			//	 snd_strerror (err),err);
			DBG_PRINT("write negative :%d\n",err);
			break;
		}
#if 1
#ifdef DULPLEX_ENABLE
		memset(buf,0,sizeof(buf));
		if ((err = snd_pcm_readi (capture_handle, buf,6144)) < 0) {
			DBG_PRINT("read negative :%d\n",err);
			//fprintf (stderr, "read from audio interface failed (%s)\n",
			//	 snd_strerror (err));
			break;
		}
		fwrite(&buf,sizeof(buf),1,cp_filp);
#endif
#endif
	}

	if(pb_filp)
		fclose(pb_filp);
	if(playback_handle)
		snd_pcm_close (playback_handle);
#ifdef DULPLEX_ENABLE
	if(cp_filp)
		fclose(cp_filp);
	snd_pcm_close (capture_handle);
#endif
	return 0;
}
