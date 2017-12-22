#include "TutorialAppStd.h"
#include "../../Libraries/Fischertechnik/common.h"
#include "../../Libraries/Fischertechnik/FtShmem.h"
#include "../../Libraries/Fischertechnik/ftProInterface2013SocketCom.h"

// .........................................................................

// Constructor
ARBCameraThread::ARBCameraThread(const AString &Ip, const int Port) :
	AThread(),
	msIp(Ip),
	miPort(Port)
{
}

// .........................................................................

// Destructor
ARBCameraThread::~ARBCameraThread()
{
	Terminate(true);
}

// .........................................................................

// Tries to receive the next camera frame
bool ARBCameraThread::GetCameraFrame(AMemStream &Image)
{
	ALockThis();
	if (mxiFrameCounter != -1)
	{
		Image.Close();
		mmsLastFrame.SaveToStream(&Image);
		return true;
	}

	return false;
}
// .........................................................................

// Defines if a connection is wanted. Resets the failed state
void ARBCameraThread::SetEnabled(const bool State)
{
	ALockThis();
	if (mbIsEnabled)
	{
		return;
	}

	mbShallEnable = State;
	mbFailed = false;
	mbIsEnabled = false;

	Start();
}

// .........................................................................

// Returns if the camera is enabled and connected
const bool ARBCameraThread::GetEnabled()
{
	ALockThis();
	return mbIsEnabled;
}

// .........................................................................

// Returns if the connection failed
const bool ARBCameraThread::GetFailed()
{
	ALockThis();
	return mbFailed;
}

// .........................................................................

// Returns the frame counter
const AInt64 ARBCameraThread::GetFrameCounter()
{
	ALockThis();
	return mxiFrameCounter;
}

// .........................................................................

// Receives the most recent frame
const bool ARBCameraThread::GetFrameInt(AMemStream &Target, const bool RequestNext)
{
	if (!mlpCamSocket)
		return false;

	int result;

	Target.Close();
	AMemStream cams;

	// Read frame header
	ftIF2013Response_CameraOnlineFrame response;
	result = (int)mlpCamSocket->Receive(cams, sizeof(response));

	// result 0 means socket closed, <0 means error, otherwise #bytes received
	if (~cams != sizeof(response))
	{
		return false;
	}
	else
	{
		memcpy(&response, cams.GetDataPointer(), sizeof(response));
	}
	if (response.m_id != ftIF2013DataId_CameraOnlineFrame)
	{
		ALog("Mismatching camera response id");
		return false;
	}

	Target.Reserve(response.m_framesizecompressed);

	// Read frame body
	{
		// Read compressed frame
		size_t nRead = 0;
		size_t framesize = response.m_framesizecompressed;
		mlpCamSocket->Receive(Target, framesize);

		Target.Seek(0);
	}

	// Send Acknowledge
	ftIF2013Acknowledge_CameraOnlineFrame ack;
	ack.m_id = ftIF2013AcknowledgeId_CameraOnlineFrame;

	AMemStream snd;
	snd.Write(&ack, sizeof(ack));
	snd.Seek(0);

	result = (int)mlpCamSocket->Send(snd, snd.GetSize());
	if (result != sizeof(ack))
	{
		ALog("Error while sending acknowledge");
		return false;
	}

	return true;
}

// .........................................................................

// Thread tick handling
void ARBCameraThread::Tick()
{	
	bool	shallBeEnabled = false,
			isEnabled = false,
			isFailed = false;

	while(!this->GetIsTerminated())
	{

		{
			ALockThis();
			shallBeEnabled = mbShallEnable;
			isEnabled = mbIsEnabled;
			isFailed = mbFailed;
		}

		// check if connection has changed
		if (shallBeEnabled && !isEnabled)
		{
			mlpCamSocket = ANew(AIOSocket);
			bool success = mlpCamSocket->Connect(false, msIp, miPort);

			// update states
			{
				ALockThis();
				if (!success)
				{
					ADelete(mlpCamSocket);
					mbFailed = true;
					isEnabled = mbIsEnabled = false;
				}
				else
				{
					mbFailed = false;
					isEnabled = mbIsEnabled = true;
				}
			}
		}
		if (shallBeEnabled == false && isEnabled)
		{
			ADelete(mlpCamSocket);
			{
				ALockThis();
				isEnabled = mbIsEnabled = false;
			}
		}

		if (isEnabled && mlpCamSocket)
		{
			AMemStream tmp;
			if (GetFrameInt(tmp))
			{
				ALockThis();
				mmsLastFrame.Close();
				tmp.SaveToStream(&mmsLastFrame);
				mxiFrameCounter++;
			}
		}
		else
		{
			AThread::Sleep(0.0);
		}

		AThread::Tick();
	}

	if (GetIsTerminated() && isEnabled)
	{
		AMemStream tmp;
		GetFrameInt(tmp, false);
		if (mlpCamSocket)
		{
		}
		ADelete(mlpCamSocket);
	}
}