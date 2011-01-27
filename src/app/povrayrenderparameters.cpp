// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006 Park "segfault" J. K. <mastermind@planetmono.org>

#include "povrayrenderparameters.h"

namespace Konstruktor
{

// TODO copy all props

POVRayRenderParameters::POVRayRenderParameters()
{
	width_ = 1024;
	height_ = 768;
	antialiasing_ = true;

	seamWidth_ = 0.8f;
	
	backgroundColor_ = QColor(32, 32, 32);
	
	drawPlane_ = true;
	planePositionAuto_ = true;
	planePosition_ = 0.0f;
	planeColor_ = QColor(165, 165, 165);
	
	latitude_ = 135.0f;
	longitude_ = 30.0f;
	radius_ = 1.0f;
	fov_ = 60.0f;
	orthographic_ = false;
	
	lights_ = 1;
	lightsLongitude_ = 60.0f;
	lightsLatitudeOffset_ = 180.0f - 22.5f;
	lightsRadius_ = 2.0f;
	lightsIntensity_ = 1.0f;
	lightsColor_ = QColor(255, 255, 255);
}

POVRayRenderParameters::POVRayRenderParameters(const POVRayRenderParameters &p)
{
	width_ = p.width();
	height_ = p.height();
	antialiasing_ = p.antialiasing();
}

POVRayRenderParameters POVRayRenderParameters::operator= (const POVRayRenderParameters &p) const
{
	POVRayRenderParameters n;
	
	n.setWidth(p.width());
	n.setHeight(p.height());
	n.setAntialiasing(p.antialiasing());
	
	return n;
}

}
