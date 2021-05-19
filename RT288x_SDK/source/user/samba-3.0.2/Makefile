all:
	$(MAKE) -C source

romfs: all

	$(ROMFSINST) source/bin/CP850.so /lib/CP850.so
	$(ROMFSINST) source/bin/CP437.so /lib/CP437.so
	$(ROMFSINST) source/bin/libbigballofmud.so /lib/libbigballofmud.so
	ln -snf libbigballofmud.so $(ROOTDIR)/romfs/lib/libbigballofmud.so.0
	$(ROMFSINST) source/bin/nmbd /sbin/nmbd
	$(ROMFSINST) -e CONFIG_USER_SAMBA_ALL source/bin/smbd /sbin/smbd
	$(ROMFSINST) -e CONFIG_USER_SAMBA_ALL source/bin/smbpasswd /sbin/smbpasswd
	$(ROMFSINST) -e CONFIG_USER_SAMBA_ALL /bin/samba.sh
	$(ROMFSINST) -e CONFIG_USER_SAMBA_ALL /bin/samba_add_dir.sh

clean:
	$(MAKE) -C source clean
