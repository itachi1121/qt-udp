#include "lidar.h"
#include "ui_lidar.h"

/***********************************全局变量***********************************/

bool SocketRecvFlag;   //
bool SocketRecvDisplay; //接收显示

/****************************************************************************/

lidar::lidar(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::lidar)
{
    ui->setupUi(this);
    socketRecvCnt = 0;
    SocketRecvFlag = 0;
    SocketRecvDisplay = 1;
    NetworkInit();     //网口助手初始化
    QTimer *timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timerUpdate()));
    timer->start(500);

}

lidar::~lidar()
{
    delete ui;
}

void lidar::NetworkInit()
{
    udpsocket = new QUdpSocket(this);
    bool ok;
    quint16 udpPort = ui->lineEdit_Socket_RecvPort->text().toShort(&ok);
    udpsocket->bind(QHostAddress::AnyIPv4, udpPort);
    connect(udpsocket, SIGNAL(readyRead()), this, SLOT(udpRecvMsg()));
}

void lidar::lidar_calc_sample(QByteArray recvData)
{
    lidarData data[12];
    //unsigned char var[recvData.size()];
    data[0].azimuth_angle = ((quint8)recvData.data()[2] + (quint8)recvData.data()[3] * qPow(2,8)) * 0.01;
    data[0].range[0] = ((quint8)recvData.data()[4] + (quint8)recvData.data()[5] * qPow(2, 8)) * 4;
    data[0].intensity[0] = (quint8)recvData.data()[6];
    data[0].range[1] = ((quint8)recvData.data()[7] + (quint8)recvData.data()[8] * qPow(2, 8)) * 4;
    data[0].intensity[1] = (quint8)recvData.data()[9];
    data[0].range[2] = ((quint8)recvData.data()[10] + (quint8)recvData.data()[11] * qPow(2, 8)) * 4;
    data[0].intensity[2] = (quint8)recvData.data()[12];
    data[0].range[21] = ((quint8)recvData.data()[67] + (quint8)recvData.data()[68] * qPow(2, 8)) * 4;
    data[0].intensity[21] = (quint8)recvData.data()[69];

    data[1].azimuth_angle = ((quint8)recvData.data()[102] + (quint8)recvData.data()[103] * qPow(2,8)) * 0.01;
    data[1].range[0] = ((quint8)recvData.data()[104] + (quint8)recvData.data()[105] * qPow(2, 8)) * 4;
    data[1].intensity[0] = (quint8)recvData.data()[106];
    data[1].range[1] = ((quint8)recvData.data()[107] + (quint8)recvData.data()[108] * qPow(2, 8)) * 4;
    data[1].intensity[1] = (quint8)recvData.data()[109];
    data[1].range[2] = ((quint8)recvData.data()[110] + (quint8)recvData.data()[111] * qPow(2, 8)) * 4;
    data[1].intensity[2] = (quint8)recvData.data()[112];
    //bool ok;
    //memcpy(&var[0], recvData.data(), recvData.size());
    //printf("%x %x\n", var[4],  var[5]);
    //fflush(stdout);
    static quint64 i=0 ;
//    qDebug() << "Azimuth Angle1" << i++ << ":  [方位角]" << data[0].azimuth_angle  << "  [距离]" << (quint32)(data[0].range[9]) << (quint8)recvData.at(31) << " " << (quint8)recvData.at(32)<< "  [强度]"<< data[0].intensity[9];
    qDebug() << "Azimuth Angle1" << i++ << ":  [方位角]" << data[0].azimuth_angle  << "  [距离]" << (quint32)(data[0].range[21]) << "  [强度]"<< data[0].intensity[21];
   // qDebug() << "Azimuth Angle2 " << ":  [azimuth]" << data[1].azimuth_angle  << "  [range]" << data[1].range[0] << "  " << (quint8)recvData.at(104) << " " << (quint8)recvData.at(105) << "  [intensity]"<< data[1].intensity[0];
    //qDebug() << "Azimuth Angle3 " << ": " << (recvData.data()[202] + recvData.data()[203] * qPow(2,8)) * 0.01;
    //qDebug() << "Azimuth Angle4 " << ": " << (recvData.data()[302] + recvData.data()[303] * qPow(2,8)) * 0.01;
}

void lidar::udpRecvMsg()
{
    QByteArray readData;
    QHostAddress address;
    quint16 port;
    readData.resize(udpsocket->bytesAvailable());//根据可读数据来设置空间大小
    udpsocket->readDatagram(readData.data(),readData.size(),&address,&port); //读取数据
    if(readData.size() == 1206)
    {
        lidar_calc_sample(readData);
    }

    if(SocketRecvDisplay == 1)
    {
        //static int i = 0;
        //printf("udp recv msg [%d]: %s\n",++i, readData.data());
        //fflush(stdout);

        QString textString = " " + QString(readData.toHex(' ').toUpper());  //16进制输出 空格分隔后转大写
        ui->textEdit_Socket_Recv->insertPlainText(textString); //insertPlainText不会自动换行，但是CPU占用率高，可能会卡
        socketRecvCnt += readData.size();
    }

}

void lidar::on_pushButton_Socket_ClearRecv_clicked()
{
    ui->textEdit_Socket_Recv->clear();
    socketRecvCnt = 0;
}

void lidar::on_pushButton_Socket_SaveRecv_clicked()
{
    QFile file(ui->socket_SaveFile->text());
    file.open(QIODevice::WriteOnly);
    QString text = ui->textEdit_Socket_Recv->toPlainText();
    //text.replace(QString("\n"), QString("\r\n"));
    QTextStream filetext(&file);
    filetext << text;
    file.close();
}

void lidar::timerUpdate()
{
    ui->label_Socket_RecvCnt->setText(QString::number(socketRecvCnt, 10));
}

void lidar::on_button_Socket_StartRecv_clicked()
{
    udpsocket->close();
    bool ok;
    quint16 udpPort = ui->lineEdit_Socket_RecvPort->text().toShort(&ok);
    udpsocket->bind(QHostAddress::AnyIPv4, udpPort);
    //SocketRecvFlag = 1;
    ui->button_Socket_StartRecv->setStyleSheet("color: green");
    ui->button_Socket_StopRecv->setStyleSheet("color: black");
}

void lidar::on_button_Socket_StopRecv_clicked()
{
    udpsocket->close();
    //SocketRecvFlag = 0;
    ui->button_Socket_StartRecv->setStyleSheet("color: black");
    ui->button_Socket_StopRecv->setStyleSheet("color: green");
}

void lidar::on_lineEdit_Socket_RecvPort_returnPressed()
{
    udpsocket->close();
    bool ok;
    quint16 udpPort = ui->lineEdit_Socket_RecvPort->text().toShort(&ok);
    udpsocket->bind(QHostAddress::AnyIPv4, udpPort);
}

void lidar::on_pushButton_Socket_Send_clicked()
{
    QHostAddress udpAddr(ui->lineEdit_Socket_SendIP->text());
    bool ok;
    quint16 udpPort = ui->lineEdit_Socket_SendPort->text().toShort(&ok);
    QString msgSend = ui->textEdit_Socket_Send->toPlainText();

    QStringList strList = msgSend.split(" ", QString::SkipEmptyParts);  //
    QByteArray str_send;

    for(int i = 0;i < strList.size(); i++)
    {
        str_send += QString2Hex(strList[i]);
    }

    udpsocket->writeDatagram(str_send, udpAddr, udpPort);
}


//将一个字符串转换成十六进制
QByteArray lidar::QString2Hex(QString str)
{
    QByteArray senddata;
    int hexdata,lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    senddata.resize(len/2);
    char lstr,hstr;
    for(int i=0; i<len; )
    {
        hstr=str[i].toLatin1();
        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if(i >= len)
        break;
        lstr = str[i].toLatin1();
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata*16+lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
    return senddata;
}


char lidar::ConvertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
        return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
        return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
        return ch-'a'+10;
    else return (-1);
}

void lidar::on_checkBox_Display_clicked(bool checked)
{
    if(checked)
    {
        SocketRecvDisplay = 1;
    }
    else
    {
        SocketRecvDisplay = 0;
    }
}
