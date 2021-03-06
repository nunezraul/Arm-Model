
#ifndef SO_MODEL_H
#define SO_MODEL_H

# include <vector>
# include <gsim/gs_color.h>
# include <gsim/gs_vec.h>
# include "ogl_tools.h"
# include "armmodel/model.h"
# include <fstream>
# include <string>


class SoModel : public GlObjects
{
private:
	std::vector<GsVec> P; // coordinates
	std::vector<GsVec> N; // colors
	GsVec norm, v1, v2;
	int _numpoints;         // saves the number of points
	Model m;

public:
	class Light {
	public:
		GsVec pos;
		GsColor amb, dif, spe;
		Light (const GsVec& p, const GsColor& a, const GsColor& d, const GsColor& s)
			: pos(p), amb(a), dif(d), spe(s) {}
		Light() {}
	};

	class Material {
	public: 
		GsColor amb, dif, spe;
		float sh; //Shininess
		Material (const GsColor& a, const GsColor& d, const GsColor&s, float sn)
			: amb(a), dif(d), spe(s), sh(sn){}
		Material(){}
	};

	SoModel();
	void init(const GlProgram& prog, int part);
	void draw(GsMat& tr, GsMat& pr, const Light& l);
	void build(float len, float rt, float rb, int nfaces, bool shading);


};

#endif //SO_CAPSULE_H