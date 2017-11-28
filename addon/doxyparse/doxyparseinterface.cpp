#include "doxyparseinterface.h"

void DoxyparseInterface::linkable_symbol(int l, const char *sym,
	Definition *symDef, Definition *context)
{
	if (!symDef) {
		// in this case we have a local or external symbol

		// TODO record use of external symbols
		// TODO must have a way to differentiate external symbols from local variables
	}
}
