#pragma once

#include <string>

#include "math/Vector2.inl"

class Application;
class Window;
class ShaderProgram;
class Material;
class Font;

class SplashWindow
{
	Application* application;
	Window* window;
	ShaderProgram* textShader;
	Material* textMat;
	Font* textFont;

public:
	SplashWindow(Application* application, std::wstring title, Vector2<int> size);
	~SplashWindow();

	Window* getWindow();

	void redraw();
	
	//void setProgress(float amt); // Must call redraw() after
	//void setProgressText(std::wstring text); // Must call redraw() after
};
