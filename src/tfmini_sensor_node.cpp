#include <tfmini.h>

int main(int argc, char **argv)
{
  ros::init(argc, argv, "tfmini_sensor_node");
  ros::NodeHandle nh("~");
  std::string id = "tfmini";
  std::string portName;
  int baud_rate;
  benewake::tfmini *tfmini_obj;

  nh.param("serial_port", portName, std::string("/dev/ttyUSB0"));
  nh.param("baud_rate", baud_rate, 115200);

  tfmini_obj = new benewake::tfmini("/dev/ttyUSB0", 115200);
  ros::Publisher pub_range = nh.advertise<sensor_msgs::Range>(id, 1000, true);
  sensor_msgs::Range tfmini_range;
  tfmini_range.radiation_type = sensor_msgs::Range::INFRARED;
  tfmini_range.field_of_view = 0.04;
  tfmini_range.min_range = 0.3;
  tfmini_range.max_range = 12;
  tfmini_range.header.frame_id = id;
  float dist = 0;
  ROS_INFO_STREAM("Start processing ...");

  while(ros::master::check() && ros::ok())
  {
    ros::spinOnce();
    ROS_INFO_STREAM(tfmini_obj);
    dist = tfmini_obj->getDist();
    if(dist > 0 && dist < tfmini_range.max_range)
    {
      tfmini_range.range = dist;
      tfmini_range.header.stamp = ros::Time::now();
      pub_range.publish(tfmini_range);
    }
    else if(dist == -1.0)
    {
      ROS_ERROR_STREAM("Failed to read data. tfmini ros node stopped!");
      break;
    }
    else if(dist == 0.0)
    {
      ROS_ERROR_STREAM("Data validation error!");
    }
  }

  tfmini_obj->closePort();
}
