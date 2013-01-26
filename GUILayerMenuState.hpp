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

	enum{
		EVENT_NEWGAME = 1,
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

} // namespace

//================================================//