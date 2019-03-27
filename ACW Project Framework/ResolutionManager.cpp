#include "ResolutionManager.h"



ResolutionManager::ResolutionManager(ContactManifold* contactManifold, const int positionIterations, const int velocityIterations, const float positionEpsilon, const float velocityEpsilon) : m_positionIterationsDone(0), m_positionIterations(positionIterations), m_velocityIterationsDone(0), m_velocityIterations(velocityIterations), m_positionEpsilon(positionEpsilon), m_velocityEpsilon(velocityEpsilon), m_contactManifold(contactManifold)
{
}


ResolutionManager::~ResolutionManager()
{
}

void ResolutionManager::ResolveContacts(const float dt)
{
	m_positionIterationsDone = 0;
	m_velocityIterationsDone = 0;

	//If we have no contacts then we return
	if (!m_contactManifold->GetNumberOfPoints())
	{
		return;
	}

	//Prepare Contacts
	PrepareContacts(dt);

	AdjustPositions(dt);

	AdjustVelocities(dt);
}

void ResolutionManager::PrepareContacts(const float dt)
{
	for (unsigned int collision = 0; collision < m_contactManifold->GetNumberOfPoints(); ++collision)
	{
		auto &point = m_contactManifold->GetPoint(collision);

		point.CalculateInternals(dt);
	}
}

void ResolutionManager::AdjustPositions(const float dt)
{
	//Now adjust positions and resolve penetrations
	unsigned int i = 0;
	unsigned int index = 0;

	XMVECTOR linearChange[2];
	XMVECTOR angularChange[2];

	auto deltaPosition = XMVECTOR();
	auto maxPenetration = 0.0f;

	m_positionIterationsDone = 0;

	while (m_positionIterationsDone < m_positionIterations)
	{
		//Find the biggest penetration
		maxPenetration = m_positionEpsilon; // Set small value so we ignore any small penetrations, position epsilon
		index = m_contactManifold->GetNumberOfPoints();

		for (i = 0; i < m_contactManifold->GetNumberOfPoints(); i++)
		{
			auto &point = m_contactManifold->GetPoint(i);

			if (point.penetrationDepth > maxPenetration)
			{
				maxPenetration = point.penetrationDepth;
				index = i;
			}
		}

		if (index == m_contactManifold->GetNumberOfPoints())
		{
			break;
		}

		auto &point = m_contactManifold->GetPoint(index);

		point.MatchAwakeState(); //Match awake state

		point.ResolvePenetration(linearChange, angularChange, maxPenetration);

		//Update contacts with new penetrations so we don't resolve the same penetration again
		//And also so other contacts don't have the wrong penetrations

		for (i = 0; i < m_contactManifold->GetNumberOfPoints(); i++)
		{
			auto &otherPoint = m_contactManifold->GetPoint(i);

			for (unsigned int b = 0; b < 2; b++) if (otherPoint.contactID[b])
			{
				for (unsigned int d = 0; d < 2; d++)
				{
					if (otherPoint.contactID[b] == point.contactID[d])
					{
						deltaPosition = XMVectorAdd(linearChange[d], XMVector3Cross(angularChange[d], otherPoint.relativeContactPosition[b]));

						XMStoreFloat(&otherPoint.penetrationDepth, XMVectorAdd(XMLoadFloat(&otherPoint.penetrationDepth), XMVectorScale(XMVector3Dot(deltaPosition, otherPoint.contactNormal), (b?1:-1))));
					}
				}
			
			}
		}

		m_positionIterationsDone++;
	}
}

void ResolutionManager::AdjustVelocities(const float dt)
{
	//Now need to update velocities
	XMVECTOR velocityChange[2];
	XMVECTOR angularVelocityChange[2];
	auto deltaVelocity = XMVECTOR();

	while (m_velocityIterationsDone < m_velocityIterations)
	{
		auto max = m_velocityEpsilon;
		auto index = m_contactManifold->GetNumberOfPoints();

		for (unsigned i = 0; i < m_contactManifold->GetNumberOfPoints(); i++)
		{
			auto &point = m_contactManifold->GetPoint(i);

			if (point.desiredDeltaVelocity > max)
			{
				max = point.desiredDeltaVelocity;
				index = i;
			}
		}

		if (index == m_contactManifold->GetNumberOfPoints())
		{
			break;
		}

		auto &point = m_contactManifold->GetPoint(index);

		point.MatchAwakeState(); //Match awake state

		point.ApplyVelocityChange(velocityChange, angularVelocityChange);

		//Update contacts with new velocity changes

		for (unsigned i = 0; i < m_contactManifold->GetNumberOfPoints(); i++)
		{
			auto &localPoint = m_contactManifold->GetPoint(i);

			for (unsigned b = 0; b < 2; b++) if (localPoint.contactID[b])
			{
				for (unsigned d = 0; d < 2; d++)
				{
					if (localPoint.contactID[b] == point.contactID[d])
					{
						deltaVelocity = XMVectorAdd(velocityChange[d], XMVector3Cross(angularVelocityChange[d], localPoint.relativeContactPosition[b]));

						const auto contactToWorldTranspose = XMMatrixTranspose(localPoint.contactToWorld);

						auto contactDeltaVelocity = XMVECTOR();
						contactDeltaVelocity = XMVector3Transform(deltaVelocity, contactToWorldTranspose);

						localPoint.contactVelocity += XMVectorScale(contactDeltaVelocity, (b?-1:1));

						localPoint.CalculateDesiredDeltaVelocity(dt);
					}
				}
			}
		}

		m_velocityIterationsDone++;
	}
}
