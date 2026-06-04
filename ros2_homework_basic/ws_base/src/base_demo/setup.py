from setuptools import setup
from glob import glob
import os

package_name = 'base_demo'

setup(
    name=package_name,
    version='0.0.0',
    packages=[package_name],
    data_files=[
        ('share/ament_index/resource_index/packages', ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        #安装launch文件
        (os.path.join('share', package_name, 'launch'), glob('launch/*.launch.py')),
        #安装config文件
        (os.path.join('share', package_name, 'config'), glob('config/*.yaml')),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='gaopin',
    maintainer_email='1559174431@qq.com',
    description='turtle homework',
    license='TODO',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'turtle_8_node = base_demo.turtle_8_node:main',
        ],
    },
)