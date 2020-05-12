#include "Cloth.h"
#include <cmath>
#include <cstring>

void Cloth::drop(float delta_time)
{
	// RK4 积分
	memcpy(v1, particle_velocity, particle_cnt * sizeof(double));
	calc_a(a1, particle_positions, v1);
	calc_v(v2, particle_velocity, delta_time / 2, a1);
	calc_p(tmp_positions, particle_positions, delta_time / 2, v1);
	calc_a(a2, tmp_positions, v2);
	calc_v(v3, particle_velocity, delta_time / 2, a2);
	calc_p(tmp_positions, particle_positions, delta_time / 2, v2);
	calc_a(a3, tmp_positions, v3);
	calc_v(v4, particle_velocity, delta_time, a3);
	calc_p(tmp_positions, particle_positions, delta_time, v3);
	calc_a(a4, tmp_positions, v4);

	// 更新位置数组与速度数组
	for (int i = 0; i <= grids; i++) {
		for (int j = 0; j <= grids; j++) {
			float v1x = (float)v1[i * (grids + 1) * 3 + j * 3];
			float v1y = (float)v1[i * (grids + 1) * 3 + j * 3 + 1];
			float v1z = (float)v1[i * (grids + 1) * 3 + j * 3 + 2];
			float v2x = (float)v2[i * (grids + 1) * 3 + j * 3];
			float v2y = (float)v2[i * (grids + 1) * 3 + j * 3 + 1];
			float v2z = (float)v2[i * (grids + 1) * 3 + j * 3 + 2];
			float v3x = (float)v3[i * (grids + 1) * 3 + j * 3];
			float v3y = (float)v3[i * (grids + 1) * 3 + j * 3 + 1];
			float v3z = (float)v3[i * (grids + 1) * 3 + j * 3 + 2];
			float v4x = (float)v4[i * (grids + 1) * 3 + j * 3];
			float v4y = (float)v4[i * (grids + 1) * 3 + j * 3 + 1];
			float v4z = (float)v4[i * (grids + 1) * 3 + j * 3 + 2];
			float& px = particle_positions[i * (grids + 1) * 3 + j * 3];
			float& py = particle_positions[i * (grids + 1) * 3 + j * 3 + 1];
			float& pz = particle_positions[i * (grids + 1) * 3 + j * 3 + 2];
			px += (delta_time / 6) * (v1x + 2 * v2x + 2 * v3x + v4x);
			py += (delta_time / 6) * (v1y + 2 * v2y + 2 * v3y + v4y);
			pz += (delta_time / 6) * (v1z + 2 * v2z + 2 * v3z + v4z);

			double a1x = a1[i * (grids + 1) * 3 + j * 3];
			double a1y = a1[i * (grids + 1) * 3 + j * 3 + 1];
			double a1z = a1[i * (grids + 1) * 3 + j * 3 + 2];
			double a2x = a2[i * (grids + 1) * 3 + j * 3];
			double a2y = a2[i * (grids + 1) * 3 + j * 3 + 1];
			double a2z = a2[i * (grids + 1) * 3 + j * 3 + 2];
			double a3x = a3[i * (grids + 1) * 3 + j * 3];
			double a3y = a3[i * (grids + 1) * 3 + j * 3 + 1];
			double a3z = a3[i * (grids + 1) * 3 + j * 3 + 2];
			double a4x = a4[i * (grids + 1) * 3 + j * 3];
			double a4y = a4[i * (grids + 1) * 3 + j * 3 + 1];
			double a4z = a4[i * (grids + 1) * 3 + j * 3 + 2];
			double& vx = particle_velocity[i * (grids + 1) * 3 + j * 3];
			double& vy = particle_velocity[i * (grids + 1) * 3 + j * 3 + 1];
			double& vz = particle_velocity[i * (grids + 1) * 3 + j * 3 + 2];
			vx += (delta_time / 6) * (a1x + 2 * a2x + 2 * a3x + a4x);
			vy += (delta_time / 6) * (a1y + 2 * a2y + 2 * a3y + a4y);
			vz += (delta_time / 6) * (a1z + 2 * a2z + 2 * a3z + a4z);

			// 处理地板碰撞
			if (py < -4.95f) { py = -4.95f; vy = .0; }
			// 处理球体碰撞
			double d = dis(px, py, pz, 0.0, 0.0, 0.0);
			if (d < 1.05) {
				px *= 1.05f / (float)d;
				py *= 1.05f / (float)d;
				pz *= 1.05f / (float)d;
				double c = (vx * px + vy * py + vz * pz) / 1.05;
				vx -= c * px;
				vy -= c * py;
				vz -= c * pz;
			}
		}
	}

	// 处理固定
	if (fix1) {
		double& vx = particle_velocity[0];
		double& vy = particle_velocity[1];
		double& vz = particle_velocity[2];
		float& px = particle_positions[0];
		float& py = particle_positions[1];
		float& pz = particle_positions[2];
		vx = vy = vz = 0.0; px = pz = (float)-len / 2; py = 5.0f;
	}
	if (fix2) {
		double& vx = particle_velocity[grids * 3];
		double& vy = particle_velocity[grids * 3 + 1];
		double& vz = particle_velocity[grids * 3 + 2];
		float& px = particle_positions[grids * 3];
		float& py = particle_positions[grids * 3 + 1];
		float& pz = particle_positions[grids * 3 + 2];
		vx = vy = vz = 0.0; px = (float)len / 2;  pz = (float)-len / 2; py = 5.0f;
	}
	if (fix3) {
		double& vx = particle_velocity[grids * (grids + 1) * 3];
		double& vy = particle_velocity[grids * (grids + 1) * 3 + 1];
		double& vz = particle_velocity[grids * (grids + 1) * 3 + 2];
		float& px = particle_positions[grids * (grids + 1) * 3];
		float& py = particle_positions[grids * (grids + 1) * 3 + 1];
		float& pz = particle_positions[grids * (grids + 1) * 3 + 2];
		vx = vy = vz = 0.0; px = (float)-len / 2;  pz = (float)len / 2; py = 5.0f;
	}
	if (fix4) {
		double& vx = particle_velocity[grids * (grids + 1) * 3 + grids * 3];
		double& vy = particle_velocity[grids * (grids + 1) * 3 + grids * 3 + 1];
		double& vz = particle_velocity[grids * (grids + 1) * 3 + grids * 3 + 2];
		float& px = particle_positions[grids * (grids + 1) * 3 + grids * 3];
		float& py = particle_positions[grids * (grids + 1) * 3 + grids * 3 + 1];
		float& pz = particle_positions[grids * (grids + 1) * 3 + grids * 3 + 2];
		vx = vy = vz = 0.0f; px = pz = (float)len / 2; py = 5.0f;
	}
}

void Cloth::restore()
{
	float d = (float)len / grids;
	float z = (float)-len / 2;
	int iter = 0;
	for (int i = 0; i <= grids; i++) {
		float x = (float)-len / 2;
		for (int j = 0; j <= grids; j++) {
			tmp_positions[iter++] = particle_positions[iter] = x;
			tmp_positions[iter++] = particle_positions[iter] = 5.0f;
			tmp_positions[iter++] = particle_positions[iter] = z;
			x += d;
		}
		z += d;
	}
	memset(particle_velocity, 0.0, particle_cnt * sizeof(particle_velocity[0]));
	fix1 = fix2 = fix3 = fix4 = true;
}

void Cloth::initCloth()
{
	float d = (float)len / grids;
	float z = (float)-len / 2;
	int iter = 0;
	for (int i = 0; i <= grids; i++) {
		float x = (float)-len / 2;
		for (int j = 0; j <= grids; j++) {
			tmp_positions[iter++] = particle_positions[iter] = x;
			tmp_positions[iter++] = particle_positions[iter] = 5.0f;
			tmp_positions[iter++] = particle_positions[iter] = z;
			x += d;
		}
		z += d;
	}
	//	1---0
	//	|   |
	//	2---3
	iter = 0;
	for (int i = 0; i < grids; i++) {
		for (int j = 0; j < grids; j++) {
			unsigned int p1 = i * (grids + 1) + j;
			unsigned int p0 = p1 + 1;
			unsigned int p2 = p1 + (grids + 1);
			unsigned int p3 = p0 + (grids + 1);
			top_indices[iter] = p1;
			bottom_indices[iter++] = p1;
			top_indices[iter] = p2;
			bottom_indices[iter++] = p3;
			top_indices[iter] = p3;
			bottom_indices[iter++] = p2;
			top_indices[iter] = p1;
			bottom_indices[iter++] = p1;
			top_indices[iter] = p3;
			bottom_indices[iter++] = p0;
			top_indices[iter] = p0;
			bottom_indices[iter++] = p3;
		}
	}
}

double Cloth::dis(double x1, double y1, double z1, double x2, double y2, double z2)
{
	double dx = x1 - x2, dy = y1 - y2, dz = z1 - z2;
	return sqrt(dx * dx + dy * dy + dz * dz);
}

void Cloth::calc_v(double* outv, double* v, float t, double* a)
{
	for (int i = 0; i <= grids; i++) {
		for (int j = 0; j <= grids; j++) {
			double ax = a[i * (grids + 1) * 3 + j * 3];
			double ay = a[i * (grids + 1) * 3 + j * 3 + 1];
			double az = a[i * (grids + 1) * 3 + j * 3 + 2];
			double vx = v[i * (grids + 1) * 3 + j * 3];
			double vy = v[i * (grids + 1) * 3 + j * 3 + 1];
			double vz = v[i * (grids + 1) * 3 + j * 3 + 2];
			outv[i * (grids + 1) * 3 + j * 3] = vx + t * ax;
			outv[i * (grids + 1) * 3 + j * 3 + 1] = vy + t * ay;
			outv[i * (grids + 1) * 3 + j * 3 + 2] = vz + t * az;
		}
	}
}

void Cloth::calc_a(double* outa, float* p, double* v)
{
	int directions[] = { 0, 1, 1, 0, 1, 1, 1, -1, 0, 2, 2, 0 };
	memset(outa, 0.0, particle_cnt * sizeof(outa[0]));
	for (int i = 0; i <= grids; i++) {
		for (int j = 0; j <= grids; j++) {
			double p1x = p[i * (grids + 1) * 3 + j * 3];
			double p1y = p[i * (grids + 1) * 3 + j * 3 + 1];
			double p1z = p[i * (grids + 1) * 3 + j * 3 + 2];
			double v1x = v[i * (grids + 1) * 3 + j * 3];
			double v1y = v[i * (grids + 1) * 3 + j * 3 + 1];
			double v1z = v[i * (grids + 1) * 3 + j * 3 + 2];
			double& a1x = outa[i * (grids + 1) * 3 + j * 3];
			double& a1y = outa[i * (grids + 1) * 3 + j * 3 + 1];
			double& a1z = outa[i * (grids + 1) * 3 + j * 3 + 2];
			// 弹簧力与阻尼力
			for (int ii = 0; ii < 12; ii += 2) {
				if ((i + directions[ii]) < 0 || (i + directions[ii]) > grids || (j + directions[ii + 1]) < 0 || (j + directions[ii + 1]) > grids) continue;
				double p2x = p[(i + directions[ii]) * (grids + 1) * 3 + (j + directions[ii + 1]) * 3];
				double p2y = p[(i + directions[ii]) * (grids + 1) * 3 + (j + directions[ii + 1]) * 3 + 1];
				double p2z = p[(i + directions[ii]) * (grids + 1) * 3 + (j + directions[ii + 1]) * 3 + 2];
				double v2x = v[(i + directions[ii]) * (grids + 1) * 3 + (j + directions[ii + 1]) * 3];
				double v2y = v[(i + directions[ii]) * (grids + 1) * 3 + (j + directions[ii + 1]) * 3 + 1];
				double v2z = v[(i + directions[ii]) * (grids + 1) * 3 + (j + directions[ii + 1]) * 3 + 2];
				double& a2x = outa[(i + directions[ii]) * (grids + 1) * 3 + (j + directions[ii + 1]) * 3];
				double& a2y = outa[(i + directions[ii]) * (grids + 1) * 3 + (j + directions[ii + 1]) * 3 + 1];
				double& a2z = outa[(i + directions[ii]) * (grids + 1) * 3 + (j + directions[ii + 1]) * 3 + 2];
				double l = (ii < 4) ? l1 : ((ii < 8) ? l2 : l3);
				double d = dis(p1x, p1y, p1z, p2x, p2y, p2z);
				double tmp1 = ks * (d - l) / d; // 胡克-标量项
				a1x += tmp1 * (p2x - p1x) / m;
				a2x += tmp1 * (p1x - p2x) / m;
				a1y += tmp1 * (p2y - p1y) / m;
				a2y += tmp1 * (p1y - p2y) / m;
				a1z += tmp1 * (p2z - p1z) / m;
				a2z += tmp1 * (p1z - p2z) / m;
				double tmp2 = kd * ((p2x - p1x) * (v2x - v1x) + (p2y - p1y) * (v2y - v1y) + (p2z - p1z) * (v2z - v1z)) / pow(d, 2); // 阻尼-标量项
				a1x += tmp2 * (p2x - p1x) / m;
				a2x += tmp2 * (p1x - p2x) / m;
				a1y += tmp2 * (p2y - p1y) / m;
				a2y += tmp2 * (p1y - p2y) / m;
				a1z += tmp2 * (p2z - p1z) / m;
				a2z += tmp2 * (p1z - p2z) / m;
			}
			// 重力
			a1y -= gravity / m;
		}
	}
}

void Cloth::calc_p(float* outp, float* p, float t, double* v)
{
	for (int i = 0; i <= grids; i++) {
		for (int j = 0; j <= grids; j++) {
			double vx = v[i * (grids + 1) * 3 + j * 3];
			double vy = v[i * (grids + 1) * 3 + j * 3 + 1];
			double vz = v[i * (grids + 1) * 3 + j * 3 + 2];
			float px = p[i * (grids + 1) * 3 + j * 3];
			float py = p[i * (grids + 1) * 3 + j * 3 + 1];
			float pz = p[i * (grids + 1) * 3 + j * 3 + 2];
			outp[i * (grids + 1) * 3 + j * 3] = px + (float)vx * t;
			outp[i * (grids + 1) * 3 + j * 3 + 1] = py + (float)vy * t;
			outp[i * (grids + 1) * 3 + j * 3 + 2] = pz + (float)vz * t;
		}
	}
}
