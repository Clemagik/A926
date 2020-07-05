/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)
This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.
You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// Copyright (c) 1996-2018, Live Networks, Inc.  All rights reserved
// A test program that demonstrates how to stream - via unicast RTP
// - various kinds of file on demand, using a built-in RTSP server.
// main program
 
#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BRILLO_X2000
#ifdef BRILLO_X2000
#include "CameraRecorder.h"
#else
#include "isvp_camera.h"
#endif

UsageEnvironment* env;

// To make the second and subsequent client for each stream reuse the same
// input stream as the first client (rather than playing the file from the
// start for each client), change the following "False" to "True":
Boolean reuseFirstSource = False;

// To stream *only* MPEG-1 or 2 video "I" frames
// (e.g., to reduce network bandwidth),
// change the following "False" to "True":
Boolean iFramesOnly = False;

char const* inputFileName = "/tmp/camera_h264_fifo";
//char const* inputFileName = "/tmp/t.h264";

static void announceStream(RTSPServer* rtspServer, ServerMediaSession* sms,
                char const* streamName, char const* inputFileName);

int main(int argc, char** argv) {
	// Begin by setting up our usage environment:
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	env = BasicUsageEnvironment::createNew(*scheduler);

    OutPacketBuffer::maxSize = 300000;

	UserAuthenticationDatabase* authDB = NULL;
#ifdef ACCESS_CONTROL
	// To implement client access control to the RTSP server, do the following:
	authDB = new UserAuthenticationDatabase;
	authDB->addUserRecord("username1", "password1"); // replace these with real strings
	// Repeat the above with each <username>, <password> that you wish to allow
	// access to the server.
#endif
	int fd;

#ifdef BRILLO_X2000
    iotdev_camera_init();
#else
	jz_camera_h264_init(); // jz isvp-sdk: init.
#endif
	unlink(inputFileName);
		
	if (mkfifo(inputFileName, 0777) < 0) {
			  *env << "mkfifo Failed\n";
			  exit(1);;
	}

	if (fork() > 0) {
		fd = open(inputFileName, O_RDWR | O_CREAT | O_TRUNC | O_NONBLOCK, 0777);
		if (fd < 0) {
			  *env << "Failed open fifo\n";
			  exit(1);;
		}   
		while (1) {
#ifdef BRILLO_X2000
            iotdev_camera_start_by_fifo(fd, 0);
            sleep(~0l);
#else
			  jz_camera_h264_get_stream(fd ,0);//jz isvp-sdk: get stream.
#endif
		}
	} else {
		// Create the RTSP server:
		RTSPServer* rtspServer = RTSPServer::createNew(*env, 8554, authDB);
		if (rtspServer == NULL) {
		  *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
		  exit(1);
		}

		char const* descriptionString
		  = "Session streamed by \"JZ-Camera-H264\"";

		// Set up each of the possible streams that can be served by the
		// RTSP server.  Each such stream is implemented using a
		// "ServerMediaSession" object, plus one or more
		// "ServerMediaSubsession" objects for each audio/video substream.

		// A H.264 video elementary stream:
		char const* streamName = "stream";
		ServerMediaSession* sms
		  = ServerMediaSession::createNew(*env, streamName, streamName,
					      descriptionString);
		sms->addSubsession(H264VideoFileServerMediaSubsession
			       ::createNew(*env, inputFileName, reuseFirstSource));
		rtspServer->addServerMediaSession(sms);

		announceStream(rtspServer, sms, streamName, inputFileName);
	}

	env->taskScheduler().doEventLoop(); // does not return

	return 0; // only to prevent compiler warning
}

static void announceStream(RTSPServer* rtspServer, ServerMediaSession* sms,
			   char const* streamName, char const* inputFileName) {
	char* url = rtspServer->rtspURL(sms);
	UsageEnvironment& env = rtspServer->envir();
	env << "\n\"" << streamName << "\" stream, from the file \""
	    << inputFileName << "\"\n";
	env << "Play this stream using the URL \"" << url << "\"\n";
	delete[] url;
}
