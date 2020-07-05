INCLUDES = -I$(EXT_INCLUDE) -I../UsageEnvironment/include -I../groupsock/include -I../liveMedia/include -I../BasicUsageEnvironment/include
##### Change the following for your environment: 
CROSS_COMPILE?=		mips-linux-gnu-
COMPILE_OPTS =		$(INCLUDES) -I. -O2 -DSOCKLEN_T=socklen_t -DNO_SSTREAM=1 -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64 -march=xburst2 -mfp64 -DLINUX -DHAVE_ANDROID_OS -DNO_OPENSSL=1
C =			c
C_COMPILER =		$(CROSS_COMPILE)gcc
C_FLAGS =		$(COMPILE_OPTS)
CPP =			cpp
CPLUSPLUS_COMPILER =	$(CROSS_COMPILE)g++
CPLUSPLUS_FLAGS =	$(COMPILE_OPTS) -Wall -DBSD=1 -std=gnu++11
OBJ =			o
LINK =			$(CROSS_COMPILE)g++ -o
LINK_OPTS =		-mfp64
CONSOLE_LINK_OPTS =	$(LINK_OPTS)
LIBRARY_LINK =		$(CROSS_COMPILE)ar cr 
LIBRARY_LINK_OPTS =	$(LINK_OPTS)
LIB_SUFFIX =			a
LIBS_FOR_CONSOLE_APPLICATION =
LIBS_FOR_GUI_APPLICATION =
EXE =
##### End of variables to change

UNICAST_STREAMER_APPS = RTSPServer$(EXE)
UNICAST_APPS = $(UNICAST_STREAMER_APPS)

ALL = $(UNICAST_APPS)
all: $(ALL)

.$(C).$(OBJ):
	$(C_COMPILER) -c $(C_FLAGS) $<       

.$(CPP).$(OBJ):
	$(CPLUSPLUS_COMPILER) -c $(CPLUSPLUS_FLAGS) $<

RTSP_SERVER_OBJS = jzRTSPServer.$(OBJ)

USAGE_ENVIRONMENT_DIR = ../UsageEnvironment
USAGE_ENVIRONMENT_LIB = $(USAGE_ENVIRONMENT_DIR)/libUsageEnvironment.$(LIB_SUFFIX)
BASIC_USAGE_ENVIRONMENT_DIR = ../BasicUsageEnvironment
BASIC_USAGE_ENVIRONMENT_LIB = $(BASIC_USAGE_ENVIRONMENT_DIR)/libBasicUsageEnvironment.$(LIB_SUFFIX)
LIVEMEDIA_DIR = ../liveMedia
LIVEMEDIA_LIB = $(LIVEMEDIA_DIR)/libliveMedia.$(LIB_SUFFIX)
GROUPSOCK_DIR = ../groupsock
GROUPSOCK_LIB = $(GROUPSOCK_DIR)/libgroupsock.$(LIB_SUFFIX)
LOCAL_LIBS =	$(LIVEMEDIA_LIB) $(GROUPSOCK_LIB) \
		$(BASIC_USAGE_ENVIRONMENT_LIB) $(USAGE_ENVIRONMENT_LIB)

#INGENIC_ISVP_SDK = $(EXT_LIB)/libisvpcamera-h264.$(LIB_SUFFIX) \
				   $(EXT_LIB)/$(ISVP_DIR)/libimp.a	\
				   $(EXT_LIB)/$(ISVP_DIR)/libalog.a	
#INGENIC_ISVP_SDK = $(EXT_LIB)/libiotdevcamera-h264.$(LIB_SUFFIX)
EXTRA_LIBS = $(INGENIC_ISVP_SDK) -lpthread -lm -lrt \
			 --sysroot=$(EXT_ROOT) \
			 -L$(EXT_SHAREDS) \
			 -Wl,-rpath=$(EXT_SHAREDS) \
			 -liotdevcamera-h264 \
#			 -lcutils -lutils -ldlog -lstagefright -lbinder \
			 -lcamera_client -lstagefright_foundation -liotdevcamera-h264
			 #--sysroot=$(EXT_ROOT) \
			 --sysroot=/home/xryuan/brillo_x2000/out/product/brillo/sysroot \

LIBS =			$(LOCAL_LIBS) $(LIBS_FOR_CONSOLE_APPLICATION) $(EXTRA_LIBS)

RTSPServer$(EXE):	$(RTSP_SERVER_OBJS) $(LOCAL_LIBS)
	$(LINK)$@ $(CONSOLE_LINK_OPTS) $(RTSP_SERVER_OBJS) $(LIBS)

clean:
	-rm -rf *.$(OBJ) $(ALL) core *.core *~ include/*~

##### Any additional, platform-specific rules come here:
