#include "spacemouse.h"

#ifdef MESHLAB_SPACEMOUSE
#include <3DconnexionClient/ConnexionClientAPI.h>

void spaceMouseDispatch(const void* state);

namespace {
constexpr float AXIS_MAX = 350.0f; // typical full-deflection raw value

void messageHandler(unsigned int, unsigned int type, void* arg)
{
	if (type == kConnexionMsgDeviceState)
		spaceMouseDispatch(arg);
}
}

void spaceMouseDispatch(const void* state)
{
	SpaceMouse::instance().handleState(state);
}

SpaceMouse::SpaceMouse()
{
	// Weak-linked: if the driver is not installed the symbol is null.
	if (&SetConnexionHandlers == nullptr)
		return;
	// Run on the main run loop so signals are emitted on the GUI thread.
	if (SetConnexionHandlers(messageHandler, nullptr, nullptr, false) != 0)
		return;
	clientId = RegisterConnexionClient(
		kConnexionClientWildcard, (uint8_t*)"\007MeshLab",
		kConnexionClientModeTakeOver, kConnexionMaskAll);
	registered = true;
}

SpaceMouse::~SpaceMouse()
{
	if (registered) {
		UnregisterConnexionClient(clientId);
		CleanupConnexionHandlers();
	}
}

void SpaceMouse::handleState(const void* s)
{
	const ConnexionDeviceState* st = static_cast<const ConnexionDeviceState*>(s);
	if (st->command == kConnexionCmdHandleAxis) {
		auto n = [](int16_t v) { return qBound(-1.0f, v / AXIS_MAX, 1.0f); };
		// device axes: x right, y towards user (push/pull), z down -> remap to x right, y up, z towards user
		emit motion(n(st->axis[0]), -n(st->axis[2]), n(st->axis[1]),
		            n(st->axis[3]), -n(st->axis[5]), n(st->axis[4]));
	}
	else if (st->command == kConnexionCmdHandleButtons) {
		unsigned int now = st->buttons;
		emit buttonsChanged(now & ~lastButtons, lastButtons & ~now);
		lastButtons = now;
	}
}
#else
SpaceMouse::SpaceMouse() {}
SpaceMouse::~SpaceMouse() {}
void SpaceMouse::handleState(const void*) {}
#endif

SpaceMouse& SpaceMouse::instance()
{
	static SpaceMouse inst;
	return inst;
}
