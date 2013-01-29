//================================================//

#ifndef __GUILAYER_HPP__
#define __GUILAYER_HPP__

//================================================//

#include "stdafx.h"
#include "Base.hpp"

//================================================//

/* Abstract class for all GUI layers */
class GUILayer
{
public:
	GUILayer(int* eventId);
	~GUILayer(void);

	virtual void create(void) = 0;  // Create all widgets in layer
	virtual void destroy(void);		// Destroy all widgets in layer
	virtual void resetWidgets(void){}

	void setVisible(bool visible);
	void setOverlayContainerMaterialName(Ogre::String container, Ogre::String material);

	// Some functions to retrieve GUI data
	Ogre::String getWidgetText(std::string name);
	Ogre::String getListBoxSelectedText(std::string listBoxName);

	MyGUI::WidgetPtr getWidgetPtr(std::string name);
	Ogre::OverlayContainer* getOverlayContainer(Ogre::String name);

protected:
	int*									m_eventId;

	std::vector<MyGUI::WidgetPtr>			m_widgets;
	std::vector<Ogre::OverlayContainer*>	m_overlayContainers;
	Ogre::Overlay*							m_overlay;
	bool									m_useOverlay;
};

//================================================//

#endif

//================================================//