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
 �����쳣ʶ��
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
 ���ݴ����ͼƬ��ַ������Ӧ���жϽ��
 1��񿾮�����򣬼������������ϽǼ����½�����㣻
     2��񿾮�������Ӧ���ԣ����磺0��ʾ��������1��ʾ���쳣��
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
		//ͼƬ�Ŀ���
		int width = src.cols;
		int height = src.rows;

		//ͼƬ����Ľ�����أ�0������1�쳣
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
		//�������ַ�����ת��
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
 ����ͼƬ�����ػ��ֳ���Ȥ�����ͼƬ,Ŀǰ��������Ϊָ�� 
*/
Mat getROI(Mat srcImage){

	//��ö�ֵ��ͼ�Ŀ��
	int width = srcImage.cols;
	int height = srcImage.rows;
	Mat roiImage;

	//�и����Ȥ����
	Rect rect(width / 3, height / 3, width / 2, height / 2);
	srcImage(rect).copyTo(roiImage);

	//����Ȥ���������Բ���

	return roiImage;

}
//���������ӵĵ�ĸ���������һ����������Ϊ�������ӵ�

int countConnectedPoints(cv::Mat src){

	vector<vector<cv::Point>> contours;
	//Ѱ�ұ߽���ͨ��
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
 ����ͼƬ��ַ���ж�ָ����ͼƬ�Ƿ��������ġ���������0���쳣����1
 grayImage ����ͼƬ

*/
int  imgProcess(Mat grayImage){

	//�����ɫ���ظ���
	int blackCount = 0;
	int whiteCount = 0;
	try{
		
		Mat grayImage0 = grayImage.clone();
		//imshow("Դͼ��", grayImage);
		cvtColor(grayImage, grayImage0, CV_BGR2GRAY);

		//�Ƚ�ͼƬ���н�ȡ�ٽ��з���
		grayImage0 = getROI(grayImage0);

		//��ͼƬ���з���
		Size dsize = Size(576, 324);
		Mat grayImage2 = Mat(dsize, CV_32S);
		resize(grayImage0, grayImage2, dsize);
		//imshow("�Ҷ�ͼ��", grayImage2);

		//ȷ��ROI��Ŀǰ�ǹ̶��ģ�
		Mat ROI = getROI(grayImage2);

		if (1){

			cv::Mat testROI = ROI.clone();
			GaussianBlur(testROI, testROI, cv::Size(3, 3), 0, 0);
			//Canny��Ե���
			Canny(testROI, testROI, 180, 255, 3);
			//����500�ı�Ե��ֵ
			int count = countConnectedPoints(testROI);
			if (count >= 500){
				return 1;
			}
			else if (count <= 150){//��������ڹ��߽�����ʱ���н϶�ĺڵ㣬��������������
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
			//cout << "ͼƬ����Ϊ��" << imgName << "�ľ��ǿ��ܳ����쳣" << endl;
			return 1;

		}
		else{
			//cout << "ͼƬ����Ϊ��" << imgName << "�ľ���δ�����쳣" << endl;
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