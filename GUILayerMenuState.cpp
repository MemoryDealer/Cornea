//================================================//

#include "GUILayerMenuState.hpp"

//================================================//

namespace GUIMenuState
{

//================================================//

// Define constant widget names
const Ogre::String GUIRootLayer::BUTTON_RESUMEGAME		= "Button_ResumeGame";
const Ogre::String GUIRootLayer::BUTTON_NEWGAME			= "Button_NewGame";
const Ogre::String GUIRootLayer::BUTTON_LOADGAME		= "Button_LoadGame";
const Ogre::String GUIRootLayer::BUTTON_OPTIONS			= "Button_Options";
const Ogre::String GUIRootLayer::BUTTON_CREDITS			= "Button_Credits";
const Ogre::String GUIRootLayer::BUTTON_EXITGAME		= "Button_ExitGame";

//================================================//

void GUIRootLayer::create(void)
{
	int buttonWidth = 300, buttonHeight = 26;
	int screenWidth = Base::getSingletonPtr()->m_pViewport->getActualWidth();
	int screenHeight = Base::getSingletonPtr()->m_pViewport->getActualHeight();

	// Resume Game button
	MyGUI::ButtonPtr buttonResumeGame = Base::getSingletonPtr()->m_GUI->createWidget<MyGUI::Button>("Button", 
		(screenWidth / 2) - (buttonWidth / 2), (screenHeight / 2) - (buttonHeight / 2) - 125, buttonWidth, buttonHeight, MyGUI::Align::Default, "Main", BUTTON_RESUMEGAME);
	buttonResumeGame->setCaption("Resume Game");
	buttonResumeGame->eventMouseButtonClick += MyGUI::newDelegate(this, &GUIRootLayer::notifyMouseButtonClick);
	m_widgets.push_back(buttonResumeGame);

	// New Game button
	MyGUI::ButtonPtr buttonNewGame = Base::getSingletonPtr()->m_GUI->createWidget<MyGUI::Button>("Button",
		(screenWidth / 2) - (buttonWidth / 2), (screenHeight / 2) - (buttonHeight / 2) - 75, buttonWidth, buttonHeight, MyGUI::Align::Default, "Main", BUTTON_NEWGAME);
	buttonNewGame->setCaption("New Game");
	buttonNewGame->eventMouseButtonClick += MyGUI::newDelegate(this, &GUIRootLayer::notifyMouseButtonClick);
	m_widgets.push_back(buttonNewGame);

	// Load Game button
	MyGUI::ButtonPtr buttonLoadGame = Base::getSingletonPtr()->m_GUI->createWidget<MyGUI::Button>("Button",
		(screenWidth / 2) - (buttonWidth / 2), (screenHeight / 2) - (buttonHeight / 2) - 25, buttonWidth, buttonHeight, MyGUI::Align::Default, "Main", BUTTON_LOADGAME);
	buttonLoadGame->setCaption("Load Game");
	buttonLoadGame->eventMouseButtonClick += MyGUI::newDelegate(this, &GUIRootLayer::notifyMouseButtonClick);
	m_widgets.push_back(buttonLoadGame);

	// Options button
	MyGUI::ButtonPtr buttonOptions = Base::getSingletonPtr()->m_GUI->createWidget<MyGUI::Button>("Button",
		(screenWidth / 2) - (buttonWidth / 2), (screenHeight / 2) - (buttonHeight / 2) + 25, buttonWidth, buttonHeight, MyGUI::Align::Default, "Main", BUTTON_OPTIONS);
	buttonOptions->setCaption("Options");
	buttonOptions->eventMouseButtonClick += MyGUI::newDelegate(this, &GUIRootLayer::notifyMouseButtonClick);
	m_widgets.push_back(buttonOptions);

	// Credits button
	MyGUI::ButtonPtr buttonCredits = Base::getSingletonPtr()->m_GUI->createWidget<MyGUI::Button>("Button",
		(screenWidth / 2) - (buttonWidth / 2), (screenHeight / 2) - (buttonHeight / 2) + 75, buttonWidth, buttonHeight, MyGUI::Align::Default, "Main", BUTTON_CREDITS);
	buttonCredits->setCaption("Credits");
	buttonCredits->eventMouseButtonClick += MyGUI::newDelegate(this, &GUIRootLayer::notifyMouseButtonClick);
	m_widgets.push_back(buttonCredits);

	// Exit Game button
	MyGUI::ButtonPtr buttonExitGame = Base::getSingletonPtr()->m_GUI->createWidget<MyGUI::Button>("Button",
		(screenWidth / 2) - (buttonWidth / 2), (screenHeight / 2) - (buttonHeight / 2) + 125, buttonWidth, buttonHeight, MyGUI::Align::Default, "Main", BUTTON_EXITGAME);
	buttonExitGame->setCaption("Exit Game");
	buttonExitGame->eventMouseButtonClick += MyGUI::newDelegate(this, &GUIRootLayer::notifyMouseButtonClick);
	m_widgets.push_back(buttonExitGame);
}

//================================================//

void GUIRootLayer::notifyMouseButtonClick(MyGUI::WidgetPtr _sender)
{
	if(_sender->getName() == BUTTON_RESUMEGAME){
		*m_eventId = EVENT_RESUMEGAME;
	}
	else if(_sender->getName() == BUTTON_NEWGAME){
		*m_eventId = EVENT_NEWGAME;
	}
	else if(_sender->getName() == BUTTON_LOADGAME){
		*m_eventId = EVENT_LOADGAME;
	}
	else if(_sender->getName() == BUTTON_OPTIONS){
		*m_eventId = EVENT_OPTIONS;
	}
	else if(_sender->getName() == BUTTON_CREDITS){
		*m_eventId = EVENT_CREDITS;
	}
	else if(_sender->getName() == BUTTON_EXITGAME){
		*m_eventId = EVENT_EXITGAME;
	}
}

//================================================//
//================================================//

const Ogre::String GUINewGameLayer::EDITBOX_NAME		= "EditBox_Name";
const Ogre::String GUINewGameLayer::BUTTON_START		= "Button_Start";

//================================================//

void GUINewGameLayer::create(void)
{
	int buttonWidth = 300, buttonHeight = 26;
	int screenWidth = Base::getSingletonPtr()->m_pViewport->getActualWidth();
	int screenHeight = Base::getSingletonPtr()->m_pViewport->getActualHeight();

	// Name edit box
	MyGUI::EditPtr editName = Base::getSingletonPtr()->m_GUI->createWidget<MyGUI::EditBox>("EditBox", 
		(screenWidth / 2) - (buttonWidth / 2), (screenHeight / 2) - (buttonHeight / 2) - 25, buttonWidth, buttonHeight, MyGUI::Align::Default, "Main", EDITBOX_NAME);
	editName->eventKeySetFocus += MyGUI::newDelegate(this, &GUINewGameLayer::notifyMouseFocusSet);
	m_widgets.push_back(editName);

	// Start button
	MyGUI::ButtonPtr buttonStart = Base::getSingletonPtr()->m_GUI->createWidget<MyGUI::Button>("Button",
		(screenWidth / 2) - (buttonWidth / 2), (screenHeight / 2) - (buttonHeight / 2) + 25, buttonWidth, buttonHeight, MyGUI::Align::Default, "Main", BUTTON_START);
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
	static_cast<MyGUI::EditPtr>(this->getWidgetPtr(EDITBOX_NAME))->setCaption("Enter Your Name...");
	m_editNameCleared = false;
}

//================================================//

void GUINewGameLayer::notifyMouseButtonClick(MyGUI::WidgetPtr _sender)
{
	if(_sender->getName() == BUTTON_START){
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
//================================================//

const Ogre::String GUILoadGameLayer::STATICTEXT_CHOOSE	= "StaticText_Choose";
const Ogre::String GUILoadGameLayer::LISTBOX_SAVES		= "ListBox_Saves";
const Ogre::String GUILoadGameLayer::BUTTON_LOAD		= "Button_Load";

//================================================//

void GUILoadGameLayer::create(void)
{
	int buttonWidth = 300, buttonHeight = 26;
	int listWidth = 300, listHeight = 150;
	int screenWidth = Base::getSingletonPtr()->m_pViewport->getActualWidth();
	int screenHeight = Base::getSingletonPtr()->m_pViewport->getActualHeight();

	// Choose static text
	MyGUI::StaticTextPtr textChoose = Base::getSingletonPtr()->m_GUI->createWidget<MyGUI::StaticText>("StaticText",
		(screenWidth / 2) - (listWidth / 2), (screenHeight / 2) - (listHeight / 2) - 150, listWidth, 26, MyGUI::Align::Default, "Main", STATICTEXT_CHOOSE);
	textChoose->setCaption("Choose a saved game:");
	textChoose->setColour(MyGUI::Colour(1.0, 1.0, 1.0));
	textChoose->setTextColour(MyGUI::Colour(1.0, 1.0, 1.0));
	m_widgets.push_back(textChoose);

	// Saves listbox
	MyGUI::ListPtr listBox = Base::getSingletonPtr()->m_GUI->createWidget<MyGUI::ListBox>("ListBox", 
		(screenWidth / 2) - (listWidth / 2), (screenHeight / 2) - (listHeight / 2) - 100, listWidth, listHeight, MyGUI::Align::Default, "Main", LISTBOX_SAVES);
	listBox->eventListSelectAccept += MyGUI::newDelegate(this, &GUILoadGameLayer::notifyListSelectAccept);
	m_widgets.push_back(listBox);

	// Load button
	MyGUI::ButtonPtr buttonLoad = Base::getSingletonPtr()->m_GUI->createWidget<MyGUI::Button>("Button",
		(screenWidth / 2) - (buttonWidth / 2), (screenHeight / 2) - (buttonHeight / 2) + 50, buttonWidth, buttonHeight, MyGUI::Align::Default, "Main", BUTTON_LOAD);
	buttonLoad->setCaption("Start Game");
	buttonLoad->eventMouseButtonClick += MyGUI::newDelegate(this, &GUILoadGameLayer::notifyMouseButtonClick);
	m_widgets.push_back(buttonLoad);
	
	this->setVisible(false);
}

//================================================//

void GUILoadGameLayer::resetWidgets(void)
{
	// Add save games to the listbox
	MyGUI::ListPtr listBox = static_cast<MyGUI::ListPtr>(this->getWidgetPtr(LISTBOX_SAVES));
	listBox->deleteAllItems();
	
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	WIN32_FIND_DATA		fd		= {0};
	HANDLE				hFind	= NULL;

	// TODO: Look in "Saves" directory
	hFind = FindFirstFile("*.save", &fd);
	do{
		// Remove extension
		char* p = fd.cFileName;
		
		listBox->addItem(MyGUI::UString(fd.cFileName));
	} while(FindNextFile(hFind, &fd));

	FindClose(hFind);
#endif
}

//================================================//

void GUILoadGameLayer::notifyMouseButtonClick(MyGUI::WidgetPtr _sender)
{
	if(_sender->getName() == BUTTON_LOAD){
		*m_eventId = EVENT_LOAD;
	}
}

//================================================//

void GUILoadGameLayer::notifyListSelectAccept(MyGUI::ListPtr _sender, size_t index)
{
	if(_sender->getName() == LISTBOX_SAVES){
		*m_eventId = EVENT_LOAD;
	}
}

//================================================//

} // namespace

//================================================//