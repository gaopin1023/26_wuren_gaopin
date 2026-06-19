import math  #导入数学库，用于进行计算

from nav_msgs.msg import Path
from geometry_msgs.msg import PoseStamped


class PathHandler:  #定义路径处理类，用于处理接受到的规划路径
    def __init__(self):
        self.path_points = []  #用于存储路径点（格式为x,y,航向角）
        self.path_received = False #用于存储是否成功接受到路径（修复拼写recieved→received）

    def update_global_path(self, path_msg: Path):   #接受Path消息，解析路径点并存入path_points
        # 修复逻辑：先判断输入消息是否为空，再清空本地路径
        if len(path_msg.poses) == 0:
            self.path_received = False
            return
        self.path_points.clear()   #清空上一次的路径点

        #循环遍历每一个路径点，获取x,y和航向角
        for i in path_msg.poses:
            x = i.pose.position.x
            y = i.pose.position.y
            #获取姿态四元数的四个分量，最终获取航向角
            qx = i.pose.orientation.x
            qy = i.pose.orientation.y
            qz = i.pose.orientation.z
            qw = i.pose.orientation.w  #修复拼写poze→pose
            #航向角yaw
            yaw = self.quaternion_to_yaw(qw,qx,qy,qz)
            #存储数据
            self.path_points.append((x,y,yaw))
        self.path_received = True

    #定义函数：将四元数转换为航向角yaw
    def quaternion_to_yaw(self,w,x,y,z):
        siny_cosp = 2*(w*z+x*y)
        cosy_cosp = 1-2*(y*y+z*z)
        #上式相除得到航向角的正切值
        yaw = math.atan2(siny_cosp,cosy_cosp)
        return yaw

    #定义函数：计算欧式距离
    def calc_distance(self,x1,y1,x2,y2):
        dx = x2 - x1
        dy = y2 - y1
        return math.hypot(dx, dy)

    #查找路径上距离小车距离最近的点
    def find_closest_point_idx(self,car_x,car_y,search_range):
        #初始距离设置为无穷大
        min_dist = float("inf")
        #初始化最近点为-1
        closest_idx = -1
        #遍历所有路径上的点
        for idx,(px,py,_) in enumerate(self.path_points):
            #计算小车和点的欧式距离
            dist = self.calc_distance(car_x, car_y, px, py)
            #寻找最小距离 
            if dist < min_dist and dist < search_range:
                min_dist = dist
                closest_idx = idx
        return closest_idx   #如果没有找到，则返回-1

    #从最近点向后找到路径上距离为前视距离的点，即目标点（修复函数命名）
    def find_lookahead_point(self, car_x, car_y, closest_idx, lookahead_dist):
        # 修复：初始化为None，避免无预瞄点时返回(0,0)错误坐标
        target_x, target_y = None, None
        accumulated_dist = 0.0
        #从最近点开始索引，往后一直到倒数第二个点
        for i in range(closest_idx, len(self.path_points)-1):
            x0, y0, _ = self.path_points[i]
            x1, y1, _ = self.path_points[i+1] #修复 self.path.points → self.path_points
            seg_dist = self.calc_distance(x0, y0, x1, y1)
            #判断：如果该微小线段距离加上累积距离大于前视距离，则目标点就在该线段上
            if seg_dist + accumulated_dist >= lookahead_dist:
                ratio = (lookahead_dist - accumulated_dist) / seg_dist
                target_x = x0 + ratio * (x1 - x0)
                target_y = y0 + ratio * (y1 - y0)
                break
            accumulated_dist += seg_dist #修复变量名 accumulate_dist→accumulated_dist
        return (target_x, target_y) if target_x is not None else None

    #判断前方路径是否为90度直角弯（修复注释错别字）
    def is_turn_point(self, point_idx, turn_angle_thresh):
        #第一个点和最后一个点无法判断航向，直接去掉
        if point_idx <= 0 or point_idx >= len(self.path_points)-1: #修复 self.path.points → self.path_points
            return False
        #获取前一个点航向角（修复注释错别字）
        prev_yaw = self.path_points[point_idx-1][2]
        #获取后一个点航向角
        next_yaw = self.path_points[point_idx+1][2]
        #计算前后航向角的差值绝对值
        yaw_diff = abs(next_yaw - prev_yaw)
        #控制角度差值在0~180°区间
        yaw_diff = min(yaw_diff, 2 * math.pi - yaw_diff)
        #若航向突变超过阈值，则判定为直角弯
        return yaw_diff >= turn_angle_thresh

