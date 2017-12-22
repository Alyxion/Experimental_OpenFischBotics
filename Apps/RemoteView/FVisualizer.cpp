// ----------------------------------------------------------------------------

#include "TutorialAppStd.h"

// ----------------------------------------------------------------------------

// Constructor
FVisualizer::FVisualizer()
{

}

// ............................................................................

// Destructor
FVisualizer::~FVisualizer()
{

}

// ............................................................................

// Visualizes given component
void FVisualizer::Visualize(FCanvas* Canvas, FComponent *Component, const FVector Offset, const float Scaling, const FVisualSide Side)
{
	FUNUSED_ALWAYS(Canvas);
	FUNUSED_ALWAYS(Component);
	FUNUSED_ALWAYS(Offset);
	FUNUSED_ALWAYS(Scaling);
	FMatrix3DD baseMatrix;
	baseMatrix.SetIdentity();

	FVector rotation(0.0, 0.0, 0.0);

	baseMatrix.RecalcByOrientation(FVector(0.0, 0.0, 0.0), rotation, 1.0, true);

	AList<FVisualizedQuad> quads;
	quads.Reserve(1000);

	FVisualQualityOptions options;
	CollectQuadsRecursive(Component, baseMatrix, quads, options);

	// for all quads do...
	for (FVisualizedQuad &cur : quads)
	{
		Canvas->SetColor(cur.mclColor);
		Canvas->SetLineWidth(1.f);
		for (int edge = 0; edge < 4; ++edge)	// for all 4 edges do...
		{
			FVector &curEdge = cur.mavEdges[edge],
					&nextEdge = cur.mavEdges[(edge + 1) % 4];

			Canvas->DrawLine(FPoint((float)curEdge.mX*Scaling+Offset.mX, (float)curEdge.mY*Scaling+Offset.mY), FPoint((float)nextEdge.mX*Scaling+Offset.mX, (float)nextEdge.mY*Scaling+Offset.mY));
		}
	}
}

// ............................................................................

// Collects all quadrangles and transforms them recursive
void FVisualizer::CollectQuadsRecursive(FComponent* Component, const AMatrix3DD &Matrix, AList<FVisualizedQuad> &Quads, const FVisualQualityOptions &Options)
{
	int originCount = ~Quads;
	int vec = Component->GetVisualElementCount();
	for (int i = 0; i < vec; ++i)
	{
		Component->GetVisualElementAt(i)->GetQuads(Quads, Options);
	}


	FMatrix3DD localMatrix;
	FMatrix3DD localTransformation;
	localTransformation.RecalcByOrientation(Component->GetPosition(), Component->GetRotation(), 1.0, true);
	localMatrix = Matrix*localTransformation;

	// transform edges
	for (int i = originCount; i < ~Quads; ++i)
	{
		FVisualizedQuad &cur = Quads[i];
		localMatrix.Transform(cur.mavEdges[0]);
		localMatrix.Transform(cur.mavEdges[1]);
		localMatrix.Transform(cur.mavEdges[2]);
		localMatrix.Transform(cur.mavEdges[3]);
	}

	int childCount = Component->GetChildCount();
	for (int i = 0; i < childCount; ++i)
	{
		CollectQuadsRecursive(Component->GetChildAt(i), localMatrix, Quads, Options);
	}
}
