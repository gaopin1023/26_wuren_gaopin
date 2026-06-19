#规划任务

##任务整体流程

###首先完成基本配置及检查
包括ros的版本检查、永久配置ros环境变量、Gazebo的下载及版本检查、检查ros2与Gazebo桥接功能、安装并校验slam_toolbox、安装并校验Nav2导航栈、确认 A*规划器节点可用、安装URDF、TF、RViz依赖、安装rosbag、安装编译必备工具、全局依赖初始化、

###收到1号小车及2号TF树配置之前的工作
创建并编译工作空间、加载工作空间环境、创建工作包、创建常用子文件夹、再次编译、创建 slam建图launch文件、创建Nav2全局规划yaml配置文件并配置基础参数、检查所有功能包是否能被ros识别、检查工作空间编译无报错，然后进行slam_toolbox建图和Nav2全局规划A*


```bash
source /opt/ros/humble/setup.bash
source ~/.bashrc

sudo apt update
sudo apt install ros-humble-ros-gz

sudo apt update
sudo apt install ros-humble-slam-toolbox
ros2 pkg list | grep slam_toolbox

sudo apt update
sudo apt install ros-humble-navigation2 ros-humble-nav2-bringup ros-humble-nav2-planner
ros2 pkg list | grep nav2
ros2 pkg executables nav2_planner

sudo apt install ros-humble-urdf ros-humble-xacro ros-humble-tf2-tools ros-humble-rviz2
sudo apt install ros-humble-rosbag2
sudo apt install python3-colcon-common-extensions python3-rosdep
sudo rosdep init
rosdep update

mkdir -p ~/robot_ws/src
cd ~/robot_ws
colcon build
source install/setup.bash

ros2 pkg create --build-type ament_cmake map_planner \
--dependencies rclcpp rclpy slam_toolbox nav2_planner nav2_map_server tf2_geometry_msgs urdf xacro

cd ~/robot_ws/src/map_planner
mkdir launch config maps rviz worlds
cd ~/robot_ws
colcon build
source install/setup.bash

ros2 pkg list | grep -E "slam_toolbox|nav2|map_planner"
cd ~/robot_ws
colcon build --packages-select map_planner

```

###收到1号及2号文件racecar_ws
完成yaml参数配置、launch启动文件代码，将yaml配置文件通过FindPackageShare相对路径关联到启动文件
确认坐标系名称：
map_frame: map
base_frame: base_link
odom_frame: racecar/odom

一开始由于rviz话题设置问题，可视化出了问题，之后1号调了TF树，已修改回来
另外还出现了时间以及雷达消息不同步的问题，之后修改了雷达的参数和yaml的参数，并且安装了显卡配置，然后可以了
建图经历了很多版的迭代，中间出现了很多问题，浪费了很多时间，最后的地图还是有一些不太好的地方
之后进入导航，一开始tf树总是出问题，最后解决了，然后进度到导航的小车定位阶段

####建图命令
```
colcon build
source install/setup.bash

ros2 launch racecar_description display.launch.py       #启动仿真

ros2 topic echo /scan                                   #校验雷达数据

ros2 launch map_planner slam.launch.py                  #启动slam建图节点

rviz2                                                   #启动 rviz可视化


ros2 service call /slam_toolbox/save_map slam_toolbox/srv/SaveMap "{name: {data: '/home/gaopin/Desktop/temp_map'}}"                                    #保存地图

ros2 launch map_planner nav_planner.launch.py           #关闭slam节点，启动规划模块

```
####导航命令

```
ros2 launch racecar_description display.launch.py

ros2 run nav2_map_server map_server \
--ros-args \
-p yaml_filename:=/home/gaopin/Desktop/robot_ws/src/map_planner/config/map.yaml \
-p map_subscribe_transient_local:=true
ros2 lifecycle set /map_server configure
ros2 lifecycle set /map_server activate             #激活地图

ros2 run tf2_ros static_transform_publisher --frame-id map --child-frame-id racecar/odom --x 0 --y 0 --z 0 --yaw 0                                 #解决tf树问题

ros2 run nav2_amcl amcl --ros-args -p use_sim_time:=true -p base_frame_id:=racecar/base_link -p odom_frame_id:=racecar/odom -p global_frame_id:=map
ros2 lifecycle set /map_server configure
ros2 lifecycle set /map_server activate             #激活amcl

ros2 launch nav2_bringup navigation_launch.py \
  map:=/home/gaopin/Desktop/robot_ws/src/map_planner/config/map.yaml \
  use_sim_time:=true \
  params_file:=/home/gaopin/Desktop/robot_ws/src/map_planner/config/nav2_params.yaml     #启动导航          

```
###rviz配置
fixed_frame:map
添加组件:
LaserScan      topic:/scan
Map            topic:/map
Pointcloud     topic:/points
RobotModel     topic:/robot_description

导航配置
添加panel:Navigation2,navigation localization feedback全部激活
然后进行全局和控制规划，现在暂时卡在localization激活阶段


         
##目标目录结构
map_planner/
├── launch/
│   ├── slam.launch.py               #SLAM建图启动文件
│   └── nav2.launch.py               #地图服务+全局规划一键启动文件
├── config/
│   ├── slam_params.yaml             #slam_toolbox建图参数
|   ├── map.yaml
|   ├── map.pgm
|   ├── nav_params.yaml
│   └── planner_params.yaml          #Nav2 A*全局规划参数
├── maps/
│   ├── track_map.pgm                #赛道栅格地图图像文件
│   └── track_map.yaml               #赛道地图配置文件
├── rviz/ 
├── tracks/
├── worlds/
├── include/ 
├── CMakeLists.txt                   #编译配置文件
└── package.xml



