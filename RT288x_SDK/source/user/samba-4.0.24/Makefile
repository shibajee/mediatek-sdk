all:
	$(MAKE) -C source3


romfs: all
	$(ROMFSINST) source3/bin/libwbclient.so.0 /lib/libwbclient.so.0
	$(ROMFSINST) source3/bin/nmbd /sbin/nmbd
	$(ROMFSINST) -e CONFIG_USER_SAMBA_ALL source3/bin/smbd /sbin/smbd
	$(ROMFSINST) -e CONFIG_USER_SAMBA_ALL source3/bin/smbpasswd /sbin/smbpasswd
	$(ROMFSINST) -e CONFIG_USER_SAMBA_ALL /bin/samba.sh
	$(ROMFSINST) -e CONFIG_USER_SAMBA_ALL /bin/samba_add_dir.sh

clean:
	$(MAKE) -C source3 clean
	rm -f source3/bin/*
