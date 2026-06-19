from launch_ros.substitutions import FindPackageShare
from launch.substitutions import PathJoinSubstitution, LaunchConfiguration
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument

def generate_launch_description():
    #声明配置文件路径参数
    slam_config = LaunchConfiguration('slam_config')
    declare_slam_config = DeclareLaunchArgument(
        'slam_config',
        default_value=PathJoinSubstitution([
            FindPackageShare("map_planner"),
            "config",
            "slam_params.yaml"
        ]),
        description='slam_toolbox yaml配置文件路径'
    )

    #声明仿真时间开关
    use_sim_time = LaunchConfiguration('use_sim_time')
    declare_sim_time = DeclareLaunchArgument(
        'use_sim_time',
        default_value='true',
        description='是否启用仿真时间，Gazebo必须true'
    )

    #异步SLAM节点
    slam_node = Node(
        package='slam_toolbox',
        executable='async_slam_toolbox_node',
        name='slam_toolbox',
        parameters=[
            slam_config,
            {'use_sim_time': use_sim_time}  
        ],
        output='screen'
    )

    return LaunchDescription([
        declare_slam_config,
        declare_sim_time,
        slam_node
    ])
