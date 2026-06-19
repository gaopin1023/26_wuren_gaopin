from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from ament_index_python.packages import get_package_share_directory
import os

def generate_launch_description():
    #路径定义
    pkg_planner = get_package_share_directory("map_planner")
    map_yaml = os.path.join(pkg_planner, "config", "map.yaml")
    nav2_params = os.path.join(pkg_planner, "config", "planner_params.yaml")
    nav2_bringup_dir = get_package_share_directory("nav2_bringup")
    bringup_launch_file = os.path.join(nav2_bringup_dir, "launch", "bringup_launch.py")

    #校验地图文件
    if not os.path.exists(map_yaml):
        raise FileNotFoundError(f"地图文件不存在：{map_yaml}")

    #使用bringup内置map_server
    nav2_bringup_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(bringup_launch_file),
        launch_arguments={
            "map": map_yaml,
            "use_sim_time": "true",
            "params_file": nav2_params,
            "slam": "False",
            "use_composition": "False",
            "autostart": "True"
        }.items()
    )

    return LaunchDescription([nav2_bringup_launch])