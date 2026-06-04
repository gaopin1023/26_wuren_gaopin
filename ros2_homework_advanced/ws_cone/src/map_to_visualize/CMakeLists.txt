cmake_minimum_required(VERSION 3.8)
project(map_to_visualize)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(fsd_common_msgs REQUIRED)   

add_executable(vis_node src/vis_node.cpp)
ament_target_dependencies(vis_node
  rclcpp
  visualization_msgs
  fsd_common_msgs
)

install(TARGETS vis_node
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()