#pragma once

class CEntity
{
public:
	CEntity() {};
	virtual ~CEntity() {};
	virtual void Add();
	virtual void Remove();

	void UpdateRwMatrixAndFrame();
	void UpdateMatrix(MATRIX4X4 mat);
	void Render();

	// 0.3.7
	void GetMatrix(PMATRIX4X4 Matrix);
	// 0.3.7
	void SetMatrix(MATRIX4X4 Matrix);
	// 0.3.7
	void GetMoveSpeedVector(PVECTOR Vector);
	// 0.3.7
	void SetMoveSpeedVector(VECTOR Vector);

	void GetTurnSpeedVector(PVECTOR Vector);
	void SetTurnSpeedVector(VECTOR Vector);
	
	// 0.3.7
	uint16_t GetModelIndex();
	// 0.3.7
	void TeleportTo(float fX, float fY, float fZ);

	bool SetModelIndex(unsigned int uiModel);
	// 0.3.7
	bool IsAdded();
	// 0.3.7
	float GetDistanceFromCamera();
	float GetDistanceFromLocalPlayerPed();
	float GetDistanceFromPoint(float x, float y, float z);

	bool IsValidMatrix(MATRIX4X4 mat);
	bool IsValidMatrixPointer(MATRIX4X4* mat);
	bool IsValidVector(VECTOR vecSpeed);

	uintptr_t GetRWObject();

	void SetCollisionChecking(bool bState);
	void SetGravityProcessing(bool bState);
	bool GetCollisionChecking();
	
public:
	ENTITY_TYPE		*m_pEntity;
	uint32_t		m_dwGTAId;
};