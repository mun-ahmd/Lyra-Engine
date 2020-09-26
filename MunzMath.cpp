#include <iostream>
#include <cmath>

class Vector4 {
public:
float x_coordinate;
float y_coordinate;
float z_coordinate;
float w_coordinate;

//None of these functions will change the w value, mostly because it isnt used for transformations and rotations.

	Vector4() {
		x_coordinate = 1.0f, y_coordinate = 1.0f, z_coordinate = 1.0f, w_coordinate = 1.0f;
	}

	Vector4(float x, float y=1.0f , float z=1.0f, float w=1.0f) {
		x_coordinate = x, y_coordinate = y, z_coordinate = z, w_coordinate = w;
	}

	//Also all these use basic formulae, so study vectors if you don't understand those

	Vector4 add(Vector4 vectorToAdd) {
		return Vector4(
			x_coordinate + vectorToAdd.x_coordinate,
			y_coordinate + vectorToAdd.y_coordinate,
			z_coordinate+vectorToAdd.z_coordinate,
			1.0);
	}

	Vector4 subtract(Vector4 vectorToSub) {	//NOTE: vectorToSub is subtracted from the original vector, not the other way around
		return Vector4(
			x_coordinate - vectorToSub.x_coordinate,
			y_coordinate - vectorToSub.y_coordinate,
			z_coordinate-vectorToSub.z_coordinate,
			1.0
		);
	}

	Vector4 scalerProduct(float scaler, Vector4 vectorToMult = Vector4(1, 1, 1, 1)) {	//if you only want to multiply vector, you need to set scaler arg =1
		return Vector4(
			x_coordinate * scaler * vectorToMult.x_coordinate,
			y_coordinate * scaler * vectorToMult.y_coordinate,
			z_coordinate * scaler * vectorToMult.z_coordinate,
			1.0
		);
	}

	Vector4 crossProduct(Vector4 vectorToMult) {
		return Vector4(
			y_coordinate * vectorToMult.z_coordinate - z_coordinate * vectorToMult.y_coordinate,
			z_coordinate * vectorToMult.x_coordinate - x_coordinate * vectorToMult.z_coordinate,
			x_coordinate * vectorToMult.y_coordinate - y_coordinate * vectorToMult.x_coordinate
		);
	}

	float distance(Vector4 otherVector = Vector4(0, 0, 0, 0)) //gives distance of this vector from vector in arguement
													//If no parameter given it gives magnitude of vector
	{
		return sqrtf(
			  pow((x_coordinate - otherVector.x_coordinate), 2)
			+ pow((y_coordinate - otherVector.y_coordinate), 2)
			+ pow((z_coordinate - otherVector.z_coordinate), 2)
		);
	}

	Vector4 normalized() {
		float mag = distance();
		return Vector4(
			x_coordinate/mag,
			y_coordinate/mag,
			z_coordinate/mag,
			1.0
		);
	}

	void printVec() {
		std::cout << x_coordinate << ", " << y_coordinate << ", " << z_coordinate << ", " << w_coordinate;
	}
};

int main() {
	Vector4 a = Vector4();
	a.printVec();
	return 0;
};