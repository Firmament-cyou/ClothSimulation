#pragma once
class Cloth
{
public:
	bool fix1, fix2, fix3, fix4;
	float total_time;
	
	Cloth(float* p_p, unsigned int* t_i, unsigned int* b_i, int g, double l = 3.0f): particle_positions(p_p), top_indices(t_i), bottom_indices(b_i), grids(g), len(l) {
		fix1 = fix2 = fix3 = fix4 = true;
		total_time = 0.0f;
		gravity = 9.8f;
		ks = 1e4; kd = 1.5e2; m = 20.0;  l1 = l / grids; l2 = l1 * 1.41421; l3 = l1 * 2.0;
		particle_cnt = (grids + 1) * (grids + 1) * 3;
		tmp_positions = new float[particle_cnt];
		particle_velocity = new double[particle_cnt](); // 默认初始化
		v1 = new double[particle_cnt]();
		v2 = new double[particle_cnt]();
		v3 = new double[particle_cnt]();
		v4 = new double[particle_cnt]();
		a1 = new double[particle_cnt]();
		a2 = new double[particle_cnt]();
		a3 = new double[particle_cnt]();
		a4 = new double[particle_cnt]();
		initCloth();
	}

	~Cloth() {
		delete[] tmp_positions;
		delete[] particle_velocity;
		delete[] v1;
		delete[] v2;
		delete[] v3;
		delete[] v4;
		delete[] a1;
		delete[] a2;
		delete[] a3;
		delete[] a4;
	}

	void drop(float delta_time);
	void restore();
	

private:
	double len, gravity, ks, kd, m, l1, l2, l3; // ks -> 劲度系数，kd -> 阻尼力系数，m -> 质点质量
	int grids, particle_cnt;
	float* particle_positions, * tmp_positions;
	double* particle_velocity, * v1, * v2, * v3, * v4, * a1, * a2, * a3, * a4; // 万恶的四阶 Runge-Kutta 积分
	unsigned int* top_indices, * bottom_indices;

	void initCloth();

	double dis(double x1, double y1, double z1, double x2, double y2, double z2);

	void calc_v(double* outv, double* v, float t, double* a);
	void calc_a(double* outa, float* p, double* v);
	void calc_p(float* outp, float* p, float t, double* v);
};

