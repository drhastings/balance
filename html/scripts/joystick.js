var pad_radius = 40;

var id;

var sticks = new Array;

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

			this.x_value = this.finger.clientX;

			this.y_value = this.finger.clientY;

			this.boundaries();
		}
	}

	this.canvas.addEventListener('touchstart', start.bind(this), 0);

	function end(event) {
		if (this.finger.identifier == event.changedTouches[0].identifier)
		{
			this.x_value = this.canvas.width / 2;

			this.y_value = this.canvas.height / 2;

			this.finger = null;
		}
	}

	this.canvas.addEventListener('touchend', end.bind(this), 0);

	function move(event) {
		if (this.finger.identifier == event.changedTouches[0].identifier);
		{
			this.x_value = this.finger.clientX;

			this.y_value = this.finger.clientY;

			this.boundaries();
		}
	}

	this.canvas.addEventListener('touchmove', move.bind(this), 0);
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

sticks[0] = new joystick(1,1,1,1);

sticks[0].draw();

function doOnOrientationChange()
{
  switch(window.orientation) 
  {  
    case -90:
    case 90:
			clearTimeout(id);
			for (x = 0; x < sticks.length; x++)
			{
				sticks[x].canvas.parentNode.removeChild(sticks[x].canvas);
			}	
 			sticks = [];
			sticks[0] = new joystick(window.innerWidth / 2, window.innerHeight, 1,1,0,0);
			sticks[0].draw();
 			sticks[0].canvas.style.position="absolute";
			sticks[0].canvas.style.top="0px";
			sticks[0].canvas.style.left="0px";
			sticks[0].canvas.style.width="50%";
			sticks[0].canvas.style.height="100%";
			sticks[0].canvas.style.background="black";
 			sticks[1] = new joystick(window.innerWidth / 2, window.innerHeight, 0,0,1,1);
			sticks[1].draw();
 			sticks[1].canvas.style.position="absolute";
			sticks[1].canvas.style.top="0px";
			sticks[1].canvas.style.left="50%";
			sticks[1].canvas.style.width="50%";
			sticks[1].canvas.style.height="100%";
			sticks[1].canvas.style.background="black";
			id = window.setTimeout(send_timeout, 100);
      break; 
    default:
			clearTimeout(id);
			for (x = 0; x < sticks.length; x++)
			{
				sticks[x].canvas.parentNode.removeChild(sticks[x].canvas);
			}		sticks = [];
			sticks[0] = new joystick(window.innerWidth,window.innerHeight,1,1,1,1);
			sticks[0].draw();
 			sticks[0].canvas.style.position="absolute";
			sticks[0].canvas.style.top="0px";
			sticks[0].canvas.style.left="0px";
			sticks[0].canvas.style.width="100%";
			sticks[0].canvas.style.height="100%";
			sticks[0].canvas.style.background="black";
			id = window.setTimeout(send_timeout, 100);
      break; 
  }
}

window.addEventListener('orientationchange', doOnOrientationChange);

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

	if (!window.orientation)
	{
		var x = sticks[0].getX().toFixed(5);
		var y = sticks[0].getY().toFixed(5);

		if (x == 0 && y == 0) follow = "";

		var message = spin.concat(x, speed, y, follow);
		xmlhttp.send(message); 
	}
	else
	{
		var x = sticks[0].getX().toFixed(5);
		var y = sticks[1].getY().toFixed(5);

		if (x == 0 && y == 0) follow = "";

		xmlhttp.send(spin.concat(x, speed, y, follow)); 
	}
}

function send_timeout()
{
	send_data();

	id = window.setTimeout(send_timeout, 100);
}
  // Initial execution if needed
doOnOrientationChange();
//requestAnimationFrame(stick.draw);
