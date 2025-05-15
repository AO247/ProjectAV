//#pragma once
//#include <SimpleMath.h>
//
//using namespace DirectX::SimpleMath;
//
//class IntersectData
//{
//public:
//	IntersectData(bool doesIntersect, Vector3 direction, Vector3 collisionPoint, Vector3 collisionPointRadiusForFirstBody, Vector3 collisionPointRadiusForSecondBody) :
//		doesIntersect(doesIntersect),
//		direction(direction),
//		collisionPoint(collisionPoint),
//		collisionPointRadiusForFirstBody(collisionPointRadiusForFirstBody),
//		collisionPointRadiusForSecondBody(collisionPointRadiusForSecondBody) {}
//
//	IntersectData(bool doesIntersect, Vector3 direction, Vector3 collisionPoint, Vector3 collisionPointRadiusForFirstBody, Vector3 collisionPointRadiusForSecondBody, bool OBBFullSurfaceCollision) :
//		doesIntersect(doesIntersect),
//		direction(direction),
//		collisionPoint(collisionPoint),
//		collisionPointRadiusForFirstBody(collisionPointRadiusForFirstBody),
//		collisionPointRadiusForSecondBody(collisionPointRadiusForSecondBody),
//	    OBBFullSurfaceCollision(OBBFullSurfaceCollision){}
//
//	bool GetDoesIntersect();
//	float GetDistance();
//	Vector3 GetDirection();
//	Vector3 GetCollisionPoint();
//	Vector3 GetCollisionPointRadiusForFirstBody();
//	Vector3 GetCollisionPointRadiusForSecondBody();
//private:
//	bool doesIntersect;
//	Vector3 direction;
//	Vector3 collisionPoint;
//	Vector3 collisionPointRadiusForFirstBody;
//	Vector3 collisionPointRadiusForSecondBody;
//	bool OBBFullSurfaceCollision = true;
//};