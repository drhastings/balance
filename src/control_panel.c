#include "../includes/control_panel.h"
#include "../includes/state.h"

struct string *count_out(struct variable *var, struct string *string)
{
  char buffer[32];

  memset(buffer, 0, 32);

  sprintf(buffer, "%f", *(float *)var->value);

//  (*(int *)var->value)++;

  return new_string(buffer);
}

void *count_in(struct variable *var, struct string *string)
{
  (*(float *)var->value) = atof(string->buffer);

  return 0;
}

void remote(struct robot *robot)
{
	robot->rotational_velocity_setpoint = -robot->in_spin * robot->rotational_velocity_max;

	robot->linear_velocity_setpoint = robot->in_speed * robot->linear_velocity_max;
}

void start_control_panel(struct robot *robot, CUBE_STATE_T *state)
{
	robot->server = new_server("Robot", 80);

  struct node *node = new_node("html", NULL, NULL);

  struct page *page = new_page("/spin_loop.html", node, robot->server);

  node = new_node("head", NULL, page->root);

  node = new_node("body", NULL, page->root);

	struct property *prop = new_property("method", "post", NULL);

  node = new_node("form", prop, node);

  struct variable *var;

	struct text *text = new_text("Tilt offset:", node);

	prop = new_property("value", "10", NULL);

  var = new_variable(page, "tilt_offset", robot->tilt_loop->target, 4, count_in, count_out);
	
	append_to_property(prop, new_property("name", "tilt_offset", NULL));

	prop->value->variable = var;

  new_node("input", prop, node);

  new_node("br", NULL, node);

	text = new_text("Left Motor Boost:", node);

	prop = new_property("value", "10", NULL);

  var = new_variable(page, "left_boost", &robot->left_motor.boost, 4, count_in, count_out);
	
	append_to_property(prop, new_property("name", "left_boost", NULL));

	prop->value->variable = var;

  new_node("input", prop, node);

	text = new_text("Right Motor Boost:", node);

	prop = new_property("value", "10", NULL);

  var = new_variable(page, "right_boost", &robot->right_motor.boost, 4, count_in, count_out);
	
	append_to_property(prop, new_property("name", "right_boost", NULL));

	prop->value->variable = var;

  new_node("input", prop, node);

  new_node("br", NULL, node);

	new_text("Tilt P:", node);

	prop = new_property("value", "10", NULL);

  var = new_variable(page, "tilt_p", &robot->tilt_loop->P, 4, count_in, count_out);
	
	append_to_property(prop, new_property("name", "tilt_p", NULL));

	prop->value->variable = var;

  new_node("input", prop, node);

	new_text("Tilt D:", node);

	prop = new_property("value", "10", NULL);

  var = new_variable(page, "tilt_d", &robot->tilt_loop->D, 4, count_in, count_out);
	
	append_to_property(prop, new_property("name", "tilt_d", NULL));

	prop->value->variable = var;

  new_node("input", prop, node);

	new_text("Tilt Min:", node);

	prop = new_property("value", "10", NULL);

  var = new_variable(page, "tilt_min", &robot->tilt_loop->min, 4, count_in, count_out);
	
	append_to_property(prop, new_property("name", "tilt_min", NULL));

	prop->value->variable = var;

  new_node("input", prop, node);

	new_text("Tilt Max:", node);

	prop = new_property("value", "10", NULL);

  var = new_variable(page, "tilt_max", &robot->tilt_loop->max, 4, count_in, count_out);
	
	append_to_property(prop, new_property("name", "tilt_max", NULL));

	prop->value->variable = var;

  new_node("input", prop, node);

  new_node("br", NULL, node);

	text = new_text("Linear P:", node);

	prop = new_property("value", "10", NULL);

  var = new_variable(page, "linear_p", &robot->linear_loop->P, 4, count_in, count_out);
	
	append_to_property(prop, new_property("name", "linear_p", NULL));

	prop->value->variable = var;

  new_node("input", prop, node);

	new_text("Linear D:", node);

	prop = new_property("value", "10", NULL);

  var = new_variable(page, "linear_d", &robot->linear_loop->D, 4, count_in, count_out);
	
	append_to_property(prop, new_property("name", "linear_d", NULL));

	prop->value->variable = var;

  new_node("input", prop, node);

	new_text("Linear Min:", node);

	prop = new_property("value", "10", NULL);

  var = new_variable(page, "linear_min", &robot->linear_loop->min, 4, count_in, count_out);
	
	append_to_property(prop, new_property("name", "linear_min", NULL));

	prop->value->variable = var;

  new_node("input", prop, node);

	new_text("Linea Max:", node);

	prop = new_property("value", "10", NULL);

  var = new_variable(page, "linear_max", &robot->linear_loop->max, 4, count_in, count_out);
	
	append_to_property(prop, new_property("name", "linear_max", NULL));

	prop->value->variable = var;

  new_node("input", prop, node);

  new_node("br", NULL, node);

	text = new_text("Spin P:", node);

	prop = new_property("value", "10", NULL);

  var = new_variable(page, "spin_p", &robot->spin_loop->P, 4, count_in, count_out);
	
	append_to_property(prop, new_property("name", "spin_p", NULL));

	prop->value->variable = var;

  new_node("input", prop, node);

	new_text("Spin D:", node);

	prop = new_property("value", "10", NULL);

  var = new_variable(page, "spin_d", &robot->spin_loop->D, 4, count_in, count_out);
	
	append_to_property(prop, new_property("name", "spin_d", NULL));

	prop->value->variable = var;

  new_node("input", prop, node);

	new_text("Spin Min:", node);

	prop = new_property("value", "10", NULL);

  var = new_variable(page, "spin_min", &robot->spin_loop->min, 4, count_in, count_out);
	
	append_to_property(prop, new_property("name", "spin_min", NULL));

	prop->value->variable = var;

  new_node("input", prop, node);

	new_text("Spin Max:", node);

	prop = new_property("value", "10", NULL);

  var = new_variable(page, "spin_max", &robot->spin_loop->max, 4, count_in, count_out);
	
	append_to_property(prop, new_property("name", "spin_max", NULL));

	prop->value->variable = var;

  new_node("input", prop, node);

  new_node("br", NULL, node);

	text = new_text("Linear Max Speed :", node);

	prop = new_property("value", "10", NULL);

  var = new_variable(page, "l_max", &robot->linear_velocity_max, 4, count_in, count_out);
	
	append_to_property(prop, new_property("name", "l_max", NULL));

	prop->value->variable = var;

  new_node("input", prop, node);

	new_text("Linear Acceleration:", node);

	prop = new_property("value", "10", NULL);

  var = new_variable(page, "l_accel", &robot->linear_acceleration, 4, count_in, count_out);
	
	append_to_property(prop, new_property("name", "l_accel", NULL));

	prop->value->variable = var;

  new_node("input", prop, node);

	new_text("Spin Velocity Max:", node);

	prop = new_property("value", "10", NULL);

  var = new_variable(page, "s_max", &robot->rotational_velocity_max, 4, count_in, count_out);
	
	append_to_property(prop, new_property("name", "s_max", NULL));

	prop->value->variable = var;

  new_node("input", prop, node);

	new_text("Spin Acceleration:", node);

	prop = new_property("value", "10", NULL);

  var = new_variable(page, "s_accel", &robot->rotational_acceleration, 4, count_in, count_out);
	
	append_to_property(prop, new_property("name", "s_accel", NULL));

	prop->value->variable = var;

  new_node("input", prop, node);

  new_node("br", NULL, node);

	new_text("Wheel base:", node);

	prop = new_property("value", "10", NULL);

  var = new_variable(page, "wheel_base", &robot->position.wheel_base, 4, count_in, count_out);
	
	append_to_property(prop, new_property("name", "wheel_base", NULL));

	prop->value->variable = var;

  new_node("input", prop, node);

  new_node("br", NULL, node);

	new_text("Linear Seek:", node);

	prop = new_property("value", "10", NULL);

  var = new_variable(page, "line_seek", &robot->linear_seek_const, 4, count_in, count_out);
	
	append_to_property(prop, new_property("name", "line_seek", NULL));

	prop->value->variable = var;

  new_node("input", prop, node);

	new_text("Spin Seek:", node);

	prop = new_property("value", "10", NULL);

  var = new_variable(page, "spin_seek", &robot->spin_seek_const, 4, count_in, count_out);
	
	append_to_property(prop, new_property("name", "spin_seek", NULL));

	prop->value->variable = var;

  new_node("input", prop, node);

  new_node("br", NULL, node);

	new_text("Pan:", node);

	prop = new_property("value", "10", NULL);

  var = new_variable(page, "pan", &robot->turret.pan, 4, count_in, count_out);
	
	append_to_property(prop, new_property("name", "pan", NULL));

	prop->value->variable = var;

  new_node("input", prop, node);

	new_text("Tilt:", node);

	prop = new_property("value", "10", NULL);

  var = new_variable(page, "tilt", &robot->turret.tilt, 4, count_in, count_out);
	
	append_to_property(prop, new_property("name", "tilt", NULL));

	prop->value->variable = var;

  new_node("input", prop, node);

  new_node("br", NULL, node);

  new_text("U value:", node);

	prop = new_property("value", "10", NULL);

  var = new_variable(page, "u_value", &state->color[0], 4, count_in, count_out);
	
	append_to_property(prop, new_property("name", "u_value", NULL));

	prop->value->variable = var;

  new_node("input", prop, node);

	new_text("V value:", node);

	prop = new_property("value", "10", NULL);

  var = new_variable(page, "v_value", &state->color[1], 4, count_in, count_out);
	
	append_to_property(prop, new_property("name", "v_value", NULL));

	prop->value->variable = var;

  new_node("input", prop, node);

  new_node("br", NULL, node); 

	prop = new_property("type", "submit", NULL);

  new_node("input", prop, node);

  node = new_node("html", NULL, NULL);

  page = new_page("/remote.html", node, robot->server);

  node = new_node("head", NULL, page->root);

  node = new_node("body", NULL, page->root);

  var = new_variable(page, "spin", &robot->in_spin, 4, count_in, count_out);

	var = new_variable(page, "speed", &robot->in_speed, 4, count_in, count_out);

	var = new_variable(page, "follow", &robot->is_following, 4, count_in, count_out);

	var = new_variable(page, "new_point", robot, 4, coord_in, NULL);

	var = new_variable(page, "end_point", robot, 4, coord_in_last, NULL);

	var = new_variable(page, "clear", robot, 4, coord_clear, NULL);

	var = new_variable(page, "loop", robot, 4, coord_loop, NULL);

  node = new_node("odometry", NULL, NULL);

  page = new_page("/location.html", node, robot->server);

  var = new_variable(page, "pos_x", &robot->position2.position.x, 4, count_in, count_out);

	text = new_text("0", node);

	text->contents->variable = var;

  var = new_variable(page, "pos_y", &robot->position2.position.y, 4, count_in, count_out);

	text = new_text(",", node);

	text = new_text("0", node);

	text->contents->variable = var;

	text = new_text(",", node);

  var = new_variable(page, "heading", &robot->position2.heading, 4, count_in, count_out);

	text = new_text("0", node);

	text->contents->variable = var;

	text = new_text(",", node);

  var = new_variable(page, "inertial_x", &robot->position.x_inertial, 4, count_in, count_out);

	text = new_text("0", node);

	text->contents->variable = var;

	text = new_text(",", node);

  var = new_variable(page, "inertial_y", &robot->position.y_inertial, 4, count_in, count_out);

	text = new_text("0", node);

	text->contents->variable = var;

	text = new_text(",", node);

  var = new_variable(page, "inertial_heading", &robot->position.incr_inertial_heading, 4, count_in, count_out);

	text = new_text("0", node);

	text->contents->variable = var;

	text = new_text(",", node);
 
 var = new_variable(page, "following", &robot->is_following, 4, count_in, count_out);

	text = new_text("0", node);

	text->contents->variable = var;

	text = new_text(",", node);
 
	var = new_variable(page, "dest", robot, 4, coord_in, coord_out);

	text = new_text("0", node);

	text->contents->variable = var;

	text = new_text(",", node);
 
	var = new_variable(page, "tilt", &robot->position.tilt, 4, count_in, count_out);

	text = new_text("0", node);

	text->contents->variable = var;

	text = new_text(",", node);
 
	var = new_variable(page, "left_count", &robot->left_count, 4, count_in, count_out);

	text = new_text("0", node);

	text->contents->variable = var;

	text = new_text(",", node);
 
	var = new_variable(page, "right_count", &robot->right_count, 4, count_in, count_out);

	text = new_text("0", node);

	text->contents->variable = var;

  node = new_node("graphs", NULL, NULL);

  page = new_page("/graph_output.html", node, robot->server);

  var = new_variable(page, "tilt", &robot->position.traction_value, 4,
		count_in, count_out);

	text = new_text("0", node);

	text->contents->variable = var;

	text = new_text(",", node);

  var = new_variable(page, "position", &robot->position.linear_count, 4,
		count_in, count_out);

	text = new_text("0", node);

	text->contents->variable = var;

}

