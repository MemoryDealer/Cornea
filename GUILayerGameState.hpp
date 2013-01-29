//================================================//

#ifndef __GUILAYERGAMESTATE_HPP__
#define __GUILAYERGAMESTATE_HPP__

//================================================//

#include "GUILayer.hpp"

//================================================//

namespace GUIGameState
{

//================================================//

class GUIHudLayer : public GUILayer
{
public:
	GUIHudLayer(int* eventId) : GUILayer(eventId){}

	void create(void);

	// Setter functions


	// --- //

	static const Ogre::String BUTTON_INFO;

	static const Ogre::String OVERLAY_RETICULE;

protected:
	// Overlay elements
	Ogre::OverlayContainer*		m_overlayReticule;
};

//================================================//

} // namespace

//================================================//

#endif

//================================================//