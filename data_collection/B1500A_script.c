

// Telegraph Noise.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

// WGFMU_template.cpp //

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "wgfmu.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
using namespace std;

void writeDebugResults(int channelId, const char* fileName) {
    FILE* fp;
    errno_t err = fopen_s(&fp, fileName, "w");
    cout << "File Name is " << fileName << "\n";

    if (err != 0) {
   	 cout << "File opening failed with error code: " << err << "\n";
   	 return;
    }
    if (fp == nullptr) {
   	 cout << "File pointer is null.\n";
   	 return;
    }

    int measuredSize, totalSize;
    WGFMU_getMeasureValueSize(channelId, &measuredSize, &totalSize);
    cout << "Measured Size: " << measuredSize << ", Total Size: " << totalSize << "\n";
    if (measuredSize <= 0) {
   	 cout << "No measure values to write.\n";
   	 return;
    }
    for (int i = 0; i < measuredSize; i++) {
   	 double time, value;
   	 if (WGFMU_getMeasureValue(channelId, i, &time, &value) != 0) {
   		 cout << "Failed to get measure value for index: " << i << "\n";
   		 continue;
   	 }
   	 fprintf(fp, "%.9lf, %.9lf\n", time, value);
    }
    fclose(fp);
}
#define VDSWEEP 0

int main()
{

    //Currenty sweeping Vds for constant Vgs
    // Only 6 values tested at 1 time; this is within the loop
    // So, 1 file per 6 bias values
    //Currently, stepsize is 25mV, 25x6 = 150mV.
#if VDSWEEP > 0
    const int numDataFiles = 4;

    double vginit = 0.85;
    double vd0 = 0.05;
    double stepsize = 0.025;
    double vdinitarr[numDataFiles];
    //double vdinitarr[numDataFiles] = { 0.05, 0.2, 0.35 };
    for (int i = 0; i < numDataFiles; i++) {
   	 vdinitarr[i] = vd0 + i * stepsize * 6;
    }
#else
    const int numDataFiles = 1;

    double vdinit = 0.5;
    double vg0 = 0.4;
    double stepsize = 0.05;
    double vginitarr[numDataFiles];
    //double vdinitarr[numDataFiles] = { 0.05, 0.2, 0.35 };
    for (int i = 0; i < numDataFiles; i++) {
   	 vginitarr[i] = vg0 + i * stepsize * 6;
    }


#endif


    for (int piter = 0; piter < numDataFiles; piter++) {
   	 int experimentPoints = 650000;
   	 double experimentInterval = 1e-4;
   	 double experimentAverage = 0.9e-4;

   	 // rising edge time.
   	 double time0 = 0.05;
   	 double time1 = time0 + experimentInterval * experimentPoints + experimentAverage;
   	 double rest = 3;

   	 int channel1 = 101;
   	 int channel2 = 102;

   	 // OFFLINE
   	 WGFMU_clear();

   	 //Vg = 0.8
#if VDSWEEP > 0
   	 double vgValues[] = { 0.65,0.65,0.65,0.65,0.65,0.65 };
   	 for (int i = 0; i < 6; i++)
   	 {
   		 vgValues[i] = vginit;
   	 }

   	 // Vd
   	 double vdinit = vdinitarr[piter];
   	 double vdValues[] = { 0.65,0.65,0.65,0.65,0.65,0.65 };
   	 for (int i = 0; i < 6; i++)
   	 {
   		 vdValues[i] = vdinit + i * stepsize;
   	 }
#else
   	 double vdValues[] = { 0.65,0.65,0.65,0.65,0.65,0.65 };
   	 for (int i = 0; i < 6; i++)
   	 {
   		 vdValues[i] = vdinit;
   	 }

   	 // Vg
   	 double vginit = vginitarr[piter];
   	 double vgValues[] = { 0.65,0.65,0.65,0.65,0.65,0.65 };
   	 for (int i = 0; i < 6; i++)
   	 {
   		 vgValues[i] = vginit + i * stepsize;
   	 }
#endif


   	 //double vdValues[] = { vd,0.vd +.25, vd + .25*2,vd +.25*3,vd +.25*4,vd+.25*4};

   	 //double vgValues[] = { 0.75,0.8, 0.85, 0.9,0.95, 1 };
   	 //double vdValues[] = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 };

   	 WGFMU_createPattern("v1", 0);
   	 //WGFMU_addVector("v1", time0, 0.7);
   	 //WGFMU_addVector("v1", time1, 0.7);

   	 for (double v : vgValues) {
   		 WGFMU_addVector("v1", time0, v);
   		 WGFMU_addVector("v1", time1, v);
   		 WGFMU_addVector("v1", time0, 0);
   		 WGFMU_addVector("v1", rest, 0);
   	 }

   	 //Vd = 0.5
   	 WGFMU_createPattern("v2", 0);
   	 //WGFMU_addVector("v2", time0, 0.2);
   	 //WGFMU_addVector("v2", time1, 0.2);
   	 for (double v : vdValues) {
   		 WGFMU_addVector("v2", time0, v);
   		 WGFMU_addVector("v2", time1, v);
   		 WGFMU_addVector("v2", time0, 0);
   		 WGFMU_addVector("v2", rest, 0);
   	 }

   	 double timeMultiplier = 0;
   	 //WGFMU_setMeasureEvent("v2", "measurement", time0,
   	 //    experimentPoints, experimentInterval, experimentAverage,
   	 //    WGFMU_MEASURE_EVENT_DATA_AVERAGED);
   	 for (double v : vgValues) {
   		 // WGFMU_setMeasureEvent(pattern, event, start time, measurement points, double interval, double average, int rdata)
   		 WGFMU_setMeasureEvent("v2", "measurement", ((timeMultiplier + 1) * time0) + (timeMultiplier * time1) + (timeMultiplier * (rest + time0)),   //+ (timeMultiplier * (rest+time0))
   			 experimentPoints, experimentInterval, experimentAverage,
   			 WGFMU_MEASURE_EVENT_DATA_AVERAGED);
   		 timeMultiplier++;
   	 }

   	 WGFMU_addSequence(channel1, "v1", 1);
   	 WGFMU_addSequence(channel2, "v2", 1);

   	 // ONLINE
   	 WGFMU_openSession("GPIB0::18::INSTR");
   	 WGFMU_initialize();
   	 WGFMU_setOperationMode(channel1, WGFMU_OPERATION_MODE_FASTIV);
   	 WGFMU_setMeasureMode(channel1, WGFMU_MEASURE_MODE_VOLTAGE);

   	 // This Value may subject to change based on the required resolution
   	 WGFMU_setMeasureCurrentRange(channel2, WGFMU_MEASURE_CURRENT_RANGE_1MA);

   	 WGFMU_setOperationMode(channel2, WGFMU_OPERATION_MODE_FASTIV);
   	 WGFMU_setMeasureMode(channel2, WGFMU_MEASURE_MODE_CURRENT);
   	 WGFMU_connect(channel1);
   	 WGFMU_connect(channel2);
   	 WGFMU_execute();
   	 WGFMU_waitUntilCompleted();


   	 //    writeDebugResults(channel2, "C:/wgfmu_example/RFSOI_pbits/AUTO_TEST_nthin56_Vd(" << vdValues[0]) << ")_Vg0.65_10kHz_2K_3900000_1MA.csv");
   		 /*
   		 //string s = Arrays.toString(vdValues);
   		 vdstring=
   		 for (int i = 0; i < 6; i++)
   		 {
   			 vdstringvdValues[i]
   		 }
   		 */
#if VDSWEEP > 0
   	 string vdValuesStr[6];
   	 for (int i = 0; i < 6; i++)
   	 {
   		 vdValuesStr[i] = to_string(vdValues[i]);
   		 vdValuesStr[i] = vdValuesStr[i].substr(0, vdValuesStr[i].find(".") + 4);
   	 }
   	 string vdPrints = vdValuesStr[0];
   	 for (int i = 1; i < 6; i++)
   	 {
   		 vdPrints += ("," + vdValuesStr[i]);

   	 }
   	 string vginitprint = to_string(vginit);
   	 vginitprint = vginitprint.substr(0, vginitprint.find(".") + 4);
   	 string finalVdPrints = "C:/wgfmu_example/RFSOI_pbits/nthin56_Vd(" + vdPrints + ")_Vg" + vginitprint + "_10kHz_2K_3900000_1MA.csv";
   	 const char* charVdPrints = finalVdPrints.c_str();
   	 writeDebugResults(channel2, charVdPrints);

#else
   	 string vgValuesStr[6];
   	 for (int i = 0; i < 6; i++)
   	 {
   		 vgValuesStr[i] = to_string(vgValues[i]);
   		 vgValuesStr[i] = vgValuesStr[i].substr(0, vgValuesStr[i].find(".") + 4);
   	 }
   	 string vgPrints = vgValuesStr[0];
   	 for (int i = 1; i < 6; i++)
   	 {
   		 vgPrints += ("," + vgValuesStr[i]);

   	 }
   	 string vdinitprint = to_string(vdinit);
   	 vdinitprint = vdinitprint.substr(0, vdinitprint.find(".") + 4);
   	 string finalVgPrints = "C:/wgfmu_example/RFSOI_pbits/nthin56_Vg(" + vgPrints + ")_Vd" + vdinitprint + "_10kHz_2K_3900000_1MA.csv";
   	 const char* charVgPrints = finalVgPrints.c_str();
   	 writeDebugResults(channel2, charVgPrints);

#endif

   	 //writeDebugResults(channel2, "C:/wgfmu_example/RFSOI_pbits/nthin56_Vd( 0.05,to,0.175)_Vg0.75_10kHz_2K_3900000_1MA.csv");
   	 WGFMU_initialize();
   	 WGFMU_closeSession();;
    }
}



