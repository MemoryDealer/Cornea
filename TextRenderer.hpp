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

	void addText(Text* text);
	void removeText(Text* text);

	void setText(const int index, const std::string& text);

private:
	Ogre::OverlayManager*	m_pOverlayMgr;
	Ogre::Overlay*			m_pOverlay;
	Ogre::OverlayContainer* m_panel;

	std::vector<Text*>		m_texts;
	int						m_numTexts;
};

//================================================//


//================================================//
//================================================//

class Text
{
public:
	Text(void);
	~Text(void);

	std::string		text;
	std::string		id;
	std::string		font, size;
	Ogre::Real		timeout;
	Ogre::Real		x, y;
	Ogre::Real		width, height;
	Ogre::ColourValue colour;

	Ogre::OverlayElement* element;

private:
	
};

//================================================//

#endif

//================================================//