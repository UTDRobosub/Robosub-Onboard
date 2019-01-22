
#include "robot.h"

Serial *serial1;
int lastPolledSensorData = -1;

char lastReceivedMessage[256];

void receiveMessage(char* message, int length, bool needsresponse, char** response, int* responselength){
    memcpy(lastReceivedMessage, message, length);
    lastReceivedMessage[length] = 0;
}

void initRobotState(){
    string port = Util::execCLI("ls /dev | grep tty[AU]");
    cout<<"using serial port /dev/"<<port<<endl;
    serial1 = new Serial("/dev/" + port.substr(0,port.length()-1), 115200, receiveMessage, false);
}

void updateRobotTelemetry(DataBucket& state){
    serial1->receiveAllMessages();

    auto imu = Util::splitString(string(lastReceivedMessage), ',');

    cout<<lastReceivedMessage<<endl;

    state["imu"] = { };

    if (imu.size() < 17) return;

    state["imu"]["ax"] = imu[ 0]; // acceleration x (G)
    state["imu"]["ay"] = imu[ 1]; // acceleration y (G)
    state["imu"]["az"] = imu[ 2]; // acceleration z (G)
    state["imu"]["gx"] = imu[ 3]; // gyroscope x (deg/sec)
    state["imu"]["gy"] = imu[ 4]; // gyroscope y (deg/sec)
    state["imu"]["gz"] = imu[ 5]; // gyroscope z (deg/sec)
    state["imu"]["mx"] = imu[ 6]; // magnetometer x (uT)
    state["imu"]["my"] = imu[ 7]; // magnetometer y (uT)
    state["imu"]["mz"] = imu[ 8]; // magnetometer z (uT)
    state["imu"]["qw"] = imu[ 9]; // quaternion w
    state["imu"]["qx"] = imu[10]; // quaternion x
    state["imu"]["qy"] = imu[11]; // quaternion y
    state["imu"]["qz"] = imu[12]; // quaternion z
    state["imu"]["ep"] = imu[13]; // euler angle pitch (deg)
    state["imu"]["er"] = imu[14]; // euler angle roll  (deg)
    state["imu"]["ey"] = imu[15]; // euler angle yaw   (deg)
    state["imu"]["ch"] = imu[16]; // compass heading   (deg)
}

void updateRobotControls(DataBucket& state){
    try {
        double mbl = state["motors"]["bl"];
        cout<<mbl<<endl;
    } catch (exception &e) {
        cout << e.what() << endl;
    }
}
