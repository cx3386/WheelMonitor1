#include "stdafx.h"
#include "PLCSerial.h"


PLCSerial::PLCSerial(QObject *parent) : QObject(parent)
	, sensorA(false)
	, sensorB(false)
	, stopSensor(false)
	, isConnect(false)
{
}



PLCSerial::~PLCSerial()
{	
	//sensorTimer->deleteLater();
	plcSerialPort->deleteLater();
	plcSerialPort->close();
}

void PLCSerial::init()
{
	plcSerialPort = new QSerialPort;
	plcSerialPort->setPortName("COM3");
	plcSerialPort->setBaudRate(QSerialPort::Baud9600);
	plcSerialPort->setParity(QSerialPort::EvenParity);
	plcSerialPort->setDataBits(QSerialPort::Data7);
	plcSerialPort->setStopBits(QSerialPort::TwoStop);
	plcSerialPort->close();
	if (!plcSerialPort->open(QIODevice::ReadWrite)) {
		qWarning() << QString("Can't connect PLC, error code %2").arg(plcSerialPort->error());
	}
	else
		isConnect = true;
}

void PLCSerial::Alarm(const char* lightcolor)		//应该改为Alarm(AlarmColor alarmcolor)的形式，通过&Mask判断FLag的值,再对plcData（不要定义为全局变量）赋值
{
	plcData = lightcolor;
	
	if (plcData == ALARM_LIGHT_GREEN)
	{
		emit setUiAlarm(AlarmColorGreen);
		currentAlarmColor = AlarmColorGreen;
	}
	else if (plcData == ALARM_LIGHT_RED)
	{
		emit setUiAlarm(AlarmColorRed);
		currentAlarmColor = AlarmColorRed;
	}
	else if (plcData == ALARM_LIGHT_YELLOW)		//yellow light(waring) cannot override red
	{
		if (currentAlarmColor == AlarmColorRed)
		{
			return;
		}
		else
		{
			emit setUiAlarm(AlarmColorYellow);
			currentAlarmColor = AlarmColorYellow;
		}
	}
	plcSerialPort->write(plcData);
	if (plcSerialPort->waitForBytesWritten(100))
	{
		if (plcSerialPort->waitForReadyRead(100))
		{
			QByteArray responseData = plcSerialPort->readAll();
			while (plcSerialPort->waitForReadyRead(100))
				responseData += plcSerialPort->readAll();
			if (responseData != "@00WR0045*\r")
				qWarning() << "Error: Alarm Light wrong return";
		}
	}
}

bool PLCSerial::startWheelSensor()	
{
	if (isConnect)
	{
		sensorTimer = new QTimer;
		connect(sensorTimer, SIGNAL(timeout()), this, SLOT(loopWheelSensor()));
 		sensorTimer->start(1000);		//0 is error, 1 is ok.
		emit isStartWheelSensor(true);
		return true;
	}
	else
	{
		qWarning() << QString("Can't connect PLC, error code %2").arg(plcSerialPort->error());
		emit isStartWheelSensor(false);
		return false;
	}
	
}
bool PLCSerial::stopWheelSensor()
{
	sensorTimer->stop();
	delete sensorTimer;
	sensorTimer = nullptr;
	emit isStopWheelSensor(true);
	return true;
}

void PLCSerial::loopWheelSensor()
{
	plcData = "@00RR0000000141*\r";
	plcSerialPort->write(plcData);
	if (plcSerialPort->waitForBytesWritten(100)) {
		if (plcSerialPort->waitForReadyRead(100)) {
			QByteArray responseData = plcSerialPort->readAll();
			while (plcSerialPort->waitForReadyRead(100))
				responseData += plcSerialPort->readAll();

			if (responseData == "@00RR00000040*\r")
			{
				if (sensorA == true && sensorB == false) 
				{
					emit stopSave();
				}
				sensorA = false;
				sensorB = false;
				//00 10->00，即A的下降沿，则认为A出界，停止录像
			}	//如果状态改变则发出信号否则return
			else if (responseData == "@00RR00000242*\r")
			{
				sensorA = true;
				sensorB = false;
			}	//01
			else if (responseData == "@00RR00000848*\r")
			{
				//if (sensorA == false && sensorB == true)
				//	return;
				//else
				//{
				//	sensorA = false;
				//	sensorB = true;
				//	emit stopSave();
				//}
				if (sensorA == false && sensorB == false)
				{
					emit startSave();
				}
				sensorA = false;
				sensorB = true;
			}	//10 00->01,即B的上升沿，认为B进入，开始录像
			//planA不变的话就return不管，一旦变成现在的信号（无论从哪个状态变化来的）则判断原来就执行动作；planB触发现在状况后，对上一个状态进行筛选，如果是希望的状态（唯一），则执行动作
			//planB的执行更加严格，因为它要求上一次状态检测是符合要求的（甚至要却决于另一个传感器）
			else if (responseData == "@00RR00000A31*\r")
			{
				qWarning() << "Error: the wheel sensor: A&B";
			}	//11
			else qWarning() << "Error: the wheel sensor wrong response";

		}
		else {
			//QString str(tr("Wait read response timeout %1")
			//.arg(QTime::currentTime().toString()));
			qWarning() << "Wait read response timeout";
		}
	}
	else {

		//QString str(tr("Wait write request timeout %1")
		//.arg(QTime::currentTime().toString()));
		qWarning() << "Wait write request timeout";
	}
	//qDebug() << "SensorA: " << sensorA << "SensorB: " << sensorB;

}