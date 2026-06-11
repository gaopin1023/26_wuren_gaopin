#include <iostream>
#include <Eigen/Dense>  //Eigen线性代数库，此处用来存x、y二维向量
#include <cmath>

using namespace Eigen;
using namespace std;

int main()
{
    //初始状态X=[x;y]=[0;0]
    Vector2d X(0.0, 0.0);
    //理论最优解
    const Vector2d X_star(3.0, 3.0);
    double eta = 0.05;  //学习率，也就是步长，控制梯度下降移动幅度
    int iter_cnt = 0;   //迭代次数计数器
    const double eps = 1e-3;  //收敛精度阈值

    while (true)
    {
        //计算梯度
        Vector2d grad;  //定义二维向量存放梯度值
        grad(0) = X(0) - 3.0;   //对x求偏导，得到梯度分量
        grad(1) = 10.0 * (X(1) - 3.0); //对y求偏导，得到梯度分量

        //梯度下降更新
        X = X - eta * grad;
        iter_cnt++;  //迭代次数加1

        //计算2范数误差
        double error = (X - X_star).norm();  //计算当前解与最优解之间的误差范数
        if (error < eps)  //如果误差小于设定的阈值，认为已经收敛，退出循环
            break;
    }

    cout << "===== 梯度下降结果 =====" << endl;
    cout << "学习率 η = " << eta << endl;
    cout << "迭代总次数 = " << iter_cnt << endl;
    cout << "收敛解 x = " << X(0) << " , y = " << X(1) << endl;
    cout << "与(3,3)误差范数 = " << (X - X_star).norm() << endl;
    return 0;
}