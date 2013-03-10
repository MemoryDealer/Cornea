//================================================//

#ifndef __COMPOSITOR_HPP__
#define __COMPOSITOR_HPP__

//================================================//

#include "stdafx.h"
#include "HelperLogic.hpp"

//================================================//

#define COMPOSITOR_MOTION_BLUR		"MotionBlur"
#define COMPOSITOR_INVERT			"Invert"
#define COMPOSITOR_NIGHT_VISION		"NightVision"
#define COMPOSITOR_BLOOM			"Bloom"
#define COMPOSITOR_GLASS			"Glass"
#define COMPOSITOR_OLD_TV			"Old TV"
#define COMPOSITOR_BW				"B&W"
#define COMPOSITOR_EMBOSSED			"Embossed"
#define COMPOSITOR_SHARPEN_EDGES	"SharpenEdges"
#define COMPOSITOR_POSTERIZE		"Posterize"
#define COMPOSITOR_LAPLACE			"Laplace"
#define COMPOSITOR_TILING			"Tiling"
#define COMPOSITOR_OLD_MOVIE		"Old Movie"
#define COMPOSITOR_HDR				"HDR"
#define COMPOSITOR_GAUSSIAN_BLUR	"GaussianBlur"
#define COMPOSITOR_TESTMRT			"TestMRT"
#define COMPOSITOR_RADIAL_BLUR		"Radial Blur"
#define COMPOSITOR_ASCII			"ASCII"
#define COMPOSITOR_HALFTONE			"Halftone"
#define COMPOSITOR_DITHER			"Dither"
#define COMPOSITOR_HEAT_VISION		"HeatVision"

//================================================//

namespace Sparks
{

//================================================//

class Compositor
{
public:
	Compositor(void);
	Compositor(Ogre::SceneManager* mgr, Ogre::Viewport* vp);
	~Compositor(void);

	void setup(void);
	void setEnabled(const Ogre::String& name, bool enabled);

protected:
	void registerCompositors(void);
	void createEffects(void);

	// --- //

	Ogre::SceneManager* m_pSceneMgr;
	Ogre::Viewport*		m_pViewport;

	typedef std::map<Ogre::String, Ogre::CompositorLogic*> CompositorLogicMap; // ::type?
	CompositorLogicMap m_compositorLogics;
	Ogre::StringVector m_compositorNames;

private:

};

//================================================//

}

//================================================//

#endif

//================================================//