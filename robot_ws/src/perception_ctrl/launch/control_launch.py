from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    pure_pursuit_node = Node(
        package="perception_ctrl",
        executable="pure_pursuit_node",
        name="pure_pursuit_controller",
        output="screen"
    )
    return LaunchDescription([pure_pursuit_node])
