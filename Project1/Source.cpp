#include "opencv2/opencv.hpp" 
#include "opencv2\imgproc\imgproc.hpp"
#include <iostream>
using namespace cv;
using namespace std;

static double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

int main()
{
	Mat img, gray, bw, dst;
	VideoCapture cam(0);
	std::vector<std::vector<Point>> contours;
	std::vector<Point> approx;

	img = imread("shapes.jpg");
	while (1)
	{
		cam.read(img);
		cvtColor(img, gray, COLOR_BGR2GRAY);

		//detect circles
		vector<Vec3f> circles;
		HoughCircles(gray, circles, HOUGH_GRADIENT, 2, gray.rows / 4, 200, 100);
		for (size_t i = 0; i < circles.size(); i++)
		{
			Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
			int radius = cvRound(circles[i][2]);
			// draw the circle center
			circle(img, center, 3, Scalar(0, 255, 0), -1, 8, 0);
			// draw the circle outline
			circle(img, center, radius, Scalar(0, 0, 255), 3, 8, 0);
		}

		blur(gray, bw, Size(3, 3));
		Canny(gray, bw, 80, 240, 3);

		findContours(bw.clone(), contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		img.copyTo(dst);

		for (int i = 0; i < contours.size(); i++)
		{
			
			// Approximate contour with accuracy proportional
			// to the contour perimeter
			cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);

			// Skip small or non-convex objects
			if (std::fabs(cv::contourArea(contours[i])) < 100 || !cv::isContourConvex(approx))
				continue;



			for (int j = 0; j < contours[i].size() - 1; j++)
			{
				//cout << contours[i][j] << "\n";
				line(dst, contours[i][j], contours[i][j + 1], Scalar(94, 206, 165, 255), 2);
			}

			if (approx.size() == 3)
			{

				std::cout << "tri";
				putText(dst, "tri", contours[i][0], FONT_HERSHEY_SIMPLEX, 1, Scalar(94, 206, 165, 255), 2);
				for (int j = 0; j < contours[i].size() - 1; j++)
				{
					//cout << contours[i][j] << "\n";
					line(dst, contours[i][j], contours[i][j + 1], Scalar(94, 206, 165, 255), 2);
				}
			}
			else if (approx.size() >= 4 && approx.size() <= 6)
			{
				std::cout << "4";
				// Number of vertices of polygonal curve
				int vtc = approx.size();

				// Get the cosines of all corners
				std::vector<double> cos;
				for (int j = 2; j < vtc + 1; j++)
					cos.push_back(angle(approx[j%vtc], approx[j - 2], approx[j - 1]));

				// Sort ascending the cosine values
				std::sort(cos.begin(), cos.end());

				// Get the lowest and the highest cosine
				double mincos = cos.front();
				double maxcos = cos.back();

				// Use the degrees obtained above and the number of vertices
				// to determine the shape of the contour
				if (vtc == 4)
				{
					std::cout << "rect\n";
					//contours.at(i);

					Point p;
					p.x = 50;
					p.y = 50;
					Scalar s;
					s.all(50);

					
					putText(dst, "Rect", contours[i][0], FONT_HERSHEY_SIMPLEX, 1, Scalar(94, 206, 165, 255), 2);
				}
				else if (vtc == 5)
				{
					putText(dst, "Penta", contours[i][0], FONT_HERSHEY_SIMPLEX, 1, Scalar(94, 206, 165, 255), 2);
					std::cout << "penta\n";
				}
				else if (vtc == 6)
				{
					putText(dst, "Hexa", contours[i][0], FONT_HERSHEY_SIMPLEX, 1, Scalar(94, 206, 165, 255), 2);
					std::cout << "hexa\n";
				}
				for (int j = 0; j < contours[i].size() - 1; j++)
				{
					//cout << contours[i][j] << "\n";
					line(dst, contours[i][j], contours[i][j + 1], Scalar(94, 206, 165, 255), 2);
				}
				//setLabel(dst, "HEXA", contours[i]);
			}
			else
			{
				putText(dst, "Circle", contours[i][0], FONT_HERSHEY_SIMPLEX, 1, Scalar(94, 206, 165, 255), 2);
				std::cout << "circle\n";
				for (int j = 0; j < contours[i].size() - 1; j++)
				{
					//cout << contours[i][j] << "\n";
					line(dst, contours[i][j], contours[i][j + 1], Scalar(94, 206, 165, 255), 2);
				}
				// Detect and label circles
				/*double area = cv::contourArea(contours[i]);
				cv::Rect r = cv::boundingRect(contours[i]);
				int radius = r.width / 2;

				if (std::abs(1 - ((double)r.width / r.height)) <= 0.2 &&
					std::abs(1 - (area / (CV_PI * (radius*radius)))) <= 0.2)
					setLabel(dst, "CIR", contours[i]);*/
			}
		}


		imshow("dst", dst);
		imshow("image", img);
		imshow("gray", gray);
		imshow("bw", bw);
		waitKey(1);
	}

	waitKey(0);
	return 0;
}

