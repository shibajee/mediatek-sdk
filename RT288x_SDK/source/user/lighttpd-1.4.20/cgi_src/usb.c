#include "utils.h"
#include <stdlib.h>

#include "user_conf.h"

#if defined CONFIG_USB && defined CONFIG_USER_UVC_STREAM
void set_uvc(char *input)
{
	char *enable, *resolution, *fps, *port;

	enable = resolution = fps = port = NULL;
	web_debug_header();
	// fetch from web input
	enable = strdup(web_get("enabled", input, 1));
	resolution = strdup(web_get("resolution", input, 1));
	fps = strdup(web_get("fps", input, 1));
	port = strdup(web_get("port", input, 1));

	// set to nvram
	nvram_bufset(RT2860_NVRAM, "WebCamEnabled", enable);
	nvram_bufset(RT2860_NVRAM, "WebCamResolution", resolution);
	nvram_bufset(RT2860_NVRAM, "WebCamFPS", fps);
	nvram_bufset(RT2860_NVRAM, "WebCamPort", port);
	nvram_commit(RT2860_NVRAM);

	// setup device
	do_system("killall uvc_stream");
	if (0 == strcmp(enable, "1"))
		do_system("uvc_stream -r %s -f %s -p %s -b",
				resolution, fps, port);
	free_all(4, enable, resolution, fps, port);
}
#endif

#if defined CONFIG_USB && defined CONFIG_USER_P910ND
void set_printer(char *input)
{
	char *enable = NULL;

	web_debug_header();
	// fetch from web input
	enable = strdup(web_get("enabled", input, 1));
	// set to nvram
	nvram_bufset(RT2860_NVRAM, "PrinterSrvEnabled", enable);
	nvram_commit(RT2860_NVRAM);

	// setup device
	do_system("killall p910nd");
	if (0 == strcmp(enable, "1"))
		do_system("p910nd -b -f /dev/lp0");
	free(enable);
}
#endif

#if defined CONFIG_USB && defined CONFIG_USER_MTDAAPD
void set_itunes(char *input)
{
	char *enable, *dir_path, *name;

	enable = dir_path = name = NULL;
	web_debug_header();
	// fetch from web input
	enable = strdup(web_get("enabled", input, 1));
	dir_path = strdup(web_get("dir_path", input, 1));
	name = strdup(web_get("srv_name", input, 1));
	// set to nvram
	nvram_bufset(RT2860_NVRAM, "iTunesEnable", enable);
	nvram_bufset(RT2860_NVRAM, "iTunesDir", dir_path);
	nvram_bufset(RT2860_NVRAM, "iTunesSrvName", name);
	nvram_commit(RT2860_NVRAM);

	// setup device
	do_system("killall mt-daapd; killall mDNSResponder");
	if (strcmp(enable, "1") == 0)
		do_system("config-iTunes.sh \"%s\" \"%s\" \"%s\"",
			   name,
			   nvram_bufget(RT2860_NVRAM, "Password"),
			   dir_path);
	free_all(3, enable, dir_path, name);
}
#endif

#if defined CONFIG_USER_STORAGE && defined CONFIG_USER_MINIDLNA
void add_media_dir(char *input)
{
	FILE *fp = NULL;
	struct media_config cfg[4];
	int index = 0, empty_cfg = 0;
	char *dir_path = NULL;

	// fetch from web input
	dir_path = strdup(web_get("dir_path", input, 0));

	memset(cfg, 0, sizeof(cfg));
	fp = fetch_media_cfg(cfg, 1);
	if (!fp) {
		DBG_MSG("fail to create write permission in .media_config");
		goto final;
	}

	// DBG_MSG("%s", dir_path);
	while (4 > index)
	{
		if (0 == strcmp(dir_path, cfg[index].path))
		{
			DBG_MSG("Existed Media Shared Dir: %s\n", dir_path);
			goto final;
		} else if (0 == strlen(cfg[index].path)) {
			empty_cfg |= 1<<index;
		}
		index++;
	}
	if (empty_cfg == 0)
	{
		DBG_MSG("Media Server Shared Dirs exceed 4");
	} else {
		//DBG_MSG("empty config: %x", empty_cfg);
		for (index=0; index<4; index++) {
			if (empty_cfg & 1<<index) {
				strcpy(cfg[index].path, dir_path);
				break;
			}
		}
		fwrite(cfg, sizeof(cfg), 1, fp);
	}
	if (fp)
		fclose(fp);
final:
	free(dir_path);
	web_back_parentpage();
}

void del_media_dir(char *input)
{
	struct media_config cfg[4];
	FILE *fp = NULL;
	int index;

	memset(cfg, 0, sizeof(cfg));
	fp = fetch_media_cfg(cfg, 1);
	if (!fp) {
		DBG_MSG("fail to create write permission in .media_config");
		goto final;
	}

	index = strtol(web_get("media_dir", input, 0), NULL, 10);
	memset(&cfg[index].path, 0, sizeof(cfg[index].path));
	fwrite(cfg, sizeof(cfg), 1, fp);
	if (fp)
		fclose(fp);
final:
	web_redirect(getenv("HTTP_REFERER"));
}

static void run_media_server()
{
	int i;
	struct media_config cfg[4];

	memset(cfg, 0, sizeof(cfg));
	fetch_media_cfg(cfg, 0);
	do_system("storage.sh media \"%s\" \"%s\" \"%s\" \"%s\"", 
		  cfg[0].path, cfg[1].path, cfg[2].path, cfg[3].path);
}

void set_media_server(char *input)
{
	char *media_enabled = NULL;
	int i;

	// fetch from web input
	web_debug_header();
	media_enabled = strdup(web_get("media_enabled", input, 1));
	WebTF(input, "media_name", RT2860_NVRAM, "mediaSrvName", 1);

	// set to nvram
	nvram_bufset(RT2860_NVRAM, "mediaSrvEnabled", media_enabled);
	nvram_commit(RT2860_NVRAM);

	if (!strcmp(media_enabled, "1"))
		run_media_server();

	free(media_enabled);
}
#endif

void usb_init(void)
{
	printf("\n##### Hotplug USB device init #####\n");
	sleep(1);           // wait for sub-storage initiation
#if defined CONFIG_USER_STORAGE && defined CONFIG_USER_MINIDLNA
	run_media_server();
#endif
#if defined CONFIG_USB && defined CONFIG_USER_UVC_STREAM
	DBG_MSG("UVC init\n");
	const char *webcamebl = nvram_bufget(RT2860_NVRAM, "WebCamEnabled");
	do_system("killall uvc_stream");
	if (0 == strcmp(webcamebl, "1"))
	{
		DBG_MSG("UVC start\n");
		do_system("uvc_stream -r %s -f %s -p %s -b",
			   nvram_bufget(RT2860_NVRAM, "WebCamResolution"),
			   nvram_bufget(RT2860_NVRAM, "WebCamFPS"),
			   nvram_bufget(RT2860_NVRAM, "WebCamPort"));
	}
#endif
#if defined CONFIG_USB && defined CONFIG_USER_P910ND
	DBG_MSG("P910ND init\n");
	const char *printersrvebl = nvram_bufget(RT2860_NVRAM, "PrinterSrvEnabled");
	do_system("killall p910nd");
	if (0 == strcmp(printersrvebl, "1"))
	{
		DBG_MSG("P910ND start\n");
		do_system("p910nd -b -f /dev/lp0");
	}
#endif
#if defined CONFIG_USB && defined CONFIG_USER_MTDAAPD
	do_system("killall mt-daapd; killall mDNSResponder");
	if (strcmp(nvram_bufget(RT2860_NVRAM, "iTunesEnable"), "1") == 0)
		do_system("config-iTunes.sh \"%s\" \"%s\" \"%s\"",
			   nvram_bufget(RT2860_NVRAM, "iTunesSrvName"),
			   nvram_bufget(RT2860_NVRAM, "Password"),
			   nvram_bufget(RT2860_NVRAM, "iTunesDir"));
#endif
}

int main(int argc, char *argv[]) 
{
	char *form, *inStr;
	long inLen;

	nvram_init(RT2860_NVRAM);
	if ((argc > 1) && (!strcmp(argv[1], "init"))) {
		usb_init();
		goto leave;
	}
	inLen = strtol(getenv("CONTENT_LENGTH"), NULL, 10) + 1;
	if (inLen <= 1) {
		DBG_MSG("get no data!");
		goto leave;
	}
	inStr = malloc(inLen);
	memset(inStr, 0, inLen);
	fgets(inStr, inLen, stdin);
	// DBG_MSG("%s\n", inStr);
	form = web_get("page", inStr, 0);
#if defined CONFIG_USB && defined CONFIG_USER_UVC_STREAM
	if (!strcmp(form, "uvc"))
		set_uvc(inStr);
#endif
#if defined CONFIG_USB && defined CONFIG_USER_P910ND
	if (!strcmp(form, "printer"))
		set_printer(inStr);
#endif
#if defined CONFIG_USB && defined CONFIG_USER_MTDAAPD
	if (!strcmp(form, "itunes"))
		set_itunes(inStr);
#endif
#if defined CONFIG_USER_STORAGE && defined CONFIG_USER_MINIDLNA
	if (!strcmp(form, "mediaAddDir"))
		add_media_dir(inStr);
	else if (!strcmp(form, "mediaDelDir"))
		del_media_dir(inStr);
	else if (!strcmp(form, "media"))
		set_media_server(inStr);
#endif
	free(inStr);
leave:
	nvram_close(RT2860_NVRAM);
	
	return 0;
}
