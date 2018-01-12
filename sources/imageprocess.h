#pragma once
#include "plcserial.h"
#include "robustmatcher.h"
#include <QObject>
#include <opencv2/opencv.hpp>

#define MISS_TEST_SPEED -8.88

struct ImageProcessParameters {
	ImageProcessParameters();
	double angle2Speed;
	double warningRatio;
	double alarmRatio;
	int radius_min;
	int radius_max;
	int gs1;
	int gs2;
	double dp;
	double minDist;
	double param1;
	double param2;
	bool sensorTriggered;
	cv::Rect roiRect;
};

class OCR;

class ImageProcess : public QObject {
	Q_OBJECT
public:
	ImageProcess(QObject *parent = Q_NULLPTR);
	~ImageProcess();

	static ImageProcessParameters g_imgParam;
	cv::Mat imageMatches;
	cv::Mat frameToShow;

private:
	cv::Mat srcImg;
	cv::Mat calibratedFrame;
	double refSpeed;
	OCR * ocr;
	// int iImgNoCycle;
	cv::Mat cameraUndistort(cv::Mat src);
	int previousAlarmLevel(const QString & num) const;
	int coreImageProcess(); // 0-no cycle, 1-matches success

	void foreverPreProcess();

	void alarmThisWheel();
	std::vector<double> rtSpeeds;
	std::vector<double> refSpeeds;
	int nImgCount; // has 0/1/2 src imgs to match. when 2, begin to match
				   // process; when 0,1, wait the next src img

	int nFragments;
	// void resetCoreProcess();
	bool bIsInArea;
	bool bWheelStopped;
	bool bStopProcess;
	// QTime time;
	// int in_out_time; //ms
	QMutex mutex;
	RobustMatcher rMatcher;

signals:
	// void resultReady(const QString &result);
	void imageProcessReady();
	void showAlarmNum(const QString &s);
	void showRealtimeImage();
	void showImageMatches();
	void setAlarmLight(PLCSerial::AlarmColor alarmcolor);

	void showWheelNum(const QString &s);
	void showWheelSpeed(double speed);

	void insertRecord(const QString &num, int alarmLevel, double absError, double refspeed, int fragment, int totalmatch, int validmatch, const QString &speeds, const QString &videopath);
	//void readLastAlarm();

	public slots:
	void doImageProcess();
	void startImageProcess();
	void stopImageProcess();
	void sensorIN();
	void sensorOUT();
	void wheelTimeout();
	//{
	// QString result;
	///* ... here is the expensive or blocking operation ... */
	// emit resultReady(result);
};
