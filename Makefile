APPLICATION = test_bme68x
BOARD ?= lora3a-h10
RIOTBASE ?= $(CURDIR)/../RIOT
LORA3ABASE ?= $(CURDIR)/../lora3a-boards
EXTERNAL_BOARD_DIRS=$(LORA3ABASE)/boards
EXTERNAL_MODULE_DIRS=$(LORA3ABASE)/modules
EXTERNAL_PKG_DIRS=$(LORA3ABASE)/pkg
QUIET ?= 1
DEVELHELP ?= 1
PORT ?= /dev/ttyUSB0

USEMODULE += printf_float
USEMODULE += ztimer_usec
USEMODULE += ztimer_msec
USEMODULE += saul_default

CFLAGS += -DENABLE_ACME1=1

include $(RIOTBASE)/Makefile.include
