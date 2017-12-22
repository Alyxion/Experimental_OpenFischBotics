#include "TutorialAppStd.h"

// ----------------------------------------------------------------------------

// Constructor
FCompSensor::FCompSensor(FComponent* Parent, const FSensorInputType InputType) :
	FComponent(Parent, FComponentType::Sensor),
	mInputType(InputType)
{
	memset(maxiUpdateTimes, 0, sizeof(maxiUpdateTimes));
	memset(maiValues, 0, sizeof(maiValues));
}

// ............................................................................

// Destructor
FCompSensor::~FCompSensor()
{

}

// ............................................................................

// Sets the sensors valid value range
void FCompSensor::SetValueRange(const int Min, const int Max)
{
	miValueMinimum = Min;
	miValueMaximum = Max;
}

// ............................................................................

// Connects the sensor to given controller and input port
void FCompSensor::Connect(FController *Controller, const int MajorPort, const int MinorPort)
{
	SetController(Controller);
	if(Controller)
	{
		Controller->SetupSensor(this, mInputType, MajorPort, MinorPort);
	}
}

// ............................................................................

// Adds new data to this sensor
void FCompSensor::UpdateData(const FList<int> &Data, const FInt64 *TimeStamps)
{
	for (int i = Data.GetCount() - 1; i >= 0; --i)	// add new data in reverse order
	{
		if(Data[i]>=miValueMinimum && Data[i]<=miValueMaximum)	// skip invalid data
		{
			miCurrentIndex = (miCurrentIndex + 1) % OFB_MAX_CACHED_SENSOR_VALUES;
			maiValues[miCurrentIndex] = Data[i];
			maxiUpdateTimes[miCurrentIndex] = TimeStamps[i];
			
			if (miValidValues < OFB_MAX_CACHED_SENSOR_VALUES)	// remember how many valid values are available
			{
				miValidValues++;
			}
		}
	}
}

// ............................................................................

// Returns the sensor's values
void FCompSensor::GetValues(const int Maximum, FList<int> &List, FList<FInt64> &TimeStamps)
{
	int realMax = Maximum;
	if (realMax < 0)
	{
		realMax = 0;
	}
	if (realMax > miValidValues)
	{
		realMax = miValidValues;
	}

	List.Clear();
	TimeStamps.Clear();
	int curIndex = miCurrentIndex;
	for (int i = 0; i < realMax; ++i)
	{
		List.Add(maiValues[curIndex]);
		TimeStamps.Add(maxiUpdateTimes[curIndex]);
		curIndex--;
		if (curIndex < 0)	// restart at the end when reaching -1
			curIndex += OFB_MAX_CACHED_SENSOR_VALUES;
	}
}
