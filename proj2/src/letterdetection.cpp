#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"




using namespace std;
using namespace cv;

#define PERCENT_MATCH = 0.9

bool pixelsMatch(Vec3f, Vec3f);
void showImageExit(string, Mat);
void showImage(string, Mat);
vector<Mat> insertROIs(vector<Vec4i>, vector<vector<Point>>,Mat, Mat);
void FindLetters(vector<Mat>, vector<Mat>);

int main(int argc, char** argv){

	//Vectors
	std::vector<Mat> img_rois;
	std::vector<Mat> img2_rois;

	Mat img= imread(argv[1]);
	Mat img_orig = img.clone();
	Mat img2= imread(argv[2]);

	//FIXME:Maybe Gaussian Blur?
	

	//TODO to GrayScale and Binary
	cvtColor(img,img,CV_BGR2GRAY);
	cvtColor(img2,img2,CV_BGR2GRAY);
	threshold(img,img,80,255.0, THRESH_BINARY_INV | THRESH_OTSU); //FIXME: Maybe change tresh value??
	threshold(img2,img2,80,255.0, THRESH_BINARY_INV | THRESH_OTSU);
	//showImageExit("img",img);
	//showImageExit("img2",img2);



	//FIXME: Morphological Gradient Needed???
	cout<<"\nMORPH_ELLIPSE";
	Mat structElem = getStructuringElement(MORPH_ELLIPSE, Size(3,3));
	morphologyEx(img, img, MORPH_GRADIENT, structElem);
	morphologyEx(img2, img2, MORPH_GRADIENT, structElem);
	//showImageExit("img",img);

	//connect regions
	cout<<"\nMORPH_CLOSE";
	morphologyEx(img,img,MORPH_CLOSE,structElem);
	morphologyEx(img2,img2,MORPH_CLOSE,structElem);
	//showImageExit("img",img);
	//TODO: Test in Dynamic Bar to see whats best

	//TODO restrict area
	cout<<"\nFind Contours";
	Mat mask = Mat::zeros(img.size(),CV_8UC1);
	Mat mask2 = Mat::zeros(img2.size(),CV_8UC1);
	
	vector<vector<Point>> contours;
	vector<vector<Point>> contours2;
	
	vector<Vec4i> hierarchy;
	vector<Vec4i> hierarchy2;

	findContours(img, contours, hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
	findContours(img2, contours2, hierarchy2,CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
	

	img_rois = insertROIs(hierarchy,contours,img, mask);
	img2_rois = insertROIs(hierarchy2,contours2,img2, mask2);
	
	FindLetters(img_rois, img2_rois);
	cout<<"\nPress any key to exit..."<<endl;
	cvDestroyAllWindows();
	return 1;
}


bool pixelsMatch(Vec3f p1, Vec3f p2){
	if(p1.val[0] == p2.val[0] && p1.val[1] == p2.val[1] && p1.val[2] == p2.val[2]) return true;
	return false;
}

void showImageExit(string str, Mat img)
{
	namedWindow(str,WINDOW_AUTOSIZE);
	imshow(str,img);
	waitKey(0);
	destroyWindow(str);
	
}

void showImage(string str, Mat img)
{
	namedWindow(str,WINDOW_AUTOSIZE);
	imshow(str,img);
	waitKey(0);
	
}

vector<Mat> insertROIs(vector<Vec4i> hierarchy, vector<vector<Point>> contours,Mat image, Mat mask)
{	
        
	vector<Mat> tmp;
	for(int idx=0;idx >=0; idx = hierarchy[idx][0])
	{
		Rect rect = boundingRect(contours[idx]);
        Mat maskROI(mask, rect);
        maskROI = Scalar(0, 0, 0);

        RotatedRect rrect = minAreaRect(contours[idx]);
		//TODO: Rotate rect to 0º
		
		Mat crop = image(rrect.boundingRect());
		tmp.insert(tmp.begin(),crop);
		showImageExit("contours",crop);
	}
	return tmp;
}

void FindLetters(vector<Mat> obj, vector<Mat> objpred)
{
	cout<<"\nFindLetters";
	for(vector<Mat>::iterator it = obj.begin(); it != obj.end(); ++it)
	{
		int MatchCount =0;
		for(vector<Mat>::iterator it2 = objpred.begin(); it2 != objpred.end(); ++it2)
		{
			Mat ittmp = *it;
			showImage("original",ittmp);			
			Mat ittmp2 = *it2;
			//Mat dst;	

			if(ittmp.rows == ittmp2.rows  && ittmp.cols == ittmp2.cols) cout<<"\nBoth images have the same size."; 	
			else if(ittmp.rows<ittmp2.rows && ittmp.cols<ittmp2.cols) resize(ittmp, ittmp, Size(ittmp2.cols, ittmp2.rows));
			else resize(ittmp2,ittmp2, Size(ittmp.cols,ittmp.rows));
			cvDestroyAllWindows();
			showImage("notinvertes",ittmp);
			showImage("inverted",ittmp2);
			Mat subs;
			subtract(ittmp,ittmp2,subs);
			showImage("subs",subs);

			for(int x = 0; x<subs.rows; x++){
				for(int y=0; y<subs.cols; y++){
					if(subs.at<Vec3b>(x,y).val[0] == 255) MatchCount++;
				}
			}
			MatchCount=(subs.rows*subs.cols)-MatchCount;
			float per = (double)((double)(MatchCount)/(double)(subs.rows*subs.cols));
			if(per >= (float) 0.90) cout<<"\nFound it!";
			else cout<<"\nNot found";
			cout<<"\nMatched percentage:\t"<<std::to_string(per)<<endl;
		}
	}
}




