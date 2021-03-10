#ifndef LIDAR_H
#define LIDAR_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QFile>
#include <QTimer>
#include <QtCore/qmath.h>

QT_BEGIN_NAMESPACE
namespace Ui { class lidar; }
QT_END_NAMESPACE

struct lidarData{
    float azimuth_angle;  //方位角
    quint32 range[32];     //距离范围
    quint8 intensity[32];  //强度
};

class lidar : public QMainWindow
{
    Q_OBJECT

public:
    lidar(QWidget *parent = nullptr);
    ~lidar();
    QUdpSocket *udpsocket;

    void lidar_calc_sample(QByteArray data);


private:
    Ui::lidar *ui;
    void NetworkInit();
    qint64 socketRecvCnt;
    char ConvertHexChar(char ch);
    QByteArray QString2Hex(QString str);
private slots:
    void udpRecvMsg();
    void timerUpdate();
    void on_pushButton_Socket_ClearRecv_clicked();
    void on_pushButton_Socket_SaveRecv_clicked();
    void on_button_Socket_StartRecv_clicked();
    void on_button_Socket_StopRecv_clicked();
    void on_lineEdit_Socket_RecvPort_returnPressed();
    void on_pushButton_Socket_Send_clicked();
    void on_checkBox_Display_clicked(bool checked);
};
#endif // LIDAR_H
