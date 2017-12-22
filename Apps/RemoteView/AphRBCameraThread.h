#pragma once

class ARBCameraThread : public AThread
{
protected:
	AIOSocket	*mlpCamSocket = nullptr;	///< Camera socket
	AInt64		mxiFrameCounter = -1;		///< The frame counter
	AMemStream	mmsLastFrame;				///< The most recent frame
	bool		mbShallEnable = false,		///< Defines if the camera shall be connected
				mbIsEnabled = false,		///< Defines if the camera is connected
				mbFailed = false;			///< Defines if the connection failed
	AString		msIp;						///< The IP to connect to
	int			miPort = 0;					///< The port to connect to

protected:
	//! Receives the most recent frame
	/** @param [in][out] Target The target stream
	* @param [in] RequestNext Defines if also the next frame shall be requested
	* @return True if any frame is available*/
	OFB_API const bool GetFrameInt(AMemStream &Target, const bool RequestNext = true);

public:
	//! Constructor
	/** @param [in] Ip The target ip
	  * @param [in] Port The target port */
	OFB_API ARBCameraThread(const AString &Ip, const int Port);

	//! Destructor
	OFB_API ~ARBCameraThread();

	//! Defines if a connection is wanted. Resets the failed state
	/** @param [in] State The new state */
	OFB_API void SetEnabled(const bool State);

	//! Returns if the camera is enabled and connected
	/** @return Enabled */
	OFB_API const bool GetEnabled();

	//! Returns if the connection failed
	/** @return True if the connection failed */
	OFB_API const bool GetFailed();

	//! Returns the frame counter
	/** @return The frame counter. -1 on failure / if no picture is available yet */
	OFB_API const AInt64 GetFrameCounter();

	//! Receives the most recent frame
	/** @param [in][out] Target The target stream
	* @return True if any frame is available */
	bool GetCameraFrame(AMemStream &Image);

	// Thread tick handling
	OFB_API void Tick() override;
};