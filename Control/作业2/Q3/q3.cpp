#include <iostream>
#include <iomanip>

//隔离C库头文件，解决链接兼容问题
extern "C" {
#include <osqp/osqp.h>
}

using c_int = OSQPInt;
using c_float = OSQPFloat;

int main()
{
    //优化变量数量：x、y
    c_int n = 2;
    //约束数量：一条约束不等式x+y≤4
    c_int m = 1; 

    //QP二次型矩阵P=[[1,0],[0,10]]
    c_float P_x[2] = {1.0, 10.0};  //非零元素的值数组，P矩阵是对角矩阵，非零元素分别为1和10
    c_int   P_i[2] = {0, 1};       //行索引数组，第0个元素在第0行，第1个元素在第1行
    c_int   P_p[3] = {0, 1, 2};    //P矩阵的CSC格式：第0列从索引0开始，第1列从索引1开始，元素总数2
    OSQPCscMatrix* P_mat = OSQPCscMatrix_new(n, n, 2, P_x, P_i, P_p);//创建P矩阵的CSC格式表示,对应数学上的矩阵

    //一次项q=[-3,-30]^T
    c_float q[2] = {-3.0, -30.0};

    //约束矩阵A=[1,1]（逻辑同上）
    c_float A_x[2] = {1.0, 1.0};
    c_int   A_i[2] = {0,    0};
    c_int   A_p[3] = {0, 1, 2};
    OSQPCscMatrix* A_mat = OSQPCscMatrix_new(m, n, 2, A_x, A_i, A_p);

    //约束上下界l≤Ax≤u，下界设为负无穷（因为只给了上界4）
    c_float l[1] = {-OSQP_INFTY};
    c_float u[1] = {4.0};

    //求解器参数
    OSQPSettings settings;
    osqp_set_default_settings(&settings);  //加载初始化求解器默认参数
    settings.verbose = 1;   //打印迭代过程

    //求解器句柄
    OSQPSolver* solver = nullptr;
    c_int ret = osqp_setup(&solver, P_mat, q, A_mat, l, u, m, n, &settings);  //设置求解器，传入问题数据和参数
    if (ret != 0)
    {
        std::cerr << "OSQP初始化失败，错误码：" << ret << std::endl;
        OSQPCscMatrix_free(P_mat);
        OSQPCscMatrix_free(A_mat);
        return -1;
    }

    //执行求解，求解器会用ADMM算法迭代优化，直到满足收敛条件或达到最大迭代次数
    osqp_solve(solver);

    //存储原始解与对偶乘子
    c_float x[2];
    c_float mu[1];
    c_float prim_inf_cert[1];
    c_float dual_inf_cert[1];
    OSQPSolution solution;  //结构体用于存储求解结果，包括原始解、对偶乘子以及收敛状态等信息
    solution.x = x;
    solution.y = mu;
    solution.prim_inf_cert = prim_inf_cert;
    solution.dual_inf_cert = dual_inf_cert;
    osqp_get_solution(solver, &solution);

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "==================== Q3 OSQP二次规划求解结果 ====================" << std::endl;
    std::cout << "最优横坐标 x* = " << x[0] << " 理论值：13/11 ≈ 1.181818" << std::endl;
    std::cout << "最优纵坐标 y* = " << x[1] << " 理论值：31/11 ≈ 2.818182" << std::endl;
    std::cout << "拉格朗日乘子 μ* = " << mu[0] << " 理论值：20/11 ≈ 1.818182" << std::endl;

    const OSQPInfo* info = solver->info;
    std::cout << "---------------------------------------------------------" << std::endl;
    std::cout << "收敛状态：" << info->status << std::endl;
    std::cout << "迭代总次数：" << info->iter << std::endl;
    std::cout << "=========================================================" << std::endl;

    //释放内存
    osqp_cleanup(solver);
    OSQPCscMatrix_free(P_mat);
    OSQPCscMatrix_free(A_mat);

    return 0;
}