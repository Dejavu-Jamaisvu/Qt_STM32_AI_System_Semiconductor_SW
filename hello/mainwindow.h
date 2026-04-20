#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>



// namespace

typedef enum {
    /* 0~9: 시스템 공통 상태 */
    ID_SYS_HEARTBEAT    = 0,    // 시스템 동작 여부 (Alive signal)
    ID_SYS_UPTIME       = 1,    // 시스템 가동 시간 (millis)
    ID_SYS_TEMP         = 2,    // MCU 내부 온도 센서
    ID_SYS_VREF         = 3,    // MCU 내부 참조 전압

    /* 10~29: 환경 센서 데이터 (주로 Float/Int) */
    ID_ENV_TEMP         = 10,   // 외부 온도 센서
    ID_ENV_HUMI         = 11,   // 외부 습도 센서
    ID_ENV_PRESS        = 12,   // 대기압 센서
    ID_ENV_LIGHT        = 13,   // 조도 센서 (Lux)

    /* 30~49: 사용자 입력 기기 (주로 Bool/Int) */
    ID_IN_BUTTON_1      = 30,   // 사용자 버튼 1 (Blue Button 등)
    ID_IN_BUTTON_2      = 31,   // 사용자 버튼 2
    ID_IN_SW_DIP        = 32,   // 딥 스위치 상태
    ID_IN_ENC_POS       = 33,   // 엔코더 회전 위치

    /* 50~69: 액추에이터 상태 피드백 (주로 Bool/Int) */
    ID_OUT_LED_STATE    = 50,   // LED On/Off 상태
    ID_OUT_MOTOR_SPEED  = 51,   // 모터 현재 속도 (RPM)
    ID_OUT_RELAY        = 52,   // 릴레이 작동 상태

    /* 70~89: 모션 및 위치 데이터 (주로 Float) */
    ID_IMU_ACCEL_X      = 70,   // 가속도 X축
    ID_IMU_ACCEL_Y      = 71,   // 가속도 Y축
    ID_IMU_ACCEL_Z      = 72,   // 가속도 Z축
    ID_IMU_GYRO_X       = 73,   // 자이로 X축

    /* 100+: 에러 및 알람 코드 */
    ID_ALARM_CRITICAL   = 100,  // 치명적 오류 코드
    ID_ALARM_WARN       = 101   // 단순 경고 코드
} SensorID;

typedef enum {
    TYPE_UINT8   = 0,
    TYPE_INT32   = 1,
    TYPE_FLOAT   = 2,
    TYPE_BOOL    = 3,
    TYPE_STRING  = 4  // 이번 구현에서는 4바이트 고정 길이를 위해 지원하지 않음
} DataType;








QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_connectButton_clicked();
    void readData();

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    QByteArray m_buffer; // 데이터를 임시로 담아둘 버퍼 추가

    void parseProtocol(const QByteArray &packet);
};
#endif // MAINWINDOW_H
