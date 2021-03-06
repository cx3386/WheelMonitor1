#pragma once

#include "ui_SettingDialog.h"
#include <QDialog>
#include <opencv2/opencv.hpp>

class ConfigHelper;
class SettingDialog : public QDialog {
	Q_OBJECT

public:
	SettingDialog(ConfigHelper* ch, QWidget* parent = Q_NULLPTR);
	~SettingDialog();

private:
	Ui::SettingDialog ui;
	ConfigHelper *helper;

	void ui2ini();

private slots:
	void roiSlot(int);
	void ini2ui();
	void onClickSelSaveDir();
	void onClickClearCacheNow();
	void onClickClearCacheAll();
	void onUiChanged();
signals:
	void clearVideo(int mode); //!< mode 1-now, 0-all.
};
