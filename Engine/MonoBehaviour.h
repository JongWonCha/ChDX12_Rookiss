#pragma once
#include "Component.h"
class MonoBehaviour : public Component
{
public:
	MonoBehaviour();
	virtual ~MonoBehaviour();

private:
	virtual void FinalUpdate() sealed {}	// FinalUpdate는 엔진 관련 부분만 업데이트 하는 페이즈임. 때문에 구현하지 못하게 봉인함.

};

