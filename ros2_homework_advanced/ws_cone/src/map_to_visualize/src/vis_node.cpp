#include <rclcpp/rclcpp.hpp>  
#include <visualization_msgs/msg/marker_array.hpp>  //rviz可视化MarkerArray消息类型
#include <fsd_common_msgs/msg/map.hpp>   //自定义Map消息
#include <fsd_common_msgs/msg/cone.hpp>  //自定义Cone消息

//定义锥桶可视化节点
class ConeVisualizer : public rclcpp::Node
{
public:
  ConeVisualizer() : Node("cone_visualizer")
  {
    //订阅Map消息，队列大小10，每次收到消息调用mapCallback
    map_sub_ = this->create_subscription<fsd_common_msgs::msg::Map>(
      "/estimation/slam/map", 10,
      std::bind(&ConeVisualizer::mapCallback, this, std::placeholders::_1));

    //创建MarkerArray发布器，用于发送可视化标记到rviz
    marker_pub_ = this->create_publisher<visualization_msgs::msg::MarkerArray>(
      "/cone_markers", 10);

    RCLCPP_INFO(this->get_logger(), "ConeVisualizer started");  // 节点启动日志
  }

private:
  //Map消息回调函数，将锥桶信息转换为rviz Marker
  void mapCallback(const fsd_common_msgs::msg::Map::SharedPtr msg)
  {
    visualization_msgs::msg::MarkerArray marker_array;  //创建MarkerArray
    int id = 0;  //每个Marker的唯一ID

    //Lambda函数：将一组锥桶消息转换为Marker并添加到marker_array
    auto add_cones = [&](const std::vector<fsd_common_msgs::msg::Cone>& cones,
                         float r, float g, float b) {
      for (const auto& cone : cones) {
        visualization_msgs::msg::Marker marker;  //创建单个 Marker
        marker.header = msg->header;             //使用Map消息的时间戳和坐标系
        marker.ns = "cones";                      //命名空间
        marker.id = id++;                         //唯一 ID
        marker.type = visualization_msgs::msg::Marker::CYLINDER;  //圆柱形表示锥桶
        marker.action = visualization_msgs::msg::Marker::ADD;     //添加或更新
        marker.pose.position = cone.position;     //Marker位置与锥桶位置一致
        marker.pose.orientation.w = 1.0;          //默认朝向
        marker.scale.x = 0.2;                     //Marker宽度
        marker.scale.y = 0.2;                     //Marker深度
        marker.scale.z = 0.3;                     //Marker高度
        marker.color.r = r;                        //颜色R
        marker.color.g = g;                        //颜色G
        marker.color.b = b;                        //颜色B
        marker.color.a = 0.8;                      //半透明
        marker.lifetime = rclcpp::Duration::from_seconds(0.0);  //永久显示
        marker_array.markers.push_back(marker);    //添加到MarkerArray
      }
    };

    //分别添加不同类型的锥桶并指定颜色
    add_cones(msg->cone_yellow, 1.0, 1.0, 0.0);     //黄色锥桶
    add_cones(msg->cone_blue,   0.0, 0.0, 1.0);     //蓝色锥桶
    add_cones(msg->cone_red,    1.0, 0.0, 0.0);     //红色锥桶
    add_cones(msg->cone_unknown, 0.5, 0.5, 0.5);   //未知锥桶灰色

    //如果MarkerArray非空，则发布到cone_markers
    if (!marker_array.markers.empty()) {
      marker_pub_->publish(marker_array);
    }
  }

  rclcpp::Subscription<fsd_common_msgs::msg::Map>::SharedPtr map_sub_;  // Map 订阅器
  rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr marker_pub_;  // MarkerArray 发布器
};

//主函数，初始化ros2节点并循环运行
int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);  //初始化ros2
  rclcpp::spin(std::make_shared<ConeVisualizer>());  //运行节点，处理回调
  rclcpp::shutdown();  //退出ros2
  return 0;
}