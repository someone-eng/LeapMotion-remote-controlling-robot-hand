/* Copyright (C) 2012-2017 Ultraleap Limited. All rights reserved.
 *
 * Use of this code is subject to the terms of the Ultraleap SDK agreement
 * available at https://central.leapmotion.com/agreements/SdkAgreement unless
 * Ultraleap has signed a separate license agreement with you or your
 * organisation.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//leap motion using
#include "LeapC.h"
#include "ExampleConnection.h"

// Serial using
#include <windows.h>
#include <string.h>

#if 1   //Open Debug print
#define LOGD(...) printf(__VA_ARGS__)
#else   //close Debug print
#define LOGD(...)
#endif

#if 1   //Open error print
#define LOGE(...) printf(__VA_ARGS__)
#else   //close error print
#define LOGE(...)
#endif

 //Buffer size
#define BUF_SIZE    8192
#define EXIT_STR    "exit"
#define I_EXIT      "I exit.\r\n"
#define I_RECEIVE   "I receive.\r\n"

int64_t lastFrameID = 0; //The last frame received

//Open serial
HANDLE OpenSerial(const char* com, //serial name: COM1，COM2...
    int baud,       //BAUD: CBR_9600、CBR_19200、CBR_38400、CBR_115200、CBR_230400、CBR_460800
    int byteSize,   //value: 7、8；
    int parity,     //value: NOPARITY、ODDPARITY、EVENPARITY、MARKPARITY、SPACEPARITY
    int stopBits)   //value: ONESTOPBIT、ONE5STOPBITS、TWOSTOPBITS；
{
    DCB dcb;
    BOOL b = FALSE;
    COMMTIMEOUTS CommTimeouts;
    HANDLE comHandle = INVALID_HANDLE_VALUE;

    //Open Serial
    comHandle = CreateFile(com,            //Serial name
        GENERIC_READ | GENERIC_WRITE,      //readable, writable  				 
        0,            // No Sharing                               
        NULL,         // No Security                              
        OPEN_EXISTING,// Open existing port only                     
        FILE_ATTRIBUTE_NORMAL,            // Non Overlapped I/O                           
        NULL);        // Null for Comm Devices

    if (INVALID_HANDLE_VALUE == comHandle) {
        LOGE("CreateFile fail\r\n");
        return comHandle;
    }

    // Set IO buffer size
    b = SetupComm(comHandle, BUF_SIZE, BUF_SIZE);
    if (!b) {
        LOGE("SetupComm fail\r\n");
    }

    //Set I/O timeout
    CommTimeouts.ReadIntervalTimeout = MAXDWORD;//read interval timeout
    CommTimeouts.ReadTotalTimeoutMultiplier = 0;//read time multiplier
    CommTimeouts.ReadTotalTimeoutConstant = 0;//read timeout constant
    CommTimeouts.WriteTotalTimeoutMultiplier = 1;//write total timeout multiplier
    CommTimeouts.WriteTotalTimeoutConstant = 1;//write total timeout constant
    b = SetCommTimeouts(comHandle, &CommTimeouts); //set timeout
    if (!b) {
        LOGE("SetCommTimeouts fail\r\n");
    }

    //set serial status
    GetCommState(comHandle, &dcb);//get current status
    dcb.BaudRate = baud; //baud
    dcb.ByteSize = byteSize; //byte size
    dcb.Parity = parity; //parity
    dcb.StopBits = stopBits; //stopbits
    b = SetCommState(comHandle, &dcb);//set
    if (!b) {
        LOGE("SetCommState fail\r\n");
    }

    return comHandle;
}

typedef struct {
    float x, y, z;
} Vector3;

float Vec3_length(Vector3 v){
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

float Vec3_distance(Vector3 a, Vector3 b) {
    Vector3 diff = { a.x - b.x, a.y - b.y, a.z - b.z };
    return Vec3_length(diff);
}

short fmap(float x, float in_min, float in_max, short out_min, short out_max) {
    float scale = (float)(out_max - out_min) / (in_max - in_min);
    return (short)((x - in_min) * scale + out_min);
}

int main(int argc, char** argv) {
  OpenConnection();
  while(!IsConnected)
    millisleep(100); //wait a bit to let the connection complete

  printf("Connected.");
  LEAP_DEVICE_INFO* deviceProps = GetDeviceProperties();
  if(deviceProps)
    printf("Using device %s.\n", deviceProps->serial);

  // bluetooth part
  BOOL b = FALSE;
  DWORD wRLen = 0;
  DWORD wWLen = 0;
  char buf[BUF_SIZE] = { 0 };
  HANDLE comHandle = INVALID_HANDLE_VALUE;//serial handle   

  //Open Serial
  const char* com = "COM3";
  comHandle = OpenSerial(com, CBR_38400, 8, NOPARITY, ONESTOPBIT);
  if (INVALID_HANDLE_VALUE == comHandle) {
      LOGE("OpenSerial COM3 fail!\r\n");
      return -1;
  }
  LOGD("Open COM3 Successfully!\r\n");

  // convert float to str
  typedef union {
      short i;
      char c[sizeof(short)];
  } uTemp;

  uTemp u;
  Vector3 v, v0;
  float dist[5];
  float in_min[5] = { 93,109,113,102,95 };
  float in_max[5] = { 54,26,24,28,33 };
  const char * finger[5] = {"T", "I", "M", "R", "L"};

  for (;;) {
      LEAP_TRACKING_EVENT* frame = GetFrame();
      //printf("%lli\n", frame->tracking_frame_id);
      if (frame && (frame->tracking_frame_id > lastFrameID)) {
          lastFrameID = frame->tracking_frame_id;
          if (frame->nHands > 0) {
              LEAP_HAND* hand = &frame->pHands[0];
              v0.x = hand->palm.position.x;
              v0.y = hand->palm.position.y;
              v0.z = hand->palm.position.z;
              for (uint32_t i = 0; i < 5; i++) {
                  v.x = hand->digits[i].distal.next_joint.x;
                  v.y = hand->digits[i].distal.next_joint.y;
                  v.z = hand->digits[i].distal.next_joint.z;
                  dist[i] = Vec3_distance(v, v0);
                  u.i = fmap(dist[i], in_min[i], in_max[i], 0, 269); //0-269为舵机的转动角度范围
                  b = WriteFile(comHandle, finger[i], 1, &wWLen, NULL);
                  b = WriteFile(comHandle, u.c, sizeof(u.c), &wWLen, NULL);  //strlen->sizeof就正常了
                  Sleep(10);  //wait for BT.read()
                  printf("%d\n", u.i);
                  //b = WriteFile(comHandle, "E", 1, &wWLen, NULL);
              }

              //printf("Hand id %i is a %s hand with distance (%f, %f, %f, %f, %f).\n",
              //    hand->id,
              //    (hand->type == eLeapHandType_Left ? "left" : "right"),
              //    dist[0],
              //    dist[1],
              //    dist[2],
              //    dist[3],
              //    dist[4]);
          }
          //}
          
      }
  }
  return 0;
}
//End-of-Sample
