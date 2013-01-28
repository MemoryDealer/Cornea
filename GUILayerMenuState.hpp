//================================================//

#include "GUILayer.hpp"

//================================================//

namespace GUIMenuState
{

//================================================//

class GUIRootLayer : public GUILayer
{
public:
	GUIRootLayer(int* eventId) : GUILayer(eventId){}

	void create(void);

	// --- //

	static const Ogre::String	BUTTON_RESUMEGAME;
	static const Ogre::String	BUTTON_NEWGAME;
	static const Ogre::String	BUTTON_LOADGAME;
	static const Ogre::String	BUTTON_OPTIONS;
	static const Ogre::String	BUTTON_CREDITS;
	static const Ogre::String	BUTTON_EXITGAME;

	enum{
		EVENT_RESUMEGAME = 0,
		EVENT_NEWGAME,
		EVENT_LOADGAME,
		EVENT_OPTIONS,
		EVENT_CREDITS,
		EVENT_EXITGAME,
		EVENT_NULL
	};

protected:

	// Event functions
	void notifyMouseButtonClick(MyGUI::WidgetPtr _sender);
};

//================================================//
//================================================//

class GUINewGameLayer : public GUILayer
{
public:
	GUINewGameLayer(int* eventId) : GUILayer(eventId){}

	void create(void);
	void resetWidgets(void);

	// --- //

	static const Ogre::String	EDITBOX_NAME;
	static const Ogre::String	BUTTON_START;

	enum{
		EVENT_START = GUIRootLayer::EVENT_NULL + 1, // Start at last one plus one
		EVENT_NULL
	};

protected:
	bool	m_editNameCleared;

	// Event functions
	void notifyMouseButtonClick(MyGUI::WidgetPtr _sender);
	void notifyMouseFocusSet(MyGUI::WidgetPtr _sender, MyGUI::WidgetPtr _old);
};

//================================================//
//================================================//

class GUILoadGameLayer : public GUILayer
{
public:
	GUILoadGameLayer(int* eventId) : GUILayer(eventId){}

	void create(void);
	void resetWidgets(void);

	// --- //

	static const Ogre::String	STATICTEXT_CHOOSE;
	static const Ogre::String	LISTBOX_SAVES;
	static const Ogre::String	BUTTON_LOAD;

	enum{
		EVENT_LOAD = GUINewGameLayer::EVENT_NULL + 1,
		EVENT_NULL
	};

protected:

	// Event functions
	void notifyMouseButtonClick(MyGUI::WidgetPtr _sender);
	void notifyListSelectAccept(MyGUI::ListPtr _sender, size_t index);
};

//================================================//
//================================================//

//================================================//

} // namespace

//================================================//