OUTFILE = exampleIMU
SRC_FILES = example.c timing.c imu.c imu_utils.c imu_setup.c
OBJ_FILES = $(SRC_FILES: %.c=%.o)
OUT_DIR = $(HOME)/cmpt433/public/myApps/
CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
C_FLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror

all: main

main:
	$(CC_C) $(CFLAGS) -pthread $(OBJ_FILES) -o $(OUT_DIR)/$(OUTFILE) -lm
	
%.o: %.c
	$(CC_C) $(C_FLAGS) -c $< -o $@

clean:
	rm -f $(OUT_DIR)/$(OUTFILE)