#pragma once

//Speeds up the build process, reduces the size of our header files by excluding the less used APIs
#define WIN32_LEAN_AND_MEAN

//Need windows header to create and destroy windows and use other windows API functions
#include <Windows.h>

#include "InputManager.h"
#include "GraphicsRenderer.h"

class System
{
public:
	System(); // Default Constructor
	System(const System& other); // Copy Constructor
	System(System&& other) noexcept; // Move Constructor
	~System(); // Destructor

	System& operator = (const System& other); // Copy Assignment Operator
	System& operator = (System&& other) noexcept; // Move Assignment Operator

	void Run();

	bool GetInitializationState() const;

	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);

private:
	bool Frame();
	void InitializeWindows(int& screenWidth, int& screenHeight);
	void ShutdownWindows();

	bool m_initializationFailed;

	float m_screenWidth;
	float m_screenHeight;

	LPCSTR m_applicationName{};
	HINSTANCE m_hInstance{};
	HWND m_hwnd;

	InputManager* m_input{};
	GraphicsRenderer* m_graphics{};

};

//This exists so we can redirect the windows system messaging to our MessageHandler in the system class
//^^^ I guess I'll have to look out for the usefulness of this... ^^^

//Function prototype
static LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);

//Global
static System* applicationHandle = nullptr;