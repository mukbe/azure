#pragma once
#include "stdafx.h"

enum class EmitterMode { Play, Pause, Stop };
enum class RenderMode { Billboard = 0, StretchedBillboard, HorizontalQuad, End };
enum class ShaderMode { ADDITIVE, MULTIFLY, POW, ALPHABLEND, End };
enum class ShapeType { None = 0, Circle, Sphere, Box, End };


struct ParticleData
{
	D3DXVECTOR3 Position;
	D3DXCOLOR Color;
	D3DXVECTOR2 Size;
	float Rotation;

	D3DXVECTOR3 Direction;	//����
	D3DXVECTOR3 Speed;	//����ġ

	float LifeTime;	//�̳��� ������� �ð�
	float RemainTime;	//�����ִ� ����

	D3DXVECTOR3 Gravity;
	D3DXVECTOR3 Force;

	int NowFrame;
	float FrameTimer;
	float Fps;
	int LoopCount;
	UINT bLoop;

	D3DXVECTOR3 QuadPositions[4];
};

struct BurstInfo
{
	float Time;
	int Count;

	int Cycles;
	float Intervals;

	float Timer;
};

struct ShapeInfo
{
	ShapeType type;

};

