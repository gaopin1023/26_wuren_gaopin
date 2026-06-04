#!/usr/bin/env python3
import rclpy, math, time
from rclpy.node import Node
from geometry_msgs.msg import Twist
from turtlesim.msg import Pose
from turtlesim.srv import TeleportAbsolute

class Turtle8(Node):
    def __init__(self):
        super().__init__('turtle_8_node')

        #加载参数
        self.declare_parameter('radius_x', 2.5)
        self.declare_parameter('radius_y', 2.0)
        self.declare_parameter('speed', 0.8)
        self.rx = self.get_parameter('radius_x').value
        self.ry = self.get_parameter('radius_y').value
        self.k = self.get_parameter('speed').value

        #订阅位姿
        self.pose = None
        self.pose_sub = self.create_subscription(Pose, '/turtle1/pose', self.pose_cb, 10)

        #复位乌龟到中心点
        self.teleport_client = self.create_client(TeleportAbsolute, '/turtle1/teleport_absolute')
        self.teleport_client.wait_for_service()
        req = TeleportAbsolute.Request()
        req.x = 5.544
        req.y = 5.544
        req.theta = 0.0
        
        #异步调用并等待完成
        future = self.teleport_client.call_async(req)
        rclpy.spin_until_future_complete(self, future, timeout_sec=1.0)
        if future.result() is not None:
            self.get_logger().info('复位服务调用成功')
        else:
            self.get_logger().warn('复位服务调用失败')
        
        #等待乌龟实际到达中心点（通过位姿回调确认）
        timeout = 1.0  #最多等1秒
        start = time.time()
        while self.pose is None and (time.time() - start) < timeout:
            rclpy.spin_once(self, timeout_sec=0.05)
        #再等待位姿更新到中心点（允许误差0.01）
        start = time.time()
        while (time.time() - start) < timeout:
            if self.pose and abs(self.pose.x - 5.544) < 0.01 and abs(self.pose.y - 5.544) < 0.01:
                break
            rclpy.spin_once(self, timeout_sec=0.05)
        self.get_logger().info(f'乌龟已到达中心点: ({self.pose.x:.3f}, {self.pose.y:.3f})')
        
        #发布速度命令 
        self.cmd_pub = self.create_publisher(Twist, '/turtle1/cmd_vel', 10)
        self.t = 0.0
        self._print_counter = 0
        #启动定时器（20Hz）
        self.timer = self.create_timer(0.05, self.control)
        self.get_logger().info('开始画8字')

    def pose_cb(self, msg):
        self.pose = msg

    def control(self):
        if self.pose is None:
            return

        #期望位置（8字形）
        x_des = 5.544 + self.rx * math.sin(2 * self.t)
        y_des = 5.544 + self.ry * math.sin(self.t)
        self.t += 0.05

        #比例控制
        err_x = x_des - self.pose.x
        err_y = y_des - self.pose.y
        distance = math.hypot(err_x, err_y)
        linear = min(0.6, distance * self.k)

        target_angle = math.atan2(err_y, err_x)
        angle_err = target_angle - self.pose.theta
        angle_err = math.atan2(math.sin(angle_err), math.cos(angle_err))
        angular = min(3.0, angle_err * 2.5)

        cmd = Twist()
        cmd.linear.x = linear
        cmd.angular.z = angular
        self.cmd_pub.publish(cmd)

        #定期打印位置（每30次约1.5秒）
        self._print_counter += 1
        if self._print_counter % 30 == 0:
            self.get_logger().info(f'当前位置: ({self.pose.x:.3f}, {self.pose.y:.3f}), 期望: ({x_des:.3f}, {y_des:.3f})')

def main(args=None):
    rclpy.init(args=args)
    node = Turtle8()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()