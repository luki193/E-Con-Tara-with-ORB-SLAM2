// The uvc_cam package provides a simple interface to webcams visible through
// the uvcvideo driver. It includes code adapted from 'guvcview', which in
// turn adapted some code from 'luvcview'.
// Copyright (C) 2009 Morgan Quigley
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////

#ifndef UVC_CAM_H
#define UVC_CAM_H

#include <string>
#include <linux/videodev2.h>
#include <stdint.h>

/**********************************************************************/
#include <linux/videodev2.h>
#include <libv4l2.h>
#include <glib.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <iostream>
#include <limits>
#include <stdlib.h>

//Extension unit header
#include "xunit_lib_tara.h"

//#define TRUE 0
//#define FALSE 1

using namespace std;
/*******************************************************************/

extern "C"
{
bool InitExtensionUnit(const char*);

bool UninitExtensionUnit();

bool EnableMasterMode();

bool EnableTriggerMode();
}

namespace uvc_cam
{

enum v4l2_uvc_exposure_auto_type
{
	V4L2_UVC_EXPOSURE_MANUAL = 1,
	V4L2_UVC_EXPOSURE_AUTO = 2,
	V4L2_UVC_EXPOSURE_SHUTTER_PRIORITY = 4,
	V4L2_UVC_EXPOSURE_APERTURE_PRIORITY = 8
};


static const int exp_vals[]=
{
	V4L2_UVC_EXPOSURE_MANUAL,
	V4L2_UVC_EXPOSURE_AUTO,
	V4L2_UVC_EXPOSURE_SHUTTER_PRIORITY,
	V4L2_UVC_EXPOSURE_APERTURE_PRIORITY
};



#define CSU32 const static uint32_t

class Cam
{
public:
  enum mode_t { MODE_RGB, MODE_MJPG, MODE_YUYV, MODE_BAYER, MODE_Y16 } mode_;
  Cam(const char *device, mode_t _mode = MODE_RGB,
    int _width = 640, int _height = 480, int _fps = 30);
   	UINT8 MajorVersion;
	UINT8 MinorVersion1;
	UINT16 MinorVersion2;
	UINT16 MinorVersion3;
	bool IsStereo;
	int index;
  ~Cam();
  static void enumerate();
  int grabStereo(unsigned char **frame, uint32_t &bytes_used, unsigned char **left_frame , unsigned char **right_frame, unsigned char **concat_frame );
  int grab(unsigned char **frame, uint32_t &bytes_used);
  void release(unsigned buf_idx);
  bool set_auto_white_balance(bool on);
  void set_motion_thresholds(int lum, int count);
  int set_control(uint32_t id, int val);
  int get_control(uint32_t id, int *value);
  int GetListofDeviceseCon(void);
  void showFirmwareVersion();
  BOOL IsStereoDeviceAvail(char *pid);
  
  
  
private:
//***************************************


	typedef struct _VidDevice
	{
		char *device;
		char *friendlyname;
		char *bus_info;
		char *vendor;
		char *product;
		short int deviceID;
	} VidDevice;

	//Enumerated devices list
	typedef struct _LDevices
	{
		VidDevice *listVidDevices;
		int num_devices;
	} LDevices;

	//Stores the device instances of all enumerated devices
	LDevices *DeviceInstances;
	
	char *DeviceInfo;
//********************	************


  std::string device_;

  int device_file_h_;
  int motion_threshold_luminance_;
  int motion_threshold_count;

  unsigned width_, height_, fps_;
  v4l2_format format_;
  v4l2_capability capability_;
  v4l2_streamparm stream_parm_;
  v4l2_requestbuffers request_buffers_;
  v4l2_buffer buffer_;
  v4l2_timecode time_code_;
  static const unsigned NUM_BUFFERS = 2;
  void *buffer_mem_[NUM_BUFFERS];
  unsigned buffer_length_;
  unsigned char *rgb_frame_;
  unsigned char *last_yuv_frame_;

  unsigned char *y16_frame_;
  unsigned char *left_frame_;
  unsigned char *right_frame_;
  unsigned char *concat_frame_;
  /*------------------------- new camera class controls ---------------------*/
  CSU32 V4L2_CTRL_CLASS_USER_NEW = 0x00980000;
  CSU32 V4L2_CID_BASE_NEW = V4L2_CTRL_CLASS_USER_NEW | 0x900;
  CSU32 V4L2_CID_POWER_LINE_FREQUENCY_NEW = V4L2_CID_BASE_NEW + 24;
  CSU32 V4L2_CID_HUE_AUTO_NEW = V4L2_CID_BASE_NEW + 25;
  CSU32 V4L2_CID_WHITE_BALANCE_TEMPERATURE_NEW = V4L2_CID_BASE_NEW + 26;
  CSU32 V4L2_CID_SHARPNESS_NEW = V4L2_CID_BASE_NEW + 27;
  CSU32 V4L2_CID_BACKLIGHT_COMPENSATION_NEW = V4L2_CID_BASE_NEW + 28;
  CSU32 V4L2_CID_LAST_NEW = V4L2_CID_BASE_NEW + 29;

  CSU32 V4L2_CTRL_CLASS_CAMERA_NEW = 0x009A0000;	/* Camera class controls */
  CSU32 V4L2_CID_CAMERA_CLASS_BASE_NEW = V4L2_CTRL_CLASS_CAMERA_NEW | 0x900;
  CSU32 V4L2_CID_EXPOSURE_AUTO_NEW = V4L2_CID_CAMERA_CLASS_BASE_NEW + 1;
  CSU32 V4L2_CID_EXPOSURE_ABSOLUTE_NEW = V4L2_CID_CAMERA_CLASS_BASE_NEW + 2;
  CSU32 V4L2_CID_EXPOSURE_AUTO_PRIORITY_NEW = V4L2_CID_CAMERA_CLASS_BASE_NEW+3;
  CSU32 V4L2_CID_PAN_RELATIVE_NEW = V4L2_CID_CAMERA_CLASS_BASE_NEW + 4;
  CSU32 V4L2_CID_TILT_RELATIVE_NEW = V4L2_CID_CAMERA_CLASS_BASE_NEW + 5;
  CSU32 V4L2_CID_PAN_RESET_NEW = V4L2_CID_CAMERA_CLASS_BASE_NEW + 6;
  CSU32 V4L2_CID_TILT_RESET_NEW	= V4L2_CID_CAMERA_CLASS_BASE_NEW + 7;
  CSU32 V4L2_CID_PAN_ABSOLUTE_NEW = V4L2_CID_CAMERA_CLASS_BASE_NEW + 8;
  CSU32 V4L2_CID_TILT_ABSOLUTE_NEW = V4L2_CID_CAMERA_CLASS_BASE_NEW + 9;
  CSU32 V4L2_CID_FOCUS_ABSOLUTE_NEW = V4L2_CID_CAMERA_CLASS_BASE_NEW + 10;
  CSU32 V4L2_CID_FOCUS_RELATIVE_NEW = V4L2_CID_CAMERA_CLASS_BASE_NEW + 11;
  CSU32 V4L2_CID_FOCUS_AUTO_NEW = V4L2_CID_CAMERA_CLASS_BASE_NEW + 12;
  CSU32 V4L2_CID_CAMERA_CLASS_LAST = V4L2_CID_CAMERA_CLASS_BASE_NEW + 13;

  /*--------------- old private class controls ------------------------------*/
  CSU32 V4L2_CID_PRIVATE_BASE_OLD = 0x08000000;
  CSU32 V4L2_CID_BACKLIGHT_COMPENSATION_OLD = V4L2_CID_PRIVATE_BASE_OLD + 0;
  CSU32 V4L2_CID_POWER_LINE_FREQUENCY_OLD = V4L2_CID_PRIVATE_BASE_OLD + 1;
  CSU32 V4L2_CID_SHARPNESS_OLD = V4L2_CID_PRIVATE_BASE_OLD + 2;
  CSU32 V4L2_CID_HUE_AUTO_OLD = V4L2_CID_PRIVATE_BASE_OLD + 3;
  CSU32 V4L2_CID_FOCUS_AUTO_OLD = V4L2_CID_PRIVATE_BASE_OLD + 4;
  CSU32 V4L2_CID_FOCUS_ABSOLUTE_OLD = V4L2_CID_PRIVATE_BASE_OLD + 5;
  CSU32 V4L2_CID_FOCUS_RELATIVE_OLD = V4L2_CID_PRIVATE_BASE_OLD + 6;
  CSU32 V4L2_CID_PAN_RELATIVE_OLD = V4L2_CID_PRIVATE_BASE_OLD + 7;
  CSU32 V4L2_CID_TILT_RELATIVE_OLD = V4L2_CID_PRIVATE_BASE_OLD + 8;
  CSU32 V4L2_CID_PANTILT_RESET_OLD = V4L2_CID_PRIVATE_BASE_OLD + 9;
  CSU32 V4L2_CID_EXPOSURE_AUTO_OLD = V4L2_CID_PRIVATE_BASE_OLD + 10;
  CSU32 V4L2_CID_EXPOSURE_ABSOLUTE_OLD = V4L2_CID_PRIVATE_BASE_OLD + 11;
  CSU32 V4L2_CID_WHITE_BALANCE_TEMP_AUTO_OLD	= V4L2_CID_PRIVATE_BASE_OLD+12;
  CSU32 V4L2_CID_WHITE_BALANCE_TEMP_OLD = V4L2_CID_PRIVATE_BASE_OLD + 13;
  CSU32 V4L2_CID_PRIVATE_LAST = V4L2_CID_WHITE_BALANCE_TEMP_OLD + 1;
  // dynamic controls
  CSU32 UVC_CTRL_DATA_TYPE_RAW = 0;
  CSU32 UVC_CTRL_DATA_TYPE_SIGNED	= 1;
  CSU32 UVC_CTRL_DATA_TYPE_UNSIGNED	= 2;
  CSU32 UVC_CTRL_DATA_TYPE_BOOLEAN = 3;
  CSU32 UVC_CTRL_DATA_TYPE_ENUM = 4;
  CSU32 UVC_CTRL_DATA_TYPE_BITMASK = 5;
  CSU32 V4L2_CID_BASE_EXTCTR = 0x0A046D01;
  CSU32 V4L2_CID_BASE_LOGITECH = V4L2_CID_BASE_EXTCTR;
  //CSU32 V4L2_CID_PAN_RELATIVE_LOGITECH = V4L2_CID_BASE_LOGITECH;
  //CSU32 V4L2_CID_TILT_RELATIVE_LOGITECH = V4L2_CID_BASE_LOGITECH + 1;
  CSU32 V4L2_CID_PANTILT_RESET_LOGITECH = V4L2_CID_BASE_LOGITECH + 2;
  CSU32 V4L2_CID_FOCUS_LOGITECH = V4L2_CID_BASE_LOGITECH + 3;
  CSU32 V4L2_CID_LED1_MODE_LOGITECH = V4L2_CID_BASE_LOGITECH + 4;
  CSU32 V4L2_CID_LED1_FREQUENCY_LOGITECH = V4L2_CID_BASE_LOGITECH + 5;
  CSU32 V4L2_CID_DISABLE_PROCESSING_LOGITECH = V4L2_CID_BASE_LOGITECH + 0x70;
  CSU32 V4L2_CID_RAW_BITS_PER_PIXEL_LOGITECH = V4L2_CID_BASE_LOGITECH + 0x71;
  CSU32 V4L2_CID_LAST_EXTCTR = V4L2_CID_RAW_BITS_PER_PIXEL_LOGITECH;

};

}

#endif

