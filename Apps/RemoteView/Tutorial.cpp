// ----------------------------------------------------------------------------

#include "TutorialAppStd.h" 

// ----------------------------------------------------------------------------

#define APH_APP_LEVEL_AAC 1
#define APH_APP_EVENT_HANDLER		TutorialAppEventHandler
#define APH_APP_PRODUCT_NAME		Tutorial
#define APH_APP_PRODUCT_IDENTIFIER	Tutorial
#define APH_APP_COMPANY_NAME		MIKEMANN
#define APH_APP_COMPANY_IDENTIFIER	MIKEMANN
#define APH_APP_PRODUCT_VERSION		2.0.1
#define APH_APP_COPYRIGHT			"Copyright (C) Michael Ikemann, 2016"
#define APH_APP_DEFAULT_ACTIVITY	"TutorialHomeActivity"

// ----------------------------------------------------------------------------

#include "../../../LBNavigator/Aphereon4/Source/AphBaseAppInc.h"
#include "TAPingPong.h"

// ----------------------------------------------------------------------------

class TutorialAppManager;

// ----------------------------------------------------------------------------

/** @brief Application event handling class */
class APH_APP_EVENT_HANDLER : public AApplicationEventHandlerPD
{
protected:
	TutorialAppManager*	mlpManager;

public:
	//! Constructor
	inline APH_APP_EVENT_HANDLER() : 
		mlpManager(NULL)		
	{
	}

	//! Pre-initialization handler
	void Initialize();
};

// ----------------------------------------------------------------------------

/** @brief Application main window */
class TutorialAppWindow : public AACMainWindow
{
protected:

public:
	//! Constructor
	TutorialAppWindow(AQConnection* con) : AACMainWindow(con,AString("MoHaAc remote control"),"Icon.ico||",1800,800,AQWindowTypes::Convertible)
	{
		SetGraphicsApiLevel(ASQ3DHardwareApiLevels::ShaderLevel2);	
		Center();

		TAPingPong* pingPong = ANew(TAPingPong)(this);
		pingPong->SetOrderLevel(AQOrderLevels::SplashScreen);
		pingPong->SetAlignment(AQAlignment::Client);		
	}

	//! Destructor
	~TutorialAppWindow()
	{
		AApplication.Terminate();
	}
};

// ----------------------------------------------------------------------------

/** @brief PingPong main activity window class */
class TutorialHomeActivityWindowClass : public AObjectClass
{
protected:
public:
	//! Constructor
	TutorialHomeActivityWindowClass() : AObjectClass("TutorialHomeActivityWindow", "AACMainWindow", true) {}

	// Object creation
	AObject* CreateInstance(AObject* Parent /* = NULL */, AObject* Parameters /* = NULL */)
	{
		AQWindow* result = ANew(TutorialAppWindow)((AQConnection*)Parent);

#ifdef _WIN32
		result->SetGraphicsApiLevel(ASQ3DHardwareApiLevels::Base);
#else
		result->SetGraphicsApiLevel(ASQ3DHardwareApiLevels::Base);
#endif

		return result;
	}
};

// ----------------------------------------------------------------------------

/** @brief GridGlobe application manager class */
class TutorialAppManager : public AACAdvApplicationManager
{
protected:
	AInt64	mxiConstructionTime;

public:
	//! Constructor
	TutorialAppManager(AQConnection *Connection) : 
	AACAdvApplicationManager(NULL,Connection,AString())
	{
	}

	//! Supported document handling
	bool HandleUserMayCreate(AMDocumentType* Type)
	{
		return false;
	}

	//! Returns the application's loading status to let the splash screen now when to disappear
	AString GetLoadingStatus()
	{
		return AString("done");
	}

	//! Returns if the application uses the advanced theme graphics. Overwrite this function if you require the advanced theme graphics */
	bool GetUsesAdvancedTheme() { return true; }
};
// ----------------------------------------------------------------------------

//! Pre-initialization handler
void APH_APP_EVENT_HANDLER::Initialize()
{
	AQServer* server = AQServer::GetLocalServer();	
	AString basePath = AApplication.GetDataPath();
	server->RegisterResPath(basePath);

	AQConnection* con = AQServer::GetLocalServer()->CreateLocalConnection();

	mlpManager = ANew(TutorialAppManager)(con);

	ARegisterObjectClass(TutorialHomeActivityWindow);
}

// .........................................................................

#include "../../../LBNavigator/Aphereon4/Source/AphBaseAppIncMain.h"
