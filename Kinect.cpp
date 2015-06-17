#include "Kinect.h"

Kinect::Kinect()
{
	for( int i=0; i<15; i++ )
	{
		pos[i] = Vector3f();
		quat[i] = Quaternion();
	}
}

Kinect::~Kinect()
{

}

bool Kinect::init()
{
	try {
		nite::Status niteRet = nite::NiTE::initialize();
		if ( niteRet != nite::STATUS_OK ) {
            throw std::runtime_error( "initialize" );
        }

        niteRet = userTracker.create();
        if ( niteRet != nite::STATUS_OK ) {
            throw std::runtime_error( "userTracker.create" );
        }
		return true;
	}
    catch ( exception& ) {
        cout << openni::OpenNI::getExtendedError() << endl;
		return false;
    }
}

void Kinect::Update()
{
	nite::Status rc = userTracker.readFrame( &userFrame );
	if (rc != nite::STATUS_OK)
	{
		cout << "Kinect Error : Update() failed to get next data" << endl;
		return;
	}

	const nite::Array<nite::UserData>& users = userFrame.getUsers();
    for ( int i = 0; i < users.getSize(); ++i ) {
        const nite::UserData& user = users[i];
		// 新しいユーザーであれば骨格トラッキング開始
        if ( user.isNew() ) {
            userTracker.startSkeletonTracking( user.getId() );
        }
		// トラッキング中であれば
        else if ( !user.isLost() ) {
			// 骨格情報の取得
            const nite::Skeleton& skeelton = user.getSkeleton();
            if ( skeelton.getState() == nite::SkeletonState::SKELETON_TRACKED ) {
				/*
					nite::JointType
					0: 頭
					1: 首
					2: 左肩 3: 右肩
					4: 左肘 5: 右肘
					6: 左手首 7: 右手首
					8: 胸
					9: 左脇腹 10: 右脇腹
					11: 左膝 12: 右膝
					13: 左足首 14: 右足首

					座標系
					x:右が正 y:上が正 z:後が正
				*/
				Quaternion temp[15];

				// 前後左右の傾き判定
				for( int j=0; j<15; j++ )
				{
					const nite::SkeletonJoint& joint = skeelton.getJoint((nite::JointType)j);
					if( joint.getPositionConfidence()>0.5f )
						pos[j] = Vector3f(-joint.getPosition().x, joint.getPosition().y, -joint.getPosition().z);
					posConf[j] = joint.getPositionConfidence();
				}
            }
        }
    }
}

cv::Scalar colors[] = {
    cv::Scalar( 1, 0, 0 ),
    cv::Scalar( 0, 1, 0 ),
    cv::Scalar( 0, 0, 1 ),
    cv::Scalar( 1, 1, 0 ),
    cv::Scalar( 1, 0, 1 ),
    cv::Scalar( 0, 1, 1 ),
};

cv::Mat Kinect::DrawUser()
{
    cv::Mat depthImage;

    openni::VideoFrameRef depthFrame = userFrame.getDepthFrame();
    if ( depthFrame.isValid() ) {
        openni::VideoMode videoMode = depthFrame.getVideoMode();
        depthImage = cv::Mat( videoMode.getResolutionY(),
                                videoMode.getResolutionX(),
                                CV_8UC4 );

        openni::DepthPixel* depth = (openni::DepthPixel*)depthFrame.getData();
        const nite::UserId* pLabels = userFrame.getUserMap().getPixels();
        for (int i = 0; i < (depthFrame.getDataSize()/sizeof(openni::DepthPixel)); ++i) {
            // 画像インデックスを生成
            int index = i * 4;

            // 距離データを画像化する
            UCHAR* data = &depthImage.data[index];
            if ( pLabels[i] != 0 ) {
                data[0] *= colors[pLabels[i]][0];
                data[1] *= colors[pLabels[i]][1];
                data[2] *= colors[pLabels[i]][2];
            }
            else {
                // 0-255のグレーデータを作成する
                // distance : 10000 = gray : 255
                int gray = ~((depth[i] * 255) / 10000);
                data[0] = gray;
                data[1] = gray;
                data[2] = gray;
            }
        }
    }

    return depthImage;
}

bool Kinect::IsReady()
{
	bool flag = true;
	for( int i=0; i<8; i++ )
		if( pos[i]==Vector3f() || posConf[i]<0.5f )
		{
			flag = false;
			break;
		}
	return flag;
}

Quaternion* Kinect::GetBoneRot()
{
	bool calcFlag = true;
	for( int i=0; i<8; i++ )
		if( pos[i]==Vector3f() )
		{
			calcFlag = false;
			break;
		}

	if( calcFlag )
	{
		if( posConf[1]>=0.7f && posConf[9]>=0.7f && posConf[10]>=0.7f )
		{
			Vector3f v1 = Vector3f(0.0f, 1.0f, 0.0f);
			Vector3f v2 = (pos[1] - (pos[9]+pos[10])/2.0f);
			v2.x = 0.0f;
			v2.normalize();
			float theta = -acosf( v1.dot( v2 ) );
			if( abs(theta) < 0.1f || theta > 0.0f )
				quat[1] = Quaternion();
			else
			{
				Vector3f axis = v1.cross(v2).normalize();
				float s = sin(theta/2.0f);
				quat[1] = Quaternion(axis.x*s,axis.y*s,axis.z*s,cos(theta/2.0f));
			}
		}

		Vector3f vec[6];
		vec[0] = Vector3f(2.18f, -1.67f, 0.0f).normalize();
		vec[1] = Vector3f(-2.18f, -1.67f, 0.0f).normalize();
		vec[2] = (pos[4] - pos[2]).normalize();
		vec[3] = (pos[5] - pos[3]).normalize();
		vec[4] = (pos[6] - pos[4]).normalize();
		vec[5] = (pos[7] - pos[5]).normalize();
		for( int i=0; i<4; i++ )
		{
			if( posConf[i+2]>=0.5f && posConf[i+4]>=0.5f)
			{
				float theta = -acosf( vec[i].dot( vec[i+2] ) )*0.9f;
				if( abs(theta) < FLT_EPSILON )
					continue;
				Vector3f axis = vec[i].cross(vec[i+2]).normalize();
				float s = sin(theta/2.0f);
				quat[i+2] = Quaternion(axis.x*s,axis.y*s,axis.z*s,cos(theta/2.0f));
			}
		}
	}

	return quat;
}

int Kinect::GetLArmVec( Vector3f* save )
{
	for( int i=1; i<4; i++ )
		if( pos[i*2]==Vector3f() )
			return 0;
		else if( posConf[i*2]<0.5f )
		{
			if( armVec[0]==Vector3f() )
				return 0;
			*save = armVec[0];
			return 1;
		}

	Vector3f vec[2];
	vec[0] = (pos[4] - pos[2]).normalize();
	vec[1] = (pos[6] - pos[4]).normalize();

	if( abs(acosf( vec[0].dot( vec[1] ) )) > 0.6f )
	{
		armVec[0]=Vector3f();
		return 0;
	}

	armVec[0] = (vec[0]+vec[1]).normalize();
	*save = armVec[0];
	return 1;
}

int Kinect::GetRArmVec( Vector3f* save )
{
	for( int i=1; i<4; i++ )
		if( pos[i*2]==Vector3f() )
			return 0;
		else if( posConf[i*2]<0.5f )
		{
			if( armVec[1]==Vector3f() )
				return 0;
			*save = armVec[1];
			return 1;
		}

	Vector3f vec[2];
	vec[0] = (pos[5] - pos[3]).normalize();
	vec[1] = (pos[7] - pos[3]).normalize();

	if( abs(acosf( vec[0].dot( vec[1] ) )) > 0.6f )
	{
		armVec[1]=Vector3f();
		return 0;
	}

	armVec[1] = (vec[0]+vec[1]).normalize();
	*save = armVec[1];
	return 1;
}

float Kinect::GetBodyAngleY()
{
	if( posConf[1]<0.7f || posConf[9]<0.7f || posConf[10]<0.7f )
		return 0.0f;

	Vector3f vec = pos[1] - (pos[9]+pos[10])/2.0f;
	if( vec.z<20.0f )
		return 0.0f;
	float angle = atan2(vec.z,vec.y);
	if( angle>0.8f )
		angle=0.8f;

	return angle;
}

float Kinect::GetBodyAngleXZ()
{
	if( posConf[2]<0.7f || posConf[3]<0.7f )
		return 0.0f;

	Vector3f vec = pos[2] - pos[3];
	if( abs(vec.z)<10.0f )
		return 0.0f;

	return -atan2(vec.z,vec.x)/20.0f;
}

bool Kinect::isJumping()
{
	static Vector3f lLegPos;
	static Vector3f rLegPos;

	if( posConf[13]<0.7f || posConf[14]<0.7f )
		return false;

	if( pos[13].y < lLegPos.y )
		lLegPos = pos[13];
	if( pos[14].y < rLegPos.y )
		rLegPos = pos[14];

	if( lLegPos.y+30.0f < pos[13].y && rLegPos.y+30.0f < pos[14].y )
		return true;
	else
		return false;
}