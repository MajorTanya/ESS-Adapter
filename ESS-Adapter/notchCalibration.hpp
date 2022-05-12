// notchCalibration.hpp

#include <vector_type.h>
extern uint8_t Notch_Snap_Strength;

float pythagDist(uint8_t x1, uint8_t y1,uint8_t x2, uint8_t y2);

class CornerNotch{ 

	public:
		int8_t Xvalue, Yvalue;  // signed
		float  correction;
		int8_t Xsign, Ysign, diffSign;
 
	CornerNotch(int8_t inX, int8_t inY) {
		this->setXY(inX, inY);
	}
	
	void setXY(int8_t inX, int8_t inY) {
		Xvalue = inX;
		Yvalue = inY;
		correction = (abs(Xvalue) - abs(Yvalue))/2.0;
		diffSign = constrain(correction,-1,1);
		Xsign = constrain(Xvalue,-1,1);
		Ysign = constrain(Yvalue,-1,1);
		correction = abs(correction);
	}
	
	void applyCorrection(uint8_t ucoords[2]) {
		//notchSnapping(ucoords);
		//gateSnapping(ucoords);
		notchGravity(ucoords);
	}
	
	// Applies a slight correction to the diagonal corner values to nudge the analog value towards the ideal 45 degree angle. The end result is a correction proportional to the closeness of the current coordinate to the physical notch.
	void notchGravity(uint8_t ucoords[2]) {
		float dist = pythagDist(ucoords[0],ucoords[1],Xvalue+128,Yvalue+128);
		// Constrain the max value of dist to the difference.
		dist = dist<correction?dist:correction;

		// Invert the map. This means that distance starts at "0" and the closer we get
		// to the corner notch the higher the value gets. The correction factor is strongest
		// when we are at the corner notch, and fades as we get farther.
		dist = -(dist-correction);
		//Xdist = map(Xdist, 0, Xdiff, Xdiff, 0);
		//Ydist = map(Ydist, 0, Ydiff, Ydiff, 0);
		// The correction factor is to nudge the coordinate towards the true 45 line.
		// We need to add/subtract depending whether the correction factor is negative
		// or positive. (A negative dist means X < Y. We also need to flip the correction
		// if the axis is negative.
		
		Serial.println();
		Serial.println();
		Serial.println(trunc(ucoords[0] - (dist*Xsign*diffSign))-128);
		Serial.println(trunc(ucoords[1] + (dist*Ysign*diffSign))-128);
		Serial.println();
		ucoords[0] = trunc(ucoords[0] - (dist*Xsign*diffSign));
		ucoords[1] = trunc(ucoords[1] + (dist*Ysign*diffSign));
	}
	
	void notchSnapping(uint8_t ucoords[2]) {
		vec3_t v1 = {ucoords[0]-(Xvalue+128)-(correction*Xsign),ucoords[1]-(Yvalue+128)+(correction*Ysign)};

		if (v1.mag() <= Notch_Snap_Strength) {
			//v1 /= 2;
			ucoords[0]-=v1.x;
			ucoords[1]-=v1.y;
		}
	}
	
	void gateSnapping(uint8_t ucoords[2]) {
			vec3_t v1 = {ucoords[0]-(Xvalue+128),ucoords[1]-(Yvalue+128)};
	
			if (v1.mag() <= Notch_Snap_Strength) {
				//v1 /= 2;
				ucoords[0]-=v1.x-(correction*Xsign);
				ucoords[1]-=v1.y+(correction*Ysign);
			}
		}
};

extern CornerNotch notches[4];

void notchCorrection(uint8_t ucoords[2]);
