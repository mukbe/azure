#include "stdafx.h"
#include "ModelBone.h"



ModelBone::ModelBone()
	:parent(NULL)
{
}


ModelBone::~ModelBone()
{
	childs.clear();
}

void ModelBone::Clone(void ** clone)
{
	ModelBone* bone = new ModelBone();
	bone->index = index;
	bone->name = name;
	bone->transform = transform;
	bone->absoluteTransform = absoluteTransform;

	bone->parentIndex = parentIndex;
	bone->parent = nullptr;

	*clone = bone;
}
