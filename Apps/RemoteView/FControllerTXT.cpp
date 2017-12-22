#include "TutorialAppStd.h"

// ----------------------------------------------------------------------------

// Constructor
FControllerTXT::FControllerTXT(FComponent* Parent) :
	FController(Parent)
{
}

// ............................................................................

// Destructor
FControllerTXT::~FControllerTXT()
{

}

// ............................................................................

// Synchronization handling
const FInt64 FControllerTXT::SynchInt(const FInt64 CurTick)
{
	if (!mlpConnection)
	{
		return CurTick;
	}

	FLock(*mlpConnection);
	AInt64 lastUpdateTick = mlpConnection->GetLastDataUpdateTick();
	if (mlpConnection->GetLastDataUpdateTick() != CurTick || mbNewConnection)
	{
		for (int i = 0; i < ~mlComponents; ++i)	// check all components
		{
			FRegisteredComponent* cur = mlComponents[i];
			if (cur->mxiLastModification == CurTick)
			{
				if (cur->mType == FComponentType::Sensor)	// sensor modified ?
				{
					mlpConnection->SetSensorMode(cur->miMinorIndex,
						cur->mInputType == FSensorInputType::Analog10v ? FTControllerSensorMode::AnalogVolt :
						cur->mInputType == FSensorInputType::Analog5kOhm ? FTControllerSensorMode::AnalogOhm :
						cur->mInputType == FSensorInputType::Digital10v ? FTControllerSensorMode::DigiVolt :
						cur->mInputType == FSensorInputType::Digital5kOhm ? FTControllerSensorMode::DigiOhm :
						cur->mInputType == FSensorInputType::Ultrasonic ? FTControllerSensorMode::Ultrasonic :
						FTControllerSensorMode::None
						);
				}

				if (cur->mType == FComponentType::Engine)
				{
					FCompEngine* engine = cur->mlpComponent->AsEngine();

					if (engine && cur->mlpComponent->AsEngine()->GetLastModification()>lastUpdateTick || mbNewConnection)
					{
						float outputPower = engine->GetOutputPower();
						if (cur->miMinorIndex != -1)
						{
							float power = outputPower > 0 ? outputPower : 0.f;
							mlpConnection->SetEngineOutput(cur->miMinorIndex, power);
						}
						if (cur->miSecondaryIndex != -1)
						{
							float power = outputPower < 0 ? -outputPower : 0.f;
							mlpConnection->SetEngineOutput(cur->miSecondaryIndex, power);
						}
					}
				}
			}
		}

		FInt64 timeStamps[OFB_MAX_HISTORY_COUNT];
		FList<int> sensorStates = mlpConnection->GetSensorInputs(CurTick, timeStamps);
		FList<int> newValues;
		int historyEntries = (int)sensorStates.GetCount() / OFB_MAX_SENSOR_INPUTS;
		newValues.Reserve(historyEntries);

		for (int currentPort = 0; currentPort < OFB_MAX_SENSOR_INPUTS; ++currentPort)	// for all ports
		{
			for (int i = 0; i < ~mlComponents; ++i)	// check all components
			{
				FRegisteredComponent* cur = mlComponents[i];
				FCompSensor* sensor = cur->mlpComponent->AsSensor();

				if(!sensor)		// skip if it's no sensor
					continue;

				if (cur->miMinorIndex == currentPort)		// using current port
				{
					newValues.Clear();
					for (int he = 0; he < historyEntries; ++he)	// collect all values for current port
					{
						newValues.Add(sensorStates[currentPort + he*OFB_MAX_SENSOR_INPUTS]);

						sensor->UpdateData(newValues, timeStamps);
					}
				}
			}
		}

		mbNewConnection = false;
		return lastUpdateTick;
	}

	return CurTick;
}

// ............................................................................

// Tries to enable the controller
void FControllerTXT::SetEnabled(const bool State)
{
	if (State)
	{
		if (mlpConnection && mlpConnection->GetConnected())
		{
			return;
		}

		ADelete(mlpConnection);
		mbNewConnection = true;
		mlpConnection = ANew(FTControllerConnection)("192.168.6.2/192.168.7.2/192.168.8.2/192.168.9.2");
		mlpConnection->Connect();
	}	// of enabling
	else
	{
		if (mlpConnection->GetConnected())
		{
			ADelete(mlpConnection);
		}
	}	// of disabling
}

// ............................................................................

// Returns the controller's state
const bool FControllerTXT::GetEnabled()
{
	return mlpConnection && mlpConnection->GetConnected();
}

// ............................................................................

// Returns if a connection error occurred
const bool FControllerTXT::GetErrorOccurred()
{
	return mlpConnection && mlpConnection->GetFailed();
}