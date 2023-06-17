SDK_ROOT ?=
GNU_INSTALL_ROOT ?=

PREFIX := "$(GNU_INSTALL_ROOT)"arm-none-eabi-
PRJ_ROOT := .
OUTPUT_DIR := _build

ifndef SDK_ROOT
  $(error SDK_ROOT is not set)
endif

INC_FOLDERS += \
  $(PRJ_ROOT)/include

SRC_FILES = \
  $(PRJ_ROOT)/src/main.c \
  $(PRJ_ROOT)/src/max2769.c \
  $(PRJ_ROOT)/src/spis.c \
  $(PRJ_ROOT)/src/snapshot_transmitter.c \
  $(PRJ_ROOT)/src/timestamping.c 

include $(SDK_ROOT)/Makefile