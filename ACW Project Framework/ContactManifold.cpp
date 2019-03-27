#include "ContactManifold.h"

ContactManifold::ContactManifold() : m_numberOfPoints(0)
{
}

ContactManifold::~ContactManifold() = default;

void ContactManifold::Add(ManifoldPoint &point)
{
	m_points.push_back(point);
	m_numberOfPoints++;
}

void ContactManifold::Clear()
{
	m_points.clear();
	m_numberOfPoints = 0;
}

unsigned int ContactManifold::GetNumberOfPoints() const
{
	return m_numberOfPoints;
}

ManifoldPoint& ContactManifold::GetPoint(int index)
{
	return m_points[index];
}
