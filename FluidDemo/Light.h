#pragma once
#include <Windows.h>
#include <xnamath.h>

struct DirectLight
{
	DirectLight(){ ZeroMemory(this, sizeof(DirectLight)); }

	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
	XMFLOAT3 direction;
	float	unused;		//��4D������������
};

struct PointLight
{
	PointLight(){ ZeroMemory(this, sizeof(PointLight)); }

	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;

	XMFLOAT3 position;
	float range;

	XMFLOAT3 attenuation;//˥������
	float	unused;		//��4D������������
};

struct SpotLight
{
	SpotLight() { ZeroMemory(this, sizeof(SpotLight)); }

	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;

	XMFLOAT3  direction;
	float range;

	XMFLOAT3 position;
	float spot;

	XMFLOAT3 attenuation;
	float	unused;		//��4D������������
};

struct Material
{
	Material() { ZeroMemory(this, sizeof(Material)); }

	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular; 
	//XMFLOAT4 reflect;
};