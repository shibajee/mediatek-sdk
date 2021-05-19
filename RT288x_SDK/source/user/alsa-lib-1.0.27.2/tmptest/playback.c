#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
	      
#define I2S_PAGE_SIZE		(3*4096)//(1152*2*2*2)


int main (int argc, char *argv[])
{
	int i;
	int err;
	int rate = 44100;
	short buf[(I2S_PAGE_SIZE/2)];
	FILE * filp;
	snd_pcm_t *playback_handle;
	snd_pcm_hw_params_t *hw_params;

	if ((err = snd_pcm_open (&playback_handle, argv[1], SND_PCM_STREAM_PLAYBACK,0)) < 0) {
			fprintf (stdout, "cannot open audio device %s (%s)\n", argv[1],snd_strerror (err));
			exit (1);
	}

	if(argc > 2){
		filp = fopen(argv[2],"r");
		if(!filp){
			fprintf (stdout, "cannot allocate hardware parameter structure \n");
			exit (1);
		}

	}

	if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
		fprintf (stdout, "cannot allocate hardware parameter structure (%s)\n",
			 snd_strerror (err));
		exit (1);
	}
	if ((err = snd_pcm_hw_params_any (playback_handle, hw_params)) < 0) {
		fprintf (stdout, "cannot initialize hardware parameter structure (%s)\n",
			 snd_strerror (err));
		exit (1);
	}
	if ((err = snd_pcm_hw_params_set_access (playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		fprintf (stdout, "cannot set access type (%s)\n",
			 snd_strerror (err));
		exit (1);
	}

	if ((err = snd_pcm_hw_params_set_format (playback_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
		fprintf (stdout, "cannot set sample format (%s)\n",
			 snd_strerror (err));
		exit (1);
	}

	if ((err = snd_pcm_hw_params_set_rate_near (playback_handle, hw_params, &rate, 0)) < 0) {
		fprintf (stdout, "cannot set sample rate (%s)\n",
			 snd_strerror (err));
		exit (1);
	}

	if ((err = snd_pcm_hw_params_set_channels (playback_handle, hw_params, 1)) < 0) {
		fprintf (stdout, "cannot set channel count (%s)\n",
			 snd_strerror (err));
		exit (1);
	}

	if ((err = snd_pcm_hw_params (playback_handle, hw_params)) < 0) {
		fprintf (stdout, "cannot set parameters (%s)\n",
			 snd_strerror (err));
		exit (1);
	}

	snd_pcm_hw_params_free (hw_params);
	if ((err = snd_pcm_prepare (playback_handle)) < 0) {
		fprintf (stdout, "cannot prepare audio interface for use (%s)\n",
			 snd_strerror (err));
		exit (1);
	}

	//for (i = 0; i < 10; ++i) {
	while(!feof(filp)){
		err = fread(buf,1,sizeof(buf),filp);
		if(err != sizeof(buf)){
			if(err == 0){
				printf("file eof \n");
				exit(1);
			}
			else{
				fprintf (stdout, "cannot set parameters\n");
				exit(1);
			}
		}
		if ((err = snd_pcm_writei (playback_handle, buf, 6144)) != 6144) {
			fprintf (stdout, "write to audio interface failed (%s) - err:%d\n",
				 snd_strerror (err),err);
			exit (1);
		}
	}
	fclose(filp);
	snd_pcm_close (playback_handle);
	return 0;
}
