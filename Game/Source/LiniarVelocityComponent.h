#ifndef __LINIAR_VELOCITY_COMPONENT_H__
#define __LINIAR_VELOCITY_COMPONENT_H__

#include "Component.h"

class LiniarVelocityComponent : public Component
{
public:
	LiniarVelocityComponent(Timer* timer) : Component("Liniar Velocity", ComponentID::LINIAR_VELOCITY_COMPONENT)
	{
		gameTimer = timer;
	}
	~LiniarVelocityComponent() {}

	void Start(Shape3D* afected)
	{
		initialPosition = afected->GetPosition();
	}

	void Update(Shape3D* afected)
	{
		if (gameTimer->GetTimerState() != RUNNING) return;

		Point3D pos = afected->GetPosition();
		float vel = velocity / 200;
		int nx = 1, ny = 1, nz = 1;

		!negx ? nx = -1 : nx = 1;
		!negy ? ny = -1 : ny = 1;
		!negz ? nz = -1 : nz = 1;

		if (x) pos.x += vel * nx;
		if (y) pos.y += vel * ny;
		if (z) pos.z += vel * nz;
		afected->SetPosition(pos);
	}

	void Draw(bool* onWindow = nullptr)
	{
		char xText[3] = { 'x', ' ', '\0' };
		char yText[3] = { 'y', ' ', '\0' };
		char zText[3] = { 'z', ' ', '\0' };

		negx ? xText[1] = '-' : xText[1] = ' ';
		negy ? yText[1] = '-' : yText[1] = ' ';
		negz ? zText[1] = '-' : zText[1] = ' ';

		ImGui::SliderFloat("Velocity", &velocity, 0.1f, 100.0f);

		if (ImGui::Button(xText)) negx = !negx; ImGui::SameLine();
		ImGui::PushID("x");
		ImGui::Checkbox("", &x); 
		ImGui::PopID();
		ImGui::SameLine();

		if (ImGui::Button(yText)) negy = !negy; ImGui::SameLine();
		ImGui::PushID("y");
		ImGui::Checkbox("", &y);
		ImGui::PopID();
		ImGui::SameLine();

		if (ImGui::Button(zText)) negz = !negz; ImGui::SameLine();
		ImGui::PushID("z");
		ImGui::Checkbox("Z", &z);
		ImGui::PopID();
	}

	void End(Shape3D* afected)
	{
		afected->SetPosition(initialPosition);
	}

private: // Methods

private: // Variables

	Point3D initialPosition = {};
	float velocity = 0.1f;
	bool x = true, y = false, z = false;
	bool negx = false, negy = false, negz = false;
};

#endif // !__LINIAR_VELOCITY_COMPONENT_H__

