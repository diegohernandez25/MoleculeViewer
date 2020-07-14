#include <iostream>
#include <ctype.h>
#include <map>
#include <fstream>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

void detect(string);
map<string,Mat> loadImages();
Point get_neighbour(Point, vector<Point>, int);

const int thresh_max=255;
int threshold1= 30;
int thresh = 110;
int rad = 10;
Mat bw, src, large2;
string name;
vector< tuple<string,Point> > at;

void on_trackbar2(int, void*){
	Mat temp = large2.clone();
	for (unsigned int i = 0; i < at.size(); i++){
		circle(temp, get<1>(at[i]), rad, Scalar(0,0,255));
	}
	imshow("Radius", temp);
}

void on_trackbar(int, void*){
	threshold(src, bw, thresh, 255.0, THRESH_BINARY_INV);
	imshow( "Threshold", bw );
}

void show_img(string str, Mat img){
	namedWindow( str, cv::WINDOW_AUTOSIZE );
	imshow(str, img);
}

int connected(Point a, Point b, vector<vector<Point> > connections, int threshold){ //função que retorna numero de ligações entre dois átomos.
	int cons = 0;
	for (unsigned int i = 0; i < connections.size(); i++){
		Point na = get_neighbour(a, connections[i], threshold);
		Point nb = get_neighbour(b, connections[i], threshold);
		if ((na.x != -1 || na.y != -1) && (nb.x != -1 || nb.y != -1)) cons++;
	}
	return cons;
}

double calculate_distance(Point a, Point b){  //função que calcula distancia
	return sqrt(pow((b.x - a.x),2)+pow((b.y - a.y),2));
}

Point get_neighbour(Point atom, vector<Point> edges, int threshold){ //função que determina o vizinho de um certo átomo, se existir.
	for (unsigned int i = 0; i < edges.size(); i++){
		Point edge = edges[i];
		if (calculate_distance(atom, edge) <= 2*threshold)
			return edge;
	}
	return Point(-1,-1);
}

void write_molecule(vector< tuple<string,Point> > atoms, vector< tuple<int, int,int> > cons, vector< tuple<double, double, double > > pos){ //função que escreve a molécula no ficheiro
	ofstream file;
	file.open("output.txt");
	for (unsigned int i = 0; i < atoms.size(); i++){
		cout << get<0>(atoms[i]) << " " << get<0>(pos[i]) << " " << get<1>(pos[i]) << " " << get<2>(pos[i]) << endl;
		file << get<0>(atoms[i]) << " " << get<0>(pos[i]) << " " << get<1>(pos[i]) << " " << get<2>(pos[i]) << endl;
	}
	cout << "/" << endl;
	file << "/" << endl;
	for (unsigned int i = 0; i < cons.size(); i++){
		cout << get<0>(cons[i])<<" "<< get<1>(cons[i]) << " "<< get<2>(cons[i])<< endl;
		file << get<0>(cons[i])<<" "<< get<1>(cons[i]) << " "<< get<2>(cons[i])<< endl;
	}
	file.close();
}

vector<int> get_connected(int idx, vector< tuple<int, int,int> > cons){ // função que vai buscar átomos ligados a um certo átomo
	vector<int> neighbours;
	for (unsigned int i = 0; i < cons.size(); i++){
		if (get<0>(cons[i]) == idx)
			neighbours.insert(neighbours.begin(), get<1>(cons[i]));
		if (get<1>(cons[i]) == idx)
			neighbours.insert(neighbours.begin(), get<0>(cons[i]));
	}
	return neighbours;
}

bool occupied(tuple<double, double, double> p, vector< tuple<double, double, double > > pos){ // função que diz se uma posição ja está ocupada por outro átomo.
	for (unsigned int i = 0; i < pos.size(); i++){
		if(get<0>(pos[i]) == get<0>(p) && get<1>(pos[i]) == get<1>(p) && get<2>(pos[i]) == get<2>(p))
			return true;
	}
	return false;
}

void make_molecule(vector< tuple<string,Point> > atoms, vector<vector<Point> > connections, int threshold){
	vector< tuple<int, int, int> > cons;             // lista de tuplos com atomos ligados. Formato -> (atomo1, atomo2, número de ligaçoes)
	vector< tuple<double, double, double > > pos;    // Lista com as coordenadas dos átomos
	for (unsigned int i = 0; i < atoms.size(); i++){  // inicialização da lista com as coordenadas dos átomos com tudo a 0
		pos.insert(pos.begin(), make_tuple(0,0,0));
	}
	for (unsigned int i = 0; i < atoms.size(); i++) {
		//cout << get<0>(atoms[i]) << " " << get<1>(atoms[i]) << endl;
		for (unsigned int j = i+1; j < atoms.size(); j++) {
			if (connected(get<1>(atoms[i]),get<1>(atoms[j]), connections, threshold) != 0){   //Procura de todos os átomos ligados e tipo de ligação.
				cons.insert(cons.begin(), make_tuple(i,j,connected(get<1>(atoms[i]),get<1>(atoms[j]), connections, threshold)));
				cout << i << " and " << j << " are connected with " << connected(get<1>(atoms[i]),get<1>(atoms[j]), connections, threshold) << endl;
			}
		}
	}
	vector<int> alreadyPositioned; //lista com átomos ja posicionados
	for (unsigned int i = 0; i < atoms.size(); i++){  // algoritmos de decisao de coordenadas para os átomos.
		vector<int> neighbours = get_connected(i, cons);
		if (neighbours.size() > 1) { 											// primeiro posiciona-se aqueles com mais que duas ligações.
			alreadyPositioned.push_back(i);
			for (unsigned int j = 0; j < neighbours.size(); j++){
				if (find(alreadyPositioned.begin(), alreadyPositioned.end(), neighbours[j]) != alreadyPositioned.end() || get_connected(neighbours[j], cons).size() < 2){
					neighbours.erase(neighbours.begin() + j);
					j--;
				}
			}
			for (unsigned int j = 0; j < neighbours.size(); j++){
				cout << "Atom is: "<<get<0>(atoms[i]) << ", In position: " <<  get<0>(pos[i]) <<"," << get<1>(pos[i]) <<","<< get<2>(pos[i]) << endl;
				//cout << get_connected(neighbours[j], cons).size() << endl;
				pos[neighbours[j]] = pos[i];
				switch (j){
					case 0:
						if (get<0>(pos[i]) < 0) get<0>(pos[neighbours[j]]) = get<0>(pos[i]) - 1.0;
						else
							get<0>(pos[neighbours[j]]) = get<0>(pos[i]) + 1.0;
						break;
					case 1:
						if (get<2>(pos[i]) < 0) get<0>(pos[neighbours[j]]) = get<0>(pos[i]) - 1.0;
						else
							get<2>(pos[neighbours[j]]) = get<2>(pos[i]) + 1.0;
						break;
					case 2:
						if (get<0>(pos[i]) < 0) get<0>(pos[neighbours[j]]) = get<0>(pos[i]) + 1.0;
						else
							get<0>(pos[neighbours[j]]) = get<0>(pos[i]) - 1.0;
						break;
					case 3:
						if (get<2>(pos[i]) < 0) get<0>(pos[neighbours[j]]) = get<0>(pos[i]) + 1.0;
						else
							get<2>(pos[neighbours[j]]) = get<2>(pos[i]) - 1.0;
					}
					cout << "Atom out is: "<<get<0>(atoms[neighbours[j]]) << ", In position: " <<  get<0>(pos[neighbours[j]]) <<"," << get<1>(pos[neighbours[j]]) <<","<< get<2>(pos[neighbours[j]]) << endl;
			}
		}
	}
	for (unsigned int i = 0; i < atoms.size(); i++){
		vector<int> neighbours = get_connected(i, cons);
		if (neighbours.size() > 1) {                        // depois posicionam-se atomos com so uma ligação.
			alreadyPositioned.push_back(i);
			for (unsigned int j = 0; j < neighbours.size(); j++){
				if (find(alreadyPositioned.begin(), alreadyPositioned.end(), neighbours[j]) != alreadyPositioned.end()){
					neighbours.erase(neighbours.begin() + j);
					j--;
				}
			}
			for (unsigned int j = 0; j < neighbours.size(); j++){
				cout << "Atom is: "<<get<0>(atoms[i]) << ", In position: " <<  get<0>(pos[i]) <<"," << get<1>(pos[i]) <<","<< get<2>(pos[i]) << endl;
				//cout << get_connected(neighbours[j], cons).size() << endl;
				tuple<double, double, double> fpos = pos[i];
				if (!occupied(make_tuple(get<0>(fpos) + 1, get<1>(fpos) , get<2>(fpos)), pos))
					get<0>(fpos) += 1.0;
				else if (!occupied(make_tuple(get<0>(fpos), get<1>(fpos) , get<2>(fpos)+1), pos))
					get<2>(fpos) += 1.0;
				else if (!occupied(make_tuple(get<0>(fpos) - 1, get<1>(fpos) , get<2>(fpos)), pos))
					get<0>(fpos) -= 1.0;
				else if (!occupied(make_tuple(get<0>(fpos), get<1>(fpos) , get<2>(fpos)-1), pos))
					get<2>(fpos) -= 1.0;
				pos[neighbours[j]] = fpos;
				cout << "Atom out is: "<<get<0>(atoms[neighbours[j]]) << ", In position: " <<  get<0>(pos[neighbours[j]]) <<"," << get<1>(pos[neighbours[j]]) <<","<< get<2>(pos[neighbours[j]]) << endl;
			}
		}
	}
	write_molecule(atoms, cons, pos); // funçãp para gerar conteudo do ficheiro para simulação
}

Mat insertROIs(vector<Point> contours,Mat image, Mat mask)
{

	Mat tmp;
	Rect rect = boundingRect(contours);
 	Mat maskROI(mask, rect);
	maskROI = Scalar(0, 0, 0);

	RotatedRect rrect = minAreaRect(contours);
	//TODO: Rotate rect to 0º

	Mat M, rotated, cropped;
	float angle = rrect.angle;
	Size rectSize = rrect.size;

	if(rrect.angle < -45.)
	{
		angle += 90;
		swap(rectSize.width, rectSize.height);
	}

	//get the rotation matrix
	M = getRotationMatrix2D(rrect.center, angle, 1.0);
	// perform the affine transformation
	warpAffine(image, rotated, M, image.size(), INTER_CUBIC);
	// crop the resulting image
	getRectSubPix(rotated, rectSize, rrect.center, cropped);

	//show_img("cropped",cropped);

	return cropped;
}

map<string,Mat> loadImages()
{
	string letters[] = {"H","O","F","P","N","C","I"};
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
	cout<<"\nFindLetters" << endl;
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

		if(per >= (float) 0.70) cout<<"\nFound it! It's a " << x.first;
		else cout<<"\nNot found";
		cout<<"\nMatched percentage parcial:\t"<<std::to_string(per)<<endl;
		cout << "\nMath percentage total:\t"<<to_string(per2)<<endl;
		//waitKey(0);
	}

	cout<<"This character corresponds to:\t"<<(atom)<<endl;
	return atom;
}


int main(int argc, char** argv){
	detect(argv[1]);
	return 1;
}

void detect(string input){
	vector< tuple<string,Point> > atoms;
	vector<vector<Point>> connections;
	int limit = 3;
	atoms.clear();
	connections.clear();
    Mat large = imread(input);
	if (large.cols > 1000 || large.rows > 600)
		resize(large,large, Size(1000,600));
	large2 = large.clone();

    cvtColor(large, src, CV_BGR2GRAY);
	//show_img("before gaussian", small);
	//GaussianBlur( small, small, cv::Size( 3, 3 ),0 );
	//show_img("gaussian", small);

	namedWindow("Threshold", WINDOW_AUTOSIZE);
	createTrackbar( "Threshold", "Threshold", &thresh, thresh_max, on_trackbar );
    on_trackbar( thresh, 0 );
    waitKey(0);

	show_img("threshold", bw);
    Mat grad;
    Mat morphKernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    morphologyEx(bw, grad, MORPH_OPEN, morphKernel);

	Mat connected;
    morphKernel = getStructuringElement(MORPH_RECT, Size(3,3));
    morphologyEx(bw, connected, MORPH_CLOSE, morphKernel);

	Mat mask = Mat::zeros(bw.size(), CV_8UC1);
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours(connected, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    // filter contours
    for(int idx = 0; idx >= 0; idx = hierarchy[idx][0]){
        Rect rect = boundingRect(contours[idx]);
        Mat maskROI(mask, rect);
        maskROI = Scalar(0, 0, 0);
        // fill the contour
        drawContours(mask, contours, idx, Scalar(255, 255, 255), CV_FILLED);
		//show_img("mask", mask);
				cout << "contours: " << contours[idx] << endl;
        RotatedRect rrect = minAreaRect(contours[idx]);

        Scalar color;
        int thickness = 1;
		Point2f pts[4];
        rrect.points(pts);
		bool is_line = rrect.size.height/rrect.size.width > limit || rrect.size.height/rrect.size.width < 1.0/(double)limit;
		if (!is_line){
				if (contours[idx].size() > 4) {
					cout << "contours size: " << contours[idx].size() << endl;
					thickness = 2;
					color = Scalar(0, 255, 0);
					Moments mu = moments(contours[idx], false);
					Point center = Point(mu.m10/mu.m00 , mu.m01/mu.m00);
					cout << "center: " << center << endl;
					Mat letter = insertROIs(contours[idx], bw, mask);
					string l = FindLetters(letter);
					atoms.insert(atoms.begin(), make_tuple(l,center));
				}
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
						if (rrect.size.height/rrect.size.width > 3)
							threshold1 = (int)rrect.size.height*5;
						else
							threshold1 = (int)rrect.size.width*5;
						cout << "Threshold is: " << threshold1 << endl;
      	}
        for (int i = 0; i < 4; i++)
        {
            line(large2, Point((int)pts[i].x, (int)pts[i].y), Point((int)pts[(i+1)%4].x, (int)pts[(i+1)%4].y), color, thickness);
        }
    }
		show_img("final",large2);
		waitKey(0);

		at = atoms;
		namedWindow("Radius", WINDOW_AUTOSIZE);
		createTrackbar( "Radius", "Radius", &rad, threshold1, on_trackbar2);
    	on_trackbar2( rad, 0 );
    	waitKey(0);

		make_molecule(atoms, connections, rad);
}
