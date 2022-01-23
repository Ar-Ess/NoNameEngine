#ifndef __LINIAR_VELOCITY_COMPONENT_H__
#define __LINIAR_VELOCITY_COMPONENT_H__

#include "Component.h"

enum LiniarBehaviour
{
	INFINIT,
	LOOPING,
	OSCILATION
};

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
		direction = true;
	}

	void Update(Shape3D* afected)
	{
		if (gameTimer->GetTimerState() != RUNNING) return;

		Point3D pos = afected->GetPosition();
		Point3D n = { 1.0f, 1.0f, 1.0f };

		float vel = velocity / 200;

		afected->velocity = velocity;

		!negx ? n.x = -1 : n.x = 1;
		!negy ? n.y = -1 : n.y = 1;
		!negz ? n.z = -1 : n.z = 1;

		switch (lB)
		{
		case LiniarBehaviour::INFINIT: UpdateInfinite(&pos, vel, n); break;
		case LiniarBehaviour::LOOPING: UpdateLooping(&pos, vel, n); break;
		case LiniarBehaviour::OSCILATION: UpdateOscilation(&pos, vel, n); break;
		}

		afected->SetPosition(pos);
	}

	void Draw(Shape3D* afected, bool* onWindow = nullptr)
	{
		char xText[3] = { 'x', ' ', '\0' };
		char yText[3] = { 'y', ' ', '\0' };
		char zText[3] = { 'z', ' ', '\0' };

		negx ? xText[1] = '-' : xText[1] = ' ';
		negy ? yText[1] = '-' : yText[1] = ' ';
		negz ? zText[1] = '-' : zText[1] = ' ';

		const char* items[] = { "INFINITE", "LOOPING", "OSCILATION" };

		ImGui::Text("Behaviour:");
		ImGui::PushID("Combo");
		ImGui::Combo("", &currentBehaviour, items, 3);
		lB = (LiniarBehaviour)currentBehaviour;
		ImGui::PopID();
		ImGui::Spacing();

		ImGui::PushID("SliderVel");
		ImGui::Text("Velocity:");
		if(ImGui::SliderFloat("", &velocity, 0.1f, 100.0f)) afected->velocity = velocity;
		ImGui::PopID();
		ImGui::Spacing();

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

		if (lB != INFINIT)
		{
			ImGui::Spacing();
			ImGui::PushID("SliderMaxDistance");
			ImGui::Text("Limit Distance:");
			ImGui::SliderFloat("", &maxDistance, 1.f, 100.0f);
			ImGui::PopID();
		}
	}

	void End(Shape3D* afected)
	{
		afected->SetPosition(initialPosition);
	}

private: // Methods

	void UpdateInfinite(Point3D* pos, float vel, Point3D n)
	{
		if (x) pos->x += vel * n.x;
		if (y) pos->y += vel * n.y;
		if (z) pos->z += vel * n.z;
	}

	void UpdateLooping(Point3D* pos, float vel, Point3D n)
	{
		if (initialPosition.Distance(*pos) >= maxDistance) pos->Set(initialPosition.x, initialPosition.y, initialPosition.z);

		if (x) pos->x += vel * n.x;
		if (y) pos->y += vel * n.y;
		if (z) pos->z += vel * n.z;
	}

	void UpdateOscilation(Point3D* pos, float vel, Point3D n)
	{
		if (direction && initialPosition.Distance(*pos) >= maxDistance)
		{
			direction = false;
			finalPosition = *pos;
		}
		else if (!direction && finalPosition.Distance(*pos) >= maxDistance)
		{
			direction = true;
		}

		int neg = 1;
		direction ? neg = 1 : neg = -1;

		if (x) pos->x += vel * n.x * neg;
		if (y) pos->y += vel * n.y * neg;
		if (z) pos->z += vel * n.z * neg;
	}

private: // Variables

	Point3D initialPosition = {};
	Point3D finalPosition = {};
	LiniarBehaviour lB = INFINIT;
	float velocity = 0.1f;
	bool x = true, y = false, z = false;
	bool negx = false, negy = false, negz = false;
	float maxDistance = 100.0f;
	int currentBehaviour = 0;
	bool direction = true;
};

#endif // !__LINIAR_VELOCITY_COMPONENT_H__

