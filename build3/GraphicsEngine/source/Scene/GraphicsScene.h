#pragma once
#include "pch.h"
#include "SceneObject/Object3D.h"
#include "SceneObject/Object2D.h"
#include "SceneObject/ScreenText.h"
#include "SceneObject/SkyBox.h"
#include "SceneObject/Button.h"

#include "Camera.h"

namespace GraphicsEngine {
	class GRAPHICS_API GraphicsScene
	{
		using Object3D_ref = std::reference_wrapper<Object3D>;
		using Object2D_ref = std::reference_wrapper<Object2D>;
		using ScreenText_ref = std::reference_wrapper<ScreenText>;
	public:
		void addSceneObject(Object3D& object) { sceneObjects.emplace_back(object); }
		void popSceneObject() { sceneObjects.pop_back(); }

		void addTextObject(ScreenText& text) { screenTexts.emplace_back(text); }
		void popTextObject() { screenTexts.pop_back(); }

		void addTextureObject(Object3D& object) { screenTextureObjects.emplace_back(object); }

		void setCamera(Camera& cam) { camera = &cam; }
		void setSkybox(GraphicsEngine::SkyBox& sb) { skyBox = &sb; }
		
		void setButton(Button& b) { button = &b; }
		void resetButton() { button->Click(false); button = nullptr; }
	public:
		std::vector<Object3D_ref> sceneObjects;
		std::vector<Object2D>* sceneObjects2D; // hit points
		std::vector<Object3D_ref> screenTextureObjects;
		std::vector<ScreenText_ref> screenTexts;
		SkyBox* skyBox = nullptr;
		Button* button = nullptr;
		Camera* camera = nullptr;
	};
}

