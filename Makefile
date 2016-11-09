#
# Makefile to build mbed library
#

DIR=.
DUMMY=$(shell )
OBJDIR=build
KERNELDIR=../..

# Tools
GCC_TARGET = arm-none-eabi
AR = $(GCC_TARGET)-ar
CC = $(GCC_TARGET)-gcc -O2 -nostdlib -std=gnu99 -fno-strict-aliasing -mcpu=arm926ej-s -mlittle-endian
CXX = $(GCC_TARGET)-g++ -O2 -nostdlib -std=gnu++11 -fno-strict-aliasing -mcpu=arm926ej-s -mlittle-endian -fpermissive

LIBRARY_FILE=libmbed.a

ALL_OBJS +=

# FIXME: dummy
CFLAGS += -I${DIR}/dummy
ALL_CXX_OBJS += dummy/dummy.o dummy/rt_CMSIS.o dummy/Thread.o
ALL_OBJS += dummy/syscall.o dummy/us_ticker.o
CFLAGS += -I${KERNELDIR}/include

# FIXME: hard-coded files for EV3RT
CFLAGS += -I${KERNELDIR}/target/ev3_gcc \
		  -I${KERNELDIR}/arch \
		  -I${KERNELDIR}/arch/arm_gcc/am1808 \
		  -I${KERNELDIR}/arch/arm_gcc/common \
		  -I${KERNELDIR}/modules/btstack/platforms/ev3rt \
		  -I${DIR}/arch/ev3rt
ALL_OBJS += ${DIR}/arch/ev3rt/btstack_emac.o

# mbed-os
CFLAGS += -I${DIR}/mbed-os \
		  -I${DIR}/mbed-os/hal \
		  -I${DIR}/mbed-os/hal/hal \
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
ALL_OBJS += mbed-os/platform/mbed_error.o \
			mbed-os/platform/mbed_interface.o

# USBHost
USBHOSTDIR = mbed-os/features/unsupported/USBHost
CFLAGS += -I${USBHOSTDIR}/USBHost \
		  -I${USBHOSTDIR}/USBHostHub
ALL_CXX_OBJS += ${USBHOSTDIR}/USBHostSerial/USBHostSerial.o \
				${USBHOSTDIR}/USBHost/USBHost.o \
				${USBHOSTDIR}/USBHost/USBEndpoint.o \
				${USBHOSTDIR}/USBHost/USBDeviceConnected.o \
				${USBHOSTDIR}/USBHostHub/USBHostHub.o 

# EthernetInterface over lwIP (FEATURE_LWIP)
LWIPIFDIR = mbed-os/features/FEATURE_LWIP/lwip-interface
NETSOCKDIR = mbed-os/features/netsocket
LWIPDIR = ${LWIPIFDIR}/lwip/src
CFLAGS += -I${LWIPIFDIR} \
		  -I${LWIPIFDIR}/lwip-sys \
		  -I${LWIPDIR}/include \
		  -Imbed-os/features \
		  -I${NETSOCKDIR}
ALL_OBJS += ${LWIPIFDIR}/lwip_stack.o \
			${LWIPIFDIR}/lwip-sys/arch/lwip_sys_arch.o \
			${LWIPDIR}/api/lwip_api_lib.o \
			${LWIPDIR}/api/lwip_api_msg.o \
			${LWIPDIR}/api/lwip_netbuf.o \
			${LWIPDIR}/api/lwip_tcpip.o \
			${LWIPDIR}/core/ipv4/lwip_dhcp.o \
			${LWIPDIR}/core/ipv4/lwip_etharp.o \
			${LWIPDIR}/core/ipv4/lwip_icmp.o \
			${LWIPDIR}/core/ipv4/lwip_igmp.o \
			${LWIPDIR}/core/ipv4/lwip_ip4.o \
			${LWIPDIR}/core/ipv4/lwip_ip4_addr.o \
			${LWIPDIR}/core/ipv4/lwip_ip4_frag.o \
			${LWIPDIR}/core/lwip_dns.o \
			${LWIPDIR}/core/lwip_inet_chksum.o \
			${LWIPDIR}/core/lwip_init.o \
			${LWIPDIR}/core/lwip_ip.o \
			${LWIPDIR}/core/lwip_mem.o \
			${LWIPDIR}/core/lwip_memp.o \
			${LWIPDIR}/core/lwip_netif.o \
			${LWIPDIR}/core/lwip_pbuf.o \
			${LWIPDIR}/core/lwip_stats.o \
			${LWIPDIR}/core/lwip_tcp.o \
			${LWIPDIR}/core/lwip_tcp_in.o \
			${LWIPDIR}/core/lwip_tcp_out.o \
			${LWIPDIR}/core/lwip_timeouts.o \
			${LWIPDIR}/core/lwip_udp.o \
			${LWIPDIR}/netif/lwip_ethernet.o
ALL_CXX_OBJS += ${LWIPIFDIR}/EthernetInterface.o \
				${NETSOCKDIR}/NetworkInterface.o \
				${NETSOCKDIR}/nsapi_dns.o \
				${NETSOCKDIR}/NetworkStack.o \
				${NETSOCKDIR}/Socket.o \
				${NETSOCKDIR}/SocketAddress.o \
				${NETSOCKDIR}/UDPSocket.o

# DhcpServer
CFLAGS += -Irepos/DhcpServer
ALL_CXX_OBJS += repos/DhcpServer/DhcpServer.o

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
