#include "loadingMsg.h"

LoadingMsg::LoadingMsg(QWidget* parent) : QDialog(parent) {
	ui.setupUi(this);
}

void LoadingMsg::updateProgress(int progress) {
	ui.progressBarScript->setValue(progress);
}

void  LoadingMsg::setLabel(QString msg) {
	ui.labelLoadingStatus->setText(msg);
}

void LoadingMsg::on_cancelButton_triggered() {
	
}