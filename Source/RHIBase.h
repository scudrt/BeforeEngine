#pragma once

class RHIBase {
public:
	virtual bool init() = 0;
	virtual void draw() = 0;
	virtual void onDestroy() = 0;
private:
	;
};
