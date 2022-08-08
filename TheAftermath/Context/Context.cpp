#include "Context.h"
#include <Windows.h>
#include <winrt/base.h>
namespace TheAftermath {
	void InitContext() {
		winrt::init_apartment();
	}
	void RemoveContext() {
		winrt::clear_factory_cache();
		winrt::uninit_apartment();
	}
}
