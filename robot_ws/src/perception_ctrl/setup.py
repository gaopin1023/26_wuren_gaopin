from setuptools import setup, find_packages
import os
from glob import glob

package_name = "perception_ctrl"

setup(
    name=package_name,
    version="0.0.0",
    packages=find_packages(),
    data_files=[
        (os.path.join("share", "ament_index", "resource_index", "packages"),
         [os.path.join("resource", package_name)]),
        (os.path.join("share", package_name), ["package.xml"]),
        (os.path.join("share", package_name, "launch"), glob("launch/*.py")),
        (os.path.join("share", package_name, "config"), glob("config/*.yaml")),
    ],
    install_requires=["setuptools"],
    zip_safe=True,
    maintainer="robot_team_4",
    maintainer_email="student@robot.com",
    description="4号感知与纯跟踪控制包",
    license="Apache-2.0",
    tests_require=["pytest"],
    entry_points={
        "console_scripts": [
            "pure_pursuit_node = perception_ctrl.pure_pursuit_node:main"
        ],
    },
)
