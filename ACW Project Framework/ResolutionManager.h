#pragma once
#include "ContactManifold.h"

//Based off and inspired by Ian Millingtons ContactResolver in the Game Physics Engine Development Book
class ResolutionManager
{
public:
	ResolutionManager(ContactManifold* contactManifold, const int positionIterations, const int velocityIterations, const float positionEpsilon, const float velocityEpsilon);
	~ResolutionManager();

	void ResolveContacts(const float dt);

private:
	void PrepareContacts(const float dt);
	void AdjustPositions(const float dt);
	void AdjustVelocities(const float dt);

	int m_positionIterationsDone;
	int m_positionIterations;
	int m_velocityIterationsDone;
	int m_velocityIterations;

	float m_positionEpsilon;
	float m_velocityEpsilon;

	ContactManifold* m_contactManifold;
};

