//================================================//

#include "GUILayerGameState.hpp"

//================================================//

namespace GUIGameState
{

//================================================//

const Ogre::String GUIHudLayer::BUTTON_INFO = "Button_Info";

const Ogre::String GUIHudLayer::OVERLAY_RETICULE = "ReticulePanel";

//================================================//

void GUIHudLayer::create(void)
{
	int buttonWidth = 800, buttonHeight = 26;
	int screenWidth = Base::getSingletonPtr()->m_pViewport->getActualWidth();
	int screenHeight = Base::getSingletonPtr()->m_pViewport->getActualHeight();

	MyGUI::ButtonPtr buttonInfo = Base::getSingletonPtr()->m_GUI->createWidget<MyGUI::Button>("Button", 
		screenWidth / 2.0, 0, buttonWidth, buttonHeight, MyGUI::Align::Default, "Main");
	m_widgets.push_back(buttonInfo);


	// Overlay //
	m_useOverlay = true;

	// Create HUD overlay
	Ogre::OverlayManager& manager = Ogre::OverlayManager::getSingleton();
	m_overlay = manager.create("Overlay");

	// Reticule
	m_overlayReticule = static_cast<Ogre::OverlayContainer*>(manager.createOverlayElement("Panel", OVERLAY_RETICULE));
	m_overlayReticule->setPosition(0.48, 0.48);
	m_overlayReticule->setDimensions(0.04, 0.04);
	m_overlayReticule->setMaterialName("Examples/Crosshair"); // for some reason this must be set before the reticule is added
	m_overlay->add2D(m_overlayReticule);
	m_overlayContainers.push_back(m_overlayReticule);

	// Show overlay
	m_overlay->show();
}

//================================================//

} // namespace

//================================================//