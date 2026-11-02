#ifndef ML_SPACEMOUSE_H
#define ML_SPACEMOUSE_H

#include <QObject>

/**
 * Thin Qt wrapper around the 3Dconnexion macOS client driver.
 * On platforms/builds without MESHLAB_SPACEMOUSE it is an inert stub.
 * Axes are normalised to [-1, 1]: x right, y up, z towards the user;
 * rx/ry/rz are rotations around those axes.
 */
class SpaceMouse : public QObject
{
	Q_OBJECT
public:
	static SpaceMouse& instance();
	bool isAvailable() const { return registered; }

signals:
	void motion(float tx, float ty, float tz, float rx, float ry, float rz);
	void buttonsChanged(unsigned int pressed, unsigned int released);

private:
	SpaceMouse();
	~SpaceMouse();
	friend void spaceMouseDispatch(const void* state);
	void handleState(const void* state);

	bool registered = false;
	unsigned short clientId = 0;
	unsigned int lastButtons = 0;
};

#endif
