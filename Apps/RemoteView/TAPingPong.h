// PingPong tutorial class - Level 1

class FComponent;
class FVisualizer;

/** @brief Basic implementation, just draws the components and refreshes the view */
class TAPingPong : public AQCView
{
protected:
	ACLASS(TAPingPong, AQCView);	///< Define relation

	AQTimer		*mlpTimer;			///< The refresh timer
	int			miTutorialLevel;	///< Tutorial level
	float		mfBallRad,			///< The ball's size
				mfTopBorderSize,	///< The top border's height
				mfBouncerWidth,		///< The bouncer's width
				mfBouncerHeight;	///< The bouncer's height
	AVector2DF	mvBallPosition;		///< The ball's position
	float		mfLeftBouncerY,		///< Left bouncer y
				mfRightBouncerY;	///< Right bouncer y
	double		mdUpdFrequency;		///< Update frequency (per second)
	AInt64		mxiLastFrame = -1;	///< Last valid frame
	bool		mbForward = false,
				mbLeft = false,
				mbRight = false,
				mbBackwards = false;
	int			miCamResIndex = 0;	///< Camera resolution index
	ASQImage* mlpCamImage = nullptr;
	FComponent	*mlpMainModel = nullptr;
	FControllerTXT *mlpController = nullptr;
	FVisualizer *mlpFVisualizer = nullptr;


public:
	// Constructor
	TAPingPong(AQCView* Parent);

	// Destructor
	~TAPingPong();

	// Override painting
	void Paint(ASQCanvas* Canvas, const AQPaintingOptions &Options);

	// Override timer event
	bool HandleTimer(AQTimerEvent* Event);

	// Key down handling
	bool HandleKeyDown(AQKeyDownEvent* Event) override;

	// Key up handling
	bool HandleKeyUp(AQKeyUpEvent* Event) override;

	// Key press handling
	bool HandleKeyPress(AQKeyPressEvent* Event) override;

	// May get keyboard focus
	bool HandleMayGetKeyboardFocus() override { return true; }
};