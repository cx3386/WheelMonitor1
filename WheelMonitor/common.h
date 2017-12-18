// include the common definition

#pragma once

//App Dir defined in main.cpp
extern QString appDirPath;     //the directory contains the app.exe, '/'e.g. C:/QQ
extern QString appFilePath;    //the file path of app.exe, '/'e.g. C:/QQ/qq.exe
extern QString captureDirPath; //capture dir
extern QString configDirPath;     //config dir
extern QString videoDirPath; //video dir
extern QString matchDirPath;	//match dir
extern QString imageDirPath; //imageDirPath
extern QString logDirPath;     //log dir
//QString dbDir = QString("%1").arg(appDirPath);	//db dir	-same as appdir
//QString iniDir = QString("%1").arg(appDirPath);	//ini dir	-same as appdir
extern QString appName;

#define BACKUP_FOLDER_NAME "WheelMonitor Bak"
//need identity of cpu and mac
//bool bNeedIdentity;