#pragma once
#ifndef FCompBrick_h
#define FCompBrick_h

// ----------------------------------------------------------------------------

/** @brief Enumeration of brick types */
enum class FBrickType
{
	Black15,		///< 15mm black brick
	Black30,		///< 30mm black brick
	Alu				///< Aluminum brick
};

// ----------------------------------------------------------------------------

/** @brief Defines an attachment side */
class FBrickAttachmentSide
{
protected:
	FVector		mvOrigin;		///< The attachments origin
	FVector		mvAttachDirect;	///< The attachment 

};

// ----------------------------------------------------------------------------

/** @brief Defines a simple brick */
class FCompBrick : public FComponent
{
	FCLASS(FCompBrick, FComponent);

protected:
	FBrickType	mType;				///< The brick's type
	double		mdWidth = 0.3,		///< The width in centimeters
				mdHeight = 0.15,	///< The height in centimeters
				mdDepth = 0.15;		///< The depth in centimeters

public:
	//! Constructor
	/** @param [in] Parent The parent component 
	  * @param [in] Type The brick type
	  * @param [in][opt] Length Length definition. Will default to 30mm for unknown types, needs to be passed if the brick type is not unique such as aluminum bricks */
	OFB_API FCompBrick(FComponent* Parent, const FBrickType Type, const float Length = 0.0);

	//! Destructor
	OFB_API ~FCompBrick() override;
};

// ----------------------------------------------------------------------------

#endif // of FCompBrick_h