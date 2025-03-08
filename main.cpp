#include <opencv2/opencv.hpp>
#include <iostream>

//彩色图转灰度二值化图像
cv::Mat binary(cv::Mat frame)
{
    cv::cvtColor(frame,frame, cv::COLOR_BGR2GRAY); 

    int thresholdValue = 128;
    cv::threshold(frame,frame, thresholdValue, 255, cv::THRESH_BINARY);

    return frame;
}

// 降噪，腐蚀，膨胀
cv::Mat dilate(cv::Mat binary)
{
    // 高斯模糊，减少噪声
    cv::GaussianBlur(binary, binary, cv::Size(5, 5), 0);

    // 定义元素结构
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(9, 9));

    // 腐蚀
    cv::erode(binary,binary,element);

    // 膨胀
    cv::dilate(binary,binary,element);

    return binary;
}

// 查找并绘制轮廓
cv::Mat contour(cv::Mat dilate,cv::Mat frame)
{
    // canny边缘检测
    cv::Mat edges;
    cv::Canny(dilate, edges, 100, 200);

    // 查找轮廓
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(edges, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    cv::Mat contourImage = cv::Mat::zeros(frame.size(), CV_8UC3);

    // 绘制查找到的轮廓(红框)
    cv::drawContours(contourImage, contours, -1, cv::Scalar(0, 0, 255), 1);

    return contourImage;
}

int main()
 {
    // 输入和输出视频文件路径
    std::string inputVideoPath = "/home/mots/yzhy/opencv/20250223-213542.mp4";  // 输入视频路径
    std::string outputVideoPath = "/home/mots/yzhy/opencv/识别图像.mp4"; // 输出视频路径

    // 打开视频文件
    cv::VideoCapture cap(inputVideoPath);
    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open video." << std::endl;
        return -1;
    }
    // 原始图像
    cv::Mat frame;
    // 二值化图像
    cv::Mat binaryImage;
    // 腐蚀膨胀图像
    cv::Mat dilateImage;
    // 轮廓图像
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

    while (true) 
    {
        // 逐帧读取视频
        cap >> frame; 
        if (frame.empty())
            break; // 如果没有更多帧，则退出循环
        
        // 二值化图像
        binaryImage = binary(frame);

        // 突出明亮区域
        dilateImage = dilate(binaryImage);

        // 查找轮廓：
        contourImage = contour(binaryImage,frame);
        
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