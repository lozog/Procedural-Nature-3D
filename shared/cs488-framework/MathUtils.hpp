#pragma once

const double PI = 3.14159265;

//---------------------------------------------------------------------------------------
template <typename T>
inline T degreesToRadians (
		T angle
) {
	return angle * T(PI) / T(180.0);
}

//---------------------------------------------------------------------------------------
template <typename T>
inline T radiansToDegrees (
		T angle
) {
	return angle * T(180.0) / T(PI);
}
