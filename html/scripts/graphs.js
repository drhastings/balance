var id;

var sticks = new Array;

function graph (width, height, variable)
{
	this.canvas = document.createElement("canvas");

	document.body.appendChild(this.canvas);

	this.canvas.width = width;

	this.canvas.height = height;

	this.history = new Array;

	this.lookback = 5000;

	for (x = 5000; x >= 0; x -= 250)
	{
		this.history.push(new Array(0, new Date() - new Date(x)));
	}
	
	this.border = 40;
}

graph.prototype.min = function()
{
	var length = this.history.length;

	var min = this.history[0][0];

	for (x = 0; x < length; x++)
	{
		if (this.history[x][0] < min)
		{
			min = this.history[x][0];
		}
	}

	return min;
}

graph.prototype.max = function()
{
	var length = this.history.length;

	var max = this.history[0][0];

	for (x = 0; x < length; x++)
	{
		if (this.history[x][0] > max)
		{
			max = this.history[x][0];
		}
	}

	return max;
}	

graph.prototype.draw = function()
{
	var inside_w = this.canvas.width - (this.border * 2);
	var inside_h = this.canvas.height - (this.border * 2);

	var max = this.max();
	var min = this.min();

	if (min == max)
	{
		max++;
		min--;
	}

	var gap = max - min;

	var y_exp = Math.log(gap) / Math.LN10;
	var x_exp = Math.log(this.lookback) / Math.LN10;

	y_exp = Math.floor(y_exp);
	x_exp = Math.floor(x_exp);

	var y_exp_factor = Math.pow(10, y_exp);
	var x_exp_factor = Math.pow(10, x_exp);

	if (max > 0)
	{
		max = Math.ceil(max / y_exp_factor) * y_exp_factor;
	}
	else
	{
		max = Math.floor(max / y_exp_factor) * y_exp_factor;
	}

	if (min > 0)
	{
		min = Math.ceil(min / y_exp_factor) * y_exp_factor;
	}
	else
	{
		min = Math.floor(min / y_exp_factor) * y_exp_factor;
	}

	gap = max - min;

	var num_y_ticks = gap / y_exp_factor;
	var num_x_ticks = this.lookback / x_exp_factor;

	var y_tick_dist = inside_h / num_y_ticks;
	var x_tick_dist = inside_w / num_x_ticks;

	num_y_ticks++;

	var middle = (max + min) / 2;

	var x_scale = inside_w / this.lookback;

	var y_scale = (inside_h / gap) / 2;

	while (new Date() - this.history[0][1] > this.lookback)
	{
		this.history.shift();
	}

	this.context = this.canvas.getContext("2d");

	this.context.clearRect(0, 0, this.canvas.width, this.canvas.height);

	this.context.beginPath();

	this.context.moveTo(this.border + .5, this.border + .5);

	this.context.lineTo(this.border + .5, this.canvas.height - this.border + .5);

	for (x = 0; x < num_y_ticks; x++)
	{
		this.context.moveTo(this.border - 2, this.border + y_tick_dist * x + .5);
		this.context.lineTo(this.border + 3, this.border + y_tick_dist * x + .5);

		var out_number = max - x * y_exp_factor;

		if (y_exp < 0)
		{
			out_number = (max - x * y_exp_factor).toFixed(-y_exp);
		}

		this.context.fillText(out_number, 2, this.border + y_tick_dist * x + 3);
	}

	for (x = 0; x < num_x_ticks; x++)
	{
		this.context.moveTo(this.border + inside_w - x * x_tick_dist + .5,
			this.border + inside_h - 2);
		this.context.lineTo(this.border + inside_w - x * x_tick_dist + .5,
			this.border + inside_h + 3);

		var out_number = (this.lookback - x * x_exp_factor) / 1000;

		if (x_exp < 0)
		{
			out_number = ((this.lookback - x * x_exp_factor) / 1000).toFixed(-x_exp);
		}

		this.context.fillText(out_number, this.border + inside_w - x * x_tick_dist
			- 4, this.border + inside_h + 10);
	}

	this.context.moveTo(this.border, inside_h + this.border + .5);
	this.context.lineTo(this.border + inside_w, inside_h + this.border + .5);

	this.context.strokeStyle = "black";

	this.context.stroke();

	this.context.beginPath();

	var length = this.history.length;

	var now = new Date();

	this.context.moveTo((this.lookback - (now - this.history[0][1])) * x_scale
			+ this.border, ((-this.history[0][0] + middle) * y_scale)
				+ this.border + (inside_h / 2));

	for (x = 1; x < length; x++)
	{
		this.context.lineTo((this.lookback - (now - this.history[x][1])) * x_scale
			+ this.border, ((-this.history[x][0] + middle) * y_scale)
				+ this.border + (inside_h / 2));
	}

	this.context.strokeStyle = "red";

	this.context.stroke();
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

			var now = new Date();

			graphs[0].history.push(new Array(parseFloat(values[0]), now));
			graphs[1].history.push(new Array(parseFloat(values[1]), now));
		}
  }
	xmlhttp.open("GET", "/graph_output.html",true);
	xmlhttp.send();
}

var graphs = new Array;

graphs.push(new graph(500, 280));
graphs.push(new graph(500, 280));

function layoutGraphs(graphs)
{
	var height = 30;

	if (graphs.length > 0)
	{
		var length = graphs.length;

		for (x = 0; x < length; x++)
		{
			graphs[x].canvas.style.position = "absolute";
			graphs[x].canvas.style.position = "absolute";
			graphs[x].canvas.style.left = "30px";
			graphs[x].canvas.style.top = height.toString().concat("px");
			graphs[x].canvas.style.height = graphs[0].canvas.height.toString().concat("px");
			graphs[x].canvas.style.width = graphs[0].canvas.width.toString().concat("px");
			graphs[x].canvas.style.background = "grey";

			height += 30 + graphs[x].canvas.height;
		}
	}
}

function draw()
{
	var length = graphs.length;

	for (i = 0; i < length; i++)
	{
		graphs[i].draw();
	}

	window.webkitRequestAnimationFrame(draw);
}

layoutGraphs(graphs);

timeout = window.setTimeout(fetch_timeout, 50);

draw();
