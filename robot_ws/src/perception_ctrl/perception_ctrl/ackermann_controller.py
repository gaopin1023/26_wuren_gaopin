#主要定义纯跟踪算法,阿克曼运动学控制器
import math
# 导入阿克曼底盘控制消息，用于生成下发给1号小车的控制指令
from ackermann_msgs.msg import AckermannDrive

# 阿克曼控制类：计算转向角、规划行驶速度、组装标准控制指令消息
class AckermannController:
    # 修复1：构造函数增加入参params，接收主节点传入的yaml参数字典
    def __init__(self, params):
        # 从yaml配置字典中读取小车硬件、算法控制参数
        self.wheel_base = params["pure_pursuit"]["wheel_base"]               # 阿克曼小车轴距(m)
        self.max_steer = params["pure_pursuit"]["max_steering_angle"]        # 前轮物理最大转向角(rad)
        self.max_speed = params["pure_pursuit"]["max_speed"]                 # 直道最大行驶速度(m/s)
        self.min_speed = params["pure_pursuit"]["min_speed"]                 # 车辆允许最低行驶速度(m/s)
        self.turn_speed = params["pure_pursuit"]["turn_speed"]               # 直角转弯低速(m/s)
        self.turn_slow_dist = params["pure_pursuit"]["turn_slowdown_dist"]   # 距离拐点提前减速距离(m)

    # 函数：利用自行车模型推导的纯跟踪公式计算前轮目标转角
    def calc_steering_angle(self, car_x, car_y, car_yaw, target_x, target_y):
        # 计算预瞄目标点相对小车全局坐标系的X、Y差值
        dx = target_x - car_x
        dy = target_y - car_y
        # 坐标转换：全局map坐标系 → 小车局部车身坐标系（车头向前，车身左侧为Y正方向）
        local_x = dx * math.cos(-car_yaw) - dy * math.sin(-car_yaw)
        local_y = dx * math.cos(-car_yaw) + dy * math.sin(-car_yaw)
        # 计算小车底盘中心到预瞄点的直线距离（实际前视距离Ld）
        Ld = math.hypot(local_x, local_y)
        # 计算车身X轴与小车→预瞄点连线的横向偏角α
        alpha = math.atan2(local_y, local_x)
        # 纯跟踪自行车模型核心公式：计算理论前轮转向角
        steer_angle = math.atan((2 * self.wheel_base * math.sin(alpha)) / Ld)
        # 硬件限幅：转向角不能超过小车物理最大左右转角
        steer_angle = max(-self.max_steer, min(self.max_steer, steer_angle))
        return steer_angle

    # 速度规划函数：检测前方直角拐点自动减速，直道全速行驶
    def get_target_speed(self, car_x, car_y, closest_idx, path_handler):
        # 标记前方路径是否存在直角转弯拐点，初始默认无转弯
        ahead_turn = False
        # 换算向前扫描路径点数量：按0.1m一个路径点，把距离转为点数
        scan_range = int(self.turn_slow_dist / 0.1)
        # 限制扫描终点不超过路径最后一个点，防止数组越界
        end_idx = min(closest_idx + scan_range, len(path_handler.path_points) - 1)
        # 从当前最近路径点向后遍历扫描范围内所有路径点
        for idx in range(closest_idx, end_idx):
            # 调用路径工具类判断当前点是否为90°直角拐点
            if path_handler.is_turn_point(idx, 1.57):
                ahead_turn = True
                break
        # 根据是否有拐点分配行驶速度
        if ahead_turn:
            target_v = self.turn_speed
        else:
            target_v = self.max_speed
        # 新增优化：速度上下限约束，保证车速不会低于最小限速
        target_v = max(self.min_speed, min(self.max_speed, target_v))
        return target_v

    # 组装标准阿克曼控制消息，下发给1号racecar_description仿真底盘
    def generate_ackermann_msg(self, steer_angle, speed):
        # 修复2：拼写错误 AchermannDrive → AckermannDrive
        # 实例化阿克曼驱动消息对象
        cmd_msg = AckermannDrive()
        # 填充计算得出的前轮转向角
        cmd_msg.steering_angle = steer_angle
        # 填充规划后的行驶速度
        cmd_msg.speed = speed
        # 限制转向角变化速率，避免前轮瞬间猛打方向导致车身抖动
        cmd_msg.steering_angle_velocity = 0.8
        # 设置车辆平缓加速度
        cmd_msg.acceleration = 0.5
        # 返回组装完成的完整控制消息，由主节点发布话题
        return cmd_msg

