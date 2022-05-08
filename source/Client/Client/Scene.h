﻿#pragma once

#include "Shader.h"

class CScene
{
public:
	CScene();
	~CScene();

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseObjects();

	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }

	bool ProcessInput();
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = nullptr);

	void ReleaseUploadBuffers();

	CHeightMapTerrain* GetTerrain() { return(m_pTerrain); }

	void CheckPlayerByObjectCollisions();
	void CheckMissileByObjectCollisions();
	void CheckMissileByTerrainCollisions();

	CPlayer* m_pPlayer = nullptr;
	std::unordered_map<int, CPlayer*> players;

	CGameObject* GetObjects(int i) { return m_ppObjects[i]; }

protected:
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = nullptr;

	CGameObject** m_ppObjects = 0;
	int							m_nObjects = 0;

	CHeightMapTerrain* m_pTerrain = nullptr;
};
