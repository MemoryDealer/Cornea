//================================================//

#ifndef __TEXTRENDERER_HPP__
#define __TEXTRENDERER_HPP__

//================================================//

#include "stdafx.h"
#include "Base.hpp"

//================================================//

class Text;

//================================================//

class TextRenderer : public Ogre::Singleton<TextRenderer>
{
public:
	TextRenderer(void);
	~TextRenderer(void);

	enum{
		UPPER = 0,
		LOWER,

		FILLER
	};

	void setText(int position, Text* text);
	void clearText(int position);

	void update(double timeSinceLastFrame);

private:
	Ogre::OverlayManager*	m_pOverlayMgr;
	Ogre::Overlay*			m_pOverlay;
	Ogre::OverlayContainer* m_panel;

	Text*					m_fillerText;
	Text*					m_upperText;
	Text*					m_lowerText;
};

//================================================//


//================================================//
//================================================//

class Text
{
public:
	Text(void);
	~Text(void);

	enum{
		STATIC = 0,
		FADE_SINE
	};

	// --- //

	std::string		text;
	std::string		id;
	std::string		font, size;
	Ogre::Real		timeout;
	Ogre::ColourValue colour;
	int				style;
	long double		sin;

	Ogre::OverlayElement* element;
	Ogre::Timer*			timer;

	void update(double timeSinceLastFrame);

private:
	
};

//================================================//

#endif

//================================================//