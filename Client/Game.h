#pragma once
class Game
{
public:
	void Init(const WindowInfo& info);
	void Update();
	void ResizeWindow(int32 width, int32 height);
};

