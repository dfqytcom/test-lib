#include "jni.h"  
#include "jni_md.h"  
#include<opencv.hpp>
#include <opencv2\opencv.hpp>
#include "SCR.h"
#include <string>
#include "com_utry_javaCallDLL_AlgorithmDLL.h"
#include "common.h"

Mat getROI(Mat srcImage);
int  imgProcess(Mat imgPathName);

/*
 井盖异常识别
*/
JNIEXPORT jstring JNICALL Java_com_utry_javaCallDLL_AlgorithmDLL_SCR
(JNIEnv *env, jobject obj, jstring testIamgePath, jstring normalImagePath){
	
	string test = "";
	
	//string to jstring
	test = jstringToString(env, testIamgePath);
	//string normal = jstringToString(env, normalImagePath);
	
	//call algorithm
	string resultString = SCR(test, "");

	//string to jstring,then return
	jstring result = stringToJstring(env, resultString);

	return result;
}

/*
 根据传入的图片地址返回相应的判断结果
 1）窨井盖区域，即矩形区域左上角及右下角坐标点；
     2）窨井盖区域对应属性，比如：0表示：正常，1表示：异常；
[{  LeftTopPoint:{x:123,y:456},
    RightBottomPoint:{x:123,y:456},
    Result:0
},{
    LeftTopPoint:{x:123,y:456},
    RightBottomPoint:{x:123,y:456},
    Result:1
}]

*/
string SCR(string testImagePath, string normalImagePath)
{
	string result = "";
	string leftTopPoint = "";
	string rightBottomPoint = ""; 
	Mat src;

	try{
		src = imread(testImagePath);
	}
	catch (Exception e){

		cout << "SCR function .Loading picture error" << endl;
	}

	try{		

		if (src.empty()){

			cout << "SCR function,there is no picture !!  " << endl;
			return "";
		}
		//图片的宽、高
		int width = src.cols;
		int height = src.rows;

		//图片处理的结果返回，0正常，1异常
		int value = imgProcess(src);

		int x1 = 2*width / 3;
		int y1 = 2*height / 3;
		int x2 = 11 * width / 12;
		int y2 = 11 * height / 12;
	
		string x1x = "";
		string x2x = "";
		string y1y = "";
		string y2y = "";
		string valueString = "";

		stringstream ss;
		//数字与字符串的转化
		ss << x1;
		ss >> x1x;		
		ss.clear();	
		ss << y1;
		ss >> y1y;
		ss.clear();
		ss << x2;
		ss >> x2x;
		ss.clear();
		ss << y2;
    	ss >> y2y;
		ss.clear();

		leftTopPoint += "LeftTopPoint:{x:" + x1x + ",y:" + y1y + "},";
		rightBottomPoint += "RightBottomPoint:{x:" + x2x + ",y:" + y2y + "},";

		ss << value;
		ss >> valueString;
		ss.clear();		
		result += "Result:" + valueString;

		result = "{" + leftTopPoint + rightBottomPoint + result + "}";

		return  result;

	}
	catch (Exception e){
		
	}
	return "";

}
/*
 传入图片，返回划分出兴趣区域的图片,目前区域是人为指定 
*/
Mat getROI(Mat srcImage){

	//获得二值化图的宽高
	int width = srcImage.cols;
	int height = srcImage.rows;
	Mat roiImage;

	//切割出兴趣区域
	Rect rect(width / 3, height / 3, width / 2, height / 2);
	srcImage(rect).copyTo(roiImage);

	//对兴趣区域进行椭圆检测

	return roiImage;

}
//计算相连接的点的个数，超过一定个数，认为是有网子的

int countConnectedPoints(cv::Mat src){

	vector<vector<cv::Point>> contours;
	//寻找边界联通点
	cv::findContours(src, contours, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);

	int max_count = 0;

	for (int i = 0; i < contours.size(); i++){

		size_t count = contours[i].size();

		if (count > max_count){

			max_count = count;
		}
	}
	return max_count;
}
/*
 传入图片地址，判断指定的图片是否是正常的。正常返回0，异常返回1
 grayImage 待测图片

*/
int  imgProcess(Mat grayImage){

	//计算黑色像素个数
	int blackCount = 0;
	int whiteCount = 0;
	try{
		
		Mat grayImage0 = grayImage.clone();
		//imshow("源图像", grayImage);
		cvtColor(grayImage, grayImage0, CV_BGR2GRAY);

		//先将图片进行截取再进行放缩
		grayImage0 = getROI(grayImage0);

		//将图片进行放缩
		Size dsize = Size(576, 324);
		Mat grayImage2 = Mat(dsize, CV_32S);
		resize(grayImage0, grayImage2, dsize);
		//imshow("灰度图像", grayImage2);

		//确定ROI（目前是固定的）
		Mat ROI = getROI(grayImage2);

		if (1){

			cv::Mat testROI = ROI.clone();
			GaussianBlur(testROI, testROI, cv::Size(3, 3), 0, 0);
			//Canny边缘检测
			Canny(testROI, testROI, 180, 255, 3);
			//超过500的边缘阈值
			int count = countConnectedPoints(testROI);
			if (count >= 500){
				return 1;
			}
			else if (count <= 150){//考虑相机在光线较弱的时候有较多的黑点，所以在这个情况下
				return 0;
			}
		}
		
		Size ROISize = ROI.size();
		int height = ROISize.height;
		int width = ROISize.width;

		
		for (int i = 0; i <height; i++){
			for (int j = 0; j < width; j++){

				int t = *(ROI.ptr<uchar>(i) +j);
				if (t < 30)
				{
					blackCount += 1;
				}
				else if (t >235){
					whiteCount += 1;

				}
			}
		}
		if (blackCount > 300  || whiteCount > 600){
			//cout << "图片名称为：" << imgName << "的井盖可能出现异常" << endl;
			return 1;

		}
		else{
			//cout << "图片名称为：" << imgName << "的井盖未发现异常" << endl;
			return 0;
		}

	}
	catch (exception e){

	}
	return 0;
}
//void main(){
//	
//		
//	string testImagePath = "H:/c++project/manholeDetect/manholeDetect/trainImages/test/34.jpg";
//	//string testImagePath = "";
//	cout << SCR(testImagePath, "") << endl;
//		system("pause");	
//	
//}