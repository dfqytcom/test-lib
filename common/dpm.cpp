#pragma execution_character_set("utf-8") //默认使用UTF8 
#include "FastDPM.h"
#include <conio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;
using namespace cv;
#pragma comment(lib, "FastDPM-2014-May.lib")


//#include "makeDll.h"
#include "jni.h"  
#include "jni_md.h"  
#include "com_utry_javaCallDLL_AlgorithmDLL.h" 
#include "common.h"


//using namespace cv;
// %OPENCV%\x86\vc12\lib opencv_world300d.lib;


	string getResultString(string plateNumber, RotatedRect rr){

		string result = "";

		string LeftTopPoint = "";
		string RightBottomPoint = "";
		string Result = "";

		double x1 = rr.center.x - rr.size.width / 2;
		double y1 = rr.center.y - rr.size.height / 2;
		double x2 = rr.center.x + rr.size.width / 2;
		double y2 = rr.center.y + rr.size.height / 2;


		string x1x = "";
		string y1y = "";
		string x2x = "";
		string y2y = "";

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

		LeftTopPoint += "LeftTopPoint:{x:" + x1x + ",y:" + y1y + "},";
		RightBottomPoint += "RightBottomPoint:{x:" + x2x + ",y:" + y2y + "},";
		Result += "Result:'" + plateNumber+"'";

		result += "{" + LeftTopPoint + RightBottomPoint + Result + "}";

		return  result;
	}


	JNIEXPORT jstring JNICALL Java_com_utry_javaCallDLL_AlgorithmDLL_DPM(JNIEnv *env, jobject obj, jstring s, jstring model_txt){

	
	//turn jstring to string 
	string img_namePath = jstringToString(env, s);
	string model_Path = jstringToString(env, model_txt);
	//cout <<"dll:"<< platePath << endl;

	//call algorithm

	//FastDPM	PM( "model_inria_14_2_features.txt" );
	//FastDPM	PM("\\test-lib\\person_final.txt");

	FastDPM	PM(model_Path);

	Mat	img_uint8 = imread(img_namePath.c_str());
	if (img_uint8.empty()){
		cout << "Cannot get image " << img_namePath << endl;
	}

	Mat	img = PM.prepareImg(img_uint8);
	PM.detect(img, -1.0f, true, true);
	cvWaitKey(3000);
	cout << "------------------------------------------------------------" << endl;

	//turn string to jstring	
	//jstring result = stringToJstring(env, value);
	
	//return result;
	return 0;
}
