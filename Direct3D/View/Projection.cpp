#include "Projection.h"

Projection::Projection()
{
	this->viewProjectionBuffer = Buffers->FindShaderBuffer<ViewProjectionBuffer>();
}
