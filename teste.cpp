#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <vector>
#include <string>

using namespace std;
using namespace cv;

void detectFace(Mat &, CascadeClassifier, CascadeClassifier, Mat, Mat);

void overlayImage(const Mat, const Mat, Mat &, Point); //opencv community
int foi;

int main(int argc, char **argv) {

	CvCapture* captura;
	Mat frame;
	Mat oculos = imread("thuglifespecs.png", -1); //IMREAD_UNCHANGED   CV_LOAD_IMAGE_UNCHANGED
	Mat chifrin = imread("chifre.png", -1);
	CascadeClassifier olho;
	CascadeClassifier face;

	if (!olho.load("haarcascade_eye_tree_eyeglasses.xml")) { //haarcascade_mcs_righteye   haarcascade_eye_tree_eyeglasses.xml
		cout << "--(!)Erro ao tentar abrir arq olhos" << endl;
		return 1;
	};

	if (!face.load("haarcascade_frontalface_alt.xml")) {
		cout << "--(!)Erro ao tentar abrir arq face" << endl;
		return 1;
	};

	captura = cvCaptureFromCAM(-1);

	while(1) {
		frame = cvQueryFrame(captura);
		detectFace(frame, olho, face, oculos, chifrin);
		cvNamedWindow("Thug", CV_WINDOW_AUTOSIZE); //CV_WINDOW_AUTOSIZE
		imshow("Thug", frame);
		char c = cvWaitKey(33);
		if(c == 27) break;
	}
	cvReleaseCapture(&captura);
	cvDestroyAllWindows();
	return 0;
}

void detectFace(Mat &frame, CascadeClassifier olho, CascadeClassifier face, Mat oclinho, Mat chifrin) {
	vector <Rect> olhos;
	vector <Rect> faces;
	Mat frame_gray;
	Mat subImages, oclinho2, dest, chifrin2;
	Mat corte;

	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	face.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

	for (size_t i = 0; i < faces.size(); i++) {

		Point pt1(faces[i].x + faces[i].width, faces[i].y + faces[i].height);
		Point pt2(faces[i].x, faces[i].y);

		//rectangle(frame, pt1, pt2, cvScalar(0, 255, 0, 30), 1, 8, 0);

		Rect ROIFACE(faces[i].x - 30, faces[i].y - 70, faces[i].width + 70, faces[i].height - 25);
		resize(chifrin, chifrin2, Size(faces[i].width + 70, faces[i].height - 25), CV_INTER_LINEAR);
		subImages = frame_gray(ROIFACE);
		chifrin2.copyTo(subImages);
		overlayImage(frame, subImages, dest, Point(faces[i].x - 30, faces[i].y - 70));
		dest.copyTo(frame);

		/*resize(oclinho, oclinho2, Size(olhos[j].width + 115, olhos[j].height + 20), 0.5, 0.5, CV_INTER_LINEAR);
		subImages = frame_gray(ROI);
		oclinho2.copyTo(subImages);
		overlayImage(frame, subImages, dest, Point(olhos[j].x - 10, olhos[j+1].y - 10));
		dest.copyTo(frame);*/

		/*olho.detectMultiScale(frame_gray, olhos, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

		for (size_t j = 0; j < olhos.size(); j++) { //olhos.size();
			 //Point(olhos[j].x - 10, olhos[j].y - 10)

			rectangle(frame, pt1, pt2, cvScalar(0, 255, 0, 30), 1, 8, 0);
			if (olhos.size() >= 2) {
				if (olhos[j].width < olhos[j+1].width && olhos[j].x + olhos[j].width <= faces[i].width * faces[i].height) {

					Point pt11(olhos[j].x + olhos[j].width, olhos[j].y + olhos[j].height);
					Point pt22(olhos[j].x, olhos[j].y);

					rectangle(frame, pt11, pt22, cvScalar(255, 0, 0, 30), 1, 8, 0);

				}
			}

			Rect ROI(olhos[j].x - 20, olhos[j].y - 10, olhos[j].width + 115, olhos[j].height + 20);
			resize(oclinho, oclinho2, Size(olhos[j].width + 115, olhos[j].height + 20), 0.5, 0.5, CV_INTER_LINEAR);
			subImages = frame_gray(ROI);
			oclinho2.copyTo(subImages);
			overlayImage(frame, subImages, dest, Point(olhos[j].x - 10, olhos[j].y - 10));
			dest.copyTo(frame);

		}*/

	}

}

void overlayImage(const Mat background, const Mat foreground, Mat &output, Point location) {
  
  background.copyTo(output);
 
	for(int y = std::max(location.y , 0); y < background.rows; ++y) {
  		int fY = y - location.y;
 
    	if(fY >= foreground.rows) break;
 
    	for(int x = std::max(location.x, 0); x < background.cols; ++x) {
			int fX = x - location.x;
 
    		if(fX >= foreground.cols) break;
 
      		double opacity = ((double)foreground.data[fY * foreground.step + fX * foreground.channels() + 3])/255.;
 
      		for(int c = 0; opacity > 0 && c < output.channels(); ++c) {
		        unsigned char foregroundPx = foreground.data[fY * foreground.step + fX * foreground.channels() + c];
		        unsigned char backgroundPx = background.data[y * background.step + x * background.channels() + c];
		        output.data[y*output.step + output.channels()*x + c] = backgroundPx * (1.-opacity) + foregroundPx * opacity;
		    }
    	}
  	}
}
