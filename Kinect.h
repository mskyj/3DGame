#ifndef _KINECT_H_
#define _KINECT_H_

#include <NiTE.h>
#include <MyOpencvLib.h>
#include "Quaternion.h"
#include <iostream>
using namespace::std;

class Kinect
{
private:
	nite::UserTracker userTracker;
	nite::UserTrackerFrameRef userFrame;
	Vector3f pos[15];
	float posConf[15];
	Quaternion quat[15];
	Vector3f armVec[2];
public:
	Kinect();
	~Kinect();
	bool init();
	void Update();
	cv::Mat DrawUser();
	bool IsReady();
	int GetLArmVec( Vector3f* save );
	int GetRArmVec( Vector3f* save );
	Quaternion* GetBoneRot();
	float GetBodyAngleY();
	float GetBodyAngleXZ();
	bool isJumping();
};

#endif