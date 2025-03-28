#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>
#include <imgui_internal.h>

#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/CustomizeObjectLayer.hpp>

#include <Geode/modify/CustomSongLayer.hpp>
#include <Geode/modify/CustomSongWidget.hpp>
#include <Geode/modify/CCTouchDispatcher.hpp>

#include <IconsMaterialDesignIcons.h>
#include "modules/EditGroupModule.hpp"
#include "modules/SelectFilterModule.hpp"
#include "modules/TransformObjectModule.hpp"
#include "modules/FooterModule.hpp"
#include "modules/ToolsModule.hpp"
#include "modules/CameraSettings.hpp"
#include "modules/ObjectListModule.hpp"

using namespace geode::prelude;

template <class R, class T>
R& from(T base, intptr_t offset) {
	return *reinterpret_cast<R*>(reinterpret_cast<uintptr_t>(base) + offset);
}

bool filterSingleObj(bool filterBool, int objParameter, std::vector<int> vec) {
	if (!filterBool) return true;
	for (int i = 0; i < vec.size(); i++) {
		if (objParameter == vec[i])
			return true;
	}
	return false;
}

class $modify(EditorUI) {
	void selectObject(GameObject* obj, bool p1) {
		if (ErGui::selectFilterRealization(obj))
			EditorUI::selectObject(obj, p1);
		return;
	}

	void selectObjects(CCArray* objArrInRect, bool p1) {
		
		//orig
		EditorUI::selectObjects(ErGui::selectFilterRealization(objArrInRect), p1);
		
		ErGui::groupInfoUpdate();
	}

	bool init(LevelEditorLayer* lel) {
		ErGui::editorUIDrawNode = CCDrawNode::create();
		this->addChild(ErGui::editorUIDrawNode);
		auto ret = EditorUI::init(lel);
		//ErGui::originalCameraPosition = lel->getChildByID("main-node")->getChildByID("batch-layer")->getPosition();

		ErGui::og_prevMode = lel->m_previewMode;
		ErGui::og_prevShad = lel->m_previewShaders;
		ErGui::og_prevPart = lel->m_previewParticles;
		ErGui::og_prevAnim = lel->m_previewAnimations;

		return ret;
	}

	bool ccTouchBegan(CCTouch* touch, CCEvent* event) {
		//std::cout << "CCTouch: X=" << touch->m_point.x << " Y=" << touch->m_point.y << "\n";
		if (this->m_selectedMode == 3 && ErGui::isLassoEnabled) {
			ErGui::editorUILassoPoints.clear();
			CCPoint pt = touch->getLocation();
			ErGui::editorUILassoPoints.push_back(pt);
			ErGui::editorUIDrawNode->clear();

			//this->m_unk244 = true;
		}
		return EditorUI::ccTouchBegan(touch, event);
	}

	void ccTouchMoved(CCTouch* touch, CCEvent* event) {
		if (this->m_selectedMode == 3 && ErGui::isLassoEnabled) {
			CCPoint pt = touch->getLocation();
			ErGui::editorUILassoPoints.push_back(pt);
			ErGui::editorUIDrawNode->clear();

			if (ErGui::editorUILassoPoints.size() > 1) {
				ErGui::editorUIDrawNode->drawPolygon(ErGui::editorUILassoPoints.data(), ErGui::editorUILassoPoints.size(), { 0, 0, 0, 0 }, 0.2f, { 0, 0.7f, 0, 1.f });
			}
		}
		EditorUI::ccTouchMoved(touch, event);
	}

	void ccTouchEnded(CCTouch* touch, CCEvent* event) {
		if (ErGui::editorUILassoPoints.size() > 2 && this->m_selectedMode == 3 && ErGui::isLassoEnabled) {
			ErGui::editorUIDrawNode->clear();
			ErGui::editorUIDrawNode->drawPolygon(ErGui::editorUILassoPoints.data(), ErGui::editorUILassoPoints.size(), { 0, 0, 0, 0 }, 0.2f, { 0, 1.f, 0, 1.f });

			CCArray* objArr = CCArray::create();
			for (auto obj : CCArrayExt<GameObject*>(GameManager::sharedState()->getEditorLayer()->m_objects)) {

				auto editorLayer = GameManager::sharedState()->getEditorLayer();
				auto cameraPos = editorLayer->getChildByID("main-node")->getChildByID("batch-layer")->getPosition();
				auto cameraScale = editorLayer->getChildByID("main-node")->getChildByID("batch-layer")->getScale();

				auto objPos = obj->getPosition();
				auto newPos = cocos2d::CCPoint(
					(objPos.x * cameraScale) + cameraPos.x,
					(objPos.y * cameraScale) + cameraPos.y
				);

				//std::cout << objPos.x << " " << objPos.y << "\n";
				//std::cout << newPos.x << " " << newPos.y << "\n\n";

				if (ErGui::isPointInPolygon(newPos, ErGui::editorUILassoPoints)) {
					objArr->addObject(obj);
				}
			}
			//std::cout << "\n";
			if (objArr->count() > 0)
				this->selectObjects(objArr, false);
			CC_SAFE_RELEASE(objArr);
			ErGui::editorUIDrawNode->clear();
		}

		EditorUI::ccTouchEnded(touch, event);
	}

	void destructor() {
		CC_SAFE_RELEASE(ErGui::editorUIDrawNode);
		ErGui::editorUIDrawNode = nullptr;
		EditorUI::~EditorUI();
	}
};

//int someIterator = 0;
class $modify(CCTouchDispatcher) {
	void touches(CCSet* touches, CCEvent* event, unsigned int type) {
		//std::cout << someIterator << " - touchesHook\n";
		//someIterator++;
		CCTouchDispatcher::touches(touches, event, type);
	}
};

//class $modify(CustomizeObjectLayer) {
//	void onSelectColor(CCObject * sender) {
//		CustomizeObjectLayer::onSelectColor(sender);
//		if (GameObject* obj = GameManager::sharedState()->getEditorLayer()->m_editorUI->m_selectedObject) {
//			if (from<int>(this, 0x2d0) == 1) {
//				obj->m_activeMainColorID = from<int>(this, 0x2d4);
//			}
//			else if (from<int>(this, 0x2d0) == 2) {
//				obj->m_activeDetailColorID = from<int>(this, 0x2d4);
//			}
//		}
//	}
//};


static bool touchHasBegan = false;
static bool touchHasBegan2 = false;
static float winMultiplier = 1.5f;
static int selectedTab = 0;


void ShowMainWindow()
{
	// Окно «MainWindow» занимает всё окно приложения
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, ImVec4(0, 0, 0, 0));

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
	ImGuiWindowFlags mainWindowFlags = ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoInputs;
	ImGui::Begin("MainWindow", nullptr, mainWindowFlags);

	// Создаём док-спейс внутри основного окна
	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));


	//if (ImGui::IsItemHovered()) {
	//	ImGuiCocos::get().setShouldPassClicks(true);
	//}

	ImGui::End(); // Завершаем "MainWindow"

	ImGui::PopStyleColor(2);
}




$on_mod(Loaded) {
	AllocConsole();
	freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);

	ImGuiCocos::get().setup([] {
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigWindowsMoveFromTitleBarOnly = true;
		//io.Fonts->Clear();
		//io.Fonts->AddFontFromFileTTF("Karla-Regular.ttf", 16.f);
		//io.Fonts->Build();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		}).draw([] {

			if (auto lel = GameManager::sharedState()->getEditorLayer()) {
				// В начале каждого кадра: создание основного окна на весь экран
				//ImGui::SetNextWindowPos(ImVec2(0, 0));
				//ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
				//ImGuiWindowFlags mainWindowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus;
				//ImGui::Begin("MainWindow", nullptr, mainWindowFlags);

				//ImGui::End(); // Завершаем "MainWindow"
				// Можно добавить сюда дополнительные элементы ImGui для управления интерфейсом
				
				//ShowMainWindow();

				ErGui::renderEditGroupModule();
				ErGui::renderSelectFilter();
				ErGui::renderTransformation();
				ErGui::renderFooter();
				ErGui::renderToolsModule();
				ErGui::renderObjectList();

				//ErGui::renderCameraSettings();
				

				//ErGui::editorUIDrawNode->setPosition({ 0.f, 0.f });
				//if (ImGui::Begin("Geometry Dash-1")) {
				//
				//	//ErGui::editorUIDrawNode->setPosition({ 0.f, 0.f });
				//	lel->getChildByID("main-node")->getChildByID("batch-layer")->setPosition(ErGui::originalCameraPosition);
				//	//lel->m_previewAnimations =	ErGui::og_prevAnim;
				//	//lel->m_previewMode =		ErGui::og_prevMode;
				//	//lel->m_previewParticles =	ErGui::og_prevPart;
				//	//lel->m_previewShaders =		ErGui::og_prevShad;
				//	lel->update(1.f);
				//
				//	auto winWidth = CCDirector::sharedDirector()->getWinSize().width;
				//	auto winHeight = CCDirector::sharedDirector()->getWinSize().height;
				//
				//	auto renderTexture = CCRenderTexture::create(winWidth, winHeight);
				//
				//	renderTexture->begin();
				//	glPushAttrib(GL_ALL_ATTRIB_BITS);
				//	CCDirector::sharedDirector()->getRunningScene()->visit();
				//	glPopAttrib();
				//	renderTexture->end();
				//
				//	GLuint gameTexture = renderTexture->getSprite()->getTexture()->getName();
				//
				//
				//	ImGui::Image(static_cast<ImTextureID>(gameTexture), ImVec2(winWidth * winMultiplier, winHeight * winMultiplier), ImVec2(0, 1), ImVec2(1, 0));
				//
				//
				//	if (ImGui::IsItemHovered() && ImGui::IsMouseDown(0))
				//	{
				//		ImVec2 clickPos = ImGui::GetIO().MousePos;
				//		ImVec2 windowPos = ImGui::GetItemRectMin();
				//		std::cout << "win: X=" << winWidth << " Y=" << winHeight << "\n";
				//		std::cout << "clickPos: X=" << clickPos.x << " Y=" << clickPos.y << "\n";
				//		std::cout << "windowPos: X=" << windowPos.x << " Y=" << windowPos.y << "\n";
				//
				//		float localX = (clickPos.x - windowPos.x) / winMultiplier;
				//		float localY = (clickPos.y - windowPos.y) / winMultiplier;
				//
				//		CCTouch* touch = new CCTouch;
				//		touch->setTouchInfo(0, localX, localY);
				//
				//		std::cout << "Locals: X=" << localX << " Y=" << localY << "\n";
				//
				//		CCSet* touches = CCSet::create();
				//		touches->addObject(touch);
				//		touch->release();  // CCSet берет владение объектом
				//
				//		CCEvent* event = new CCEvent();
				//		if (!touchHasBegan) {
				//			lel->m_editorUI->ccTouchBegan(touch, event);
				//			touchHasBegan = true;
				//		}
				//		else {
				//			lel->m_editorUI->ccTouchMoved(touch, event);
				//			ErGui::originalCameraPosition = lel->getChildByID("main-node")->getChildByID("batch-layer")->getPosition();
				//		}
				//
				//
				//		CC_SAFE_RELEASE(event);
				//		//CCDirector::sharedDirector()->getTouchDispatcher()->touchesBegan(touches, nullptr);
				//		std::cout << "\n";
				//	}
				//	else if (touchHasBegan) {
				//		ImVec2 clickPos = ImGui::GetIO().MousePos;
				//		ImVec2 windowPos = ImGui::GetItemRectMin();
				//
				//		float localX = clickPos.x - windowPos.x;
				//		float localY = clickPos.y - windowPos.y;
				//
				//		CCTouch* touch = new CCTouch;
				//		touch->setTouchInfo(0, localX, localY);
				//
				//		CCEvent* event = new CCEvent();
				//
				//		lel->m_editorUI->ccTouchEnded(touch, event);
				//		touchHasBegan = false;
				//		CC_SAFE_RELEASE(event);
				//		CC_SAFE_RELEASE(event);
				//	}
				//}
				//ErGui::enableClicks();
				//ImGui::End();
				//
				//
				//
				//
				//if (ImGui::Begin("Geometry Dash-2")) {
				//	
				//	lel->getChildByID("main-node")->getChildByID("batch-layer")->setPosition({ 
				//		ErGui::originalCameraPosition.x + ErGui::cam2_offsetX,
				//		ErGui::originalCameraPosition.y + ErGui::cam2_offsetY
				//		});
				//	//lel->m_previewAnimations =	ErGui::cam2_prevAnim;
				//	//lel->m_previewMode =		false;
				//	//lel->m_previewParticles =	ErGui::cam2_prevPart;
				//	//lel->m_previewShaders =		ErGui::cam2_prevShad;
				//	lel->update(1.f);
				//	
				//	auto winWidth = CCDirector::sharedDirector()->getWinSize().width;
				//	auto winHeight = CCDirector::sharedDirector()->getWinSize().height;
				//
				//	auto renderTexture = CCRenderTexture::create(winWidth, winHeight);
				//
				//	renderTexture->begin();
				//	glPushAttrib(GL_ALL_ATTRIB_BITS);
				//	CCDirector::sharedDirector()->getRunningScene()->visit();
				//	glPopAttrib();
				//	renderTexture->end();
				//
				//	GLuint gameTexture = renderTexture->getSprite()->getTexture()->getName();
				//
				//
				//	ImGui::Image(static_cast<ImTextureID>(gameTexture), ImVec2(winWidth * winMultiplier, winHeight * winMultiplier), ImVec2(0, 1), ImVec2(1, 0));
				//
				//
				//	if (ImGui::IsItemHovered() && ImGui::IsMouseDown(0))
				//	{
				//		ImVec2 clickPos = ImGui::GetIO().MousePos;
				//		ImVec2 windowPos = ImGui::GetItemRectMin();
				//		std::cout << "win: X=" << winWidth << " Y=" << winHeight << "\n";
				//		std::cout << "clickPos: X=" << clickPos.x << " Y=" << clickPos.y << "\n";
				//		std::cout << "windowPos: X=" << windowPos.x << " Y=" << windowPos.y << "\n";
				//
				//		float localX = (clickPos.x - windowPos.x) / winMultiplier;
				//		float localY = (clickPos.y - windowPos.y) / winMultiplier;
				//
				//		CCTouch* touch = new CCTouch;
				//		touch->setTouchInfo(0, localX, localY);
				//
				//		std::cout << "Locals: X=" << localX << " Y=" << localY << "\n";
				//
				//		CCSet* touches = CCSet::create();
				//		touches->addObject(touch);
				//		touch->release();  // CCSet берет владение объектом
				//
				//		CCEvent* event = new CCEvent();
				//		if (!touchHasBegan2) {
				//			lel->m_editorUI->ccTouchBegan(touch, event);
				//			touchHasBegan2 = true;
				//		}
				//		else {
				//			lel->m_editorUI->ccTouchMoved(touch, event);
				//			ErGui::originalCameraPosition = lel->getChildByID("main-node")->getChildByID("batch-layer")->getPosition();
				//			ErGui::originalCameraPosition.x -= ErGui::cam2_offsetX;
				//			ErGui::originalCameraPosition.y -= ErGui::cam2_offsetY;
				//		}
				//
				//		CC_SAFE_RELEASE(event);
				//		//CCDirector::sharedDirector()->getTouchDispatcher()->touchesBegan(touches, nullptr);
				//		std::cout << "\n";
				//	}
				//	else if (touchHasBegan2) {
				//		ImVec2 clickPos = ImGui::GetIO().MousePos;
				//		ImVec2 windowPos = ImGui::GetItemRectMin();
				//
				//		float localX = clickPos.x - windowPos.x;
				//		float localY = clickPos.y - windowPos.y;
				//
				//		CCTouch* touch = new CCTouch;
				//		touch->setTouchInfo(0, localX, localY);
				//
				//		CCEvent* event = new CCEvent();
				//
				//		lel->m_editorUI->ccTouchEnded(touch, event);
				//		touchHasBegan2 = false;
				//		CC_SAFE_RELEASE(event);
				//		CC_SAFE_RELEASE(event);
				//	}
				//
				//}
				//ErGui::enableClicks();
				//ImGui::End();
				//
				//// Индекс текущей вкладки
				//
				//// Определяем ширину боковой панели
				//ImGui::Begin("Sidebar", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
				//ImGui::BeginGroup(); // Группируем кнопки
				//
				//// Создаём вертикальные кнопки
				//if (ImGui::Selectable("A", selectedTab == 0, 0, ImVec2(40, 40))) selectedTab = 0;
				//if (ImGui::Selectable("B", selectedTab == 1, 0, ImVec2(40, 40))) selectedTab = 1;
				//if (ImGui::Selectable("C", selectedTab == 2, 0, ImVec2(40, 40))) selectedTab = 2;
				//if (ImGui::Selectable("D", selectedTab == 3, 0, ImVec2(40, 40))) selectedTab = 3;
				//
				//ImGui::EndGroup();
				//ImGui::SameLine(); // Размещаем контент рядом
				//
				//// Основное содержимое вкладки
				//ImGui::BeginChild("TabContent", ImVec2(0, 0), true);
				//switch (selectedTab) {
				//case 0:
				//	ImGui::Text("Transform");
				//	ImGui::SliderFloat("X", nullptr, 0.0f, 100.0f);
				//	ImGui::SliderFloat("Y", nullptr, 0.0f, 100.0f);
				//	ImGui::SliderFloat("Z", nullptr, 0.0f, 100.0f);
				//	break;
				//case 1:
				//	ImGui::Text("Object Parameteres");
				//	break;
				//case 2:
				//	ImGui::Text("Animation");
				//	break;
				//case 3:
				//	ImGui::Text("Display");
				//	break;
				//}
				//ImGui::EndChild();
				//
				//ImGui::End();
			}
			});
}