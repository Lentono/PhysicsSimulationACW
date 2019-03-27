#include "Position.h"

Position::Position() : m_position(0.0f, 0.0f, 0.0f), m_newPosition(0.0f, 0.0f, 0.0f)
{
}

Position::Position(const XMFLOAT3 position) : m_position(position) , m_newPosition(0.0f, 0.0f, 0.0f) {

}

Position::Position(const float x, const float y, const float z): m_position(x, y, z), m_newPosition(0.0f, 0.0f, 0.0f) {

}

Position::Position(const Position& other) = default;

Position::Position(Position&& other) noexcept = default;

Position::~Position() = default;

Position& Position::operator=(const Position& other) = default;

//Position& Position::operator=(Position&& other) noexcept = default;

XMFLOAT3 Position::GetPosition() const {
	return m_position;
}

void Position::UpdatePosition()
{
	m_position = m_newPosition;
}


void Position::SetNewPosition(const XMFLOAT3 newPosition) {
	m_newPosition = newPosition;
}

void Position::SetNewPosition(const float x, const float y, const float z) {
	m_newPosition = XMFLOAT3(x, y, z);
}

void Position::TranslatePosition(const XMFLOAT3 translate) {
	m_position = XMFLOAT3(m_position.x + translate.x, m_position.y + translate.y, m_position.z + translate.z);
}

void Position::TranslatePosition(const float x, const float y, const float z) {
	m_position = XMFLOAT3(m_position.x + x, m_position.y + y, m_position.z + z);
}

