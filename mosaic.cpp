#include "opencv2/opencv.hpp"
#include <iostream>
#include <math.h>
#include <string.h>
#include <time.h>

#define DB 50000 // data 갯수
#define SMALL 8 // data resize할 크기

using namespace cv;
using namespace std;

int main() {
	clock_t programStart = clock();
	
	Mat img, data;
	int BGRavg[DB][3];
	int dataNumber = 0;

	// 모자이크 할 사진 로드, 크기 조정
	img = imread("C:/data/photo.jpg", IMREAD_COLOR);
	if (img.empty()) { return -1; }
	resize(img, img, Size(512, 512));
	
	clock_t dataStart = clock();

	// data를 읽고 BGR 평균을 배열에 저장
	for(int n=0; n<DB; n++) {
		char address[100] = "C:/mosaicData/img";
		char strmem[10];
		sprintf_s(strmem, "%05d", n);
		strcat_s(address, strmem);
		strcat_s(address, ".jpg");
		data = imread(address, IMREAD_COLOR);
		
		int blue = 0, green = 0, red = 0;
		for (int y = 0; y < data.rows; y++) {
			for (int x = 0; x < data.cols; x++) {
				blue += data.at<Vec3b>(y, x)[0];
				green += data.at<Vec3b>(y, x)[1];
				red += data.at<Vec3b>(y, x)[2];
			}
		}
		blue /= data.rows * data.cols;
		green /= data.rows * data.cols;
		red /= data.rows * data.cols;
		BGRavg[dataNumber][0] = blue;
		BGRavg[dataNumber][1] = green;
		BGRavg[dataNumber][2] = red;
		dataNumber++;
	}

	clock_t dataEnd = clock();
	printf("Data runtime: %.3f \n", (float)(dataEnd - dataStart) / CLOCKS_PER_SEC);

	clock_t mosaicStart = clock();

	// SMALL*SMALL 단위로 이동
	for (int i = 0; i < img.rows/SMALL; i++) {
		for (int j = 0; j < img.cols/SMALL; j++) {
			// SMALL*SMALL 부분의 평균 BGR 계산 
			int PBlue = 0, PGreen = 0, PRed = 0, min = 3*255*255, mem = 0;
			for (int y = SMALL * i; y < SMALL * (i + 1); y++) {
				for (int x = SMALL * j; x < SMALL * (j + 1); x++) {
					PBlue += img.at<Vec3b>(y, x)[0];
					PGreen += img.at<Vec3b>(y, x)[1];
					PRed += img.at<Vec3b>(y, x)[2];
				}
			}
			PBlue /= SMALL*SMALL; PGreen /= SMALL*SMALL; PRed /= SMALL*SMALL;
			
			// 가장 차이가 작은 사진의 번호 기억
			for (int d = 0; d < DB; d++) {
				int dis = pow(PBlue - BGRavg[d][0], 2.0) 
					    + pow(PGreen - BGRavg[d][1], 2.0)
					    + pow(PRed - BGRavg[d][2], 2.0);
				if (min > dis) {
					min = dis;
					mem = d;
				}
			}
			
			// 가장 차이가 작았던 사진을 로드하고 크기 조정
			Mat mini;
			char address[100] = "C:/mosaicData/img";
			char strmem[10];
			sprintf_s(strmem, "%05d", mem);
			strcat_s(address, strmem);
			strcat_s(address, ".jpg");
			mini = imread(address, IMREAD_COLOR);
			resize(mini, mini, Size(SMALL, SMALL));

			// 해당 위치에 삽입
			for (int y = SMALL * i; y < SMALL * (i + 1); y++) {
				for (int x = SMALL * j; x < SMALL * (j + 1); x++) {
					int my = y, mx = x;
					if (i > 0 || j > 0) { my = y % SMALL; mx = x % SMALL; }
					img.at<Vec3b>(y, x) = mini.at<Vec3b>(my, mx);
				}
			}
		}
	}

	clock_t mosaicEnd = clock();
	printf("Mosaic runtime: %.3f \n", (float)(mosaicEnd - mosaicStart) / CLOCKS_PER_SEC);
	
	clock_t programEnd = clock();
	printf("Total runtime: %.3f \n", (float)(programEnd - programStart) / CLOCKS_PER_SEC);

	imshow("Image", img);
	
	waitKey(0);
	return 0;
}