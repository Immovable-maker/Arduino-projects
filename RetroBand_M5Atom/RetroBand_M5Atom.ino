#include <math.h>
#include <Wire.h>
#include <BluetoothSerial.h>
#include "M5Atom.h" //지금 여기서 다포함해줬음 MPU6886.h 는 IMU이름으로다가 다하면 되고 LED는 dis라는 이름으로 다하면 됌
unsigned long t_prev, t_now;

/* Bluetooth */
BluetoothSerial SerialBT;

/* time */
#define SENDING_INTERVAL 1000
#define SENSOR_READ_INTERVAL 50
unsigned long prevSensoredTime = 0;
unsigned long curSensoredTime = 0;

/* Data buffer */
#define ACCEL_BUFFER_COUNT 125
byte aAccelBuffer[ACCEL_BUFFER_COUNT];
int iAccelIndex = 2;

/* MPU-6050 sensor */
#define MPU6886_I2C_ADDRESS       104 
#define MPU6886_WHO_AM_I          0x75
#define MPU6886_ACCEL_INTEL_CTRL  0x69
#define MPU6886_SMPLRT_DIV        0x19
#define MPU6886_INT_PIN_CFG       0x37
#define MPU6886_INT_ENABLE        0x38
#define MPU6886_ACCEL_XOUT_H      0x3B
#define MPU6886_ACCEL_XOUT_L      0x3C
#define MPU6886_ACCEL_YOUT_H      0x3D
#define MPU6886_ACCEL_YOUT_L      0x3E
#define MPU6886_ACCEL_ZOUT_H      0x3F
#define MPU6886_ACCEL_ZOUT_L      0x40

#define MPU6886_TEMP_OUT_H        0x41
#define MPU6886_TEMP_OUT_L        0x42

#define MPU6886_GYRO_XOUT_H       0x43
#define MPU6886_GYRO_XOUT_L       0x44
#define MPU6886_GYRO_YOUT_H       0x45
#define MPU6886_GYRO_YOUT_L       0x46
#define MPU6886_GYRO_ZOUT_H       0x47
#define MPU6886_GYRO_ZOUT_L       0x48

#define MPU6886_USER_CTRL         0x6A
#define MPU6886_PWR_MGMT_1        0x6B
#define MPU6886_PWR_MGMT_2        0x6C
#define MPU6886_CONFIG            0x1A
#define MPU6886_GYRO_CONFIG       0x1B
#define MPU6886_ACCEL_CONFIG      0x1C
#define MPU6886_ACCEL_CONFIG2     0x1D
#define MPU6886_FIFO_EN           0x23

typedef union accel_t_gyro_union {
  struct {
    uint8_t x_accel_h;
    uint8_t x_accel_l;
    uint8_t y_accel_h;
    uint8_t y_accel_l;
    uint8_t z_accel_h;
    uint8_t z_accel_l;
    uint8_t t_h;
    uint8_t t_l;
    uint8_t x_gyro_h;
    uint8_t x_gyro_l;
    uint8_t y_gyro_h;
    uint8_t y_gyro_l;
    uint8_t z_gyro_h;
    uint8_t z_gyro_l;
  } reg;

  struct {
    int x_accel;
    int y_accel;
    int z_accel;
    int temperature;
    int x_gyro;
    int y_gyro;
    int z_gyro;
  } value;
};

bool IMU6886Flag = false;
uint8_t FSM = 0;


void setup() 
{
  int error;
  uint8_t c;
  
  M5.begin(true, true, true,&t_prev);
  SerialBT.begin("ESP32TESTLAST11");

  M5.IMU.Init();
        
  M5.dis.drawpix(0, 0xf00000);
  M5.dis.drawpix(8, 0xf00000);

  initBuffer();
}

void loop() 
{
  curSensoredTime = millis();
  
  // Read from sensor
  if(curSensoredTime - prevSensoredTime > SENSOR_READ_INTERVAL) {
    readFromSensor();  // Read from sensor
    prevSensoredTime = curSensoredTime;
    
    // Send buffer data to remote
    if(iAccelIndex >= ACCEL_BUFFER_COUNT - 3) {
      sendToRemote();
      initBuffer();
      Serial.println("------------- Send 20 accel data to remote");
    }
  }
}

/**************************************************
 * BT Transaction
 **************************************************/
void sendToRemote() {

  SerialBT.println((char*) aAccelBuffer);
  for(int i=0;i<125;i++)
  Serial.println(aAccelBuffer[i],HEX);
  // Flush buffer
  SerialBT.flush();
}

/**************************************************
 * Read data from sensor and save it
 **************************************************/
void readFromSensor() {
  int error;
  double dT;
  accel_t_gyro_union accel_t_gyro;
  
  error = MPU6886_read (MPU6886_ACCEL_XOUT_H, (uint8_t *) &accel_t_gyro, sizeof(accel_t_gyro));
  if(error != 0) {
    Serial.print(F("Read accel, temp and gyro, error = "));
    Serial.println(error,DEC);
  }
  

  
  uint8_t swap;
  #define SWAP(x,y) swap = x; x = y; y = swap
  SWAP (accel_t_gyro.reg.x_accel_h, accel_t_gyro.reg.x_accel_l);
  SWAP (accel_t_gyro.reg.y_accel_h, accel_t_gyro.reg.y_accel_l);
  SWAP (accel_t_gyro.reg.z_accel_h, accel_t_gyro.reg.z_accel_l);
  SWAP (accel_t_gyro.reg.t_h, accel_t_gyro.reg.t_l);
  SWAP (accel_t_gyro.reg.x_gyro_h, accel_t_gyro.reg.x_gyro_l);
  SWAP (accel_t_gyro.reg.y_gyro_h, accel_t_gyro.reg.y_gyro_l);
  SWAP (accel_t_gyro.reg.z_gyro_h, accel_t_gyro.reg.z_gyro_l);
  

  
  if(iAccelIndex < ACCEL_BUFFER_COUNT && iAccelIndex > 1) {
    int tempX = accel_t_gyro.value.x_accel; //x가 그린 y가 파랑 z가 빨강
    int tempY = accel_t_gyro.value.y_accel; 
    int tempZ = accel_t_gyro.value.z_accel; // 파랭이


    char temp = (char)(tempX >> 8);
    if(temp == 0x00)
      temp = 0x7f;
    aAccelBuffer[iAccelIndex] = temp;
    iAccelIndex++;
    temp = (char)(tempX);
    if(temp == 0x00)
      temp = 0x01;
    aAccelBuffer[iAccelIndex] = temp;
    iAccelIndex++;
    
    temp = (char)(tempY >> 8);
    if(temp == 0x00)
      temp = 0x7f;
    aAccelBuffer[iAccelIndex] = temp;
    iAccelIndex++;
    temp = (char)(tempY);
    if(temp == 0x00)
      temp = 0x01;
    aAccelBuffer[iAccelIndex] = temp;
    iAccelIndex++;
    
    temp = (char)(tempZ >> 8);
    if(temp == 0x00)
      temp = 0x7f;
    aAccelBuffer[iAccelIndex] = temp;
    iAccelIndex++;
    temp = (char)(tempZ);
    if(temp == 0x00)
      temp = 0x01;
    aAccelBuffer[iAccelIndex] = temp;
    iAccelIndex++;
  }
  

}

/**************************************************
 * MPU-6050 Sensor read/write
 **************************************************/
int MPU6886_read(int start, uint8_t *buffer, int size)
{
  int i, n, error;
  
  Wire.beginTransmission(MPU6886_I2C_ADDRESS);
  
  n = Wire.write(start);
  if (n != 1)
    return (-10);
  
  n = Wire.endTransmission(false); // hold the I2C-bus
  if (n != 0)
    return (n);
  
  // Third parameter is true: relase I2C-bus after data is read.
  Wire.requestFrom(MPU6886_I2C_ADDRESS, size, true);
  i = 0;
  while(Wire.available() && i<size)
  {
    buffer[i++]=Wire.read();
  }
  if ( i != size)
    return (-11); 
  return (0); // return : no error
}

int MPU6886_write(int start, const uint8_t *pData, int size)
{
  int n, error;
  
  Wire.beginTransmission(MPU6886_I2C_ADDRESS);
  
  n = Wire.write(start); // write the start address
  if (n != 1)
    return (-20);
    
  n = Wire.write(pData, size); // write data bytes
  if (n != size)
    return (-21);
    
  error = Wire.endTransmission(true); // release the I2C-bus
  if (error != 0)
    return (error);
  return (0); // return : no error
}


/**************************************************
 * Utilities
 **************************************************/
void initBuffer() {
  iAccelIndex = 2;
  for(int i=iAccelIndex; i<ACCEL_BUFFER_COUNT; i++) {
    aAccelBuffer[i] = 0x00;
  }
  aAccelBuffer[0] = 0xfe;
  aAccelBuffer[1] = 0xfd;
  aAccelBuffer[122] = 0xfd;
  aAccelBuffer[123] = 0xfe;
  aAccelBuffer[124] = 0x00;
}
