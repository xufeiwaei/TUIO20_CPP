/*
 TUIO2 C++ Library
 Copyright (c) 2009-2014 Martin Kaltenbrunner <martin@tuio.org>
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 3.0 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library.
*/

#include "TuioComponent.h"
#include "TuioObject.h"
using namespace TUIO2;

TuioComponent::TuioComponent (TuioTime ttime, int si, float xp, float yp, float a):TuioPoint(ttime, xp,yp)
,state(TUIO_ADDED)
{
	session_id = si;
    angle = a;
	x_speed = 0.0f;
	y_speed = 0.0f;
	motion_speed = 0.0f;
	motion_accel = 0.0f;
    rotation_speed = 0.0f;
    rotation_accel = 0.0f;
	TuioPoint p(currentTime,xpos,ypos);
	path.push_back(p);
 }

TuioComponent::TuioComponent (int si, float xp, float yp, float a):TuioPoint(xp,yp)
,state(TUIO_ADDED)
{
	session_id = si;
    angle = a;
	x_speed = 0.0f;
	y_speed = 0.0f;
	motion_speed = 0.0f;
	motion_accel = 0.0f;
    rotation_speed = 0.0f;
    rotation_accel = 0.0f;
	TuioPoint p(currentTime,xpos,ypos);
	path.push_back(p);
}

TuioComponent::TuioComponent (TuioComponent *tcon):TuioPoint(tcon)
,state(TUIO_ADDED)		
{
	session_id = tcon->getSessionID();
    angle = tcon->getAngle();
	x_speed = 0.0f;
	y_speed = 0.0f;
	motion_speed = 0.0f;
	motion_accel = 0.0f;
    rotation_speed = 0.0f;
    rotation_accel = 0.0f;
	TuioPoint p(currentTime,xpos,ypos);
	path.push_back(p);
}

void TuioComponent::update (TuioTime ttime, float xp, float yp, float a) {
	TuioPoint lastPoint = path.back();
	TuioPoint::update(ttime,xp, yp);
	
	TuioTime diffTime = currentTime - lastPoint.getTuioTime();
	float dt = diffTime.getTotalMilliseconds()/1000.0f;
	float dx = xpos - lastPoint.getX();
	float dy = ypos - lastPoint.getY();
	float dist = sqrt(dx*dx+dy*dy);
	float last_motion_speed = motion_speed;
	
	x_speed = dx/dt;
	y_speed = dy/dt;
	motion_speed = dist/dt;
	motion_accel = (motion_speed - last_motion_speed)/dt;
	
    float last_angle = angle;
    float last_rotation_speed = rotation_speed;
    angle = a;
    
    double da = (angle-last_angle)/(2*M_PI);
    if (da > 0.75f) da-=1.0f;
    else if (da < -0.75f) da+=1.0f;
    
    rotation_speed = (float)da/dt;
    rotation_accel =  (rotation_speed - last_rotation_speed)/dt;
    
	TuioPoint p(currentTime,xpos,ypos);
	path.push_back(p);
	
	if (motion_accel>0) state = TUIO_ACCELERATING;
	else if (motion_accel<0) state = TUIO_DECELERATING;
    else if ((rotation_accel!=0) && (state==TUIO_STOPPED)) state = TUIO_ROTATING;
	else state = TUIO_STOPPED;
}

void TuioComponent::stop(TuioTime ttime) {
	update(ttime,xpos,ypos);
}

void TuioComponent::update (TuioTime ttime, float xp, float yp, float a, float xs, float ys, float rs, float ma, float ra) {
	TuioPoint::update(ttime,xp, yp);
    angle = a;
	x_speed = xs;
	y_speed = ys;
	motion_speed = (float)sqrt(x_speed*x_speed+y_speed*y_speed);
    rotation_speed = rs;
	motion_accel = ma;
    rotation_accel = ra;
	
	TuioPoint p(currentTime,xpos,ypos);
	path.push_back(p);
	
	if (motion_accel>0) state = TUIO_ACCELERATING;
	else if (motion_accel<0) state = TUIO_DECELERATING;
    else if ((rotation_accel!=0) && (state==TUIO_STOPPED)) state = TUIO_ROTATING;
    else state = TUIO_STOPPED;
 }

void TuioComponent::update (float xp, float yp, float a, float xs, float ys, float rs, float ma, float ra) {
	TuioPoint::update(xp,yp);
    angle = a;
	x_speed = xs;
	y_speed = ys;
	motion_speed = (float)sqrt(x_speed*x_speed+y_speed*y_speed);
    rotation_speed = rs;
	motion_accel = ma;
    rotation_accel = ra;
	
	//path.pop_back();
	TuioPoint p(currentTime,xpos,ypos);
	path.push_back(p);
	
    if (motion_accel>0) state = TUIO_ACCELERATING;
    else if (motion_accel<0) state = TUIO_DECELERATING;
    else if ((rotation_accel!=0) && (state==TUIO_STOPPED)) state = TUIO_ROTATING;
    else state = TUIO_STOPPED;
}

void TuioComponent::update (TuioComponent *tcon) {
	TuioPoint::update(tcon);
    angle = tcon->getAngle();
	x_speed = tcon->getXSpeed();
	y_speed =  tcon->getYSpeed();
	motion_speed =  tcon->getMotionSpeed();
    rotation_speed = tcon->getRotationSpeed();
	motion_accel = tcon->getMotionAccel();
    rotation_accel = tcon->getRotationAccel();
	
	TuioPoint p(tcon->getTuioTime(),xpos,ypos);
	path.push_back(p);
	
	if (motion_accel>0) state = TUIO_ACCELERATING;
	else if (motion_accel<0) state = TUIO_DECELERATING;
    else if ((rotation_accel!=0) && (state==TUIO_STOPPED)) state = TUIO_ROTATING;
	else state = TUIO_STOPPED;
}

void TuioComponent::remove(TuioTime ttime) {
	currentTime = ttime;
	state = TUIO_REMOVED;
}

int TuioComponent::getSessionID() const{
    return session_id;
}

void TuioComponent::setSessionID(int si) {
	session_id = si;
}

float TuioComponent::getAngle() const{
    return angle;
}

float TuioComponent::getAngleDegrees() const{
    return (float)(angle/M_PI*180);
}

float TuioComponent::getXSpeed() const{ 
	return x_speed;
}

float TuioComponent::getYSpeed() const{ 
	return y_speed;
}

TuioPoint TuioComponent::getPosition() const{
	TuioPoint p(xpos,ypos);
	return p;
}

std::list<TuioPoint> TuioComponent::getPath() const{
	return path;
}

float TuioComponent::getMotionSpeed() const{
	return motion_speed;
}

float TuioComponent::getMotionAccel() const{
	return motion_accel;
}

float TuioComponent::getRotationSpeed() const{
    return rotation_speed;
}

float TuioComponent::getRotationAccel() const{
    return rotation_accel;
}

int TuioComponent::getTuioState() const{ 
	return state;
}

bool TuioComponent::isMoving() const{
    if ((state==TUIO_ACCELERATING) || (state==TUIO_DECELERATING) || (state==TUIO_ROTATING)) return true;
    else return false;
}
