#pragma once

#include "Input.h"
#include "EmulatorInput.h"

using namespace Windows::UI::Core;

namespace VBA10
{
	class KeyboardInput
		: public EmulatorInput
	{		
	public:
		KeyboardInput(void);
		~KeyboardInput(void);

		const ControllerState *GetControllerState(void);
		void Update(void);

	private:
		ControllerState state;
		CoreWindow ^window;
	};
}