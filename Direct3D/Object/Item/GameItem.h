#pragma once
class Model;
class ModelAnimPlayer;
class GameItem
{//TODO 추후 본격적으로 컨텐츠 제작시 수정
private:
	Synthesize(string, name, Name)
	Synthesize(Model*, model, ModelClass)
	Synthesize(ModelAnimPlayer*, animation, Animation)
	Synthesize(int, jointIndex, JointIndex)
	Synthesize(D3DXMATRIX,finalMatrix,FinalMatrix)
	class Shader* shader;
public:
	GameItem();
	virtual ~GameItem();

	void AttachToCharater(class GameUnit* unit);

	virtual void Update();
	virtual void Render();
};

