#include <detail/ext.h>

static void GameFrameHook(bool simulating)
{
	ext_ctx.poll();
}

bool detail::SDK_OnLoad(char* error, size_t maxlen, bool late)
{
	sharesys->RegisterLibrary(myself, "Net");
	smutils->AddGameFrameHook(&GameFrameHook);
	return true;
}

void detail::SDK_OnUnload()
{
	ext_ctx.stop();
	ext_guard.reset();
	smutils->RemoveGameFrameHook(&GameFrameHook);
}
