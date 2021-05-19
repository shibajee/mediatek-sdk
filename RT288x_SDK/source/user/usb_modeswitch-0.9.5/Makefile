EXEC = usb_modeswitch

LIBDIR = $(ROOTDIR)/lib/libusb-0.1.12
LDFLAGS += -L$(LIBDIR)
CFLAGS += -I$(LIBDIR)
LDLIBS += -lusb
OBJS = usb_modeswitch.o

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

romfs:
	$(ROMFSINST) /bin/$(EXEC)
	$(ROMFSINST) conf /etc_ro/usb

clean:
	-rm -f $(EXEC) *.o

