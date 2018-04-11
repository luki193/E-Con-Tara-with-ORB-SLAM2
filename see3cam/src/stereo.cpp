#include <boost/thread.hpp>

#include <ros/ros.h>
#include <ros/time.h>

#include "uvc_cam/uvc_cam.h"
#include "sensor_msgs/Image.h"
#include "sensor_msgs/image_encodings.h"
#include "sensor_msgs/CameraInfo.h"
#include "camera_info_manager/camera_info_manager.h"
#include "image_transport/image_transport.h"
#include "std_msgs/Float64.h"

#include "uvc_camera/camera.h"

using namespace sensor_msgs;

namespace uvc_camera {

Camera::Camera(ros::NodeHandle _comm_nh, ros::NodeHandle _param_nh) :
      node(_comm_nh), pnode(_param_nh), it(_comm_nh),
      info_mgr(_comm_nh, "camera"), cam(0) {

      /* default config values */
      width = 640;
      height = 480;
      fps = 10;
      skip_frames = 0;
      frames_to_skip = 0;
      device = "/dev/video0";
      frame = "camera";
      rotate = false;
      exposure_value = 80;
      brightness_value = 4;
      
      /* set up information manager */
      std::string url;

      pnode.getParam("camera_info_url", url);

      info_mgr.loadCameraInfo(url);

      /* pull other configuration */
      pnode.getParam("device", device);

      pnode.getParam("fps", fps);
      pnode.getParam("skip_frames", skip_frames);

      pnode.getParam("width", width);
      pnode.getParam("height", height);

      pnode.getParam("frame_id", frame);
      // changing start
      pnode.getParam ("exposureValue", exposure_value);
      brightness_pub = node.advertise<std_msgs::Float64>("get_brightness", 1, true);
      
      /* advertise image streams and info streams */
      pub = it.advertise("image_raw", 1);

      info_pub = node.advertise<CameraInfo>("camera_info", 1);
      exposure_pub = node.advertise<std_msgs::Float64>("get_exposure", 1, true);

      /* initialize the cameras */
      cam = new uvc_cam::Cam(device.c_str(), uvc_cam::Cam::MODE_BAYER, width, height, fps);
      //cam->set_motion_thresholds(100, -1);
      cam->set_control(0x009a0901, 1); // exposure, auto (0 = auto, 1 = manual)
      cam->set_control(0x00980900, 8); // brightness
     cam->set_control(0x9a0902, exposure_value); // exposure time 15.6ms

    std_msgs::Float64 exposure_msg;
    exposure_msg.data=(float)exposure_value;
	exposure_pub.publish( exposure_msg );
	
    std_msgs::Float64 brightness_msg;
    brightness_msg.data=(float)brightness_value;
	brightness_pub.publish( brightness_msg );

      /* and turn on the streamer */
      ok = true;
      image_thread = boost::thread(boost::bind(&Camera::feedImages, this));

      std::string time_topic;
      pnode.getParam("time_topic", time_topic);
      time_sub = node.subscribe(time_topic, 1, &Camera::timeCb, this );
      exposure_sub = node.subscribe ("set_exposure", 1, &Camera::callBackExposure, this);
      brightness_sub = node.subscribe ("set_brightness", 1, &Camera::callBackBrightness, this);
}

	void Camera::callBackExposure (std_msgs::Float64 call_exposure_msg)
	{
		exposure_value=(float)call_exposure_msg.data;
		cam->set_control(0x9a0902, exposure_value);
		exposure_pub.publish( call_exposure_msg );
	}
	
	void Camera::callBackBrightness (std_msgs::Float64 call_brightness_msg)
	{
		brightness_value=(int)call_brightness_msg.data;
		cam -> set_control( 0x00980900,brightness_value); 
		brightness_pub.publish( call_brightness_msg );
	}
		
    void Camera::sendInfo(ImagePtr &image, ros::Time time) {
      CameraInfoPtr info(new CameraInfo(info_mgr.getCameraInfo()));

      /* Throw out any CamInfo that's not calibrated to this camera mode */
      if (info->K[0] != 0.0 &&
           (image->width != info->width
              || image->height != info->height)) {
        info.reset(new CameraInfo());
      }

      /* If we don't have a calibration, set the image dimensions */
      if (info->K[0] == 0.0) {
        info->width = image->width;
        info->height = image->height;
      }

      info->header.stamp = time;
      info->header.frame_id = frame;

      info_pub.publish(info);
    }

    void Camera::timeCb(std_msgs::Time time)
    {
      time_mutex_.lock();
    	last_time = time.data;
    	//ROS_INFO_STREAM("Next timestamp: " << last_time);
      time_mutex_.unlock();
    }

    void Camera::feedImages() {
      unsigned int pair_id = 0;
      while (ok) {
        unsigned char *img_frame = NULL;
        uint32_t bytes_used;

        int idx = cam->grab(&img_frame, bytes_used);

        time_mutex_.lock();
        ros::Time capture_time = last_time;
        time_mutex_.unlock();
//        ros::Time capture_time = last_time;

        /* Read in every frame the camera generates, but only send each
         * (skip_frames + 1)th frame. It's set up this way just because
         * this is based on Stereo...
         */
        if (skip_frames == 0 || frames_to_skip == 0) {
          if (img_frame) {
             ImagePtr image(new Image);

             image->height = height;
             image->width = width;
             image->step = width;
             image->encoding = image_encodings::BAYER_GRBG8;

             image->header.stamp = capture_time;
             image->header.seq = pair_id;

             image->header.frame_id = frame;

             image->data.resize(image->step * image->height);

             memcpy(&image->data[0], img_frame, image->data.size());

             pub.publish(image);

             sendInfo(image, capture_time);

             //ROS_INFO_STREAM("capture time: " << capture_time);

             ++pair_id;
          }

          frames_to_skip = skip_frames;
        } else {
          frames_to_skip--;
        }

        if (img_frame) cam->release(idx);
      }
    }

    Camera::~Camera() {
      ok = false;
      image_thread.join();
      if (cam) delete cam;
    }


};

