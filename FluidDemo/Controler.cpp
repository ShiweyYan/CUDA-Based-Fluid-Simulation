#include "Controler.h"
#include "InputLayer.h"

extern "C" void changeWaterbox(int i);

KeyboardControler::KeyboardControler(Camera* camera) :iKeyboardReceiver()
{
	this->camera = camera;
}

void KeyboardControler::KeyDown(int key)
{
	switch (key)
	{
	case DIK_W:
		camera->Walk(.1f);
		break;
	case DIK_A:
		camera->Strafe(-.1f);
		break;
	case DIK_S:
		camera->Walk(-.1f);
		break;
	case DIK_D:
		camera->Strafe(.1f);
		break;
	case  DIK_Z:
		changeWaterbox(0);
		break;
	case DIK_X:
		changeWaterbox(1);
		break;;
	case DIK_C:
		changeWaterbox(2);
		break;
	case  DIK_V:
		changeWaterbox(3);
		break;
	case  DIK_R:
		changeWaterbox(4);
		break;
	default:
		break;
	}
}

MouseControler::MouseControler(Camera* camera)
{
	this->camera = camera;
}

void MouseControler::MouseMoved(int dx, int dy)
{
	float radiansX = XMConvertToRadians(0.25*dx);
	float radiansY = XMConvertToRadians(0.25*dy);
	camera->Pitch(radiansY);
	camera->RotateY(radiansX);
}