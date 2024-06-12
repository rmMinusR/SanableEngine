#include "TypeLayoutView.hpp"

#include <sstream>

#include "ShaderProgram.hpp"
#include "Material.hpp"
#include "gui/HUD.hpp"
#include "gui/ImageWidget.hpp"
#include "gui/LabelWidget.hpp"

#include "Resources.hpp"

void TypeInfoView::refresh()
{
	//Remove existing
	for (int i = getTransform()->getChildrenCount() - 1; i >= 0; --i)
	{
		hud->removeWidget(getTransform()->getChild(i)->getWidget());
	}
	
	//Repopulate
	target->layout.walkFields(
		[&](const FieldInfo& f)
		{
			size_t fieldHash = std::hash<std::string>{}(f.name) ^ std::hash<TypeName>{}(f.type) ^ std::hash<TypeName>{}(f.owner);
			float hue = (fieldHash & 0xffffULL)/float(0xffffULL);
			SDL_Color color {
				uint8_t(255*( 1-1.5f*std::min(abs(hue), abs(hue-1)) )),
				uint8_t(255*( 1-1.5f*abs(hue-0.33f) )),
				uint8_t(255*( 1-1.5f*abs(hue-0.66f) )),
				255
			};

			size_t cursor = f.offset;
			size_t end = f.offset+f.size;
			size_t nLines = 0;
			while (true)
			{
				size_t lineEnd = std::min(end, (cursor/bytesPerColumn+1)*bytesPerColumn);
				nLines++;

				ImageWidget* w = hud->addWidget<ImageWidget>(imageMat, fieldSprite);
				w->getTransform()->setParent(getTransform());
				w->setTintColor(color.r, color.g, color.b, color.a);
				AnchoredPositioning* p = w->getTransform()->setPositioningStrategy<AnchoredPositioning>();
				p->snapToCorner( Vector2f(0,0), Vector2f(byteSize.x*(lineEnd-cursor), byteSize.y) );
				p->setCenterByOffsets(byteSize*Vector2f(cursor%bytesPerColumn, cursor/bytesPerColumn), Vector2f(0, 0));
				//p->setLocalRectByOffsets(
				//	Rect<float> {
				//		byteSize*Vector2f(cursor%bytesPerColumn, cursor/bytesPerColumn),
				//		Vector2f(byteSize.x*(lineEnd-cursor), byteSize.y)
				//	},
				//	getTransform()
				//);

				cursor = lineEnd;
				if (cursor >= end) break;
			}
			
			//Add label
			LabelWidget* lbl = hud->addWidget<LabelWidget>(textMat, textFont);
			{
				std::stringstream ss;
				ss << f.name << " (" << f.type.as_str() << ")";
				lbl->setText(ss.str());
			}
			
			//Simple fill to first line
			lbl->getTransform()->setParent( getTransform()->getChild(getTransform()->getChildrenCount()-nLines) );
			lbl->getTransform()->setPositioningStrategy<AnchoredPositioning>()->fillParent();
			lbl->getTransform()->setRelativeRenderDepth(1);
		},
		MemberVisibility::All,
		true
	);
}

TypeInfoView::TypeInfoView(HUD* hud, const TypeInfo* target, const Material* imageMat, const UISprite* fieldSprite, const UISprite* parentSprite, const Material* textMat, const Font* textFont) :
	Widget(hud),
	target(target),
	imageMat(imageMat),
	fieldSprite(fieldSprite),
	parentSprite(parentSprite),
	textMat(textMat),
	textFont(textFont)
{
	refresh();
}

TypeInfoView::~TypeInfoView()
{
}

const Material* TypeInfoView::getMaterial() const
{
	return nullptr;
}

void TypeInfoView::renderImmediate(Renderer* target)
{
}
