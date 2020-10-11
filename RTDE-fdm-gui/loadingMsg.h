#pragma once

#include <QDialog>
#include "ui_loading.h"


class LoadingMsg : public QDialog
{
    Q_OBJECT

public:
    explicit LoadingMsg(QWidget* parent = nullptr);
    ~LoadingMsg(){};

    void setLabel(QString msg);
    void updateProgress(int progress);

private slots:
    void on_cancelButton_triggered();


private:
    Ui::LoadingMsg ui;
};
