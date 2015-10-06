# include "so_model.h"

SoModel::SoModel()
{
	_numpoints = 0;
}

void SoModel::init(const GlProgram& prog, int part)
{
	if (part == 0) {
		m.load("armmodel/rhand.m");
	}
	else if (part == 1) {
		m.load("armmodel/rlowerarm.m");
	}
	else if (part == 2) {
		m.load("armmodel/rupperarm.m");
	}


	// Define buffers needed:
	set_program(prog);
	gen_vertex_arrays(1); // will use 1 vertex array
	gen_buffers(2);       // will use 2 buffers: one for coordinates and one for colors
	uniform_locations(10); // will send 2 variables: the 2 matrices below
	uniform_location(0, "vTransf");
	uniform_location(1, "vProj");
	uniform_location(2, "lPos");
	uniform_location(3, "la");
	uniform_location(4, "ld");
	uniform_location(5, "ls");
	uniform_location(6, "ka");
	uniform_location(7, "kd");
	uniform_location(8, "ks");
	uniform_location(9, "sh");

}

void SoModel::build(float len, float rt, float rb, int nfaces, bool shading)
{
	//int i;

	P.clear(); N.clear(); // set size to zero, just in case
	//P.reserve(18); C.reserve(18); // reserve space to avoid re-allocations from the calls below

	//Drawing hand
	for (int i = 0; i < m.vsize; i++) {
		for (int j = 0; j < m.fsize; j++) {
			P.push_back(m.V[m.F[j].va]);
			P.push_back(m.V[m.F[j].vb]);
			P.push_back(m.V[m.F[j].vc]);
		}
	}
	if (shading) {
		for (int i = 0; i < m.nsize; i++) {
			for (int j = 0; j < m.fsize; j++) {
				N.push_back(m.N[m.F[j].na]);
				N.push_back(m.N[m.F[j].nb]);
				N.push_back(m.N[m.F[j].nc]);
			}
		}
	}
	else {
		for (int i = 0; i < m.nsize; i++) {
			for (int j = 0; j < m.fsize; j++) {
				GsVec norm;
				norm = normal(m.N[m.F[j].na], m.N[m.F[j].nb], m.N[m.F[j].nc]);
				N.push_back(norm);
				N.push_back(norm);
				N.push_back(norm);
			}
		}
	}


	// send data to OpenGL buffers:
	glBindBuffer(GL_ARRAY_BUFFER, buf[0]);
	glBufferData(GL_ARRAY_BUFFER, P.size() * 3 * sizeof(float), &P[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, buf[1]);
	glBufferData(GL_ARRAY_BUFFER, N.size() * 3 * sizeof(float), &N[0], GL_STATIC_DRAW);


	// save size so that we can free our buffers and later just draw the OpenGL arrays:
	_numpoints = P.size();

	// free non-needed memory:
	P.resize(0); N.resize(0);
}

void SoModel::draw(GsMat& tr, GsMat& pr, const Light& l)
{
	GsColor c;
	c.set(m.F[0].r, m.F[0].g, m.F[0].b);
	Material m(c, c, c, .5);
	glUseProgram(prog);
	glBindVertexArray(va[0]);

	glBindBuffer(GL_ARRAY_BUFFER, buf[0]); // position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, buf[1]);  // normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);  //false means no norminization

	glUniformMatrix4fv(uniloc[0], 1, GL_FALSE, tr.e);
	glUniformMatrix4fv(uniloc[1], 1, GL_FALSE, pr.e);
	float f[4]; // we convert below our color values to be in [0,1]  
	glUniform3fv(uniloc[2], 1, l.pos.e);
	glUniform4fv(uniloc[3], 1, l.amb.get(f));
	glUniform4fv(uniloc[4], 1, l.dif.get(f));
	glUniform4fv(uniloc[5], 1, l.spe.get(f));
	glUniform4fv(uniloc[6], 1, m.amb.get(f));
	glUniform4fv(uniloc[7], 1, m.dif.get(f));
	glUniform4fv(uniloc[8], 1, m.spe.get(f));
	glUniform1fv(uniloc[9], 1, &m.sh);

	glDrawArrays(GL_TRIANGLES, 0, _numpoints);

}

