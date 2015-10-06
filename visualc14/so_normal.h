#ifndef SO_NORMAL_H
#define SO_NORMAL_H

# include <vector>
# include <gsim/gs_color.h>
# include <gsim/gs_vec.h>
# include "ogl_tools.h"
# include "armmodel\model.h"


class SoNormal : public GlObjects
{
private:
	std::vector<GsVec>   P; // coordinates
	std::vector<GsColor> C; // colors
	GsVec norm, v1, v2;
	int _numpoints;         // saves the number of points
	Model m;

public:
	SoNormal();
	void init(const GlProgram& prog, int part);
	void draw(GsMat& tr, GsMat& pr);
	void build(float len, float rt, float rb, int nfaces, bool shading);


};

#endif //SO_CAPSULE_H#pragma once
