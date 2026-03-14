#pragma once

#include "network_types.inl"

#include "core/containers/darray.h"

// Parses an URI into an provided darray. Also creates the darray.
darray* parse_URI(char* URI);