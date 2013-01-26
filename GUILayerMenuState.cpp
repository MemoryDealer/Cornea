//================================================//

#include "GUILayerMenuState.hpp"

//================================================//

namespace GUIMenuState
{

//================================================//

void GUIRootLayer::create(void)
{
	int buttonWidth = 300, buttonHeight = 26;
	int screenWidth = Base::getSingletonPtr()->m_pViewport->getActualWidth();
	int screenHeight = Base::getSingletonPtr()->m_pViewport->getActualHeight();

	// New Game button
	MyGUI::ButtonPtr buttonNewGame = Base::getSingletonPtr()->m_GUI->createWidget<MyGUI::Button>("Button",
		(screenWidth / 2) - (buttonWidth / 2), (screenHeight / 2) - (buttonHeight / 2), buttonWidth, buttonHeight, MyGUI::Align::Default, "Main", "Button_NewGame");
	buttonNewGame->setCaption("New Game");
	buttonNewGame->eventMouseButtonClick += MyGUI::newDelegate(this, &GUIRootLayer::notifyMouseButtonClick);
	m_widgets.push_back(buttonNewGame);

	// Set the pointer to visible
	MyGUI::PointerManager::getInstancePtr()->setVisible(true);
}

//================================================//

void GUIRootLayer::notifyMouseButtonClick(MyGUI::WidgetPtr _sender)
{
	if(_sender == this->getWidgetPtr("Button_NewGame")){
		*m_eventId = EVENT_NEWGAME;
	}
}

//================================================//
//================================================//

void GUINewGameLayer::create(void)
{
	int buttonWidth = 300, buttonHeight = 26;
	int screenWidth = Base::getSingletonPtr()->m_pViewport->getActualWidth();
	int screenHeight = Base::getSingletonPtr()->m_pViewport->getActualHeight();

	// Name edit box
	MyGUI::EditPtr editName = Base::getSingletonPtr()->m_GUI->createWidget<MyGUI::EditBox>("EditBox", 
		(screenWidth / 2) - (buttonWidth / 2), (screenHeight / 2) - (buttonHeight / 2) - 25, buttonWidth, buttonHeight, MyGUI::Align::Default, "Main", "EditBox_Name");
	editName->eventKeySetFocus += MyGUI::newDelegate(this, &GUINewGameLayer::notifyMouseFocusSet);
	m_widgets.push_back(editName);

	// Start button
	MyGUI::ButtonPtr buttonStart = Base::getSingletonPtr()->m_GUI->createWidget<MyGUI::Button>("Button",
		(screenWidth / 2) - (buttonWidth / 2), (screenHeight / 2) - (buttonHeight / 2) + 25, buttonWidth, buttonHeight, MyGUI::Align::Default, "Main", "Button_Start");
	buttonStart->setCaption("Start Game");
	buttonStart->eventMouseButtonClick += MyGUI::newDelegate(this, &GUINewGameLayer::notifyMouseButtonClick);
	m_widgets.push_back(buttonStart);

	// Hide this layer initially
	this->setVisible(false);
}

//================================================//

void GUINewGameLayer::resetWidgets(void)
{
	// Name edit box
	static_cast<MyGUI::EditPtr>(this->getWidgetPtr("EditBox_Name"))->setCaption("Enter Your Name...");
	m_editNameCleared = false;
}

//================================================//

void GUINewGameLayer::notifyMouseButtonClick(MyGUI::WidgetPtr _sender)
{
	if(_sender == this->getWidgetPtr("Button_Start")){
		*m_eventId = EVENT_START;
	}
}

//================================================//

void GUINewGameLayer::notifyMouseFocusSet(MyGUI::WidgetPtr _sender, MyGUI::WidgetPtr _old)
{
	if(_sender == this->getWidgetPtr("EditBox_Name")){
		
		if(!m_editNameCleared){
			static_cast<MyGUI::EditPtr>(_sender)->setCaption("");
			m_editNameCleared = true;
		}
	}
}

//================================================//

}

//================================================//