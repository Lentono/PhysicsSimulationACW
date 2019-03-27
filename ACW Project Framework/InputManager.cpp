#include "InputManager.h"

// We don't need a default/ copy/ move constructor for this class but it's best to have it defined so the compiler doesn't generate it for us
InputManager::InputManager() : m_doOnce(true), m_keys{false}
{

}

InputManager::InputManager(const InputManager& other) = default;

InputManager::InputManager(InputManager&& other) noexcept = default;

InputManager::~InputManager() = default;

InputManager& InputManager::operator=(const InputManager& other) = default;

InputManager& InputManager::operator=(InputManager&& other) noexcept = default;

void InputManager::KeyDown(unsigned int const key) {
	//Update our key array and change defined key input to pressed
	m_keys[key] = true;
}

void InputManager::KeyUp(unsigned int const key) {
	//Update our key array and change the defined key input to released
	m_keys[key] = false;
}

bool InputManager::IsKeyDown(unsigned int const key) {
	//Return state of the key
	return m_keys[key];
}

bool InputManager::IsKeyUp(unsigned int const key)
{
	return !m_keys[key];
}

bool InputManager::DoOnce()
{
	return m_doOnce;
}

void InputManager::ToggleDoOnce(const bool toggle)
{
	m_doOnce = toggle;
}
