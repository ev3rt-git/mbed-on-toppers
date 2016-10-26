#
# Makefile to build mbed library
#

DIR=.
DUMMY=$(shell )
OBJDIR=build

# Tools
GCC_TARGET = arm-none-eabi
AR = $(GCC_TARGET)-ar
CC = $(GCC_TARGET)-gcc -nostdlib -std=gnu99 -fno-strict-aliasing -mcpu=arm926ej-s -mlittle-endian
CXX = $(GCC_TARGET)-g++ -nostdlib -std=gnu++11 -fno-strict-aliasing -mcpu=arm926ej-s -mlittle-endian -fpermissive

LIBRARY_FILE=libmbed.a

ALL_OBJS +=

# FIXME: dummy
CFLAGS += -I${DIR}/dummy
ALL_CXX_OBJS += dummy/dummy.o
ALL_OBJS += dummy/syscall.o

# mbed-os
CFLAGS += -I${DIR}/mbed-os \
		  -I${DIR}/mbed-os/platform \
		  -I${DIR}/mbed-os/rtos \
		  -I${DIR}/mbed-os/drivers
ALL_CXX_OBJS += mbed-os/platform/retarget.o \
				mbed-os/drivers/FileBase.o \
				mbed-os/drivers/FileLike.o \
				mbed-os/drivers/FilePath.o \
				mbed-os/drivers/FileSystemLike.o \
				mbed-os/drivers/Stream.o \
				mbed-os/rtos/Mutex.o \
				mbed-os/rtos/Semaphore.o
#ALL_OBJS += mbed-os/platform/mbed_error.o

# USBHost
USBHOSTDIR = mbed-os/features/unsupported/USBHost
CFLAGS += -I${USBHOSTDIR}/USBHost \
		  -I${USBHOSTDIR}/USBHostHub
ALL_CXX_OBJS += ${USBHOSTDIR}/USBHostSerial/USBHostSerial.o \
				${USBHOSTDIR}/USBHost/USBHost.o \
				${USBHOSTDIR}/USBHost/USBEndpoint.o \
				${USBHOSTDIR}/USBHost/USBDeviceConnected.o \
				${USBHOSTDIR}/USBHostHub/USBHostHub.o 

#
# Include header and source files
#
vpath %.c $(SRCDIRS)

#BLE_FILES  =  ad_parser.c att.c att_server.c att_dispatch.c att_db_util.c le_device_db_memory.c gatt_client.c
#BLE_FILES  += sm.c l2cap_le.c ancs_client_lib.h ancs_client_lib.c

all: prepare-obj-folder $(LIBRARY_FILE)

clean:
	rm -f ${ALL_OBJS} dummy.o

realclean: clean
	rm -f ${LIBRARY_FILE}

dummy: dummy.o $(ALL_OBJS)
	$(CC) -o dummy dummy.o $(ALL_OBJS)

define print_cmd
  @printf "  %-7s %s\n" $1 $2 1>&2
endef

$(LIBRARY_FILE): $(ALL_OBJS) $(ALL_CXX_OBJS)
	$(call print_cmd, "AR", $@)
	@cd $(OBJDIR) && \
	$(AR) -rcs $@ $(ALL_OBJS) $(ALL_CXX_OBJS)
	@#arm-none-eabi-g++  $(ALL_OBJS) $(ALL_CXX_OBJS)

$(ALL_OBJS): %.o: %.c
	$(call print_cmd, "CC", $<)
	@mkdir -p $(shell dirname $(OBJDIR)/$@)
	@$(CC) -c $(CFLAGS) -c $< -o $(OBJDIR)/$@

$(ALL_CXX_OBJS): %.o: %.cpp
	$(call print_cmd, "CXX", $<)
	@mkdir -p $(shell dirname $(OBJDIR)/$@)
	@$(CXX) -c $(CFLAGS) -c $< -o $(OBJDIR)/$@

prepare-obj-folder:
	@mkdir -p $(OBJDIR)

.PHONY: prepare-obj-folder
