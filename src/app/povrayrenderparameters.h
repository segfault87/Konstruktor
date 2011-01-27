// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _POVRAYRENDERPARAMETERS_H_
#define _POVRAYRENDERPARAMETERS_H_

#include <QColor>

namespace Konstruktor
{

class POVRayRenderParameters
{
  public:
	POVRayRenderParameters();
	POVRayRenderParameters(const POVRayRenderParameters &p);
	~POVRayRenderParameters() {}
	
	/*
	 * Output format
	 */
	void setWidth(int p) { width_ = p; }
	void setHeight(int p) { height_ = p; }
	void setAntialiasing(bool p) { antialiasing_ = p; }
	
	int width() const { return width_; }
	int height() const { return height_; }
	bool antialiasing() const { return antialiasing_; }
	
	/*
	 * Rendering options
	 */

	void setSeamWidth(float f) { seamWidth_ = f; }
	void setBackgroundColor(QColor p) { backgroundColor_ = p; }
	void setDrawPlane(bool p) { drawPlane_ = p; }
	void setPlanePositionAuto(bool p) { planePositionAuto_ = p; }
	void setPlanePosition(float p) { planePosition_ = p; }
	void setPlaneColor(const QColor &p) { planeColor_ = p; }
	void setCameraLattitude(float p) { latitude_ = p; }
	void setCameraLongitude(float p) { longitude_ = p; }
	void setCameraRadius(float p) { radius_ = p; }
	void setCameraFOV(float p) { fov_ = p; }
	void setCameraOrthographic(bool p) { orthographic_ = p; }
	void setLights(int p) { lights_ = p; }
	void setLightsLongitude(float p) { lightsLongitude_ = p; }
	void setLightsLatitudeOffset(float p) { lightsLatitudeOffset_ = p; }
	void setLightsRadius(float p) { lightsRadius_ = p; }
	void setLightsIntensity(float p) { lightsIntensity_ = p; }
	void setLightsColor(QColor p) { lightsColor_ = p; }

	float seamWidth() const { return seamWidth_; }
	QColor backgroundColor() const { return backgroundColor_; }
	bool drawPlane() const { return drawPlane_; }
	bool planePositionAuto() const { return planePositionAuto_; }
	float planePosition() const { return planePosition_; }
	QColor planeColor() const { return planeColor_; }
	float cameraLatitude() const { return latitude_; }
	float cameraLongitude() const { return longitude_; }
	float cameraRadius() const { return radius_; }
	float cameraFOV() const { return fov_; }
	bool cameraOrthographic() const { return orthographic_; }
	int lights() const { return lights_; }
	float lightsLongitude() const { return lightsLongitude_; }
	float lightsLatitudeOffset() const { return lightsLatitudeOffset_; }
	float lightsRadius() const { return lightsRadius_; }
	float lightsIntensity() const { return lightsIntensity_; }
	QColor lightsColor() const { return lightsColor_; }
	
	POVRayRenderParameters operator= (const POVRayRenderParameters &p) const;
	
  private:
	int width_;
	int height_;
	bool antialiasing_;

	float seamWidth_;
	QColor backgroundColor_;
	bool drawPlane_;
	bool planePositionAuto_;
	float planePosition_;
	QColor planeColor_;
	float latitude_;
	float longitude_;
	float radius_;
	float fov_;
	bool orthographic_;
	int lights_;
	float lightsLongitude_;
	float lightsLatitudeOffset_;
	float lightsRadius_;
	float lightsIntensity_;
	QColor lightsColor_;
};

}

#endif
