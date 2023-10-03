#include "pch.h"
#include "Geometry.h"

#include <utility>

Geometry::Geometry(FPoint3 position, FVector3 forward)
	: m_Transform()
	, m_Pos(std::move(position))
	, m_Forward(std::move(forward))
{
	CalcTransform();
}

const FPoint3& Geometry::GetPosition() const
{
	return m_Pos;
}
void Geometry::SetPosition(const FPoint3& position)
{
	m_Pos = position;
	CalcTransform();
}

const FVector3& Geometry::GetForward() const
{
	return  m_Forward;
}
void Geometry::SetForward(const FVector3& forward)
{
	m_Forward = forward;
	CalcTransform();
}

const FMatrix4& Geometry::GetTransform() const
{
	return m_Transform;
}


void Geometry::CalcTransform()
{
	const FVector3 worldup{ 0,1,0 };

	const FVector3 right{ Cross(worldup, m_Forward) };
	const FVector3 up{ Cross(m_Forward, right) };

	m_Transform[0] = FVector4{ right, 0 };
	m_Transform[1] = FVector4{ up, 0 };
	m_Transform[2] = FVector4{ m_Forward, 0 };
	m_Transform[3] = FVector4{ m_Pos.x, m_Pos.y, m_Pos.z, 1.f };

	OnRecalculateTransform();
}
