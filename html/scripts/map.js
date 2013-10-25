var pad_radius = 20;

var id;

var sticks = new Array;

function is_touch_device() {  
  try {  
    document.createEvent("TouchEvent");  
    return true;  
  } catch (e) {  
    return false;  
  }  
}

function joystick (size_x, size_y, x_min, x_max, y_min, y_max)
{
	this.canvas = document.createElement("canvas");

	document.body.appendChild(this.canvas);

	this.canvas.width = size_x;

	this.canvas.height = size_y;

	if (this.canvas.width > this.canvas.height)
	{
		this.max = this.canvas.height / 2 - pad_radius;
	}
	else
	{
		this.max = this.canvas.width / 2 - pad_radius;
	}

	this.x_min = x_min;

	this.x_max = x_max;

	this.y_min = y_min;

	this.y_max = y_max;

	this.x_value = this.canvas.width / 2;

	this.y_value = this.canvas.height / 2;

	this.finger = null;

	function start(event) {
		if (!this.finger)
		{
			this.finger = event.changedTouches[0];

			this.x_value = this.finger.clientX - (window.innerWidth - 260);

			this.y_value = this.finger.clientY - (window.innerHeight - 260);

			this.boundaries();
		}
	}

	function mouse_start(event) {
		this.x_value = event.clientX - (window.innerWidth - 260);

		this.y_value = event.clientY - (window.innerHeight - 260);

		this.boundaries();

		this.move_func = mouse_move.bind(this);

		this.canvas.addEventListener('mousemove', this.move_func, 0);
	}

	function end(event) {
		if (this.finger.identifier == event.changedTouches[0].identifier)
		{
			this.x_value = this.canvas.width / 2;

			this.y_value = this.canvas.height / 2;

			this.finger = null;
		}
	}

	function mouse_end(event) {
		this.x_value = this.canvas.width / 2;

		this.y_value = this.canvas.height / 2;

		this.canvas.removeEventListener('mousemove', this.move_func, 0);
	}

	function move(event) {
		if (this.finger.identifier == event.changedTouches[0].identifier);
		{
			this.x_value = this.finger.clientX - (window.innerWidth - 260);

			this.y_value = this.finger.clientY - (window.innerHeight - 260);

			this.boundaries();
		}
	}

	function mouse_move(event) {
		this.x_value = event.clientX - (window.innerWidth - 260);

		this.y_value = event.clientY - (window.innerHeight - 260);

		this.boundaries();
	}

	if (is_touch_device())
	{
		this.canvas.addEventListener('touchstart', start.bind(this), 0);
		this.canvas.addEventListener('touchend', end.bind(this), 0);
		this.canvas.addEventListener('touchmove', move.bind(this), 0);
	}
	else
	{
		this.canvas.addEventListener('mousedown', mouse_start.bind(this), 0);
		this.canvas.addEventListener('mouseup', mouse_end.bind(this), 0);
	}
}

function send_data()
{
	var xmlhttp;
	if (window.XMLHttpRequest)
	{// code for IE7+, Firefox, Chrome, Opera, Safari
		  xmlhttp=new XMLHttpRequest();
	}
	else
  {// code for IE6, IE5
  	xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
  }
	xmlhttp.onreadystatechange=function()
  {
	  if (xmlhttp.readyState==4 && xmlhttp.status==200)
	  {
  	  document.getElementById("myDiv").innerHTML=xmlhttp.responseText;
	  }
  }
	xmlhttp.open("POST","remote.html",true);
	var spin = "spin="
	var speed = "&speed="
	var follow = "&follow=0"

	var x = stick.getX().toFixed(5);
	var y = stick.getY().toFixed(5);

	if (x == 0 && y == 0) follow = "";

	var message = spin.concat(x, speed, y, follow);
	xmlhttp.send(message); 
}

function send_timeout()
{
	send_data();

	id = window.setTimeout(send_timeout, 100);
}

joystick.prototype.getX = function()
{
	if (this.x_value > this.canvas.width / 2)
	{
		return (this.x_value - this.canvas.width / 2) / this.max;
	}
	else
	{
		return -(this.canvas.width / 2 - this.x_value) / this.max;
	}
}

joystick.prototype.getY = function()
{
	if (this.y_value > this.canvas.height / 2)
	{
		return -(this.y_value - this.canvas.height / 2) / this.max;
	}
	else
	{
		return (this.canvas.height / 2 - this.y_value) / this.max;
	}
}

joystick.prototype.boundaries = function()
{
	if (this.x_value > (this.x_max * this.max) + (this.canvas.width / 2))
	{
		this.x_value = (this.x_max * this.max) + (this.canvas.width / 2); 
	}

	if (this.x_value < (this.canvas.width / 2) - (this.x_min * this.max))
	{
		this.x_value = ((this.canvas.width / 2)) - (this.x_min * this.max);
	}

	if (this.y_value > (this.y_max * this.max) + (this.canvas.height / 2))
	{
		this.y_value = (this.y_max * this.max) + (this.canvas.height / 2); 
	}

	if (this.y_value < (this.canvas.height / 2) - (this.y_min * this.max))
	{
		this.y_value = ((this.canvas.height / 2)) - (this.y_min * this.max);
	}
}

joystick.prototype.draw = function()
{
	this.context = this.canvas.getContext("2d");

	this.context.clearRect(0, 0, this.canvas.width, this.canvas.height);

	this.context.beginPath();

	this.context.arc(this.x_value, this.y_value, pad_radius, 0, 2 * Math.PI);

	var gradient = this.context.createRadialGradient(this.x_value, this.y_value, 5, this.x_value, this.y_value, 37);

	gradient.addColorStop(0, "Red");

	gradient.addColorStop(1, "Orange");

	this.context.fillStyle = gradient;

	this.context.closePath();

	this.context.fill();

	var wrapper = this.draw.bind(this);

	window.webkitRequestAnimationFrame(wrapper);
}

function map(width, height, scale, path)
{
	this.canvas = document.createElement("canvas");

	document.body.appendChild(this.canvas);

	this.canvas.width = width;

	this.canvas.height = height;

	this.scale = scale;

	this.pos_x = 0;

	this.pos_y = 0;

	this.heading = 0;

	this.inertial_x = 0;

	this.inertial_y = 0;

	this.inertial_heading = 0;

	this.x_hist = [];

	this.y_hist = [];

	this.inertial_x_hist = [];

	this.inertial_y_hist = [];

	this.waypoint_x = [];

	this.waypoint_y = [];

	this.trail_length = 15;

	this.x_dest = null;

	this.y_dest = null;
}

map.prototype.draw = function()
{
	var context = this.canvas.getContext("2d");

	context.clearRect(0, 0, this.canvas.width, this.canvas.height);

	context.font = "bold 12px sans-serif";

	context.textBaseline = "middle";

	context.stroke();

	context.beginPath();

	var grid_line = this.canvas.height / 2;

	var first_number = ((this.canvas.height / 2) / (50 * this.scale) >> 0 ) * .5

	grid_line %= (50 * this.scale);

	grid_line = Math.round(grid_line) - .5;

	for (grid_line; grid_line < this.canvas.height; grid_line += (50 * this.scale))
	{
		context.moveTo(30, grid_line);

		context.lineTo(this.canvas.width, grid_line);

		context.fillText(first_number, 10, grid_line);

		first_number -= .5;
	}

	context.textBaseline = "bottom";

	context.textAlign = "center";

	first_number = ((this.canvas.width / 2) / (50 * this.scale) >> 0 ) * -.5;

	grid_line = this.canvas.width / 2;

	grid_line %= (50 * this.scale);

	grid_line = Math.round(grid_line) - .5;

	for (grid_line; grid_line < this.canvas.width; grid_line += (50 * this.scale))
	{
		if (grid_line > 30)
		{
			context.moveTo(grid_line, 0);

			context.lineTo(grid_line, this.canvas.height - 15);
		}
		context.fillText(first_number, grid_line, this.canvas.height);

		first_number += .5;
	}

	context.strokeStyle = "#ddd";

	context.stroke();

	context.beginPath();

	context.moveTo((this.canvas.width / 2) - (this.y_hist[0] / 10), (this.canvas.height / 2) - (this.x_hist[0] / 10));

	for (x = 0; x < this.x_hist.length; x++)
	{
		context.lineTo((this.canvas.width / 2) - (this.y_hist[x + 1] / 10), (this.canvas.height / 2) - (this.x_hist[x + 1] / 10));
	}	

	context.lineTo((this.canvas.width / 2) - (this.pos_y / 10), (this.canvas.height / 2) - (this.pos_x / 10));

	context.strokeStyle = "#009933";

	context.stroke();

	context.beginPath();

	context.moveTo((this.canvas.width / 2) - (this.inertial_y_hist[0] / 10), (this.canvas.height / 2) - (this.inertial_x_hist[0] / 10));

	for (x = 0; x < this.inertial_x_hist.length; x++)
	{
		context.lineTo((this.canvas.width / 2) - (this.inertial_y_hist[x + 1] / 10), (this.canvas.height / 2) - (this.inertial_x_hist[x + 1] / 10));
	}	

	context.lineTo((this.canvas.width / 2) - (this.inertial_y / 10), (this.canvas.height / 2) - (this.inertial_x / 10));

	context.strokeStyle = "#FF3300";

	context.stroke();

	for (x = 0; x < this.waypoint_x.length; x++)
	{
		context.beginPath();

		if (x > 0)
		{
			context.moveTo((this.canvas.width / 2) - (this.waypoint_x[x - 1]), (this.canvas.height / 2) - (this.waypoint_y[x - 1]));

			context.lineTo((this.canvas.width / 2) - (this.waypoint_x[x]), (this.canvas.height / 2) - (this.waypoint_y[x]));
		}
		else
		{	
			context.moveTo((this.canvas.width / 2) - (this.inertial_y) / 10, (this.canvas.height / 2) - (this.inertial_x) / 10);

			context.lineTo((this.canvas.width / 2) - (this.waypoint_x[x]), (this.canvas.height / 2) - (this.waypoint_y[x]));
		}
		context.moveTo((this.canvas.width / 2) - (this.waypoint_x[x]) - 5, (this.canvas.height / 2) - (this.waypoint_y[x]));
	
		context.lineTo((this.canvas.width / 2) - (this.waypoint_x[x]) + 5, (this.canvas.height / 2) - (this.waypoint_y[x]));

		context.moveTo((this.canvas.width / 2) - (this.waypoint_x[x]), (this.canvas.height / 2) - (this.waypoint_y[x]) - 5);

		context.lineTo((this.canvas.width / 2) - (this.waypoint_x[x]), (this.canvas.height / 2) - (this.waypoint_y[x]) + 5);

		context.strokeStyle = "#fbb";
		context.stroke();
	}

	if (this.x_dest)
	{
		context.beginPath();

		context.moveTo((this.canvas.width / 2) - (this.y_dest / 10) - 5, (this.canvas.height / 2) - (this.x_dest / 10))

		context.lineTo((this.canvas.width / 2) - (this.y_dest / 10) + 5, (this.canvas.height / 2) - (this.x_dest / 10))

		context.moveTo((this.canvas.width / 2) - (this.y_dest / 10), (this.canvas.height / 2) - (this.x_dest / 10) - 5);

		context.lineTo((this.canvas.width / 2) - (this.y_dest / 10), (this.canvas.height / 2) - (this.x_dest / 10) + 5);

		context.strokeStyle = "#00F";

		context.stroke();
	}

	context.save();

	context.translate((this.canvas.width / 2) - (this.pos_y / 10), (this.canvas.height / 2) - (this.pos_x / 10));

	context.rotate(-this.heading - Math.PI / 2);

	context.beginPath();

	context.moveTo(5, 0);

	context.lineTo(-5, 5);

	context.lineTo(-5, -5);

	context.lineTo(5, 0);

	context.fillStyle = "#003300";

	context.fill();

	context.restore();

	context.save();

	context.translate((this.canvas.width / 2) - (this.inertial_y / 10), (this.canvas.height / 2) - (this.inertial_x / 10));

	context.rotate(-this.inertial_heading - Math.PI / 2);

	context.beginPath();

	context.moveTo(5, 0);

	context.lineTo(-5, 5);

	context.lineTo(-5, -5);

	context.lineTo(5, 0);

	context.fillStyle = "#FF0000";

	context.fill();

	context.restore();

	var wrapper = this.draw.bind(this);

	window.webkitRequestAnimationFrame(wrapper);
}

var timeout;

function fetch_timeout()
{
	fetch_data();

	timeout = window.setTimeout(fetch_timeout, 50);
}

function fetch_data()
{
	var xmlhttp;
	if (window.XMLHttpRequest)
	{// code for IE7+, Firefox, Chrome, Opera, Safari
		  xmlhttp=new XMLHttpRequest();
	}
	else
  {// code for IE6, IE5
  	xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
  }
	xmlhttp.onreadystatechange=function()
  {
	  if (xmlhttp.readyState==4 && xmlhttp.status==200)
	  {
			var message;

  	  message = xmlhttp.responseText;

			message = message.substring(message.indexOf('\n') + 1, message.lastIndexOf('<'));

			var values = message.split(',');

			my_map.pos_x = parseFloat(values[0]);

			my_map.x_hist.push(my_map.pos_x);

			my_map.pos_y = parseFloat(values[1]);

			my_map.y_hist.push(my_map.pos_y);

			if (my_map.x_hist.length > 20 * my_map.trail_length)
			{
				my_map.x_hist.shift();
				my_map.y_hist.shift();
			}

			my_map.heading = parseFloat(values[2]);

			my_map.inertial_x = parseFloat(values[3]);

			my_map.inertial_x_hist.push(my_map.inertial_x);

			my_map.inertial_y = parseFloat(values[4]);

			my_map.inertial_y_hist.push(my_map.inertial_y);

			if (my_map.inertial_x_hist.length > 20 * my_map.trail_length)
			{
				my_map.inertial_x_hist.shift();
				my_map.inertial_y_hist.shift();
			}

			my_map.inertial_heading = parseFloat(values[5]);

			document.getElementById("y_value").innerHTML = (my_map.pos_x / 10).toFixed(2);
			document.getElementById("x_value").innerHTML = (-my_map.pos_y / 10).toFixed(2);
			document.getElementById("y_value_inertial").innerHTML = (my_map.inertial_x / 10).toFixed(2);
			document.getElementById("x_value_inertial").innerHTML = (-my_map.inertial_y / 10).toFixed(2);

			var theta = my_map.heading % Math.PI;

			if (theta < 0) theta += Math.PI;

			document.getElementById("theta_value").innerHTML = theta.toFixed(5);

			theta = my_map.inertial_heading % Math.PI;

			if (theta < 0) theta += Math.PI;

			document.getElementById("inertial_value").innerHTML = theta.toFixed(5);

			if (parseFloat(values[6]))
			{
				following = 1;
				follow_button.style.background = "#0f0";
			}		
			else
			{
				following = 0;
				follow_button.style.background = "#f00";
			}

			if (values[7] != "none")
			{
				my_map.x_dest = parseFloat(values[7]).toFixed(3);
				my_map.y_dest = parseFloat(values[8]).toFixed(3);
			}
			else
			{
				my_map.x_dest = null;
				my_map.y_dest = null;
			}
		}
  }
	xmlhttp.open("GET", "/location.html",true);
	xmlhttp.send();
}

function newsize(event)
{
	if (my_map.canvas)
	{
		my_map.canvas.parentNode.removeChild(my_map.canvas);
	}

	my_map.canvas = document.createElement("canvas");

	document.body.appendChild(my_map.canvas);

	my_map.canvas.width = window.innerWidth;

	my_map.canvas.height = window.innerHeight;

	my_map.canvas.style.position="absolute";
	my_map.canvas.style.top="0px";
	my_map.canvas.style.left="0px";
	my_map.canvas.style.width="100%";
	my_map.canvas.style.height="100%";
	my_map.canvas.style.background="white";
}

window.addEventListener('resize', newsize);

function new_coord()
{
	var xmlhttp;
	if (window.XMLHttpRequest)
	{// code for IE7+, Firefox, Chrome, Opera, Safari
		  xmlhttp=new XMLHttpRequest();
	}
	else
  {// code for IE6, IE5
  	xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
  }
	xmlhttp.open("POST","remote.html",true);

	var message = "end_point=";

	var new_x = document.forms["entry_form"]["new_x_coord"].value;

	var new_y = document.forms["entry_form"]["new_y_coord"].value;

	message = message.concat(new_y * 10, ":", 
		-new_x * 10, ":");

	console.log(message);

	xmlhttp.send(message);

	my_map.waypoint_x.push(-new_x);
	my_map.waypoint_y.push(new_y);

	return false;
}

function map_click(event)
{
	document.forms["entry_form"]["new_x_coord"].value = event.clientX - (my_map.canvas.width / 2);

	document.forms["entry_form"]["new_y_coord"].value = -event.clientY + (my_map.canvas.height / 2);

	new_coord();
}

var my_map = new map(window.innerWidth, window.innerHeight, 1, null);

my_map.canvas.style.position="absolute";
my_map.canvas.style.top="0px";
my_map.canvas.style.left="0px";
my_map.canvas.style.width="100%";
my_map.canvas.style.height="100%";
my_map.canvas.style.background="white";

var node = document.getElementById("x_value");

node.style.width = "80px";
node.style.textAlign = "right";

node = document.getElementById("y_value");

node.style.width = "80px";
node.style.textAlign = "right";

node = document.getElementById("theta_value");

node.style.width = "40px";
node.style.textAlign = "right";

node = document.getElementById("x_value");

node.style.width = "80px";
node.style.textAlign = "right";

node = document.getElementById("y_value");

node.style.width = "80px";
node.style.textAlign = "right";

node = document.getElementById("theta_value");

node.style.width = "40px";
node.style.textAlign = "right";

node = document.getElementById("x_value_inertial");

node.style.textAlign = "right";

node = document.getElementById("y_value_inertial");

node.style.textAlign = "right";

node = document.getElementById("inertial_value");

node.style.textAlign = "right";

node = document.getElementById("positions");

node.style.position = "absolute";
node.style.left = "30px";
node.style.background = "#aaa";
node.style.zIndex = 2;

node = document.getElementById("odometry_title");

node.style.background = "#009933";

node = document.getElementById("mpu_title");

node.style.background = "#ff3300";

node = document.getElementById("coord_entry");

node.style.position = "absolute";
node.style.right = "10px";
node.style.zIndex = 2;

node = document.getElementById("entry_form");

node.onsubmit = new_coord;

var follow_button = document.createElement("div");

document.body.appendChild(follow_button);

follow_button.style.position = "absolute";
follow_button.style.right = "10px";
follow_button.style.bottom = "270px";
follow_button.style.width = "40px";
follow_button.style.height = "40px";
follow_button.style.background = "#f00";
follow_button.style.zIndex = 2;

var center_button = document.createElement("div");

node = center_button.appendChild(document.createElement("span"));

node.innerHTML = "(0,0)";

node.style.textAlign = "middle";

document.body.appendChild(center_button);

center_button.style.position = "absolute";
center_button.style.textAlign = "center";
center_button.style.right = "60px";
center_button.style.bottom = "270px";
center_button.style.width = "40px";
center_button.style.height = "40px";
center_button.style.background = "#ccc";
center_button.style.zIndex = 2;

var clear_button = document.createElement("div");

node = clear_button.appendChild(document.createElement("span"));

node.innerHTML = "clear";

node.style.textAlign = "middle";

document.body.appendChild(clear_button);

clear_button.style.position = "absolute";
clear_button.style.textAlign = "center";
clear_button.style.right = "110px";
clear_button.style.bottom = "270px";
clear_button.style.width = "40px";
clear_button.style.height = "40px";
clear_button.style.background = "#ccc";
clear_button.style.zIndex = 2;

var loop_button = document.createElement("div");

node = loop_button.appendChild(document.createElement("span"));

node.innerHTML = "loop";

node.style.textAlign = "middle";

document.body.appendChild(loop_button);

loop_button.style.position = "absolute";
loop_button.style.textAlign = "center";
loop_button.style.right = "160px";
loop_button.style.bottom = "270px";
loop_button.style.width = "40px";
loop_button.style.height = "40px";
loop_button.style.background = "#ccc";
loop_button.style.zIndex = 2;

var following = 0;

var stick = new joystick(250, 250, 1, 1, 1, 1)

stick.canvas.style.position="absolute";
stick.canvas.style.bottom = "10px";
stick.canvas.style.right = "10px";
stick.canvas.style.background="black";
stick.canvas.style.zIndex = 2;

stick.draw();

id = window.setTimeout(send_timeout, 100);

function toggle(event)
{
	if (following)
	{
		following = 0;
		follow_button.style.background = "#f00";
	}
	else
	{
		following = 1;
		follow_button.style.background = "#0f0";
	}
	var xmlhttp;
	if (window.XMLHttpRequest)
	{// code for IE7+, Firefox, Chrome, Opera, Safari
		  xmlhttp=new XMLHttpRequest();
	}
	else
  {// code for IE6, IE5
  	xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
  }
	xmlhttp.open("POST","remote.html",true);
	xmlhttp.send("follow=".concat(following));
}

function center(event)
{
	var xmlhttp;

	clear();

	if (window.XMLHttpRequest)
	{// code for IE7+, Firefox, Chrome, Opera, Safari
		  xmlhttp=new XMLHttpRequest();
	}
	else
  {// code for IE6, IE5
  	xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
  }
	xmlhttp.open("POST","remote.html",true);
	xmlhttp.send("new_point=0:0:0:&follow=1");
}

function clear(event)
{
	var xmlhttp;
	if (window.XMLHttpRequest)
	{// code for IE7+, Firefox, Chrome, Opera, Safari
		  xmlhttp=new XMLHttpRequest();
	}
	else
  {// code for IE6, IE5
  	xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
  }
	xmlhttp.open("POST","remote.html",true);
	xmlhttp.send("clear=0");

	my_map.waypoint_x = [];
	my_map.waypoint_y = [];
}

function loop(event)
{
	var xmlhttp;
	if (window.XMLHttpRequest)
	{// code for IE7+, Firefox, Chrome, Opera, Safari
		  xmlhttp=new XMLHttpRequest();
	}
	else
  {// code for IE6, IE5
  	xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
  }
	xmlhttp.open("POST","remote.html",true);
	xmlhttp.send("loop=0");
}
if (is_touch_device())
{
	follow_button.addEventListener("touchend", toggle, false);
	center_button.addEventListener("touchend", center, false);
	clear_button.addEventListener("touchend", clear, false);
	loop_button.addEventListener("touchend", loop, false);
}
else
{
	follow_button.addEventListener("click", toggle, false);
	center_button.addEventListener("click", center, false);
	clear_button.addEventListener("click", clear, false);
	loop_button.addEventListener("click", loop, false);
}

my_map.canvas.addEventListener("click", map_click, false);

my_map.draw();

var timeout;

timeout = window.setTimeout(fetch_timeout, 50);
