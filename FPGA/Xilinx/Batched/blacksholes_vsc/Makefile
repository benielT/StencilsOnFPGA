
# Accelerator source files (ie. Vitis HLS code, or vpp_acc code)
# must be specified here; they will be processed by v++ --compile

ACC_SRCS := src/blacksholes_acc_hls.cpp src/blacksholes_acc_wrap.cpp


# Host source files are defined here:
HOST_SRCS := src/blacksholes_app.cpp src/blacksholes_cpu.cpp # src/stencil.cpp

### Additional compier optiones added here

#need to add for multiple tragets
# ifeq ($(findstring u50, $(DEVICE)), u50)
#   EXTRA_CFLAGS := -I include/u280
# else ifeq ($(findstring u280, $(DEVICE)), u280)
#   EXTRA_CFLAGS := -I include/u280
# else
#   EXTRA_CFLAGS := -I include/u200
# endif

EXTRA_CFLAGS := -I ./include/u280/

EXTRA_CFLAGS += -I ./src/
EXTRA_VPPFLAGS = -g

# HW profiling (set the flag used in profile.hpp)
ifeq (hw,$(TARGET))
  EXTRA_VPPFLAGS += -D ENABLE_HW_PROFILING=1
endif

include $(XILINX_VITIS)/system_compiler/examples/vpp_sc.mk

test: run