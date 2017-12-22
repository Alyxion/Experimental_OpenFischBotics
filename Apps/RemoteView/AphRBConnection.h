#pragma once

class ARBCameraThread;
struct shm_if_s;

#define OFB_MAX_HISTORY_COUNT 32	///< The maximum history length, 32 compensates a maximum GUI lag of 320ms
#define OFB_MAX_SENSOR_INPUTS 8		///< The maximum count of sensor inputs
#define OFB_MAX_ENGINES 8			///< The maximum count of engines / outputs
#define OFB_MAX_ENGINE_COUNTERS 4	///< The maximum count of encoder engine counters
#define OFB_MAX_SENSOR_DATA_SIZE (OFB_MAX_SENSOR_INPUTS*OFB_MAX_HISTORY_COUNT)				///< For each history entry all sensor entries
#define OFB_MAX_ENGINE_COUNTER_DATA_SIZE (OFB_MAX_ENGINE_COUNTERS*OFB_MAX_HISTORY_COUNT)	///< For each history entry all engine counter entries

// ----------------------------------------------------------------------------

/** @brief Mode of multipurpose input */
enum class FTControllerSensorMode
{
	None,			///< Disabled
	DigiOhm,		///< Digital ohm
	DigiVolt,		///< Digital voltage
	AnalogOhm,		///< Analog ohm
	AnalogVolt,		///< Analog voltage
	Ultrasonic		///< Ultrasonic
};

// ----------------------------------------------------------------------------

class FTControllerConnection : public AThread
{
protected:
	bool				mbConnected = false,				///< Defines if we are connected
						mbFailed = false,					///< Defines if connecting failed
						mbShallConnect = false;				///< Defines if a connection shall be established
	AInt64				mxiLastPerformanceQuery = 0,		///< Last reaction time check tick
						mxiTurnsSinceLastQuery = 0;			///< Turns since last check
	AString				msFtIpRange,						///< IP range, potential addresses separated by slash
						msFtIp;								///< Remote device ip
	AIOSocket			*mlpMainSocket = nullptr;			///< Main connection socket
	ARBCameraThread		*mlpCameraThread = nullptr;			///< The camera thread
	double				madOutputs[OFB_MAX_ENGINES];		///< Engine output values

	int					miCamResX = 320,					///< The camera's x resolution
						miCamResY = 240,					///< The camera's y resolution
						miCamRefreshRate = 50;				///< The camera's refresh rate

	AInt64				mxiLastDataUpdate = 0;														///< The tick at which the last data updated occurred
	AInt64				maxiHistoryTime[OFB_MAX_HISTORY_COUNT];										///< The tick (time) of the historical input data

	int						maiInputs[OFB_MAX_SENSOR_DATA_SIZE];									///< The sensor input states
	FTControllerSensorMode	maCurrentSensorMode[OFB_MAX_SENSOR_INPUTS];									///< The current sensor mode
	FTControllerSensorMode	maDesiredSensorMode[OFB_MAX_SENSOR_INPUTS];									///< The desired sensor mode

	UINT16				mausLastKnownResetIndex[OFB_MAX_ENGINE_COUNTERS];							///< The last known encoder motor counters
	AInt64				maxiLastMotorCounter[OFB_MAX_ENGINE_COUNTER_DATA_SIZE];						///< The last known encoder motor counters
	AInt64				maxiMotorCounterAbsolute[OFB_MAX_ENGINE_COUNTER_DATA_SIZE];					///< The encoder motor counters
	AInt64				maxiMotorTargets[OFB_MAX_ENGINE_COUNTERS];									///< The motor's targets
	AInt64				maxiMotorTargetDiff[OFB_MAX_ENGINE_COUNTERS];								///< The motor's target diffs upon setting
	AInt64				maxiMotorLastDiffTick[OFB_MAX_ENGINE_COUNTERS];								///< The motor's last movement change tick
	bool				mabMotorTargetReached[OFB_MAX_ENGINE_COUNTERS];								///< Defines if given motor has reached it's most precise target

	shm_if_s	*m_transferarea;
	// Times (in ms) when the corresponding timer was last updated
	long m_timelast[6];

	int m_nAreas = 1;

	//! Tries to connect to the FT TXT controller
	bool ConnectInt();

	//! Backups currently known device states
	OFB_API void BackupStates();

public:
	//! Constructor
	/** @param [in] Ip List of potential ip addresses, separated by slash */
	OFB_API FTControllerConnection(const AString &Ip);

	//! Destructor
	OFB_API ~FTControllerConnection();

	//! Requests the socket to connect
	OFB_API void Connect();

	//! Returns if we are successfully connected
	/** @return The current state */
	OFB_API const bool GetConnected();

	//! Returns if the connection tries failed
	/** @return True if the connection failed */
	OFB_API const bool GetFailed();

	//! Returns the last data update tick time
	/** @return The last data update's tick */
	OFB_API const AInt64 GetLastDataUpdateTick();

	//! Sets the new mode for given sensor
	/** @param [in] Index The sensor index
	  * @param [in] Mode The new mode */
	OFB_API void SetSensorMode(const int Index, const FTControllerSensorMode Mode);

	//! Returns the current (desired) sensor mode
	/** @param [in] Index The sensor index
	  * @return The current mode */
	OFB_API const FTControllerSensorMode GetSensorMode(const int Index);

	//! Changes given engine output value
	/** @param [in] Index The socket index (0..7)
	  * @param [in] Value The output value in percent */
	OFB_API void SetEngineOutput(const int Index, const double Value);

	//! Returns the counter of given motor
	/** @param [in] Index The motor / counter index
	  * @return The current value */
	OFB_API AInt64 GetMotorTurnCounter(const int Index);

	//! Returns the current motor distances
	/** @param [in][opt] TimeStamp The last valid time stamp. -1 if no valid time stamp is available
	  * @param [in][out][opt] Times An array with the size of at least OFB_MAX_HISTORY_COUNT to receive the time stamps of the motor counters history. The first invalid value is set to -1
	  * @return The motor distances */
	OFB_API const AList<AInt64> GetMotorCounters(const AInt64 TimeStamp = -1, AInt64* Times = nullptr);

	//! Returns the current sensor input values
	/** @param [in][opt] TimeStamp The last valid time stamp. -1 if no valid time stamp is available
	  * @param [in][out][opt] Times An array with the size of at least OFB_MAX_HISTORY_COUNT to receive the time stamps of the sensor history. The first invalid value is set to -1
	  * @return The sensor states */
	OFB_API const AList<int> GetSensorInputs(const AInt64 TimeStamp = -1, AInt64* Times = nullptr);

	//! Sets the desired motor distances. Set a value to -1 for deactivating given slot
	/** @param [in] Distances The list of desired motor distances */
	OFB_API void SetMotorTargetCounters(const AList<AInt64> &Distances);

	//! Returns the desired motor distances
	/** @return List with desired distances */
	OFB_API const AList<AInt64> GetMotorTargetCounters();

	//! Returns for each motor if it's target has been reached
	/** @return List of states */
	OFB_API const AList<bool> GetMotorTargetsReached();

	void SetDefaultConfig();

	bool DoTransferSimple();

	void UpdateTimers();

	// Update the I/O (e.g. universal input) configuration
	bool UpdateConfig();

	bool SendCommand(const struct ftIF2013Command_Base *commanddata, size_t commandsize, enum ftIF2013ResponseId responseid, struct ftIF2013Response_Base *responsedata, size_t responsesize);

	bool StartOnline();

	bool StopOnline();

	bool StopCamera();

	bool StartCamera();

	//! Changes the camera's resolution
	/** @param [in] Width The horizontal resolution
	  * @param [in] Height The vertical resolution
	  * @param [in] Frames The desired frame count */
	OFB_API const bool SetCameraResolution(const int Width, const int Height, const int Frames);

	unsigned int GetVersion();

	//! Returns the frame counter
	/** @return The frame counter. -1 on failure / if no picture is available yet */
	OFB_API const AInt64 GetCameraFrameCounter();

	//! Receives the most recent frame
	/** @param [in][out] Target The target stream
	* @return True if any frame is available */
	OFB_API const bool GetCameraFrame(AMemStream &Target);

	// Thread tick handling
	OFB_API void Tick() override;
};