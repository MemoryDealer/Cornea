//================================================//

#include "GUILayer.hpp"

//================================================//

GUILayer::GUILayer(int* eventId)
{
	m_eventId = eventId;
	m_useOverlay = false;
}

//================================================//

GUILayer::~GUILayer(void)
{

}

//================================================//

void GUILayer::destroy(void)
{
	// MyGUI widgets
	for(std::vector<MyGUI::WidgetPtr>::iterator itr = m_widgets.begin();
		itr != m_widgets.end();
		){
		(*itr)->detachFromLayer();
		Base::getSingletonPtr()->m_GUI->destroyWidget((*itr));
		itr = m_widgets.erase(itr);
	}

	// Ogre overlay
	if(m_useOverlay){
		for(std::vector<Ogre::OverlayContainer*>::iterator itr = m_overlayContainers.begin();
			itr != m_overlayContainers.end();
			++itr){
			m_overlay->remove2D(*itr);
			Ogre::OverlayManager::getSingletonPtr()->destroyOverlayElement(*itr);
		}

		Ogre::OverlayManager::getSingletonPtr()->destroy(m_overlay);
	}
}

//================================================//

void GUILayer::setVisible(bool visible)
{
	for(std::vector<MyGUI::WidgetPtr>::iterator itr = m_widgets.begin();
		itr != m_widgets.end();
		++itr){
		(*itr)->setVisible(visible);
	}

	if(visible){
		this->resetWidgets();
	}
}

//================================================//

void GUILayer::setOverlayContainerMaterialName(Ogre::String container, Ogre::String material)
{
	this->getOverlayContainer(container)->setMaterialName(material);
}

//================================================//

Ogre::String GUILayer::getWidgetText(std::string name)
{
	std::string data;
	Ogre::StringUtil strUtil;

	if(strUtil.startsWith(name, "Button_", false)){
		data = static_cast<MyGUI::ButtonPtr>(this->getWidgetPtr(name))->getCaption();
	}
	else if(strUtil.startsWith(name, "EditBox_", false)){
		data = static_cast<MyGUI::EditPtr>(this->getWidgetPtr(name))->getCaption();
	}

	
	return Ogre::String(data);
}

//================================================//

Ogre::String GUILayer::getListBoxSelectedText(std::string listBoxName)
{
	MyGUI::ListPtr listBox = static_cast<MyGUI::ListPtr>(this->getWidgetPtr(listBoxName));
	size_t index = listBox->getIndexSelected();

	if(index != MyGUI::ITEM_NONE){
		std::string data;

		data = listBox->getItemNameAt(index);

		return Ogre::String(data);
	}
	
	return Ogre::String("None");
}

//================================================//

MyGUI::WidgetPtr GUILayer::getWidgetPtr(std::string name)
{
	for(std::vector<MyGUI::WidgetPtr>::iterator itr = m_widgets.begin();
		itr != m_widgets.end();
		++itr){
		if((*itr)->getName() == name){
			return *itr;
		}
	}

	return nullptr; 
}

//================================================//

Ogre::OverlayContainer* GUILayer::getOverlayContainer(Ogre::String name)
{
	Ogre::StringUtil strUtil;

	for(std::vector<Ogre::OverlayContainer*>::iterator itr = m_overlayContainers.begin();
		itr != m_overlayContainers.end();
		++itr){
		if(strUtil.match((*itr)->getName(), name, true)){
			return *itr;
		}
	}

	return nullptr;
}

//================================================//