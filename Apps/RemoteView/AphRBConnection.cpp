// ----------------------------------------------------------------------------

#include "TutorialAppStd.h"
#include "../../Libraries/Fischertechnik/common.h"
#include "../../Libraries/Fischertechnik/FtShmem.h"
#include "../../Libraries/Fischertechnik/ftProInterface2013SocketCom.h"

// ----------------------------------------------------------------------------

// Constructor
FTControllerConnection::FTControllerConnection(const AString &Ip) : msFtIpRange(Ip)
{
	m_transferarea = ANew(FISH_X1_TRANSFER);
	memset(m_transferarea, 0, sizeof(FISH_X1_TRANSFER));
	SetDefaultConfig();

	mxiLastDataUpdate = ATime::GetTick();

	for (int i = 0; i < OFB_MAX_ENGINES; ++i)
	{
		madOutputs[i] = 0.0;
		mausLastKnownResetIndex[i] = 0;
	}

	// clear engine targets
	for (int i = 0; i < OFB_MAX_ENGINE_COUNTERS; ++i)
	{
		maxiMotorTargets[i] = 0;
		maxiMotorTargetDiff[i] = 0;
		maxiMotorLastDiffTick[i] = 0;
		mabMotorTargetReached[i] = true; // nothing to be done
	}

	// clear counter data
	for (int i = 0; i < OFB_MAX_ENGINE_COUNTER_DATA_SIZE; ++i)
	{
		maxiLastMotorCounter[i] = 0;
		maxiMotorCounterAbsolute[i] = 0;
	}

	// clear sensor modes
	for (int i = 0; i < OFB_MAX_SENSOR_INPUTS; ++i)
	{
		maCurrentSensorMode[i] = FTControllerSensorMode::None;
		maDesiredSensorMode[i] = FTControllerSensorMode::None;
	}

	// clear sensor data
	for (int i = 0; i < OFB_MAX_SENSOR_DATA_SIZE; ++i)
	{
		maiInputs[i] = 0;
	}

	// clear history ticks
	for (int i = 0; i < OFB_MAX_HISTORY_COUNT; ++i)
	{
		maxiHistoryTime[i] = 0;
	}

	Start();	// start thread
}

// .........................................................................

// Destructor
FTControllerConnection::~FTControllerConnection()
{
	Terminate(true);

	ADelete(mlpCameraThread);
	ADelete(mlpMainSocket);
}

// .........................................................................

// Requests the socket to connect
void FTControllerConnection::Connect()
{
	ALockThis();
	if (!mbConnected)
	{
		mbShallConnect = true;
	}
}

// .........................................................................

// Returns if we are successfully connected
const bool FTControllerConnection::GetConnected()
{
	ALockThis();
	return mbConnected;
}

// .........................................................................

// Returns if the connection tries failed
const bool FTControllerConnection::GetFailed()
{
	ALockThis();
	return mbFailed;
}

// .........................................................................

// Returns the last data update tick time
const AInt64 FTControllerConnection::GetLastDataUpdateTick()
{
	ALockThis();
	return mxiLastDataUpdate;
}

// .........................................................................

// Sets the new mode for given sensor
void FTControllerConnection::SetSensorMode(const int Index, const FTControllerSensorMode Mode)
{
	ALockThis();
	maDesiredSensorMode[Index] = Mode;
}

// .........................................................................

// Returns the current sensor mode
const FTControllerSensorMode FTControllerConnection::GetSensorMode(const int Index)
{
	ALockThis();
	return maDesiredSensorMode[Index];
}

// .........................................................................

// Changes given engine output value
void FTControllerConnection::SetEngineOutput(const int Index, const double Value)
{
	ALockThis();
	if (Index >= 0 && Index < OFB_MAX_ENGINES)
	{
		double soll = Value;
		if (soll < 0.0)
			soll = 0.0;
		if (soll > 1.0)
			soll = 1.0;

		madOutputs[Index] = soll;
	}
}

// .........................................................................

// Returns the current motor distances
const AList<AInt64> FTControllerConnection::GetMotorCounters(const AInt64 TimeStamp, AInt64* Times)
{
	AList<AInt64> result;
	ALockThis();
	for (int i = 0; i < OFB_MAX_ENGINE_COUNTERS; ++i)
	{
		result.Add(maxiMotorCounterAbsolute[i]);
	}

	return result;
}

// .........................................................................

// Returns the current sensor input values
const AList<int> FTControllerConnection::GetSensorInputs(const AInt64 TimeStamp, AInt64* Times)
{
	AList<int> result;
	ALockThis();
	for (int i = 0; i < OFB_MAX_SENSOR_INPUTS; ++i)
	{
		result.Add(maiInputs[i]);
	}
	if (Times)
	{
		Times[0] = mxiLastDataUpdate;
	}

	return result;
}

// .........................................................................

// Sets the desired motor distances. Set a value to -1 for deactivating given slot
void FTControllerConnection::SetMotorTargetCounters(const AList<AInt64> &Distances)
{
	ALockThis();
	for (int i = 0; i < OFB_MAX_ENGINE_COUNTERS; ++i)
	{
		if (i < Distances.GetCount())
		{
			if (Distances[i] != maxiMotorTargets[i])
			{
				maxiMotorTargetDiff[i] = Distances[i] - maxiMotorTargets[i];	// remember original diff
				mabMotorTargetReached[i] = false;								// new target
				maxiMotorTargets[i] = Distances[i];
				maxiMotorLastDiffTick[i] = ATime::GetTick();
			}
		}
	}
}


// .........................................................................

// Returns the current motor distances
const AList<AInt64> FTControllerConnection::GetMotorTargetCounters()
{
	AList<AInt64> result;
	ALockThis();
	for (int i = 0; i < OFB_MAX_ENGINE_COUNTERS; ++i)
	{
		result.Add(maxiMotorTargets[i]);
	}

	return result;
}

// .........................................................................

// Returns for each motor if it's target has been reached
const AList<bool> FTControllerConnection::GetMotorTargetsReached()
{
	AList<bool> result;
	ALockThis();
	for (int i = 0; i < OFB_MAX_ENGINE_COUNTERS; ++i)
	{
		result.Add(mabMotorTargetReached[i]);
	}

	return result;
}

// .........................................................................

// Returns the counter of given motor
AInt64 FTControllerConnection::GetMotorTurnCounter(const int Index)
{
	if (Index >= 0 && Index < OFB_MAX_ENGINE_COUNTERS)
		return maxiMotorCounterAbsolute[Index];

	return 0;
}

// .........................................................................

void FTControllerConnection::SetDefaultConfig()
{
	for (int i = 0; i < m_nAreas; i++)
	{
		for (int j = 0; j < IZ_UNI_INPUT; j++)
		{
			m_transferarea[i].ftX1config.uni[j].mode = MODE_R;
			m_transferarea[i].ftX1config.uni[j].digital = 1;
		}
		for (int j = 0; j < IZ_COUNTER; j++)
		{
			// 1=normal, 0=inverted
			m_transferarea[i].ftX1config.cnt[j].mode = 1;
		}
		for (int j = 0; j < IZ_MOTOR; j++)
		{
			// 0=single output O1/O2, 1=motor output M1
			m_transferarea[i].ftX1config.motor[j] = 1;
		}
		m_transferarea[i].ftX1state.config_id++;
	}
}


// .........................................................................

bool FTControllerConnection::DoTransferSimple()
{
	// Uncompressed transfer mode
	ftIF2013Command_ExchangeData command;
	memset(&command, 0, sizeof(command));
	command.m_id = ftIF2013CommandId_ExchangeData;

	// Transfer data from transfer struct to communication struct
	for (int i = 0; i < ftIF2013_nPwmOutputs; i++)
	{
		command.m_pwmOutputValues[i] = m_transferarea[0].ftX1out.duty[i];
	}
	for (int i = 0; i < ftIF2013_nMotorOutputs && i < IZ_MOTOR; i++)
	{
		command.m_motor_master[i] = m_transferarea[0].ftX1out.master[i];
		command.m_motor_distance[i] = m_transferarea[0].ftX1out.distance[i];
		command.m_motor_command_id[i] = m_transferarea[0].ftX1out.motor_ex_cmd_id[i];
	}
	for (int i = 0; i < ftIF2013_nCounters && i < IZ_COUNTER; i++)
	{
		command.m_counter_reset_command_id[i] = m_transferarea[0].ftX1out.cnt_reset_cmd_id[i];
	}
	command.m_sound_index = m_transferarea[0].sTxtOutputs.u16SoundIndex;
	command.m_sound_repeat = m_transferarea[0].sTxtOutputs.u16SoundRepeat;
	command.m_sound_command_id = m_transferarea[0].sTxtOutputs.u16SoundCmdId;

	ftIF2013Response_ExchangeData response;
	{
		if (!SendCommand(&command, sizeof(command), ftIF2013ResponseId_ExchangeData, &response, sizeof(response)))
		{
			return false;
		}
	}

	// Transfer data from communication struct to transfer struct
	// Universal Inputs
	for (int i = 0; i < ftIF2013_nUniversalInputs && i < IZ_UNI_INPUT; i++)
	{
		m_transferarea[0].ftX1in.uni[i] = response.m_universalInputs[i];
	}

	// Counters
	for (int i = 0; i < ftIF2013_nCounters && i < IZ_COUNTER; i++)
	{
		m_transferarea[0].ftX1in.cnt_in[i] = response.m_counter_input[i];
		m_transferarea[0].ftX1in.counter[i] = response.m_counter_value[i];

		// Check if finished reset counter command ID changed
		if (response.m_counter_command_id[i] != m_transferarea[0].ftX1in.cnt_reset_cmd_id[i])
		{
			// remember current finished reset counter command ID
			m_transferarea[0].ftX1in.cnt_reset_cmd_id[i] = response.m_counter_command_id[i];
			// If currently requested command is finished, set the cnt_resetted signal
			if (response.m_counter_command_id[i] == m_transferarea[0].ftX1out.cnt_reset_cmd_id[i])
			{
				m_transferarea[0].ftX1in.cnt_resetted[i] = 1;
			}
		}
	}

	// Motors
	for (int i = 0; i < ftIF2013_nMotorOutputs && i < IZ_COUNTER; i++)
	{
		// Check if finished motor command ID changed
		if (response.m_motor_command_id[i] != m_transferarea[0].ftX1in.motor_ex_cmd_id[i])
		{
			// remember current finished motor command ID
			m_transferarea[0].ftX1in.motor_ex_cmd_id[i] = response.m_motor_command_id[i];
			// If currently requested command is finshed, set the motor_ex_reached signal
			if (response.m_motor_command_id[i] == m_transferarea[0].ftX1out.motor_ex_cmd_id[i])
			{
				m_transferarea[0].ftX1in.motor_ex_reached[i] = 1;
			}
		}
	}

	// Sound
	m_transferarea[0].sTxtInputs.u16SoundCmdId = response.m_sound_command_id;

	// IR
	for (int i = 0; i < ftIF2013_nIRChannels + 1 && i < NUM_OF_IR_RECEIVER + 1; i++)
	{
		KE_IR_INPUT_V01 *channel = &m_transferarea[0].sTxtInputs.sIrInput[i];

		channel->i16JoyLeftX = response.m_ir[i].m_ir_leftX;
		channel->u16JoyLeftXtoLeft = response.m_ir[i].m_ir_leftX < 0 ? -response.m_ir[i].m_ir_leftX : 0;
		channel->u16JoyLeftXtoRight = response.m_ir[i].m_ir_leftX > 0 ? response.m_ir[i].m_ir_leftX : 0;

		channel->i16JoyLeftY = response.m_ir[i].m_ir_leftY;
		channel->u16JoyLeftYtoBackwards = response.m_ir[i].m_ir_leftY < 0 ? -response.m_ir[i].m_ir_leftY : 0;
		channel->u16JoyLeftYtoForward = response.m_ir[i].m_ir_leftY > 0 ? response.m_ir[i].m_ir_leftY : 0;

		channel->i16JoyRightX = response.m_ir[i].m_ir_rightX;
		channel->u16JoyRightXtoLeft = response.m_ir[i].m_ir_rightX < 0 ? -response.m_ir[i].m_ir_rightX : 0;
		channel->u16JoyRightXtoRight = response.m_ir[i].m_ir_rightX > 0 ? response.m_ir[i].m_ir_rightX : 0;

		channel->i16JoyRightY = response.m_ir[i].m_ir_rightY;
		channel->u16JoyRightYtoBackwards = response.m_ir[i].m_ir_rightY < 0 ? -response.m_ir[i].m_ir_rightY : 0;
		channel->u16JoyRightYtoForward = response.m_ir[i].m_ir_rightY > 0 ? response.m_ir[i].m_ir_rightY : 0;

		channel->u16ButtonOn = (response.m_ir[i].m_ir_bits & 1) ? 1 : 0;
		channel->u16ButtonOff = (response.m_ir[i].m_ir_bits & 2) ? 1 : 0;

		channel->u16DipSwitch1 = (response.m_ir[i].m_ir_bits & 4) ? 1 : 0;
		channel->u16DipSwitch2 = (response.m_ir[i].m_ir_bits & 8) ? 1 : 0;
	}

	// Timers
	UpdateTimers();

	return true;
}

// .........................................................................

void FTControllerConnection::UpdateTimers()
{
	// Update timers
	long now = clock();
	m_transferarea[0].IFTimer.Timer1ms = (UINT16)(now - m_timelast[0]);
	while (now - (m_timelast[1] + 10) >= 0) {
		m_timelast[1] += 10;
		m_transferarea[0].IFTimer.Timer10ms++;
		/* !!!!IF2008: Change fields missing !!! */
	}
	while (now - (m_timelast[2] + 100) >= 0) {
		m_timelast[2] += 100;
		m_transferarea[0].IFTimer.Timer100ms++;
	}
	while (now - (m_timelast[3] + 1000) >= 0) {
		m_timelast[3] += 1000;
		m_transferarea[0].IFTimer.Timer1s++;
	}
	while (now - (m_timelast[4] + 10000) >= 0) {
		m_timelast[4] += 10000;
		m_transferarea[0].IFTimer.Timer10s++;
	}
	while (now - (m_timelast[5] + 60000) >= 0) {
		m_timelast[5] += 60000;
		m_transferarea[0].IFTimer.Timer1min++;
	}
}

// .........................................................................

// Update the I/O (e.g. universal input) configuration
bool FTControllerConnection::UpdateConfig()
{
	for (int iExt = 0; iExt < 1; iExt++)
	{
		ftIF2013Command_UpdateConfig command;
		memset(&command, 0, sizeof(command));
		command.m_id = ftIF2013CommandId_UpdateConfig;
		command.m_config = m_transferarea[iExt].ftX1config;
		command.m_config_id = m_transferarea[iExt].ftX1state.config_id;
		command.m_extension_id = iExt;

		ftIF2013Response_UpdateConfig response;
		if (!SendCommand(&command, sizeof(command), ftIF2013ResponseId_UpdateConfig, &response, sizeof(response)))
		{
			return false;
		}
	}

	return true;
}

// .........................................................................

bool FTControllerConnection::SendCommand(const struct ftIF2013Command_Base *commanddata, size_t commandsize, enum ftIF2013ResponseId responseid, struct ftIF2013Response_Base *responsedata, size_t responsesize)
{
	AMemStream src;
	int result = (int)src.Write(commanddata, commandsize);
	src.Seek(0);
	mlpMainSocket->Send(src, commandsize);

	if (result != commandsize)
	{
		ALog("Error sending command");
		return false;
	}

	AMemStream res;
	mlpMainSocket->Receive(res, responsesize);
	if (res.GetSize() >= responsesize)
	{
		memcpy(responsedata, res.GetDataPointer(), responsesize);
		if (responsedata->m_id != responseid)
		{
			ALog("Response id mismatch");
			return false;
		}
	}

	return true;
}

// .........................................................................

// Tries to connect to the FT TXT controller
bool FTControllerConnection::ConnectInt()
{
	AStringParser posIps(msFtIpRange);
	posIps >> '/';

	ADelete(mlpMainSocket);
	mlpMainSocket = ANew(AIOSocket);

	while (!posIps.EOS())
	{
		msFtIp = posIps.Next();
		bool success = mlpMainSocket->Connect(false, msFtIp, 65000);
		if (success)
			return true;
	}

	ADelete(mlpMainSocket);
	return false;
}

// .........................................................................

bool FTControllerConnection::StartOnline()
{
	{
		ftIF2013Command_StartOnline command;
		memset(&command, 0, sizeof(command));
		command.m_id = ftIF2013CommandId_StartOnline;
		ftIF2013Response_StartOnline response;
		strncpy(command.m_name, "Online", sizeof(command.m_name) / sizeof(*command.m_name));
		if (!SendCommand(&command, sizeof(command), ftIF2013ResponseId_StartOnline, &response, sizeof(response)))
		{
			ALog("Error while starting online mode");
			return false;
		}
		else
		{
			ALog("We are online!");

			long now = clock();
			m_timelast[0] = now;
			m_timelast[1] = now;
			m_timelast[2] = now;
			m_timelast[3] = now;
			m_timelast[4] = now;
			m_timelast[5] = now;

/*			FISH_X1_TRANSFER& ct = m_transferarea[0];
			ct.ftX1config.uni[0].mode = MODE_ULTRASONIC;
			ct.ftX1config.uni[0].digital = 0;
			ct.ftX1state.config_id++;
			ct.ftX1config.uni[1].mode = MODE_ULTRASONIC;
			ct.ftX1config.uni[1].digital = 0;
			ct.ftX1state.config_id++;
			ct.ftX1config.uni[2].mode = MODE_ULTRASONIC;
			ct.ftX1config.uni[2].digital = 0;
			ct.ftX1state.config_id++; */

			bool success =  UpdateConfig();
			if (success)
			{
				success = DoTransferSimple();	// receive initial state
				if (success)
				{
					BackupStates();
				}
			}
			return success;
		}
	}
}

// .........................................................................

bool FTControllerConnection::StopOnline()
{
	{
		ftIF2013Command_StopOnline command;
		memset(&command, 0, sizeof(command));
		command.m_id = ftIF2013CommandId_StopOnline;
		ftIF2013Response_StopOnline response;
		if (!SendCommand(&command, sizeof(command), ftIF2013ResponseId_StopOnline, &response, sizeof(response)))
		{
			ALog("Error while stop online mode");
			return false;
		}
		else
		{
			ALog("We are offline!");
			return true;
		}
	}
}

// .........................................................................

bool FTControllerConnection::StopCamera()
{
	{
		ADelete(mlpCameraThread);

		ftIF2013Command_StopCameraOnline command;
		memset(&command, 0, sizeof(command));
		command.m_id = ftIF2013CommandId_StopCameraOnline;

		ftIF2013Response_StopCameraOnline response;
		if (!SendCommand(&command, sizeof(command), ftIF2013ResponseId_StopCameraOnline, &response, sizeof(response)))
		{
			ALog("Error while stopping camera");
			return false;
		}
		else
		{
			ALog("Camera offline!");
			return true;
		}
	}
}

// .........................................................................

bool FTControllerConnection::StartCamera()
{
	ADelete(mlpCameraThread);

	// int remTries = 1;
	// while (remTries >= 1)
	{
		{
			ftIF2013Command_StartCameraOnline command;
			memset(&command, 0, sizeof(command));
			command.m_id = ftIF2013CommandId_StartCameraOnline;
			command.m_width = miCamResX;
			command.m_height = miCamResY;
			command.m_framerate = 30;
			command.m_powerlinefreq = miCamRefreshRate;
			ftIF2013Response_StartCameraOnline response;
			if (!SendCommand(&command, sizeof(command), ftIF2013ResponseId_StartCameraOnline, &response, sizeof(response)))
			{
				ALog("Error while starting online mode");
				return false;
			}
			else
			{
				ALog("Camera active!");
			}
		}


		ADelete(mlpCameraThread);
		mlpCameraThread = ANew(ARBCameraThread)(msFtIp,65001);
		mlpCameraThread->SetEnabled(true);
		for (int i = 0; i < 100; ++i)
		{
			AThread::Sleep(0.05);
			if (mlpCameraThread->GetEnabled() == true || mlpCameraThread->GetFailed())
				break;
		}
		if (mlpCameraThread->GetEnabled() == false)
		{
			ADelete(mlpCameraThread);
			return false;
		}
		else
		{
			// break;
		}
	}

	if (mlpCameraThread)
	{
		ALog("Successfully connected to camera");
		return true;
	}
	else
	{
		return false;
	}
}

// .........................................................................

// Changes the camera's resolution
const bool FTControllerConnection::SetCameraResolution(const int Width, const int Height, const int Frames)
{
	if (miCamResX == Width && miCamResY == Height && miCamRefreshRate == Frames)
		return true;

	miCamResX = Width;
	miCamResY = Height;
	miCamRefreshRate = Frames;

	StopCamera();
	StartCamera();

	return true;
}

// .........................................................................

unsigned int FTControllerConnection::GetVersion()
{
	{
		ftIF2013Command_QueryStatus command;
		memset(&command, 0, sizeof(command));
		command.m_id = ftIF2013CommandId_QueryStatus;
		ftIF2013Response_QueryStatus response;

		if (!SendCommand(&command, sizeof(command), ftIF2013ResponseId_QueryStatus, &response, sizeof(response)))
		{
			return 0;
		}

		return response.m_version;
	}
}

// .........................................................................

// Backups currently known device states
void FTControllerConnection::BackupStates()
{
	FISH_X1_TRANSFER& ct = m_transferarea[0];

	for (int i = 0; i < OFB_MAX_ENGINE_COUNTERS; ++i)	// get counters
	{
		if (ct.ftX1in.cnt_resetted[i])
		{
			maxiLastMotorCounter[i] = 0;
		}
		else
		{
			maxiLastMotorCounter[i] = ct.ftX1in.counter[i];
		}
	}
}

// .........................................................................

// Thread tick handling
void FTControllerConnection::Tick()
{
	AThread::Tick();

	bool	connected,		// currently connected ?
			shallConnect,	// shall try to connect ?
			configUpdate = false;	// any configuration changes ?

	{
		ALockThis();
		connected = mbConnected;
		shallConnect = mbShallConnect;
	}

	// not connected yet but we shall try it ?
	if (!connected && shallConnect)
	{
		if(ConnectInt())
		{
			connected = GetVersion()!=0;
			connected = connected && StopOnline();
			connected = connected && StartOnline();
			connected = connected && StopCamera();
			connected = connected && StartCamera();
		}

		{
			ALockThis();
			mbFailed = !connected;
			mbConnected = connected;
			mbShallConnect = false;

			mxiLastPerformanceQuery = ATime::GetTick();
			mxiTurnsSinceLastQuery = 0;
		}
	}

	if (!connected)
	{
		AThread::Sleep(0.001);
		return;
	}

	{
		ALockThis();
		AInt64 curTime = ATime::GetTick();
		mxiTurnsSinceLastQuery++;

		if (curTime - mxiLastPerformanceQuery > 1000000)
		{
			ALog(AString::OfInt(mxiTurnsSinceLastQuery * 1000000 / (curTime - mxiLastPerformanceQuery)) + " turns per second");
			mxiLastPerformanceQuery = curTime;
			mxiTurnsSinceLastQuery = 0;
		}

		FISH_X1_TRANSFER& ct = m_transferarea[0];

		int us1 = ct.ftX1in.uni[0],
			us2 = ct.ftX1in.uni[1],
			us3 = ct.ftX1in.uni[2];

		bool ignoreMotor[OFB_MAX_ENGINES];
		for (int i = 0; i < OFB_MAX_ENGINES; ++i)
		{
			ignoreMotor[i] = false;
		}

		// update engine voltages
		for (int i = 0; i < OFB_MAX_ENGINE_COUNTERS; ++i)
		{
			if (mabMotorTargetReached[i] == false)
			{
				ignoreMotor[i * 2] = true;
				ignoreMotor[i * 2 + 1] = true;
				if (maxiMotorCounterAbsolute[i] != maxiMotorTargets[i])	// not reached target yet ?
				{
					int diff = (int)(maxiMotorTargets[i] - maxiMotorCounterAbsolute[i]);
					if (diff > 512)
						diff = 512;
					if (diff < -512)
						diff = -512;

					// if we exceeded our target it's already too far and we're done
					if (diff == 0 || (diff>0 && maxiMotorTargetDiff[i] < 0) || (diff < 0 && maxiMotorTargetDiff[i]>0))
					{
						mabMotorTargetReached[i] = true;
						continue;
					}

					if (diff == 0) // target reached, nothing to be done
					{
						ct.ftX1out.duty[i * 2 + 0] = 0;
						ct.ftX1out.duty[i * 2 + 1] = 0;
					}
					else
						// positive uneven index (clockwise), negative even index (counter clock)
						if (diff > 0)
						{
							ct.ftX1out.duty[i * 2 + 0] = 0;
							ct.ftX1out.duty[i * 2 + 1] = 512;
						}
						else
						{
							ct.ftX1out.duty[i * 2 + 0] = 512;
							ct.ftX1out.duty[i * 2 + 1] = 0;
						}
				}
				else
				{
					ct.ftX1out.duty[i * 2 + 0] = 0;
					ct.ftX1out.duty[i * 2 + 1] = 0;
					mabMotorTargetReached[i] = true;
				}
			}	// of target not reached yet
			else
			{
				ct.ftX1out.duty[i * 2 + 0] = (INT16)(madOutputs[i * 2 + 0] * 512);
				ct.ftX1out.duty[i * 2 + 1] = (INT16)(madOutputs[i * 2 + 1] * 512);
				mabMotorTargetReached[i] = true;
			}
		}	// of engine voltages

		for (int i = 0; i < OFB_MAX_SENSOR_INPUTS; ++i)
		{
			if (maCurrentSensorMode[i]!=maDesiredSensorMode[i])
			{
				configUpdate = true;

				int newMode = MODE_U;
				bool newModeDigital = false;

				switch (maDesiredSensorMode[i])
				{
				case FTControllerSensorMode::AnalogOhm:
				{
					newMode = MODE_R;
					newModeDigital = false;
					break;
				}
				case FTControllerSensorMode::AnalogVolt:
				{
					newMode = MODE_R2;
					newModeDigital = false;
					break;
				}
				case FTControllerSensorMode::DigiOhm:
				{
					newMode = MODE_R;
					newModeDigital = true;
					break;
				}
				case FTControllerSensorMode::DigiVolt:
				{
					newMode = MODE_R2;
					newModeDigital = true;
					break;
				}
				case FTControllerSensorMode::Ultrasonic:
				{
					newMode = MODE_ULTRASONIC;
					newModeDigital = false;
					break;
				}
				}

				ct.ftX1config.uni[i].mode = newMode;
				ct.ftX1config.uni[i].digital = newModeDigital;
				ct.ftX1state.config_id++;

				maCurrentSensorMode[i] = maDesiredSensorMode[i];
			}	// sensor state modified ?
		}	// for all sensors

	}	// of pre fetch update

 	if (configUpdate)
 	{
 		UpdateConfig();
 	}
 
	if (!DoTransferSimple())
	{
	}

	AInt64 curTime = ATime::GetTick();

	{	// synch feedback
		ALockThis();
		FISH_X1_TRANSFER& ct = m_transferarea[0];

		mxiLastDataUpdate = curTime;

		for (int i = 0; i < OFB_MAX_ENGINE_COUNTERS; ++i)	// get counters
		{
			if (ct.ftX1in.cnt_resetted[i] && ct.ftX1in.cnt_reset_cmd_id[i] == mausLastKnownResetIndex[i])
			{
				maxiLastMotorCounter[i] = 0;
				ct.ftX1in.cnt_resetted[i] = false;
				ct.ftX1in.counter[i] = 0;
			}
			int diff = ct.ftX1in.counter[i] - (int)maxiLastMotorCounter[i];

			// flip counter when in reverse mode
			if (ct.ftX1out.duty[i * 2]>0)
				diff = -diff;

			// any direction active ?
			if (ct.ftX1out.duty[i * 2]>0 || ct.ftX1out.duty[i * 2 + 1] > 0)
			{
				if (diff)	// remember when motor moved last time
				{
					maxiMotorLastDiffTick[i] = curTime;
				}
				else
				{
					if (curTime - maxiMotorLastDiffTick[i] > 250000)	// not moving although it should ?
					{
						mabMotorTargetReached[i] = true;	// TODO Flag as blocking. Flag as reached
					}
				}
			}

			maxiMotorCounterAbsolute[i] += diff;			

			if (ct.ftX1in.counter[i]>3000)	// reset counter when getting into critical regions
			{
				ct.ftX1out.cnt_reset_cmd_id[i]++;
				mausLastKnownResetIndex[i] = ct.ftX1out.cnt_reset_cmd_id[i];
			}
		}	// of engine counters

		for (int i = 0; i < OFB_MAX_SENSOR_INPUTS; ++i)
		{
			maiInputs[i] = ct.ftX1in.uni[i];
		}	// of sensors

		BackupStates();
	}	// of post fetch update
}

// .........................................................................

// Returns the frame counter
const AInt64 FTControllerConnection::GetCameraFrameCounter()
{
	return mlpCameraThread ? mlpCameraThread->GetFrameCounter() : -1;
}

// .........................................................................

// Receives the most recent frame
const bool FTControllerConnection::GetCameraFrame(AMemStream &Target)
{
	return mlpCameraThread ? mlpCameraThread->GetCameraFrame(Target) : false;
}
