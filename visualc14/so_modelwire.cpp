# include "so_modelwire.h"

SoModelWire::SoModelWire()
{
	_numpoints = 0;
}

void SoModelWire::init(const GlProgram& prog, int part)
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

void SoModelWire::build(float len, float rt, float rb, int nfaces, bool shading)
{
	//int i;

	P.clear(); N.clear(); // set size to zero, just in case
						  //P.reserve(18); C.reserve(18); // reserve space to avoid re-allocations from the calls below

						  //Drawing hand

	for (int i = 0; i < m.vsize; i++) {
		for (int j = 0; j < m.fsize; j++) {
			P.push_back(m.V[m.F[j].va]);
			P.push_back(m.V[m.F[j].vb]);
			P.push_back(m.V[m.F[j].vb]);
			P.push_back(m.V[m.F[j].vc]);
			P.push_back(m.V[m.F[j].vc]);
			P.push_back(m.V[m.F[j].va]);
			C.push_back(GsColor(m.F[0].r, m.F[0].g, m.F[0].b));
			C.push_back(GsColor(m.F[0].r, m.F[0].g, m.F[0].b));
			C.push_back(GsColor(m.F[0].r, m.F[0].g, m.F[0].b));
			C.push_back(GsColor(m.F[0].r, m.F[0].g, m.F[0].b));
			C.push_back(GsColor(m.F[0].r, m.F[0].g, m.F[0].b));
			C.push_back(GsColor(m.F[0].r, m.F[0].g, m.F[0].b));
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

void SoModelWire::draw(GsMat& tr, GsMat& pr)
{
	GsColor c;
	c.set(m.F[0].r, m.F[0].g, m.F[0].b);
	Material m(c, c, c, .5);
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

