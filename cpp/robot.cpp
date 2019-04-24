
#include "robot.h"

char imuLastReceivedMessage[256];

Serial* serial_imu;
Serial* serial_motorcontrol;

vector<Serial*> serialPorts;
int currentReceivingSerialIdx;

//void imuReceiveMessage(char* message, int length, bool needsresponse, char** response, int* responselength){
//	memcpy(imuLastReceivedMessage, message, length);
//	imuLastReceivedMessage[length] = 0;
//	//cout<<imuLastReceivedMessage<<endl;
//}

//void mcReceiveMessage(char* message, int length, bool needsresponse, char** response, int* responselength){
//	message[length] = 0;
//	cout<<message<<endl;
//}

void serialReceiveMessage(char* message, int length, bool needsresponse, char** response, int* responselength){
	char ident = message[0];
	
	if(ident=='m'){ //Motor controller
		serial_motorcontrol = serialPorts[currentReceivingSerialIdx];
		
		
		
	}else if(ident=='i'){ //9DoF IMU
		serial_imu = serialPorts[currentReceivingSerialIdx];
		
		
	}
}

void initRobotState(){
	string portstr = Util::execCLI("ls /dev | grep tty[AU]");
	auto ports = Util::splitString(portstr, '\n');
	for(int i=0; i<ports.size(); i++){
		cout<<ports[i]<<endl;
		Serial* serialport = new Serial("/dev/"+ports[i], 115200, serialReceiveMessage, true);
		serialPorts.push_back(serialport);
	}
	
	serial_imu = 0;
	serial_motorcontrol = 0;
	
	//string port = Util::execCLI("ls /dev | grep tty[AU]");
	//cout<<"using serial port /dev/"<<port<<endl;
	//serial1 = new Serial("/dev/" + port.substr(0,port.length()-1), 115200, receiveMessage, false);
	
	//serial_imu = new Serial("/dev/ttyACM0", 115200, imuReceiveMessage, false);
	//serial_motorcontrol = new Serial("/dev/ttyACM1", 115200, mcReceiveMessage, true);
}

void updateRobotTelemetry(DataBucket& state){
	for(int i=0; i<serialPorts.size(); i++){
		currentReceivingSerialIdx = i;
		serialPorts[i].receiveAllMessages();
	}
	
	auto imu = Util::splitString(string(imuLastReceivedMessage), ',');
	
	state["imu"] = {};
	
	if(imu.size()>=17){
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
	}
}

struct MotorValues {
	short bl;
	short br;
	short ul;
	short ur;
	short v;
};

void updateRobotControls(DataBucket& state){
	try {
		MotorValues motorvals;
		motorvals.bl = (short)state["motors"]["bl"]; //connected to pin 9 on the arduino
		motorvals.br = (short)state["motors"]["br"]; //connected to pin 11 on the arduino
		motorvals.ul = (short)state["motors"]["ul"]; //connected to pin 8 on the arduino
		motorvals.ur = (short)state["motors"]["ur"]; //connected to pin 10 on the arduino
		motorvals.v  = (short)state["motors"]["v"];  //connected to pins 5 & 6 on the arduino
		
		if(serial_motorcontrol!=0){
			serial_motorcontrol->transmitMessageFast((char*)&motorvals, sizeof(motorvals));
		}else{
			cout<<"Serial port for motor control not open"<<endl;
		}
	} catch (exception &e) {
		cout << e.what() << endl;
	}
}
