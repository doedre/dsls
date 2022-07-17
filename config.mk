SHELL := /bin/sh

NAME := dsls
PREFIX := /usr/local

INCS := -I/usr/X11R6/include
LIBS := -L/usr/X11R6/lib -lX11

CC := cc
CFLAGS := -Wall -Wextra -pedantic -std=c99 -O2 $(INCS)
LDFLAGS := $(LIBS)

OBJ_DIR := .obj
SRC_DIR := .

SRC := main.c status.c util.c
OBJ := $(SRC:.c=.o)

