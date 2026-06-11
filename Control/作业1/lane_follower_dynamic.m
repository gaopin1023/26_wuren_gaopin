%原有基础参数
lf = 2.168;
lr = 1.907;
lfr = lf + lr;
dt = 0.01;
v = 6;
sim_steps = 3760;
Ld = 12;

%新增动力学参数
m=1500;    %整车质量
Iz=2800;   %绕z轴转动惯量
Cf=18000;  %前轮侧偏刚度
Cr=18000;  %后轮侧偏刚度

%生成正弦参考规划轨迹 
%X范围0~200m，采样间隔0.1m
X_ref=0:0.1:200;
%正弦轨迹：幅值10m，空间周期30m
Y_ref=10*sin(X_ref/15);

%车辆初始状态（动力学比运动学多2个状态）
X=X_ref(1);          %全局X坐标初始值
Y=Y_ref(1)+3;        %初始横向偏移3m
phi=0;               %车身航向角
vy = 0;              %质心横向速度 
wz = 0;              %车辆横摆角速度 

%数组存储每一步坐标，用于仿真结束绘图
X_vec=zeros(1,sim_steps);
Y_vec=zeros(1,sim_steps);

%仿真主循环
for ii = 1:sim_steps
    %记录当前时刻车辆全局坐标
    X_vec(ii)=X;
    Y_vec(ii)=Y;

    %TODO2.1 控制算法：Pure Pursuit纯跟踪（和基础运动学版本一致）
    min_dist=inf;       %初始化最小距离为无穷大
    target_idx=length(X_ref); %默认预瞄点为轨迹最后一点

    %遍历全部离散参考点，筛选满足预瞄距离的最近前方点
    for k=1:length(X_ref)
        dx=X_ref(k)-X;
        dy=Y_ref(k)-Y;
        dist=sqrt(dx^2+dy^2); %欧式距离
        %筛选条件：距离大于等于预瞄距离，且是当前遍历到的最近点
        if dist>=Ld&&dist<min_dist
            min_dist=dist;
            target_idx=k;
        end
    end

    %大地坐标系预瞄点与车辆坐标差值
    dx_tar=X_ref(target_idx)-X;
    dy_tar=Y_ref(target_idx)-Y;
    %atan2(dy,dx)：求预瞄点全局方位角
    alpha=atan2(dy_tar, dx_tar)-phi; %alpha：预瞄点相对车身纵轴夹角
    %前轮转向角sigma
    sigma=atan2(2*lfr*sin(alpha),Ld);

    %TODO2.2拓展：动力学自行车模型状态更新（替换基础版运动学）
    %考虑轮胎侧偏力、车辆惯性，模拟真实车辆侧滑与动态滞后

    %计算前后轮侧偏角
    alpha_f=sigma-(vy+lf*wz)/v;   %前轮侧偏角
    alpha_r=-(vy-lr*wz)/v;        %后轮侧偏角

    Fyf= Cf * alpha_f;
    Fyr = Cr * alpha_r;
   
    dvydt=(Fyf+Fyr)/m -v*wz;    %横向速度变化率
    dwzdt=(lf*Fyf-lr*Fyr)/Iz;   %横摆角加速度

    %更新动态状态
    vy=vy+dvydt*dt;
    wz=wz+dwzdt*dt;
    phi=phi+wz*dt;   

    %全局坐标更新
    X=X+(v*cos(phi)-vy*sin(phi))*dt;
    Y=Y+(v*sin(phi)+vy*cos(phi))*dt;

    %车辆完全行驶到轨迹终点，跳出仿真循环
   if X>=X_ref(end), break; end
end 

%仿真结果绘图
figure; hold on; grid on;
%黑色虚线：规划参考正弦轨迹
plot(X_ref, Y_ref, 'k--', 'LineWidth', 2);
%红色实线：动力学模型车辆实际跟踪轨迹
plot(X_vec(1:ii), Y_vec(1:ii), 'r-', 'LineWidth', 2);
legend('参考规划轨迹', '实际行驶轨迹');
title(['Pure Pursuit动力学跟踪 (预瞄距离 Ld = ', num2str(Ld), 'm)']);
xlabel('X [m]'); ylabel('Y [m]');
axis equal;