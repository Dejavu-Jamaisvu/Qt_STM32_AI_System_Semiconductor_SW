#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    // , serial(new QSerialPort(this)) // (1) 시리얼 객체를 실제로 생성!
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_connectButton_clicked()
{
    QString portName = "/dev/ttyACM0";
    QString buardRate = "9600";

    serial=new QSerialPort(this);
    serial->setPortName(portName);
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);


    if(serial->open(QIODevice::ReadOnly)){
        ui->textBrowser->append("Conection : Port Opened!!");
    }else{
        ui->textBrowser->append("Conection : Port Opened Error!!");
        ui->textBrowser->append("Error Msg: " + serial->errorString());
    }

    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readData);

}

void MainWindow::readData(){
    // QByteArray data=serial->readAll();
    // if(data.isEmpty()==false){
    //     ui->textBrowser->append(data);
    // }

    m_buffer.append(serial->readAll());
    // QByteArray data;

    while(m_buffer.contains('$')&&m_buffer.contains('#')){
        int start = m_buffer.indexOf('$');
        int end= m_buffer.indexOf('#',start);

        if(end == -1) break;

        QByteArray packet = m_buffer.mid(start+1,end-start-1);
        m_buffer.remove(0,end+1);

        if(packet.isEmpty()==false){
            // ui->textBrowser->append(packet);
            parseProtocol(packet);
        }
    }

    // while (m_buffer.contains('\n')) {
    //     QByteArray data;
    //     int newLineIndex = m_buffer.indexOf('\n');
    //     data=m_buffer.left(newLineIndex);
    //     m_buffer.remove(0,newLineIndex+1);

    //     if(data.endsWith('\r')){
    //         data.chop(1);
    //     }

    //     if (data.isEmpty()==false) {
    //         ui->textBrowser->append(data);
    //     }

    // }
}


void MainWindow::parseProtocol(const QByteArray &packet){

    QString data = QString::fromUtf8(packet);
    QStringList parts = data.split(',');
    if(parts.size()<2) return;

    ui->textBrowser->append(packet);


    //parts[0] data count
    for(int i=1; i<parts.size();++i){

        QStringList item = parts[i].split(':');
        if(item.size()<3) continue;

        int sensorID = item[0].toInt();
        int dataType = item[1].toInt();
        QString valueStr = item[2];



        if(sensorID == ID_ENV_TEMP){
            ui->tempValue->setText(valueStr + "'C");

        }else if(sensorID==ID_OUT_LED_STATE){
            ui->ledValue->setText(valueStr);


        }
    }



}

// void MainWindow::readData(){
//     m_buffer.append(serial->readAll());
//     QByteArray data;

//     while(m_buffer.contains('$') && m_buffer.contains('#')){
//         int start = m_buffer.indexOf('$');
//         int end = m_buffer.indexOf('#', start);

//         if(end == -1) break;

//         QByteArray packet = m_buffer.mid(start + 1, end - start - 1);
//         m_buffer.remove(0, end + 1);

//         if(!packet.isEmpty()){
//             //ui->textBrowser->append(packet);
//             parseProtocol(packet);
//         }
//     }

//     /*
//     while(m_buffer.contains('\n')){
//         int line_feed_idx = m_buffer.indexOf('\n');
//         data = m_buffer.remove(0, line_feed_idx + 1);

//         if(m_buffer.contains('\r')){
//             m_buffer.chop(1);
//         }
//     }
// */
//     /*
//     QByteArray buffer = serial->readAll();
//     QByteArray data;

//     buffer.removeAt(buffer.indexOf('\r'));
//     //buffer.removeAt(buffer.indexOf('\n'));

//     ui->textBrowser->insertPlainText(buffer);
// */
// }

// void MainWindow::parseProtocol(const QByteArray &packet){
//     QString data = QString::fromUtf8(packet);
//     QStringList parts = data.split(',');

//     if(parts.size() < 2) return;

//     // parts[0] == data count
//     for(int i = 1; i < parts.size(); ++i){
//         QStringList item = parts[i].split(':');

//         if(item.size() < 3) continue;

//         int sensor_id = item[0].toInt();
//         int sensor_type = item[1].toInt();
//         QString val_str = item[2];


//         switch(sensor_id){
//         case ID_ENV_TEMP:
//             ui->tempValue->setText("Temp: " + val_str + " ℃");
//             break;
//         case ID_OUT_LED_STATE:
//             ui->ledValue->setText(val_str == "1" ? "🟢" : " ");
//             break;
//         default:
//             break;
//         }
//     }
// }



