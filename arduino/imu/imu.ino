#include <SPI.h>

#include <MPU9250_RegisterMap.h>
#include <SparkFunMPU9250-DMP.h>

#define SerialPort SerialUSB
int ACCEL_SCALE = 2; //Acceleration scale in g's. Valid values: 2 4 8 16
int GYRO_SCALE = 250; //Gyro scale in dps. Valid values: 250 500 1000 2000

/* Error codes
 * 0xFA01 IMU initialization failed 
 * 0xFA02 Updating FIFO with DMP data failed
 */
MPU9250_DMP imu; // Create an instance of the MPU9250_DMP class

void setup() {
  SerialPort.begin(115200); //Start serial port
  
  if (imu.begin() != INV_SUCCESS) {
    SerialPort.println("IMU INIT FAILED");
    //Serial.write(0xFA01); //IMU init failed
  } else {
    SerialPort.println("IMU INIT SUCCESS");
  }

  imu.setAccelFSR(ACCEL_SCALE);
  imu.setGyroFSR(GYRO_SCALE);

  imu.dmpBegin( DMP_FEATURE_SEND_RAW_ACCEL | //Send raw accelerometer data
                DMP_FEATURE_SEND_RAW_GYRO | //Send raw gyroscope data
                DMP_FEATURE_6X_LP_QUAT, //Calculate quats with accel/gyro
              100); //Set update rate to 100Hz
}

void sendData(void *data, int size) {
  //Call data send function from lib
}

struct SensorDataPacket {
  unsigned long time;
  int aScale;
  int ax, ay, az;
  int gScale;
  int gx, gy, gz;
  long qx, qy, qz, qw;
};

void setAccelScale(int newScale) {
  if (imu.setAccelFSR(newScale) != 0) {
    //Accel scale set failed
  }
}

void setGyroScale(int newScale) {
  if (imu.setGyroFSR(newScale) != 0) {
    //Gyro scale set failed
  }
}

void loop() {
  if (imu.fifoAvailable() > 0) {
    if (imu.dmpUpdateFifo() == INV_SUCCESS) {
      
      struct SensorDataPacket data = {imu.time,
        imu.getAccelSens(),
        imu.ax, imu.ay, imu.az,
        imu.getGyroSens(),
        imu.gx, imu.gy, imu.gz,
        imu.qx, imu.qy, imu.qz, imu.qw
      };

      SerialPort.println(((float)imu.ax)/imu.getAccelSens());
      
      sendData(&data, sizeof(data));
    } else {
      SerialPort.println("IMU FIFO UPDATE FAILED");
      //Serial.write(0xFA02);
    }
  }
}
