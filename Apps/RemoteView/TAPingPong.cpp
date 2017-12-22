#include "TutorialAppStd.h"
#include "TAPingPong.h"

// ............................................................................



FTControllerConnection mlpRobSocket("192.168.6.2/192.168.7.2/192.168.8.2/192.168.9.2");

ASQImage* mlpCamImage = nullptr;

// Constructor
TAPingPong::TAPingPong(AQCView* Parent) :
	AQCView(Parent),
	miTutorialLevel(1),
	mlpTimer(NULL),
	mfBallRad(0.f),
	mfBouncerWidth(0.f),
	mfBouncerHeight(0.f),
	mfTopBorderSize(0.f),
	mvBallPosition(0.f,0.f),
	mfLeftBouncerY(0.f),
	mfRightBouncerY(0.f),
	mdUpdFrequency(200.0)
{
	SetSize(Scale(100.f), Scale(100.f));
	mlpTimer = ANew(AQTimer)(this);			// create and schedule timer
	mlpTimer->Schedule(0.0, true, 0.0, false);

	// setup sizes
	mfTopBorderSize = Scale(20.f);
	mfBouncerWidth = Scale(30.f);
	mfBouncerHeight = Scale(100.f);
	mfBallRad = Scale(16.f);

	// initial ball position
	mvBallPosition = Scale(ASQPoint(160.f, 160.f));

	mfLeftBouncerY = Scale(100.f);
	mfRightBouncerY = Scale(100.f);
	
	// AFTSocket ftSocket;
	mlpMainModel = ANew(FComponent)(nullptr);

	FNew(FCompBrick)(mlpMainModel, FBrickType::Alu, 33.0);
	mlpController = ANew(FControllerTXT)(mlpMainModel);
	mlpController->SetEnabled(true);

	for (int i = 0; i < 3; ++i)
	{
		FSensorUltrasonic* us = ANew(FSensorUltrasonic)(mlpMainModel);
		us->Connect(mlpController, 0, i);
	}

	// setup motor
	FEngineEncoder* mainEngine = ANew(FEngineEncoder)(mlpMainModel);
	mainEngine->Connect(mlpController, 0, 4, 5, -1);
	mainEngine->SetOutputPower(0.33f);

	// setup light barrier
	FSensorLightBarrier* lightBarrier = ANew(FSensorLightBarrier)(mlpMainModel);
	lightBarrier->Connect(mlpController, 0, 4);

	for (int i = 0; i < 20; ++i)
	{
		AThread::Sleep(0.1);
		if (mlpController->GetEnabled() || mlpController->GetErrorOccurred())
		{
			break;
		}
	}

	if (!mlpController->GetEnabled())
	{
		exit(1);
	}
	GetInputHandler()->SetKeyboardFocus(this);
}

// ............................................................................

// Destructor
TAPingPong::~TAPingPong()
{
	ADelete(mlpMainModel);
}

// ............................................................................

// Override painting
void TAPingPong::Paint(ASQCanvas* Canvas, const AQPaintingOptions &Options)
{
	AQCView::Paint(Canvas, Options);

	// fill background
	Canvas->SetColor(aclDarkGray);
	Canvas->DrawRect2D(AVF00, GetWidth(), GetHeight());

	if (mlpCamImage && mlpCamImage->GetWidth() > 0)
	{
		float scaling = mlpCamImage->GetWidth() > mlpCamImage->GetHeight() ? GetWidth() / mlpCamImage->GetWidth() : GetHeight() / mlpCamImage->GetHeight();

		Canvas->DrawImage(mlpCamImage, ASQRect(0.f, 0.f, (float)mlpCamImage->GetWidth()*scaling, (float)mlpCamImage->GetHeight()*scaling), aclWhite);
	}
// 	for (int i = 0; i < 4; ++i)
// 	{
// 		SetFontSize(Scale(22.f), true);
// 		ASQFontHandle &fontHandle = GetFontHandle();
// 		ASQTextOutputHandle handle = fontHandle.GetHandle(AString::OfInt(mlpRobSocket->GetMotorTurnCounter(i)), true);
// 		Canvas->TextOut(handle, ASQPoint(Scale(20.f) + i*Scale(80.f), Scale(20.f)), aclRed);
// 
// 	}

	if (!mlpFVisualizer)
	{
		mlpFVisualizer = ANew(FVisualizer);
	}

//	mlpFVisualizer->Visualize(Canvas, mlpMainModel, FVector(400.f, 400.f, 0.f), 20.f, FVisualSide::Front);
//	mlpFVisualizer->Visualize(Canvas, mlpMainModel, FVector(1200.f, 400.f, 0.f), 20.f, FVisualSide::Top);

	FList<int> values;
	FList<AInt64> timeStamps;
	int maxValues = 512;
	values.Reserve(maxValues);
	timeStamps.Reserve(maxValues);

	FInt64 curTick = ATime::GetTick();
	FInt64 timeRange = 4000000;
	float	heightPerSensor = Scale(100.f),
			widthPerSensor = Scale(1500.f);

	int sensorIndex = 0;
	for (int compIndex = 0; compIndex < mlpController->GetComponentCount(); ++compIndex)
	{
		FCompSensor* sensor = mlpController->GetComponentAt(compIndex)->mlpComponent->AsSensor();
		if(!sensor)
			continue;

		sensor->GetValues(maxValues, values, timeStamps);

		float	offX = Scale(40.f),
				offY = Scale(60.f) + ((heightPerSensor + Scale(40.f))*sensorIndex),
				lastX = offX;

		AColor curColor =	sensorIndex==0 ? aclWhite :
							sensorIndex == 1 ? aclBlue :
							sensorIndex == 2 ? aclRed :
							sensorIndex == 3 ? aclYellow :
							sensorIndex == 4 ? aclPurple : aclGreen;

		Canvas->SetColor(curColor);
		Canvas->DrawFrame2D(ASQPoint(offX, offY), widthPerSensor, heightPerSensor, Scale(1.f));

		FString description = sensor->GetDescription();
		if (~description)
		{
			SetFontSize(Scale(30.f));
			ASQTextOutputHandle handle = GetFontHandle().GetHandle(description, true);
			Canvas->TextOut(handle, ASQPoint(offX, offY - handle.GetHeight()), curColor);
		}

		if(~values==0)
			continue;

		int minimum = values.First(),
			maximum = values.First();

		for (int curData = 1; curData < values.GetCount(); ++curData)
		{
			if (values[curData] < minimum)
			{
				minimum = values[curData];
			}
			if (values[curData] > maximum)
			{
				maximum = values[curData];
			}
		}

		if (sensor->GetInputType() == FSensorInputType::Ultrasonic)	// use minimum of 1 meter for ultrasonic sensors
		{
			if (maximum < 300)
				maximum = 300;
		}

		if(maximum==minimum)
		{
			sensorIndex++;
			continue;
		}

		Canvas->SetColor(curColor);
		for (int curData = 0; curData < values.GetCount(); ++curData)
		{
			if(curTick-timeStamps[curData]>timeRange)	// ignore too old entries
				continue;

			AInt64 timeOff = curTick - timeStamps[curData];
			float	percPositionX = (float)timeOff / (double)timeRange,
					percPositionY = (values[curData] / (float)maximum),
					effX = offX + percPositionX * widthPerSensor,
					effY = offY + heightPerSensor - heightPerSensor*percPositionY,
					effHeight = heightPerSensor*percPositionY,
					width = (float)(widthPerSensor/maxValues)+2;

			Canvas->DrawRect2D(ASQPoint(effX-width, effY), width, effHeight);

			lastX = effX;
		}

		sensorIndex++;
	}
}

// ............................................................................

// Override timer event
bool TAPingPong::HandleTimer(AQTimerEvent* Event)
{
	if (mlpController)
	{
		if (mlpController->Synchronize())
		{
			Repaint();
		}
	}

	if (Event->GetSender() == mlpTimer)	// repaint 60 times a second
	{
		AMemStream image;



		static AInt64 lastUpdate = 0;
		AInt64 curTick = ATime::GetTick();
// 		if (curTick - lastUpdate > 1000000/20)
// 		{
// 			lastUpdate = curTick;
// 			AInt64 curFrame = mlpRobSocket->GetCameraFrameCounter();
// 			if (curFrame!=-1 && curFrame!=mxiLastFrame && mlpRobSocket->GetCameraFrame(image))
// 			{
// 				image.Seek(0);
// 				ASQImageLoadingTask ilt(image);
// 				ilt.Execute(true, true);
// 
// 				ASQPixelBuffer* pixels = ilt.GetImage();
// 
// 				//ADelete(someImage);
// 				if (pixels)
// 				{
// 					if (!mlpCamImage)
// 					{
// 						mlpCamImage = mlpVRM->CreateImage(pixels);
// 					}
// 					else
// 					{
// 						mlpCamImage->LockCanvas();
// 						ASQDynamicImage* dynimg = mlpCamImage->AsDynamicImage();
// 						if(dynimg)
// 						{
// 							for (int i = 0; i < pixels->GetHeight(); ++i)
// 							{
// 								memcpy(dynimg->GetPixelBuffer()->GetPixels(i), pixels->GetPixels(i), pixels->GetWidth()
// 									* 4);
// 							}
// 						}
// 						mlpCamImage->UnlockCanvas();
// 					}
// 				}
// 				else
// 				{
// 					ALog("Could not decompress jpeg");
// 				}
// 			}
// 
// 			Repaint();
// 		}
	}

	return AQCView::HandleTimer(Event);
}

// ............................................................................

// Key down handling
bool TAPingPong::HandleKeyDown(AQKeyDownEvent* Event)
{
// 	switch (Event->GetKey())
// 	{
// 	case 0x57: mbForward = true; break;
// 	case 0x53: mbBackwards = true; break;
// 	case 0x44: mbRight = true; break;
// 	case 0x41: mbLeft = true; break;
// 	default:;
// 	}
// 
// 	mlpRobSocket->SetDirs(mbLeft, mbRight, mbForward, mbBackwards);
	return AQCView::HandleKeyDown(Event);
}

// ............................................................................

// Key up handling
bool TAPingPong::HandleKeyUp(AQKeyUpEvent* Event)
{
// 	switch (Event->GetKey())
// 	{
// 	case 0x57: mbForward = false; break;
// 	case 0x53: mbBackwards = false; break;
// 	case 0x44: mbRight = false; break;
// 	case 0x41: mbLeft = false; break;
// 	default:;
// 	}
// 
// 	mlpRobSocket->SetDirs(mbLeft, mbRight, mbForward, mbBackwards);
	return AQCView::HandleKeyUp(Event);
}

// ............................................................................

// Key press handling
bool TAPingPong::HandleKeyPress(AQKeyPressEvent* Event)
{
	if (Event->GetKey() == ' ')
	{
		AQCView* view = this;
		ADelete(view);
		exit(0);
	}

	return AQCView::HandleKeyPress(Event);
}