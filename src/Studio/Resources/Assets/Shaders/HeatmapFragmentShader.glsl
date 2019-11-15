#version 120
#define PI 3.14159265359

varying float widthV;
varying float heightV;
varying float numElectrodesV;

uniform vec2  electrodeAngles[8];
uniform float electrodeSignals[8];
uniform vec3  colorcode;
uniform int   exponent;

vec2 center = vec2(widthV * 0.5, heightV * 0.5);
vec2 resolution = vec2(widthV, heightV);
float radius = min(widthV, heightV) * 0.5;
float radiusElectrode = 10;


vec2 GetSphereAnglesFrom2D(vec2 coordinates)
{
	// first get the theta angle (2D angle on the circle)
	float theta        = degrees(atan(coordinates.y, coordinates.x));
	float z            = sqrt(1.0 - pow(coordinates.x, 2) - pow(coordinates.y, 2));
	float phi          = degrees(atan(sqrt(pow(coordinates.x, 2) + pow(coordinates.y, 2)), z));
	return vec2(theta, phi);
}


vec3 Get3DCoordsFromSphere(float theta, float phi)
{
	float x = sin(phi) * cos(theta);
	float y = sin(phi) * sin(theta);
	float z = cos(phi);
	return vec3(x, y, z);
}


void main()
{
	vec2 centerVector = vec2(gl_FragCoord.x - center.x, gl_FragCoord.y - center.y);
	if (length(centerVector) > radius)
	{
		gl_FragColor = vec4(vec3(0.19), 1.0);
	}
	else
	{
		// set background to white
		gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
		// normalize coordinates to fit sphere
		float circleOffsetWidth  = (radius * 2.0) - widthV;
		float circleOffsetHeight = (radius * 2.0) - heightV;
		circleOffsetWidth = circleOffsetWidth * 0.5;
		circleOffsetHeight = circleOffsetHeight * 0.5;
		// set cartesian system in the circle between [-1;1]
		float normalizedX = ((gl_FragCoord.x + circleOffsetWidth) / (radius * 2.0)) * 2.0 - 1.0;
		float normalizedY = ((gl_FragCoord.y + circleOffsetHeight) / (radius * 2.0)) * 2.0 - 1.0;
		vec2 currentNormalizedPixel = vec2(normalizedX, normalizedY);

        // get the sphere coordinates of the current fragcoord
		vec2 fragCoordAngle = GetSphereAnglesFrom2D(currentNormalizedPixel);

		// set sum variables
		float sumSignalStrength = 0.0;
		float sumWeight = 0.0;
		float weight = 0.0;

		// draw heatmap
		for (int i = 0; i < 8; i++)
		{
			float electrodeTheta = radians(electrodeAngles[i].x);
			float electrodePhi   = radians(90.0 - electrodeAngles[i].y);
			float fragCoordTheta = radians(fragCoordAngle.x);
			float fragCoordPhi   = radians(fragCoordAngle.y);

			vec3 electrodeVector = Get3DCoordsFromSphere(electrodeTheta, electrodePhi);
			vec3 fragCoordVector = Get3DCoordsFromSphere(fragCoordTheta, fragCoordPhi);

			float dotProduct = dot(electrodeVector, fragCoordVector);
			float innerAngle = acos(dotProduct);

			if (dotProduct < 0.0)
			{
				innerAngle = PI - acos(dotProduct);
			}

			weight = (innerAngle / PI);
			weight = pow(1.0 - weight, exponent);
			sumSignalStrength = sumSignalStrength + (weight * electrodeSignals[i]);
			sumWeight  = sumWeight + weight;
		}

	    float colorWeight = sumSignalStrength / sumWeight;

	    gl_FragColor = vec4(colorWeight, 0.0, 1.0 - colorWeight, 1.0);

		// draw electrodes with sphere coordinates
		for(int i = 0; i < 8; i++)
		{
			vec3 electrode = Get3DCoordsFromSphere(radians(electrodeAngles[i].x), radians(90.0 - electrodeAngles[i].y));
			vec2 electrodePosition = vec2(gl_FragCoord.x - (center.x + electrode.x * (widthV * 0.5)), gl_FragCoord.y - (center.y + electrode.y * (heightV * 0.5)));
			float lengthElectrode = length(electrodePosition);
			if (lengthElectrode < radiusElectrode)
			{
				gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
			}

		}
	}
}