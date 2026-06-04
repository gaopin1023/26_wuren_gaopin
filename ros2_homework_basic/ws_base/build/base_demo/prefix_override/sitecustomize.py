import sys
if sys.prefix == '/usr':
    sys.real_prefix = sys.prefix
    sys.prefix = sys.exec_prefix = '/home/gaopin/Desktop/ros2_ homework_basic/ws_base/install/base_demo'
