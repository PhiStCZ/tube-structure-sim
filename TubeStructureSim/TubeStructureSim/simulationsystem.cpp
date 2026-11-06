#include "simulationsystem.hpp"
#include <math.h>
#include <stdexcept>
#include <string>


float limit(float value, float bound)
{
	return std::max(std::min(bound, value), -bound);
}
void apply_gravity(node& n, vector2f gravity)
{
	n.velocity += gravity;
}
void update_node_broken_status(node& n)
{
	if (n.isFirm) return;
	for (auto&& t : n.connectedTubes) {
		if (!t->isBroken) return;
	}
	n.isBroken = true;
}
void break_tube(tube& t)
{
	t.isBroken = true;
	float half_t_weight = t.weight() / 2;
	t.n1->weight -= half_t_weight;
	t.n2->weight -= half_t_weight;
	update_node_broken_status(*t.n1);
	update_node_broken_status(*t.n2);
}
void handle_collision(tube& t, node& n) {
	vector2f n1_to_n = n.position - t.n1->position;
	vector2f dir = (t.n2->position - t.n1->position).unit();
	vector2f normal = vector2f::rotate90deg_right(dir);
	float projection = vector2f::dotProduct(n1_to_n, dir);
	float side = vector2f::dotProduct(normal, n1_to_n);
	float last_side = vector2f::dotProduct(
		vector2f::rotate90deg_right((t.n2->lastPosition - t.n1->lastPosition).unit()),
		n.lastPosition - t.n1->lastPosition
	);
	
	if (projection >= 0 && projection <= t.currentLength() && (
		((last_side > 0) && (side < TUBE_THICKNESS))
		|| ((last_side < 0) && (side > -TUBE_THICKNESS))
		)) {
		
		// reverse velocities (IF they are aiming towards one another) and apply displacement
		float divide = projection / t.length;
		vector2f relative_v_n = n.velocity - ((t.n2->velocity * divide) + (t.n1->velocity * (1 - divide)));
		vector2f normal_v = normal * vector2f::dotProduct(normal, relative_v_n);

		if (last_side < 0 != vector2f::dotProduct(normal, relative_v_n) < 0) {
			vector2f b;
			if (!n.isFirm) {
				b = normal_v;
				n.velocity -= b * (1 + t.material->bounciness);
				n.position -= b;
			}
			if (!t.n1->isFirm) {
				b = normal_v * (1 - divide);
				t.n1->velocity += b * (1 + t.material->bounciness);
				t.n1->position += b;
			}
			if (!t.n2->isFirm) {
				b = normal_v * divide;
				t.n2->velocity += b * (1 + t.material->bounciness);
				t.n2->position += b;
			}
		}

		// calc friction
		auto paralel_v = relative_v_n - normal_v;
		auto s = paralel_v.size();
		if (s != 0) {
			auto frictional_force = std::min(t.material->frictionForce, n.weight * s);
			//auto frictional_force = s * t.material->frictionForce * 100;
			auto f = paralel_v * (frictional_force / s);
			n.force -=  f;
			t.n1->force += f * (1 - divide);
			t.n2->force += f * divide;
		}
	}
}

void tb_str_simulator::update()
{
	applyGravity();
	recalculateStructuralForces();
	move();
	handleCollisions();
}
void tb_str_simulator::applyGravity()
{
	for (auto&& n : tb_str_.nodes) {
		n.velocity += gravity;
	}
}
void tb_str_simulator::recalculateStructuralForces()
{
	for (auto&& t : tb_str_.tubes) {
		if (!t.isBroken) {
			vector2f dir = (t.n1->position - t.n2->position).unit(); // pointing towards n1

			float inner_force = log2f(t.length / t.currentLength()) * t.material->structural_force;

			if (limit(inner_force, t.material->break_force) != inner_force) {
				break_tube(t);
			}
			else {
				inner_force = limit(inner_force, t.material->max_force);

				// calculate force from velocity (to avoid too much vibration)
				float velocity_resistance_force = vector2f::dotProduct(t.n2->velocity - t.n1->velocity, dir) * t.weight() * 0.5f * t.material->velocity_resistance;
				inner_force += velocity_resistance_force;

				dir *= inner_force;
				t.n1->force += dir;
				t.n2->force -= dir;
			}
		}
	}
}
void tb_str_simulator::move()
{
	for(auto&& n : tb_str_.nodes) {
		if (!n.isFirm) {
			n.position += n.moveBuffer;
			n.velocity += n.force / n.weight; // (a = F / m)
			n.lastPosition = n.position;
			n.position += n.velocity;
			n.velocity = n.velocity * DAMPEN_CONSTANT; // dampen
			n.force.x = 0; n.force.y = 0;
			n.moveBuffer.x = 0; n.moveBuffer.y = 0;
		}
		else {
			n.velocity.x = 0;
			n.velocity.y = 0;
		}
	}
}
void tb_str_simulator::handleCollisions()
{
	sortPerformanceContainers();
	constexpr float reserve = 2 * TUBE_THICKNESS;
	auto ni = npv.begin();
	for (auto&& t : tpv) {
		while ((*ni)->position.x < std::min(t->n1->position.x,t->n2->position.x) - reserve) {
			++ni;
		}
		for (auto nj = ni; nj != npv.end() && (*nj)->position.x < std::max(t->n1->position.x, t->n2->position.x) + reserve; ++nj) {
			// if n is not in t (and some collision hasn't already been handled between those before, i guess?), then handle collision
			if (!t->isBroken && !(*nj)->isBroken && (*nj) != t->n1 && (*nj) != t->n2) {
				handle_collision(*t, **nj);
			}
		}
	}
}

void tb_str_simulator::bindPerformanceContainers()
{
	for (std::size_t i = 0; i < tb_str_.tubes.size(); ++i) {
		tpv[i] = &tb_str_.tubes[i];
	}
	for (std::size_t i = 0; i < tb_str_.nodes.size(); ++i) {
		npv[i] = &tb_str_.nodes[i];
	}
}
void tb_str_simulator::sortPerformanceContainers()
{
	std::sort(tpv.begin(), tpv.end(), [](tube* a, tube* b) {
		return std::min(a->n1->position.x, a->n2->position.x) < std::min(b->n1->position.x, b->n2->position.x);
		});
	std::sort(npv.begin(), npv.end(), [](node* a, node* b) {
		return a->position.x < b->position.x;
		});
}



void tb_str_builder::insertFirmNode(vector2f position)
{
	if (state_ != state::building) throw std::runtime_error("unexpected state");

	for (auto&& n : tb_str_.nodes) {
		if (n.position == position) {
			n.isFirm = true;
			return;
		}
	}
	tb_str_.nodes.push_back(node(position, true));
}
void tb_str_builder::insertTube(vector2f p1, vector2f p2, tube_material* material)
{
	if (state_ != state::building) throw std::runtime_error("unexpected state");
	if (p1 == p2) throw std::runtime_error("tube endpoint positions must not equal");
	if (vector2f::distance(p1, p2) < material->min_length)
		throw std::runtime_error("The tube is too short for given material (minimum length is " + std::to_string(material->min_length) + ")");

	std::size_t n1i = SIZE_MAX;
	std::size_t n2i = SIZE_MAX;
	for (std::size_t i = 0; i < tb_str_.nodes.size(); ++i) {
		auto&& p = tb_str_.nodes[i].position;
		if (p == p1) n1i = i;
		else if (p == p2) n2i = i;
	}
	if (n1i == SIZE_MAX) {
		n1i = tb_str_.nodes.size();
		tb_str_.nodes.push_back({ p1, false });
	}
	if (n2i == SIZE_MAX) {
		n2i = tb_str_.nodes.size();
		tb_str_.nodes.push_back({ p2, false });
	}
	tb_str_.tubes.push_back({ 0, 0, vector2f::distance(p1, p2), material });
	tubeEndPoints_.push_back(std::pair<std::size_t, std::size_t>(n1i, n2i));
}

void tb_str_builder::bindPointers()
{
	state_ = state::ptr_bound;
	for (std::size_t i = 0; i < tubeEndPoints_.size(); ++i) {
		{
			auto&& n1 = tb_str_.nodes[tubeEndPoints_[i].first];
			tb_str_.tubes[i].n1 = &n1;
			n1.connectedTubes.push_back(&tb_str_.tubes[i]);
		}
		{
			auto&& n2 = tb_str_.nodes[tubeEndPoints_[i].second];
			tb_str_.tubes[i].n2 = &n2;
			n2.connectedTubes.push_back(&tb_str_.tubes[i]);
		}
	}
}
void tb_str_builder::calculateWeights()
{
	// this can theoretically be done at any time, but its better at the end
	for (auto&& n : tb_str_.nodes) {
		n.weight = 0;
		for (auto&& t : n.connectedTubes) {
			n.weight += t->length * t->material->density / 2;
		}
	}
}
tb_str_simulator tb_str_builder::createSimulationFromCurrentState()
{
	if (state_ != state::building) throw std::runtime_error("unexpected state");
	
	bindPointers();
	calculateWeights();
	state_ = state::invalid;
	
	return { std::move(tb_str_) };
}
