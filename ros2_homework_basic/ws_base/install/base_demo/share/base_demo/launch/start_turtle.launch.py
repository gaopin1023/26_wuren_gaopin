from launch import LaunchDescription
from launch_ros.actions import Node
import os
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    param_file = os.path.join(
        get_package_share_directory('base_demo'),
        'config',
        'turtle_param.yaml'
    )
    return LaunchDescription([
        Node(package='turtlesim', executable='turtlesim_node', name='turtlesim'),
        Node(package='base_demo', executable='turtle_8_node', name='turtle_8_node',
             parameters=[param_file])
    ])