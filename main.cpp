#include <opencv2/opencv.hpp>
#include <iostream>

int main()
 {
    // 输入和输出视频文件路径
    std::string inputVideoPath = "/home/mots/yzhy/opencv/20250223-213542.mp4";  // 输入视频路径
    std::string outputVideoPath = "/home/mots/yzhy/opencv/识别图像.mp4"; // 输出视频路径

    // 打开视频文件
    cv::VideoCapture cap(inputVideoPath);
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open video." << std::endl;
        return -1;
    }
    //原始图像
    cv::Mat frame;
    // 灰度图像
    cv::Mat grayImg;
    //裁剪后的图像
    cv::Mat croppedImage;
    //二值化图像
    cv::Mat binaryImage;
    //轮廓图像
    cv::Mat contourImage;

    // 获取视频的一些基本信息
    double fps = cap.get(cv::CAP_PROP_FPS); // 帧率
    int frameWidth = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH)); // 帧宽
    int frameHeight = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT)); // 帧高

    // 定义视频编码器和输出文件
    cv::VideoWriter out(outputVideoPath, cv::VideoWriter::fourcc('X', 'V', 'I', 'D'), fps, cv::Size(frameWidth, frameHeight));
    if (!out.isOpened()) {
        std::cerr << "Error: Could not open output video." << std::endl;
        return -1;
    }

    while (true) {
        // 逐帧读取视频
        cap >> frame; 
        if (frame.empty()) {
            break; // 如果没有更多帧，则退出循环
        }

        // 裁剪区域（x, y, width, height）
        int x = 300; // 裁剪区域左上角 x 坐标
        int y = 400; // 裁剪区域左上角 y 坐标
        int width = 600; // 裁剪区域的宽度
        int height = 600; // 裁剪区域的高度
        cv::Rect roi(x, y, width, height); // 定义感兴趣区域(roi矩形大小的重点)
        
        // 确保裁剪区域在图像范围内
        if (x + width <= frame.cols && y + height <= frame.rows) {
            croppedImage = frame(roi); // 裁剪图像
        }
        else{
            std::cout<< "ROI out of range, skipped." <<std::endl;
        }

        //彩色图转灰度图
        cv::cvtColor(frame, grayImg, cv::COLOR_BGR2GRAY); 

        // 设置二值化阈值，使用灰度图进行二值化操作
        int thresholdValue = 128;
        cv::threshold(grayImg, binaryImage, thresholdValue, 255, cv::THRESH_BINARY);

        // 查找轮廓：
        // 应用高斯模糊，减少噪声
        cv::GaussianBlur(binaryImage, binaryImage, cv::Size(5, 5), 0);

        // 边缘检测（Canny算法）
        cv::Mat edges;
        cv::Canny(binaryImage, edges, 100, 200);

        // 查找轮廓
        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(edges, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

        contourImage = cv::Mat::zeros(frame.size(), CV_8UC3);

        // 绘制查找到的轮廓(红框)
        cv::drawContours(contourImage, contours, -1, cv::Scalar(0, 0, 255), 1);

        // 在原始图像上绘制ROI矩形
        cv::rectangle(frame, roi, cv::Scalar(0, 255, 0), 2);
        cv::putText(frame, "ROI Area", cv::Point(x-10, y-10), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 0), 2);
        
        // 显示结果
        cv::imshow("Contour Image", contourImage);
        if (cv::waitKey(200) >= 0) break; //调整显示速度
        
        out << frame; //写入输出视频文件
    }

    // 释放资源
    cap.release();
    out.release();

    std::cout << "Video processing completed." << std::endl;
    return 0;
}