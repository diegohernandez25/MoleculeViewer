#include <iostream>
#include <ctype.h>
#include <map>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

void detect_text(string);
map<string,Mat> loadImages();
Point get_neighbour(Point, vector<Point>);

void show_img(string str, Mat img){
	namedWindow( str, cv::WINDOW_AUTOSIZE );
	imshow(str, img);
}

bool connected(Point a, Point b, vector<vector<Point> > connections){
	for (unsigned int i = 0; i < connections.size(); i++){
		Point na = get_neighbour(a, connections[i]);
		Point nb = get_neighbour(b, connections[i]);
		if ((na.x != 0 || na.y != 0) && (nb.x != 0 || nb.y != 0)) return true;
	}
	return false;
}

double calculate_distance(Point a, Point b){
	return sqrt(pow((b.x - a.x),2)+pow((b.y - a.y),2));
}

Point get_neighbour(Point atom, vector<Point> edges){
	int threshold = 30;
	for (unsigned int i = 0; i < edges.size(); i++){
		Point edge = edges[i];
		if (calculate_distance(atom, edge) <= threshold)
			return edge;
	}
	return Point(0,0);
}

void write_molecule(vector< tuple<string,Point> > atoms, vector< tuple<int, int> > cons, vector< tuple<double, double, double > > pos){
	for (unsigned int i = 0; i < atoms.size(); i++){
		cout << get<0>(atoms[i]) << " " << get<0>(pos[i]) << " " << get<1>(pos[i]) << " " << get<2>(pos[i]) << endl;
	}
	cout << "/" << endl;
	for (unsigned int i = 0; i < cons.size(); i++){
		cout << get<0>(cons[i])<<" "<< get<1>(cons[i]) << " 1" << endl;
	}
}

vector<int> get_connected(int idx, vector< tuple<int, int> > cons){
	vector<int> neighbours;
	for (unsigned int i = 0; i < cons.size(); i++){
		if (get<0>(cons[i]) == idx)
			neighbours.insert(neighbours.begin(), get<1>(cons[i]));
		if (get<1>(cons[i]) == idx)
			neighbours.insert(neighbours.begin(), get<0>(cons[i]));
	}
	return neighbours;
}

void make_molecule(vector< tuple<string,Point> > atoms, vector<vector<Point> > connections){
	vector< tuple<int, int> > cons;
	vector< tuple<double, double, double > > pos;
	for (unsigned int i = 0; i < atoms.size(); i++){
		pos.insert(pos.begin(), make_tuple(0,0,0));
	}
	for (unsigned int i = 0; i < atoms.size(); i++) {
		//cout << get<0>(atoms[i]) << " " << get<1>(atoms[i]) << endl;
		for (unsigned int j = i+1; j < atoms.size(); j++) {
			if (connected(get<1>(atoms[i]),get<1>(atoms[j]), connections)){
				cons.insert(cons.begin(), make_tuple(i,j));
				//cout << i << " and " << j << " are connected!" << endl;
			}
		}
	}
	for (unsigned int i = 0; i < atoms.size(); i++){
		vector<int> neighbours = get_connected(i, cons);
		for (unsigned int j = 0; j < neighbours.size(); j++){
			if (get<0>(pos[neighbours[j]]) == 0 && get<1>(pos[neighbours[j]]) == 0 && get<2>(pos[neighbours[j]]) == 0){
				pos[neighbours[j]] = pos[i];
				switch (j){
					case 0:
						get<0>(pos[neighbours[j]]) = get<0>(pos[i]) + 1.0;
						break;
					case 1:
						get<2>(pos[neighbours[j]]) = get<2>(pos[i]) + 1.0;
						break;
					case 2:
						get<0>(pos[neighbours[j]]) = get<0>(pos[i]) - 1.0;
						break;
					case 3:
						get<2>(pos[neighbours[j]]) = get<2>(pos[i]) - 1.0;
						break;
				}
			}
		}
	}
	write_molecule(atoms, cons, pos);
}

Mat insertROIs(vector<Point> contours,Mat image, Mat mask)
{

	Mat tmp;
	Rect rect = boundingRect(contours);
 	Mat maskROI(mask, rect);
	maskROI = Scalar(0, 0, 0);

	RotatedRect rrect = minAreaRect(contours);

	Mat M, rotated, cropped;
	float angle = rrect.angle;
	Size rectSize = rrect.size;

	if(rrect.angle < -45.)
	{
		angle += 90;
		swap(rectSize.width, rectSize.height);
	}

	M = getRotationMatrix2D(rrect.center, angle, 1.0);
	warpAffine(image, rotated, M, image.size(), INTER_CUBIC);
	getRectSubPix(rotated, rectSize, rrect.center, cropped);

	//show_img("cropped",cropped);

	return cropped;
}

map<string,Mat> loadImages()
{
	string letters[] = {"H","O","F","P","N","C"};
	map<string,Mat> tmpMap;
	Mat tmpMat;

	for(unsigned int i=0; i<sizeof(letters)/sizeof(letters[0]); i++)
	{
		string tmpString = letters[i];
		std::transform(tmpString.begin(), tmpString.end(), tmpString.begin(), ::tolower);
		tmpMat = imread("atoms/"+tmpString+".png");
		cvtColor(tmpMat, tmpMat, CV_BGR2GRAY);
		threshold(tmpMat, tmpMat, 50, 255.0, THRESH_BINARY);
		tmpMap[letters[i]] = tmpMat;
	}

	return tmpMap;
}


string FindLetters(Mat obj)
{
	float max_percentage=0;
	string atom = "";
	//cout<<"\nFindLetters" << endl;
	int MatchCount =0;
	int PixelNumber = 0;
	map<string,Mat> letters = loadImages();
	for (auto& x : letters)
	{
		MatchCount =0;
		PixelNumber = 0;
		Mat ittmp2 = x.second.clone();
		Mat ittmp = obj.clone();
		if(ittmp.rows == ittmp2.rows  && ittmp.cols == ittmp2.cols) cout<<"\nBoth images have the same size.";
		else if(ittmp.rows<ittmp2.rows && ittmp.cols<ittmp2.cols) resize(ittmp, ittmp, Size(ittmp2.cols, ittmp2.rows));
		else resize(ittmp2,ittmp2, Size(ittmp.cols,ittmp.rows));
		cvDestroyAllWindows();
		Mat subs;
		threshold(ittmp,ittmp, 50, 255.0, THRESH_BINARY);
		threshold(ittmp2,ittmp2, 50, 255.0, THRESH_BINARY);
		subtract(ittmp2,ittmp,subs);
		threshold(subs, subs, 50, 255.0, THRESH_BINARY);
		//show_img("1" ,ittmp);
		//show_img("2", ittmp2);
		//show_img("subs",subs);

		for(int x = 0; x<subs.rows; x++){
			for(int y=0; y<subs.cols; y++){
				if(subs.at<uchar>(x,y) == 255) MatchCount++;
				if(ittmp2.at<uchar>(x,y) == 255)  PixelNumber++;
			}
		}
		int totalPixels = subs.cols * subs.rows;
		//cout<<"total pixels:\t"<<to_string(totalPixels)<<endl;
		int whitePixels = MatchCount;
		//cout<<"total white pixels:\t"<<to_string(whitePixels)<<endl;
		int blackPixels = totalPixels - whitePixels;
		//cout<<"total black pixels:\t"<<to_string(blackPixels)<<endl;
		MatchCount=(PixelNumber)-MatchCount;
		float per = (double)((double)(MatchCount)/(double)(PixelNumber));
		float per2 = (double)((double)(blackPixels)/(double)(subs.cols*subs.rows));


		float total_per = (per + per2)/2;
		if(max_percentage < total_per){
			max_percentage = total_per;
			atom = x.first;
		}

		//if(per >= (float) 0.70) cout<<"\nFound it! It's a " << x.first;
		//else cout<<"\nNot found";
		//cout<<"\nMatched percentage parcial:\t"<<std::to_string(per)<<endl;
		//cout << "\nMath percentage total:\t"<<to_string(per2)<<endl;
		//waitKey(0);
	}

	cout<<"This character corresponds to:\t"<<(atom)<<endl;
	return atom;
}


int main(int argc, char** argv){
	detect_text(argv[1]);
	return 1;
}

void detect_text(string input){
    Mat large = imread(input);
	Mat large2 = large.clone();

	Mat small;
    cvtColor(large, small, CV_BGR2GRAY);
    Mat bw;
    threshold(small, bw, 80, 255.0, THRESH_BINARY_INV | THRESH_OTSU);

    Mat grad;
    Mat morphKernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    morphologyEx(bw, grad, MORPH_GRADIENT, morphKernel);

	Mat connected;
    morphKernel = getStructuringElement(MORPH_RECT, Size(3,3));
    morphologyEx(bw, connected, MORPH_CLOSE, morphKernel);

	Mat mask = Mat::zeros(bw.size(), CV_8UC1);
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours(connected, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    // filter contours
	vector< tuple<string,Point> > atoms;
	vector<vector<Point>> connections;
    for(int idx = 0; idx >= 0; idx = hierarchy[idx][0]){
        Rect rect = boundingRect(contours[idx]);
        Mat maskROI(mask, rect);
        maskROI = Scalar(0, 0, 0);
        // fill the contour
        drawContours(mask, contours, idx, Scalar(255, 255, 255), CV_FILLED);
		//show_img("mask", mask);
		//cout << contours[idx] << endl;
        RotatedRect rrect = minAreaRect(contours[idx]);
        double r = (double)countNonZero(maskROI) / (rrect.size.width * rrect.size.height);

        Scalar color;
        int thickness = 1;
		Point2f pts[4];
        rrect.points(pts);
        if (r > 0.25 && (rrect.size.height > 8 && rrect.size.width > 8) ){
            thickness = 2;
            color = Scalar(0, 255, 0);
			Moments mu = moments(contours[idx], false);
			Point center = Point(mu.m10/mu.m00 , mu.m01/mu.m00);
			//cout << center << endl;
			Mat letter = insertROIs(contours[idx], bw, mask);
			string l = FindLetters(letter);
			atoms.insert(atoms.begin(), make_tuple(l,center));
        }
        else {
            thickness = 1;
            color = Scalar(0, 0, 255);
			vector<Point> edges;
			Point p0, p1;
			if (abs(pts[0].x - pts[1].x) > abs(pts[0].x - pts[3].x)) {
				p0.x = (int)(pts[0].x+pts[3].x)/2;
				p0.y = (int)(pts[0].y+pts[3].y)/2;
				p1.x = (int)(pts[2].x+pts[1].x)/2;
				p1.y = (int)(pts[2].y+pts[1].y)/2;
			}
			else {
				p0.x = (int)(pts[0].x+pts[1].x)/2;
				p0.y = (int)(pts[0].y+pts[1].y)/2;
				p1.x = (int)(pts[2].x+pts[3].x)/2;
				p1.y = (int)(pts[2].y+pts[3].y)/2;
			}
			edges.insert(edges.begin(), p0);
			edges.insert(edges.begin(), p1);
			connections.insert(connections.begin(), edges);
      	}
        for (int i = 0; i < 4; i++)
        {
            line(large2, Point((int)pts[i].x, (int)pts[i].y), Point((int)pts[(i+1)%4].x, (int)pts[(i+1)%4].y), color, thickness);
        }
    }
	make_molecule(atoms, connections);
	show_img("final",large2);
	waitKey( 0 );
	destroyAllWindows();
    imwrite("cont.jpg", large2);
}
