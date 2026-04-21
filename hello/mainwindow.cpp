#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    // , serial(new QSerialPort(this)) // (1) 시리얼 객체를 실제로 생성!
{
    ui->setupUi(this);

    applyStyles();
    ui->portCombo->addItem("ttyACM0");
    ui->portCombo->addItem("ttyACM1");
    ui->baudCombo->addItem("9600");
    ui->baudCombo->addItem("115200");

    serial = new QSerialPort(this);

    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readData);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_connectButton_clicked()
{
    // QString portName = "/dev/ttyACM0";
    // QString buardRate = "9600";

    QString portName = ui -> portCombo ->currentText();
    int baud=ui->baudCombo->currentText().toInt();

    // serial=new QSerialPort(this);
    serial->setPortName(portName);
    serial->setBaudRate(baud);
    // serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    qint64 pid = getSerialPortPID(portName);

    if (pid > 0) {
        qDebug() << portName << "는 현재 사용 중입니다. PID:" << pid;
        return;
    }
    // else {
    //     // serial->open(QIODevice :: ReadWrite);
    //     if(serial->open(QIODevice :: ReadWrite)){
    //         ui->textBrowser->append("Connection : Port Opened !! ");

    //     }else{
    //         ui->textBrowser->append("Connection : Port Opened Error !! ");
    //     }
    // }

    // if(serial->open(QIODevice::ReadWrite)){
    //     ui->textBrowser->append("Conection : Port Opened!!");
    // }else{
    //     ui->textBrowser->append("Conection : Port Opened Error!!");
    //     ui->textBrowser->append("Error Msg: " + serial->errorString());
    // }

    // connect(serial, &QSerialPort::readyRead, this, &MainWindow::readData);



    if (serial->isOpen()) {
        serial->close();
        ui->connectButton->setText("Disconnected");
        ui->connectButton->setProperty("connected", false);
        ui->connectButton->style()->unpolish(ui->connectButton);
        ui->connectButton->style()->polish(ui->connectButton);
        ui->textBrowser->append("Connection : Port Closeed!!");
        return;
    }
    else if (serial->open(QIODevice::ReadWrite)) {
        ui->connectButton->setText("Connected");
        ui->connectButton->setProperty("connected", true);
        ui->connectButton->style()->unpolish(ui->connectButton);
        ui->connectButton->style()->polish(ui->connectButton);
        ui->textBrowser->append("Connection : Port Opened!!");
    }
    else{
        ui->connectButton->setText("Disconnected");
        ui->connectButton->setProperty("connected", false);
        ui->connectButton->style()->unpolish(ui->connectButton);
        ui->connectButton->style()->polish(ui->connectButton);
        ui->textBrowser->append("Connection : Port Opened Error!!");
    }

}




qint64 MainWindow :: getSerialPortPID(QString portPath) {
    QProcess process;
    // -t 옵션: PID만 출력, -u: 특정 파일 점유 확인
    process.start("lsof", QStringList() << "-t" << portPath);
    process.waitForFinished();

    QString output = process.readAllStandardOutput().trimmed();

    if (output.isEmpty()) {
        return 0; // 점유 중인 프로세스 없음

    }

    return output.toLongLong(); // 점유 중인 프로세스의 PID 반환
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
            ui->textBrowser->append(packet);
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


    //parts[0] data count
    for(int i=1; i<parts.size();++i){

        QStringList item = parts[i].split(':');
        if(item.size()<3) continue;

        int sensorID = item[0].toInt();
        int dataType = item[1].toInt();
        QString valueStr = item[2];



        if(sensorID == ID_ENV_TEMP){
            ui->tempValue->setText(valueStr + "°C");

        }else if(sensorID==ID_OUT_LED_STATE){
            ui->ledValue->setText(valueStr);


        }
    }



}



void MainWindow::on_monitorStartButton_clicked()
{
    if(serial->isOpen()){

        QString str="mon on\r\n";
        qint64 bytesWritten=serial->write(str.toUtf8());
        serial->flush();


        if(bytesWritten==-1){
            ui->textBrowser->append("transmmit error!!");
        }else{
            ui->textBrowser->append("TX -> " + str);
        }
    }
}


void MainWindow::on_monitorstopButton_clicked()
{
    if(serial->isOpen()){

        QString str="mon off\r\n";
        qint64 bytesWritten=serial->write(str.toUtf8());
        serial->flush();

        str="temp\r\n";
        bytesWritten=serial->write(str.toUtf8());
        serial->flush();

        str="led off\r\n";
        bytesWritten=serial->write(str.toUtf8());
        serial->flush();

        if(bytesWritten==-1){
            ui->textBrowser->append("transmmit error!!");
        }else{
            ui->textBrowser->append("TX -> " + str);
        }
    }
}


void MainWindow::on_ledBlinkButton_clicked()
{

    if(serial->isOpen()){

        QString str="led toggle 1000\r\n";
        qint64 bytesWritten=serial->write(str.toUtf8());

        serial->flush();
        if(bytesWritten==-1){
            ui->textBrowser->append("transmmit error!!");
        }else{
            ui->textBrowser->append("TX -> " + str);
        }
    }

}
void MainWindow::applyStyles(){
    QString qss = R"(
        QPushButton#connectButton[connected="false"]{background-color:gray; color:white;}
        QPushButton#connectButton[connected="true"]{background-color:green; color:white;}

        /* color */
        QLabel#tempValue{font-size:64px; font-weight:900;}


    )";
    this->setStyleSheet(qss);

}
