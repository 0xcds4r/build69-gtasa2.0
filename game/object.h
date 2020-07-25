#pragma once

typedef struct _ATTACHED_OBJECT
{
	int iModel;
	int iBoneID;
	VECTOR vecOffset;
	VECTOR vecRotation;
	VECTOR vecScale;
	uint32_t dwMaterialColor1;
	uint32_t dwMaterialColor2;
} ATTACHED_OBJECT;

class CObject : public CEntity
{
public:
	MATRIX4X4	m_matTarget;
	MATRIX4X4	m_matCurrent;
	uint8_t		m_byteMoving;
	float		m_fMoveSpeed;
	bool		m_bIsPlayerSurfing;

	CObject(int iModel, float fPosX, float fPosY, float fPosZ, VECTOR vecRot, float fDrawDistance);
	~CObject();

	void Process(float fElapsedTime);
	float DistanceRemaining(MATRIX4X4 *matPos);

	void SetPos(float x, float y, float z);
	void MoveTo(float x, float y, float z, float speed);

	void InstantRotate(float x, float y, float z);
};