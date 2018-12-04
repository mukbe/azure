#pragma once

class DebugDraw : public btIDebugDraw
{
public:
	DebugDraw();
	~DebugDraw();

	virtual void	setDebugMode(int debugMode) { bitDebugMode = debugMode; }
	virtual int		getDebugMode() const { return bitDebugMode; }

	//그리기
	virtual void flushLines() { DrawLine(); }

	//라인들 저장
	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor);
	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
	
	virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
	{
		drawLine(PointOnB, PointOnB + normalOnB * distance, color);
		btVector3 ncolor(0, 0, 0);
		drawLine(PointOnB, PointOnB + normalOnB * 0.01f, ncolor);
	}
	virtual void drawTransform(const btTransform& transform, btScalar orthoLen)
	{
		btVector3 start = transform.getOrigin();
		btScalar temp = orthoLen * 2.f;
		drawLine(start, start + transform.getBasis() * btVector3(temp, 0, 0), btVector3(1.f, 0, 0));
		drawLine(start, start + transform.getBasis() * btVector3(0, temp, 0), btVector3(0, 1.f, 0));
		drawLine(start, start + transform.getBasis() * btVector3(0, 0, temp), btVector3(0, 0, 1.f));
	}

	//Pass
	virtual void	reportErrorWarning(const char* warningString) {}
	virtual void	draw3dText(const btVector3& location, const char* textString) {}

private:
	friend class Physics;

	void DrawLine();

	

	struct Vertex
	{
		Vertex() {}
		Vertex(const btVector3& p, const btVector3& c)
		{
			pos[0] = p[0];
			pos[1] = p[1];
			pos[2] = p[2];
			color[0] = c[0];
			color[1] = c[1];
			color[2] = c[2];
			color[3] = 1.f;
		}
		FLOAT pos[3];
		FLOAT color[4];
	};
	struct Line
	{
		Line() {}
		Line(const Vertex& f, const Vertex& t) : from(f), to(t) {}
		Line(const btVector3& f, const btVector3& t, const btVector3& fc, const btVector3& tc)
			: from(f, fc), to(t, tc) {}

		Vertex from, to;
	};

	static const size_t MAX_LINE = 100000;

	int bitDebugMode;

	ID3D11DepthStencilState* onState;
	ID3D11DepthStencilState* offState;

	vector<Line> aLine;

	Shader * shader;
	WorldBuffer * worldBuffer;

	ID3D11Buffer* vertexBuffer;
};

