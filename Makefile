# Makefile template for shared library
 
CC = gcc # C compiler
CFLAGS = -Wall -Wextra -g -O0 # C flags
CFLAGS += -I/usr/include/opencv
CFLAGS  += -DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX -D_LINUX -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -DHAVE_LIBOPENMAX=2 -DOMX -DOMX_SKIP64BIT -ftree-vectorize -pipe -DUSE_EXTERNAL_OMX -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST -DUSE_VCHIQ_ARM \
       -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux \
       -fPIC -ftree-vectorize -pipe 

LDFLAGS = -lm -lrt -lpthread -lpigpio $(pkg-config --cflags --libs opencv) -L/home/pi/balance -Wl,-rpath=/home/pi/balance -lservelib -lmmal_core -lmmal_util -lmmal_vc_client -lvcos -lbcm_host -lGLESv2 -lEGL -lopenmaxil -L/opt/vc/lib -L./src/#linking flags
LDFLAGS += -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_flann
CFLAGS += -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux -I/opt/vc/src/hello_pi/libs/ilclient
RM = rm -f  # rm command
TARGET = robobot
 
SRCS = src/encode.c src/vector.c src/video.c src/pd.c src/quaternion.c src/position.c src/motor.c src/robot.c src/control_panel.c src/coord.c src/turret.c src/matrix.c src/ogl.c src/fly.c
#include "../includes/state.h"

 # source files
OBJS = $(SRCS:.c=.o)
 
.PHONY: all
all: ${TARGET}
 
$(TARGET): $(OBJS) main.c
	$(CC) main.c $(OBJS) $(CFLAGS) ${LDFLAGS} -o $(TARGET)

$(SRCS:.c=.d):%.d:%.c
	$(CC) $(CFLAGS) -MM $< >$@
 
include $(SRCS:.c=.d)
 
.PHONY: clean
clean:
	-${RM} ${TARGET_LIB} ${OBJS} $(SRCS:.c=.d)
