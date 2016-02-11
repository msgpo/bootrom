##
 # Copyright (c) 2015 Google Inc.
 # All rights reserved.
 #
 # Redistribution and use in source and binary forms, with or without
 # modification, are permitted provided that the following conditions are met:
 # 1. Redistributions of source code must retain the above copyright notice,
 # this list of conditions and the following disclaimer.
 # 2. Redistributions in binary form must reproduce the above copyright notice,
 # this list of conditions and the following disclaimer in the documentation
 # and/or other materials provided with the distribution.
 # 3. Neither the name of the copyright holder nor the names of its
 # contributors may be used to endorse or promote products derived from this
 # software without specific prior written permission.
 #
 # THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 # AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 # THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 # PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 # CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 # EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 # PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 # OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 # WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 # OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 # ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ##

#
# Not many source files really expected in the bootrom project.
# So let's keep it straightforward, instead of using complicated scripts
#

include $(CHIP_DIR)/Sources.mk

ifdef CONFIG_ARCH_EXTRA
  include $(ARCH_EXTRA_DIR)/Sources.mk
else
  ARCH_EXTRA_SRCDIR =
  ARCH_EXTRA_CSRC =
  ARCH_EXTRA_ASRC =
endif

MANIFEST = IID1-simple-bootrom-mnfs
MANIFEST_SRCDIR = manifest

ifeq ($(PUBLIC_KEYS_FILE), )
	PUBLIC_KEYS_FILE = $(MANIFEST_SRCDIR)/public_keys.c
endif

CMN_SRCDIR := common/src
CMN_INCFLAGS := -I$(TOPDIR)/common/include -I$(TOPDIR)/common/shared_inc

ifneq ($(BOOT_STAGE), 3)
# the error reporting over DME is for boot ROM and second stage only
# So exclude it from third stage FW build. Also exclude TFTF/FFFF code
# since they are not used by 3rd stage FW, while they rely on the error
# reporting code
CMN_CSRC += $(CMN_SRCDIR)/tftf.c
CMN_CSRC += $(CMN_SRCDIR)/ffff.c
CMN_CSRC += $(CMN_SRCDIR)/error.c
endif

CMN_CSRC += $(CMN_SRCDIR)/crypto.c
CMN_CSRC += $(CMN_SRCDIR)/epuid.c
ifeq ($(BOOT_STAGE), 2)
CMN_CSRC += $(CMN_SRCDIR)/secret_keys.c
endif
CMN_CSRC += $(CMN_SRCDIR)/utils.c
CMN_CSRC += $(CMN_SRCDIR)/ara_mailbox.c
CMN_CSRC += $(CMN_SRCDIR)/gbcore.c
CMN_CSRC += $(CMN_SRCDIR)/debug.c
CMN_CSRC += $(CMN_SRCDIR)/gbboot.c

ifeq ($(APP_CONFIG_BRIDGED_SPI),y)
CFLAGS += -DCONFIG_BRIDGED_SPI
CMN_CSRC += $(CMN_SRCDIR)/spi-gb.c
endif

CMN_ASRC =

include $(TOPDIR)/apps/$(APPLICATION)/Sources.mk

CSRC = $(CHIP_CSRC) $(CMN_CSRC) $(ARCH_EXTRA_CSRC) $(APP_CSRC)
ASRC = $(CHIP_ASRC) $(CMN_ASRC) $(ARCH_EXTRA_ASRC) $(APP_ASRC)

SRCDIRS := $(CHIP_SRCDIR) $(CMN_SRCDIR) $(APP_SRCDIR) $(ARCH_EXTRA_SRCDIR) $(MANIFEST_SRCDIR)

COBJS := $(foreach f, $(CSRC), $(OUTROOT)/$(patsubst %.c,%.o, $(f)))
AOBJS := $(foreach f, $(ASRC), $(OUTROOT)/$(patsubst %.S,%.o, $(f)))

INCLUDES := $(CMN_INCFLAGS) $(CHIPINCLUDES) $(APP_INCLUDES)
