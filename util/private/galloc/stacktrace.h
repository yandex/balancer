#pragma once

static inline int GetStackTrace(void** /*result*/, int /*max_depth*/, int /*skip_count*/) throw () {
	return 0;
}

static inline bool GetStackExtent(void* /*sp*/, void** /*stack_top*/, void** /*stack_bottom*/ ) throw () {
	return false;
}
