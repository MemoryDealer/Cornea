//================================================//

#include "GUILayer.hpp"

//================================================//

GUILayer::GUILayer(int* eventId)
{
	m_eventId = eventId;
}

//================================================//

GUILayer::~GUILayer(void)
{

}

//================================================//

void GUILayer::destroy(void)
{
	for(std::vector<MyGUI::WidgetPtr>::iterator itr = m_widgets.begin();
		itr != m_widgets.end();
		){
		(*itr)->detachFromLayer();
		Base::getSingletonPtr()->m_GUI->destroyWidget((*itr));
		itr = m_widgets.erase(itr);
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