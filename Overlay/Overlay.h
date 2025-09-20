#pragma once

namespace Overlay {
	LRESULT CALLBACK Input(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void Render();
	void Init();
}