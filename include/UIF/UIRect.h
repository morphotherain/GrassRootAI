#pragma once

struct UIRect
{
	float x = 0.0f;
	float y = 0.0f;
	float w = 0.0f;
	float h = 0.0f;

	bool Contains(float px, float py) const
	{
		return px >= x && px <= x + w && py >= y && py <= y + h;
	}

	UIRect Translated(float dx, float dy) const
	{
		return { x + dx, y + dy, w, h };
	}
};
