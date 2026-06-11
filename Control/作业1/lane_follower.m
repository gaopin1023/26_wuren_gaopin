%% 第二问：轨迹跟踪
clear; clc; close all

% 车辆参数
lfr = 2.168 + 1.907; % 轴距 L
dt = 0.01;
v = 15; 
sim_steps = 2000;
Ld=8;  %纯跟踪预瞄距离

% 参考轨迹 (正弦曲线)
X_ref = 0:0.1:200; 
Y_ref = 10 * sin(X_ref / 15); 

% 初始车辆状态 
X = X_ref(1); Y = Y_ref(1) + 3; phi = 0; 
X_vec = zeros(1, sim_steps); Y_vec = zeros(1, sim_steps);


for ii = 1:sim_steps
    X_vec(ii) = X; Y_vec(ii) = Y;
    
    
    % ===============================================================
    
    % ================= TODO 2.1: 实现某种跟踪算法 =================
    
   %1.初始化查找参数：最小距离初始化为无穷大，目标索引默认轨迹末尾
     min_dist=inf;
     target_idx=length(X_ref); 
     
     %遍历全部参考轨迹点，筛选第一个距离车辆大于等于Ld的点作为预瞄点
     for k=1:length(X_ref)
         %大地坐标系下车辆与参考点的横纵向差值
         dx=X_ref(k)-X;
         dy=Y_ref(k)-Y;
         %两点欧式距离
         dist=sqrt(dx^2+dy^2);
         %筛选条件：距离大于等于预瞄距离，且是当前找到的最近点
         if dist>=Ld&&dist<min_dist
             min_dist=dist;
             target_idx=k;
         end
     end
     %2.坐标转换：将大地坐标系预瞄点转为车身局部坐标系
     dx_tar=X_ref(target_idx)-X;
     dy_tar=Y_ref(target_idx)-Y;
     
     %atan2(dy,dx)：大地坐标系预瞄点方位角；减去车身航向phi得到相对夹角
     alpha=atan2(dy_tar, dx_tar)-phi; %预瞄点相对于车辆车身纵轴的夹角
    
     %3.求解前轮机械转向角sigma
     sigma=atan2(2*lfr*sin(alpha),Ld);

    % ===============================================================

    % ================= TODO 2.2: 车辆状态更新 =================
    % 提示: 将刚才求得的转向角 sigma 代入运动学模型（复用第一问代码），更新 X, Y, phi。
    
  %运动学自行车模型横摆角速度
  phi_dot=v*tan(sigma)/lfr;
  %欧拉离散积分更新航向角
  phi=phi+phi_dot*dt;
  %全局X、Y坐标更新，纵向速度向坐标轴投影
  X=X+v*cos(phi)*dt;
  Y=Y+v*sin(phi)*dt;
    
    % ===============================================================
   
    % 到达终点提前结束
    if X >= X_ref(end), break; end
end

% 绘图对比
figure; hold on; grid on;
plot(X_ref, Y_ref, 'k--', 'LineWidth', 2);
plot(X_vec(1:ii), Y_vec(1:ii), 'r-', 'LineWidth', 2);
legend('参考规划轨迹', '实际行驶轨迹');
title(['Pure Pursuit 跟踪 (Ld = ', num2str(Ld), 'm)']);
xlabel('X [m]'); ylabel('Y [m]'); axis equal;