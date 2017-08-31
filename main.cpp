#include "FastDPM.h"
#include "PM_type.h"
#include <conio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;
using namespace cv;
//#pragma comment(lib, "FastDPM-2014-May.lib")

int	main()
{	
		FastDPM	PM("\\test-lib\\person_final.txt");
		string	img_name = "\\test-lib\\01.jpg";
		Mat	img_uint8 = imread(img_name.c_str());
		if (img_uint8.empty()){
			//cout << "Cannot get image " << img_name << endl;
			getchar();
			return -2;
		}

		Mat	img = PM.prepareImg( img_uint8 );
		PM.detect( img, -1.0f, true, true );
		cvWaitKey(3000);
		cout<<"------------------------------------------------------------"<<endl;

 		return	0;
}