#pragma once

#include "Keyboard.h"
#include "Mouse.h"
#include "Camera.h"

class KeyboardControler :public iKeyboardReceiver
{
	Camera* camera;
public:
	KeyboardControler(Camera* camera);
	void KeyDown(int key) override;
};

class MouseControler :public iMouseReceiver
{
	Camera* camera;
public:
	MouseControler(Camera* camera);
	void MouseMoved(int dx, int dy) override;
};