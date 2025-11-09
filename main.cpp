#include "main.h"


class messenger_particle
{
public:
	vector_3 position;
	vector_3 velocity;
};

class bi_directional_edge
{
public:
	pair<vector_3, vector_3> locations;
	real_type frequency;
	real_type last_emitted;
};

class schwarzschild_black_hole
{
public:
	vector<vector_3> vertices;
	vector<bi_directional_edge> edges;
};




schwarzschild_black_hole bh;
vector<messenger_particle> photons;


// E = (N^2 - N) / 2



vector_3 random_unit_vector(void)
{
	const real_type z = dis(generator) * 2.0 - 1.0;
	const real_type a = dis(generator) * 2.0 * pi;

	const real_type r = sqrt(1.0f - z * z);
	const real_type x = r * cos(a);
	const real_type y = r * sin(a);

	return vector_3(x, y, z).normalize();
}

bool intersect_AABB(const vector_3 min_location, const vector_3 max_location, const vector_3& point)
{
	if (min_location.x <= point.x && max_location.x >= point.x &&
		min_location.y <= point.y && max_location.y >= point.y &&
		min_location.z <= point.z && max_location.z >= point.z)
	{
		return true;
	}

	return false;
}

bool intersect(
	const vector_3 location,
	const vector_3 normal,
	const real_type receiver_distance,
	const real_type receiver_radius)
{
	vector_3 min_location(-receiver_radius + receiver_distance, -receiver_radius, -receiver_radius);
	vector_3 max_location(receiver_radius + receiver_distance, receiver_radius, receiver_radius);

	return intersect_AABB(min_location, max_location, location);
}


real_type get_intersecting_line_density(
	const long long unsigned int n,
	const real_type emitter_radius,
	const real_type receiver_distance,
	const real_type receiver_distance_plus,
	const real_type receiver_radius)
{
	real_type count = 0;
	real_type count_plus = 0;

	//generator.seed(static_cast<unsigned>(0));

	//for (long long unsigned int i = 0; i < n; i++)
	//{
	//	if (i % 100000000 == 0)
	//		cout << float(i) / float(n) << endl;

	//	// Random hemisphere outward
	//	vector_3 location = random_unit_vector();

	//	location.x *= emitter_radius;
	//	location.y *= emitter_radius;
	//	location.z *= emitter_radius;

	//	vector_3 surface_normal = location;
	//	surface_normal.normalize();

	//	vector_3 normal = 
	//		random_cosine_weighted_hemisphere(
	//			surface_normal);

	//	bool i_hit = intersect(
	//		location, normal, 
	//		receiver_distance, receiver_radius);

	//	//if (i_hit)
	//	//	count += *i_hit / (2.0 * receiver_radius);
	//
	//	i_hit = intersect(
	//		location, normal,
	//		receiver_distance_plus, receiver_radius);

	//	//if (i_hit)
	//	//	count_plus += *i_hit / (2.0 * receiver_radius);
	//}

	return count_plus - count;
}

int main(int argc, char** argv)
{
	// Field line count
	const long long unsigned int n = 10;

	const real_type emitter_radius_geometrized =
		sqrt(n * log(2.0) / pi);

	const real_type receiver_radius_geometrized =
		emitter_radius_geometrized * 0.01; // Minimum one Planck unit

	const real_type emitter_area_geometrized =
		4.0 * pi
		* emitter_radius_geometrized
		* emitter_radius_geometrized;

	const real_type emitter_mass_geometrized =
		emitter_radius_geometrized
		/ 2.0;

	
	for (long long unsigned int i = 0; i < n; i++)
		bh.vertices.push_back(random_unit_vector() * emitter_radius_geometrized);

	camera_w = emitter_radius_geometrized * 5.0;

	long long unsigned int num_repulsion_rounds = 10;

	for (size_t x = 0; x < num_repulsion_rounds; x++)
	{
		//cout << x << " " << num_repulsion_rounds << endl;

		vector<vector_3> backup_points = bh.vertices;

		for (long long unsigned int i = 0; i < n; i++)
		{
			vector_3 a(0, 0, 0);

			for (long long unsigned int j = 0; j < n; j++)
			{
				if (i == j)
					continue;

				custom_math::vector_3 grav_dir = backup_points[j] - backup_points[i];

				double distance = grav_dir.length();
				grav_dir.normalize();
				custom_math::vector_3 accel = -grav_dir / pow(distance, 1.0);

				a += accel;
			}

			bh.vertices[i] += a;
			bh.vertices[i].normalize();
			bh.vertices[i] *= emitter_radius_geometrized;
		}
	}



	std::chrono::high_resolution_clock::time_point t = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::milli> elapsed = t - app_start_time;

	for (long long unsigned int i = 0; i < n; i++)
	{
		for (long long unsigned int j = i + 1; j < n; j++)
		{
			bi_directional_edge e;
			
			e.locations = pair<vector_3, vector_3>(bh.vertices[i], bh.vertices[j]);
			
			const real_type wavelength = (e.locations.first - e.locations.second).length();

			e.frequency = 1.0 / wavelength;
			e.last_emitted = elapsed.count();// *1000.0;

			vector_3 mid_way = (e.locations.first + e.locations.second) * 0.5;
			vector_3 dir = e.locations.first - e.locations.second;
			dir.normalize();

			messenger_particle p;
			
			p.position = mid_way;
			p.velocity = dir;
			photons.push_back(p);

			p.position = mid_way;
			p.velocity = -dir;
			photons.push_back(p);

			bh.edges.push_back(e);
		}
	}




	cout << setprecision(20) << endl;

	glutInit(&argc, argv);
	init_opengl(win_x, win_y);
	glutReshapeFunc(reshape_func);
	glutIdleFunc(idle_func);
	glutDisplayFunc(display_func);
	glutKeyboardFunc(keyboard_func);
	glutMouseFunc(mouse_func);
	glutMotionFunc(motion_func);
	glutPassiveMotionFunc(passive_motion_func);
	//glutIgnoreKeyRepeat(1);
	glutMainLoop();
	glutDestroyWindow(win_id);


















	//real_type start_pos =
	//	emitter_radius_geometrized
	//	+ receiver_radius_geometrized;

	//real_type end_pos = start_pos * 10;

	////swap(end_pos, start_pos);

	//const size_t pos_res = 10; // Minimum 2 steps

	//const real_type pos_step_size =
	//	(end_pos - start_pos)
	//	/ (pos_res - 1);

	//const real_type epsilon =
	//	receiver_radius_geometrized;


	//for (size_t i = 0; i < pos_res; i++)
	//{
	//	const real_type receiver_distance_geometrized =
	//		start_pos + i * pos_step_size;

	//	const real_type receiver_distance_plus_geometrized =
	//		receiver_distance_geometrized + epsilon;

	//	// beta function
	//	const real_type collision_count_plus_minus_collision_count =
	//		get_intersecting_line_density(
	//			static_cast<long long unsigned int>(n_geometrized),
	//			emitter_radius_geometrized,
	//			receiver_distance_geometrized,
	//			receiver_distance_plus_geometrized,
	//			receiver_radius_geometrized);

	//	// alpha variable
	//	const real_type gradient_integer =
	//		collision_count_plus_minus_collision_count
	//		/ epsilon;

	//	// g variable
	//	real_type gradient_strength =
	//		-gradient_integer
	//		/
	//		(receiver_radius_geometrized
	//			* receiver_radius_geometrized
	//			);

	//	//cout << gradient_strength << " " << n_geometrized / (2 * pow(receiver_distance_geometrized, 3.0)) << endl;
	//	//cout << gradient_strength / (n_geometrized / (2 * pow(receiver_distance_geometrized, 3.0))) << endl;


	//	const real_type a_Newton_geometrized =
	//		sqrt(
	//			n_geometrized * log(2.0)
	//			/
	//			(4.0 * pi *
	//				pow(receiver_distance_geometrized, 4.0))
	//		);

	//	const real_type a_flat_geometrized =
	//		gradient_strength * receiver_distance_geometrized * log(2)
	//		/ (8.0 * emitter_mass_geometrized);


	//	//const real_type g_approx = n_geometrized / (2 * pow(receiver_distance_geometrized, 3.0));
	//	//const real_type a_approx_geometrized =
	//	//	g_approx * receiver_distance_geometrized * log(2)
	//	//	/ (8.0 * emitter_mass_geometrized);


	//	const real_type dt_Schwarzschild = sqrt(1 - emitter_radius_geometrized / receiver_distance_geometrized);

	//	const real_type a_Schwarzschild_geometrized =
	//		emitter_radius_geometrized / (pi * pow(receiver_distance_geometrized, 2.0) * dt_Schwarzschild);

	//	cout << "a_Schwarzschild_geometrized " << a_Schwarzschild_geometrized << endl;
	//	cout << "a_Newton_geometrized " << a_Newton_geometrized << endl;
	//	cout << "a_flat_geometrized " << a_flat_geometrized << endl;
	//	cout << a_Schwarzschild_geometrized / a_flat_geometrized << endl;
	//	cout << endl;
	//	cout << a_Newton_geometrized / a_flat_geometrized << endl;
	//	cout << endl << endl;

	//	outfile << receiver_distance_geometrized <<
	//		" " <<
	//		(a_Schwarzschild_geometrized / a_flat_geometrized) <<
	//		endl;
	//}

}





void idle_func(void)
{
	const double dt = 0.001;

	std::chrono::high_resolution_clock::time_point t = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::milli> elapsed = t - app_start_time;

	real_type tf = elapsed.count();

	for (size_t i = 0; i < bh.edges.size(); i++)
	{
		if (tf >= bh.edges[i].last_emitted + bh.edges[i].frequency*1000.0)
		{
			vector_3 mid_way = (bh.edges[i].locations.first + bh.edges[i].locations.second) * 0.5;
			vector_3 dir = bh.edges[i].locations.first - bh.edges[i].locations.second;
			dir.normalize();

			messenger_particle p;

			p.position = mid_way;
			p.velocity = dir;
			photons.push_back(p);

			p.position = mid_way;
			p.velocity = -dir;
			photons.push_back(p);

			bh.edges[i].last_emitted = tf;
		}
	}


	for (size_t i = 0; i < photons.size(); i++)
		photons[i].position += photons[i].velocity * dt;

	glutPostRedisplay();
}

void init_opengl(const int& width, const int& height)
{
	win_x = width;
	win_y = height;

	if (win_x < 1)
		win_x = 1;

	if (win_y < 1)
		win_y = 1;

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(win_x, win_y);
	win_id = glutCreateWindow("orbit");

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glClearColor((float)background_colour.x, (float)background_colour.y, (float)background_colour.z, 1);
	glClearDepth(1.0f);

	main_camera.Set(0, 0, camera_w, camera_fov, win_x, win_y, camera_near, camera_far);
}

void reshape_func(int width, int height)
{
	win_x = width;
	win_y = height;

	if (win_x < 1)
		win_x = 1;

	if (win_y < 1)
		win_y = 1;

	glutSetWindow(win_id);
	glutReshapeWindow(win_x, win_y);
	glViewport(0, 0, win_x, win_y);

	main_camera.Set(main_camera.u, main_camera.v, main_camera.w, main_camera.fov, win_x, win_y, camera_near, camera_far);
}

// Text drawing code originally from "GLUT Tutorial -- Bitmap Fonts and Orthogonal Projections" by A R Fernandes
void render_string(int x, const int y, void* font, const string& text)
{
	for (size_t i = 0; i < text.length(); i++)
	{
		glRasterPos2i(x, y);
		glutBitmapCharacter(font, text[i]);
		x += glutBitmapWidth(font, text[i]) + 1;
	}
}
// End text drawing code.

void draw_objects(void)
{
	glDisable(GL_LIGHTING);

	glPushMatrix();


	glPointSize(2.0);
	glLineWidth(1.0f);


	glBegin(GL_POINTS);

	glColor3f(1.0, 1.0, 1.0);

	for (size_t i = 0; i < bh.vertices.size(); i++)
		glVertex3d(bh.vertices[i].x, bh.vertices[i].y, bh.vertices[i].z);

	glEnd();



	glBegin(GL_POINTS);

	glColor3f(1.0, 0.5, 0.0);

	for (size_t i = 0; i < photons.size(); i++)
		glVertex3d(photons[i].position.x, photons[i].position.y, photons[i].position.z);

	glEnd();


	glLineWidth(1.0f);


	// If we do draw the axis at all, make sure not to draw its outline.
	if (true == draw_axis)
	{
		glBegin(GL_LINES);

		glColor3f(1, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(1, 0, 0);
		glColor3f(0, 1, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 1, 0);
		glColor3f(0, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 1);

		glColor3f(0.5, 0.5, 0.5);
		glVertex3f(0, 0, 0);
		glVertex3f(-1, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, -1, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, -1);

		glEnd();
	}

	glPopMatrix();
}




void display_func(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw the model's components using OpenGL/GLUT primitives.
	draw_objects();

	if (true == draw_control_list)
	{
		// Text drawing code originally from "GLUT Tutorial -- Bitmap Fonts and Orthogonal Projections" by A R Fernandes
		// http://www.lighthouse3d.com/opengl/glut/index.php?bmpfontortho
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, win_x, 0, win_y);
		glScaled(1, -1, 1); // Neat. :)
		glTranslated(0, -win_y, 0); // Neat. :)
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glColor3d(control_list_colour.x, control_list_colour.y, control_list_colour.z);

		size_t break_size = 22;
		size_t start = 20;
		ostringstream oss;

		render_string(10, static_cast<int>(start), GLUT_BITMAP_HELVETICA_18, string("Mouse controls:"));
		render_string(10, static_cast<int>(start + 1 * break_size), GLUT_BITMAP_HELVETICA_18, string("  LMB + drag: Rotate camera"));
		render_string(10, static_cast<int>(start + 2 * break_size), GLUT_BITMAP_HELVETICA_18, string("  RMB + drag: Zoom camera"));

		render_string(10, static_cast<int>(start + 4 * break_size), GLUT_BITMAP_HELVETICA_18, string("Keyboard controls:"));
		render_string(10, static_cast<int>(start + 5 * break_size), GLUT_BITMAP_HELVETICA_18, string("  w: Draw axis"));
		render_string(10, static_cast<int>(start + 6 * break_size), GLUT_BITMAP_HELVETICA_18, string("  e: Draw text"));
		render_string(10, static_cast<int>(start + 7 * break_size), GLUT_BITMAP_HELVETICA_18, string("  u: Rotate camera +u"));
		render_string(10, static_cast<int>(start + 8 * break_size), GLUT_BITMAP_HELVETICA_18, string("  i: Rotate camera -u"));
		render_string(10, static_cast<int>(start + 9 * break_size), GLUT_BITMAP_HELVETICA_18, string("  o: Rotate camera +v"));
		render_string(10, static_cast<int>(start + 10 * break_size), GLUT_BITMAP_HELVETICA_18, string("  p: Rotate camera -v"));



		custom_math::vector_3 eye = main_camera.eye;
		custom_math::vector_3 eye_norm = eye;
		eye_norm.normalize();

		oss.clear();
		oss.str("");
		oss << "Camera position: " << eye.x << ' ' << eye.y << ' ' << eye.z;
		render_string(10, static_cast<int>(win_y - 2 * break_size), GLUT_BITMAP_HELVETICA_18, oss.str());

		oss.clear();
		oss.str("");
		oss << "Camera position (normalized): " << eye_norm.x << ' ' << eye_norm.y << ' ' << eye_norm.z;
		render_string(10, static_cast<int>(win_y - break_size), GLUT_BITMAP_HELVETICA_18, oss.str());

		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		// End text drawing code.
	}

	glFlush();
	glutSwapBuffers();
}

void keyboard_func(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'w':
	{
		draw_axis = !draw_axis;
		break;
	}
	case 'e':
	{
		draw_control_list = !draw_control_list;
		break;
	}
	case 'u':
	{
		main_camera.u -= u_spacer;
		main_camera.Set();
		break;
	}
	case 'i':
	{
		main_camera.u += u_spacer;
		main_camera.Set();
		break;
	}
	case 'o':
	{
		main_camera.v -= v_spacer;
		main_camera.Set();
		break;
	}
	case 'p':
	{
		main_camera.v += v_spacer;
		main_camera.Set();
		break;
	}

	default:
		break;
	}
}

void mouse_func(int button, int state, int x, int y)
{
	if (GLUT_LEFT_BUTTON == button)
	{
		if (GLUT_DOWN == state)
			lmb_down = true;
		else
			lmb_down = false;
	}
	else if (GLUT_MIDDLE_BUTTON == button)
	{
		if (GLUT_DOWN == state)
			mmb_down = true;
		else
			mmb_down = false;
	}
	else if (GLUT_RIGHT_BUTTON == button)
	{
		if (GLUT_DOWN == state)
			rmb_down = true;
		else
			rmb_down = false;
	}
}

void motion_func(int x, int y)
{
	int prev_mouse_x = mouse_x;
	int prev_mouse_y = mouse_y;

	mouse_x = x;
	mouse_y = y;

	int mouse_delta_x = mouse_x - prev_mouse_x;
	int mouse_delta_y = prev_mouse_y - mouse_y;

	if (true == lmb_down && (0 != mouse_delta_x || 0 != mouse_delta_y))
	{
		main_camera.u -= static_cast<float>(mouse_delta_y) * u_spacer;
		main_camera.v += static_cast<float>(mouse_delta_x) * v_spacer;
	}
	else if (true == rmb_down && (0 != mouse_delta_y))
	{
		main_camera.w -= static_cast<float>(mouse_delta_y) * w_spacer;

		if (main_camera.w < 1.1f)
			main_camera.w = 1.1f;

	}

	main_camera.Set(); // Calculate new camera vectors.
}

void passive_motion_func(int x, int y)
{
	mouse_x = x;
	mouse_y = y;
}
