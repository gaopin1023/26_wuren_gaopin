作业完成思路：
1.先看懂这个作业要干什么，学长给了我什么文件，我又缺少什么文件，即实现一个ros2节点，订阅/estimation/slam/map话题（消息类型为fsd_common_msgs/Map），来将地图中的锥桶位置用rviz可视化出来。
2.搞清楚Map.msg里有什么：发现Map.msg包含四个数组：cone_yellow、cone_blue、cone_red、cone_unknown，每个数组里的元素是 Cone.msg 类型。而 Cone.msg里有geometry_msgs/Point position字段，即锥桶的坐标。可确定写代码的思路为拿到地图消息后，遍历这四个数组，为每个锥桶创建一个Marker，位置用cone.position，颜色根据数组类型决定（黄色/蓝色/红色/灰色）。
3.参考学长给的publish_marker_template.md模板，确定Marker需要设置：header.frame_id：用地图消息自带的 frame_id，保证坐标系一致
type：锥桶用圆柱体CYLINDER      scale：半径0.2m，高度0.3m       color：不同颜色数组用不同RGB值，alpha设为0.8半透明
lifetime：设为0表示一直显示             action：设为ADD
4.参考模板里的类结构，设计自己的节点类ConeVisualizer：构造函数里创建订阅者和发布者；订阅回调函数处理地图消息，生成MarkerArray并发布；用 std::bind绑定回调函数；为了避免代码冗余，写了一个lambda函数add_cones，接收锥桶数组和RGB颜色值，循环处理每个锥桶，减少重复代码。
5.配置CMakeLists.txt和package.xml。CMakeLists.txt需要find_package(rclcpp REQUIRED)、find_package(visualization_msgs REQUIRED)
find_package(fsd_common_msgs REQUIRED)、add_executable、ament_target_dependencies
package.xml需要添加对应的<depend>标签。
6.遇到编译错误就逐步排查：依赖缺失、环境冲突等。

遇到的困难与解决方法：
1.报错说找不到头文件，因为不影响编译，故我选择忽略了，这个问题在鱼香的视频里面看到过解决方法，好像是在package或者CMakeLists里面加一个路径还是什么
2.一直编译报错No module named 'em',好像是conda环境与系统Python冲突，执行conda deactivate退出conda，使用系统 Python就解决了
3.编译报错 rosidl_typesupport_c not found，应该是ros2依赖包缺失，使用sudo apt install ros-humble-rosidl-default-generators在终端安装即可
4.播放终端未source工作空间导致一直报错，当时还挺懵的，播放前执行source install/setup.bash
5.编译成功但 rviz中看不到锥桶，配置有点问题不，把一个地方改成world好像就可以了
6.多次编译失败，因为有残留缓存忘记删除了，删除 build/install/log/后重新编译
ps:其实感觉最难的是写代码

代码启动命令：
先编译
cd ~/ws_cone
source /opt/ros/humble/setup.bash
colcon build --packages-select fsd_common_msgs
source install/setup.bash
colcon build --packages-select map_to_visualize

运行（需要三个终端）：
终端1（播放bag数据源）：cd ~/ws_cone/src/map_to_visualize
source /opt/ros/humble/setup.bash
source ~/ws_cone/install/setup.bash
ros2 bag play map_to_visualize_0.db3 --loop
终端2（运行可视化节点）：cd ~/ws_cone
source install/setup.bash
ros2 run map_to_visualize vis_node
终端3（启动rviz）:rviz2 -d ~/ws_cone/cone_rviz.rviz

学习笔记：
选择性学习古月居和鱼香的视频，以及rviz可视化教程，其实最主要的学习阵地可能是AI，因为视频看过去只能理解不太能记忆
    
