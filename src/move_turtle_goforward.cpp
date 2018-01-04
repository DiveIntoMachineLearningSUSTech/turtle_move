#include <ros/ros.h>
#include <signal.h>
#include <geometry_msgs/Twist.h>

#include <time.h>

float PI = 3.1415926;
enum status{FORWARD, TURN};

ros::Publisher cmdVelPub;
void shutdown(int sig)
{
  cmdVelPub.publish(geometry_msgs::Twist());//使机器人停止运动
  ROS_INFO("move_turtle_goforward ended!");
  ros::shutdown();
}
int main(int argc, char** argv)
{
  ros::init(argc, argv, "move_turtle_goforward");//初始化ROS,它允许ROS通过命令行进行名称重映射
  ros::NodeHandle node;//为这个进程的节点创建一个句柄
  cmdVelPub = node.advertise<geometry_msgs::Twist>("/mobile_base/commands/velocity", 1);//在/mobile_base/commands/velocity topic上发布一个geometry_msgs/Twist的消息
  ros::Rate loopRate(10);//ros::Rate对象可以允许你指定自循环的频率
  signal(SIGINT, shutdown);
  ROS_INFO("move_turtle_goforward cpp start...");
  geometry_msgs::Twist speed; // 控制信号载体 Twist message

  time_t start = time(NULL);
  time_t begin = start;
  float vel_rad = PI/3;
  time_t forward_time = 2;
  time_t turn_time = PI/2 / vel_rad;
  status s = FORWARD;

  while (ros::ok())
  {
    time_t current = time(NULL);
    time_t elapse = current - begin;
    if (elapse >= 20.0f)
      break;

    time_t seconds = current - start;
    if (s == FORWARD && seconds >= forward_time)
    {
      start = current;
      s = TURN;
      ROS_INFO("move_turtle_goforward cpp TURNING!!!!!!!...");
    }
    else if (s == TURN && seconds >= turn_time)
    {
      start = current;
      s = FORWARD;
      ROS_INFO("move_turtle_goforward cpp FORWARDING!!!!!!...");
    }

    
    speed.linear.x = 0.1; // 设置线速度为0.1m/s，正为前进，负为后退
    speed.angular.z = s==FORWARD ? 0 : vel_rad; // 设置角速度为0rad/s，正为左转，负为右转
    cmdVelPub.publish(speed); // 将刚才设置的指令发送给机器人
    loopRate.sleep();//休眠直到一个频率周期的时间
  }
  return 0;
}
