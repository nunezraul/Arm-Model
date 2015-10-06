
# include "so_normal.h"

SoNormal::SoNormal()
{
	_numpoints = 0;
}

void SoNormal::init(const GlProgram& prog, int part)
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
	uniform_locations(2); // will send 2 variables: the 2 matrices below
	uniform_location(0, "vTransf");
	uniform_location(1, "vProj");
}

void SoNormal::build(float len, float rt, float rb, int nfaces, bool shading)
{
	float pi = 3.14159265358979323846;

	P.clear(); C.clear(); // set size to zero, just in case
	//P.reserve(18); C.reserve(18); // reserve space to avoid re-allocations from the calls below

	//Drawing Normals for side of cylinder
	if (shading) {
		for (int i = 0; i < m.vsize; i++) {
			for (int j = 0; j < m.fsize; j++) {
				P.push_back(m.V[m.F[j].va]);
				P.push_back(m.V[m.F[j].va] + m.N[m.F[j].na]);
				P.push_back(m.V[m.F[j].vb]);
				P.push_back(m.V[m.F[j].vb] + m.N[m.F[j].nb]);
				P.push_back(m.V[m.F[j].vc]);
				P.push_back(m.V[m.F[j].vc] + m.N[m.F[j].nc]);
				C.push_back(GsColor::red);
				C.push_back(GsColor::red);
				C.push_back(GsColor::red);
				C.push_back(GsColor::red);
				C.push_back(GsColor::red);
				C.push_back(GsColor::red);
			}
		}
	}
	else {
		for (int i = 0; i < m.vsize; i++) {
			for (int j = 0; j < m.fsize; j++) {
				GsVec norm;
				GsVec p1(m.V[m.F[j].va]);
				GsVec p2(m.V[m.F[j].vb]);
				GsVec p3(m.V[m.F[j].vc]);

				GsVec v1 = (p2 - p1);
				GsVec v2 = (p3 - p1);
				norm = cross(v1, v2);
				norm.normalize();

				GsVec p4((p1.x + p2.x + p3.x) / 3.0, (p1.y + p2.y + p3.y) / 3.0, (p1.z + p2.z + p3.z) / 3.0);
				P.push_back(p4);
				P.push_back(p4 + norm);

				C.push_back(GsColor::red);
				C.push_back(GsColor::red);
			}
		}
	}

	
	// send data to OpenGL buffers:
	glBindBuffer(GL_ARRAY_BUFFER, buf[0]);
	glBufferData(GL_ARRAY_BUFFER, P.size() * 3 * sizeof(float), &P[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, buf[1]);
	glBufferData(GL_ARRAY_BUFFER, C.size() * 4 * sizeof(gsbyte), &C[0], GL_STATIC_DRAW);

	// save size so that we can free our buffers and later just draw the OpenGL arrays:
	_numpoints = P.size();

	// free non-needed memory:
	P.resize(0); C.resize(0);
}

void SoNormal::draw(GsMat& tr, GsMat& pr)
{
	// Draw Lines:
	glUseProgram(prog);
	glBindVertexArray(va[0]);

	glBindBuffer(GL_ARRAY_BUFFER, buf[0]); // positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, buf[1]); // colors
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, 0);

	glUniformMatrix4fv(uniloc[0], 1, GL_FALSE, tr.e);
	glUniformMatrix4fv(uniloc[1], 1, GL_FALSE, pr.e);

	glDrawArrays(GL_LINES, 0, _numpoints);

}

