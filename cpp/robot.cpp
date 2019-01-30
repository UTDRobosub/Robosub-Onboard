
#include "robot.h"

Serial *serial_imu;
Serial* serial_motorcontrol;

char imuLastReceivedMessage[256];
void imuReceiveMessage(char* message, int length, bool needsresponse, char** response, int* responselength){
    memcpy(imuLastReceivedMessage, message, length);
    imuLastReceivedMessage[length] = 0;
    //cout<<imuLastReceivedMessage<<endl;
}

void mcReceiveMessage(char* message, int length, bool needsresponse, char** response, int* responselength){
    message[length] = 0;
    cout<<message<<endl;
}

void initRobotState(){
    //string port = Util::execCLI("ls /dev | grep tty[AU]");
    //cout<<"using serial port /dev/"<<port<<endl;
    //serial1 = new Serial("/dev/" + port.substr(0,port.length()-1), 115200, receiveMessage, false);

    serial_imu = new Serial("/dev/ttyACM0", 115200, imuReceiveMessage, false);
    serial_motorcontrol = new Serial("/dev/ttyACM1", 115200, mcReceiveMessage, true);
}

void updateRobotTelemetry(DataBucket& state){
    serial_imu->receiveAllMessages();

    auto imu = Util::splitString(string(imuLastReceivedMessage), ',');

    state["imu"] = {};

    if (imu.size() < 17) return;

    state["imu"]["ax"] = stof(imu[ 0]); // acceleration x (G)
    state["imu"]["ay"] = stof(imu[ 1]); // acceleration y (G)
    state["imu"]["az"] = stof(imu[ 2]); // acceleration z (G)
    state["imu"]["gx"] = stof(imu[ 3]); // gyroscope x (deg/sec)
    state["imu"]["gy"] = stof(imu[ 4]); // gyroscope y (deg/sec)
    state["imu"]["gz"] = stof(imu[ 5]); // gyroscope z (deg/sec)
    state["imu"]["mx"] = stof(imu[ 6]); // magnetometer x (uT)
    state["imu"]["my"] = stof(imu[ 7]); // magnetometer y (uT)
    state["imu"]["mz"] = stof(imu[ 8]); // magnetometer z (uT)
    state["imu"]["qw"] = stof(imu[ 9]); // quaternion w
    state["imu"]["qx"] = stof(imu[10]); // quaternion x
    state["imu"]["qy"] = stof(imu[11]); // quaternion y
    state["imu"]["qz"] = stof(imu[12]); // quaternion z
    state["imu"]["ep"] = stof(imu[13]); // euler angle pitch (deg)
    state["imu"]["er"] = stof(imu[14]); // euler angle roll  (deg)
    state["imu"]["ey"] = stof(imu[15]); // euler angle yaw   (deg)
    state["imu"]["ch"] = stof(imu[16]); // compass heading   (deg)

    serial_motorcontrol->receiveAllMessages();
}

struct MotorValues {
    short bl;
    short br;
    short ul;
    short ur;
};

void updateRobotControls(DataBucket& state){
    try {
        MotorValues motorvals;
        motorvals.bl = (short)state["motors"]["bl"];
        motorvals.br = (short)state["motors"]["br"];
        motorvals.ul = (short)state["motors"]["ul"];
        motorvals.ur = (short)state["motors"]["ur"];

        serial_motorcontrol->transmitMessageFast((char*)&motorvals, sizeof(motorvals));
    } catch (exception &e) {
        cout << e.what() << endl;
    }
}
