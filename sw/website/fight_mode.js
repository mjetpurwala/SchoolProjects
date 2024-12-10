/*
   Hacked up by:  Mark McDermott for EE445L Lab 9
   From: various sources
   On:            7/13/24  
*/

// ---------------------------------------
// Global variables
//

var client              = null;
var U_right_local       = "0";    //To be recv from TM4C
var U_left_local        = "0";
var error_right_local   = "0";
var error_left_local    = "0";
var rpm_right_local     = "0";
var rpm_left_local      = "0";

var kp1_val             = "0";    //Extra credit to recv from TM4C
var kp2_val             = "0";
var ki1_val             = "0";
var ki2_val             = "0";
var xstar_val           = "0";


var reconnectTimeout    = 2000;

//  -----   TODO: USER INFO ----------------------------------
var eid                 = "maj3789"
var clientId            = "EE445L_63658";

//  -----   Unity Server  ----------------------------------
// var hostname        = "192.168.0.111";
// var port            = "9001";

//  -----   EER Server	----------------------------------
var hostname            = "10.159.177.113";
var port                = "9001";

// Subscribe (from board_2_webpage (b2w))
var sub_b2w             =  eid + "/b2w";

// Publish (from webpage_2_board)  
var w2b                 =  eid + "/w2b";

// Subscribe (from board_2_webpage)

var left_U_bd =   "mcdermot/b2w/u_left"; 
var right_U_bd  =   "mcdermot/b2w/u_right";
var left_E_bd  =   "mcdermot/b2w/e_left"; 
var right_E_bd  =   "mcdermot/b2w/e_right";
var left_X_bd  =   "mcdermot/b2w/x_left"; 
var right_X_bd  =   "mcdermot/b2w/x_right";


// -----------------------------------------------------------------------------------
// Function that calls itself every second to update the page
function Update_Page() {/*
	document.getElementById("left_U").innerText         = "U(t) = " + U_left_local;
	document.getElementById("right_U").innerText         = "U(t) = " + U_right_local;
	document.getElementById("left_ERR").innerText         = "E(t) = " + error_left_local;
	document.getElementById("right_ERR").innerText         = "E(t) = " + error_right_local;
	document.getElementById("left_RPM").innerText         = "X(t) = " + rpm_left_local;
	document.getElementById("right_RPM").innerText         = "X(t) = " + rpm_right_local;

	//TODO TODO TODO
	document.getElementById("ki1_mirror").innerText         = "Ki1 = " + ki1_val;
	document.getElementById("ki2_mirror").innerText         = "Ki2 = " + ki2_val;
	document.getElementById("kp1_mirror").innerText         = "Kp1 = " + kp1_val;
	document.getElementById("kp2_mirror").innerText         = "Kp2 = " + kp2_val;


	// Set Timer to 1 sec (1000 ms)
	setTimeout(Update_Page, 1000);
	*/
}  
Update_Page();


// -----------------------------------------------------------------------
// ---------------   MQTT Functions   ------------------------------------
// -----------------------------------------------------------------------
//
function connect(){
	// Set up the client

	client = new Paho.MQTT.Client(hostname, Number(port), clientId);
	
	console.info('Connecting to Server: Hostname: ', hostname, 
			    '. Port: ', port, '. Client ID: ', clientId);
    

	// set callback handlers
	client.onConnectionLost = onConnectionLost;
	client.onMessageArrived = onMessageArrived;

	// see client class docs for all the options
	var options = {
		timeout: 3,
        onSuccess: onConnect, // after connected, subscribes
		onFailure: onFail     // useful for logging / debugging
	};
	// connect the client
	client.connect(options);
	console.info('Connecting...');
}

// -----------------------------------------------------------------------------------
//
function onConnect(context) {
	console.log("Client Connected");
    
	// Subscribe to our topics	-- both with the same callback function	
	options = {qos:1, onSuccess:function(context){ console.log("subscribed"); } }
	client.subscribe(sub_b2w,       options);
}

// -----------------------------------------------------------------------------------
//
function onFail(context) {
	console.log("Failed to connect");
	//setTimeout(MQTTconnect, reconnectTimeout) ;
}

// -----------------------------------------------------------------------------------
//
function onConnectionLost(responseObject) {
	if (responseObject.errorCode !== 0) {
		console.log("Connection Lost: " + responseObject.errorMessage);
		window.alert("Lost the websocket!\nRefresh to take it back.");
	}
}

// -----------------------------------------------------------------------
// Function that will be called on press from the UI
function multi_punch(){

	//TODO FIX ME
	
	//Get values from web page
	//var msg = document.forms["kp1_msg"]["kp1_value"].value;
	

	//Create the message object
	message = new Paho.MQTT.Message("A\n");
	message.destinationName = w2b;
	message.retained = false;

	//Send the message
	client.send(message);
	console.log('Message parse:',   message);
}

// -----------------------------------------------------------------------
// Function that will be called on press from the UI
function left_punch(){

	//TODO FIX ME
	
	//Get values from web page
	//var msg = document.forms["kp1_msg"]["kp1_value"].value;
	

	//Create the message object
	message = new Paho.MQTT.Message("B\n");
	message.destinationName = w2b;
	message.retained = false;

	//Send the message
	client.send(message);
	console.log('Message parse:',   message);
}

// -----------------------------------------------------------------------
// Function that will be called on press from the UI
function right_punch(){

	//TODO FIX ME
	
	//Get values from web page
	//var msg = document.forms["kp1_msg"]["kp1_value"].value;
	

	//Create the message object
	message = new Paho.MQTT.Message("C\n");
	message.destinationName = w2b;
	message.retained = false;

	//Send the message
	client.send(message);
	console.log('Message parse:',   message);
}

// -----------------------------------------------------------------------
// Function that will be called on press from the UI
function happy(){

	//TODO FIX ME
	
	//Get values from web page
	//var msg = document.forms["kp1_msg"]["kp1_value"].value;
	

	//Create the message object
	message = new Paho.MQTT.Message("D\n");
	message.destinationName = w2b;
	message.retained = false;

	//Send the message
	client.send(message);
	console.log('Message parse:',   message);
}

// -----------------------------------------------------------------------
// Function that will be called on press from the UI
function sad(){

	//TODO FIX ME
	
	//Get values from web page
	//var msg = document.forms["kp1_msg"]["kp1_value"].value;
	

	//Create the message object
	message = new Paho.MQTT.Message("E\n");
	message.destinationName = w2b;
	message.retained = false;

	//Send the message
	client.send(message);
	console.log('Message parse:',   message);
}

// -----------------------------------------------------------------------
// Function that will be called on press from the UI
function angry(){

	//TODO FIX ME
	
	//Get values from web page
	//var msg = document.forms["kp1_msg"]["kp1_value"].value;
	

	//Create the message object
	message = new Paho.MQTT.Message("F\n");
	message.destinationName = w2b;
	message.retained = false;

	//Send the message
	client.send(message);
	console.log('Message parse:',   message);
}

// -----------------------------------------------------------------------
// Function that will be called on press from the UI
function surprised(){

	//TODO FIX ME
	
	//Get values from web page
	//var msg = document.forms["kp1_msg"]["kp1_value"].value;
	

	//Create the message object
	message = new Paho.MQTT.Message("G\n");
	message.destinationName = w2b;
	message.retained = false;

	//Send the message
	client.send(message);
	console.log('Message parse:',   message);
}

// -----------------------------------------------------------------------
// Function that will be called on press from the UI
function time(){

	//TODO FIX ME
	
	//Get values from web page
	//var msg = document.forms["kp1_msg"]["kp1_value"].value;
	

	//Create the message object
	cur_time = System.currentTimeMillis();
	message = new Paho.MQTT.Message("H:"+cur_time+"\n");
	message.destinationName = w2b;
	message.retained = false;

	//Send the message
	client.send(message);
	console.log('Message parse:',   message);
}

// -----------------------------------------------------------------------
// Function that will be called on press from the UI
function back(){

	//TODO FIX ME
	
	//Get values from web page
	//var msg = document.forms["kp1_msg"]["kp1_value"].value;
	

	//Create the message object
	message = new Paho.MQTT.Message("J\n");
	message.destinationName = w2b;
	message.retained = false;

	//Send the message
	client.send(message);
	console.log('Message parse:',   message);
}



// -----------------------------------------------------------------------
// Function that will be called on press from the UI
function set_kp1(){

	//TODO FIX ME
	
	//Get values from web page
	var msg = document.forms["kp1_msg"]["kp1_value"].value;
	console.log('Message parse:',   msg);

	//Create the message object
	message = new Paho.MQTT.Message("A" + msg);
	message.destinationName = w2b;
	message.retained = false;

	//Send the message
	client.send(message);
}
// -----------------------------------------------------------------------------------
//
function set_kp2(){
	//TODO TODO TODO
	var msg = document.forms["kp2_msg"]["kp2_value"].value;
	console.log('Message parse:',   msg);

	//Create the message object
	message = new Paho.MQTT.Message("B" + msg);
	message.destinationName = w2b;
	message.retained = false;

	//Send the message
	client.send(message);
}

// -----------------------------------------------------------------------------------
//
function set_ki1(){
	//TODO TODO TODO
	//TODO TODO TODO
	var msg = document.forms["ki1_msg"]["ki1_value"].value;
	console.log('Message parse:',   msg);

	//Create the message object
	message = new Paho.MQTT.Message("C" + msg);
	message.destinationName = w2b;
	message.retained = false;

	//Send the message
	client.send(message);
}

// -----------------------------------------------------------------------------------
//
function set_ki2(){
	//TODO TODO TODO
	//TODO TODO TODO
	var msg = document.forms["ki2_msg"]["ki2_value"].value;
	console.log('Message parse:',   msg);

	//Create the message object
	message = new Paho.MQTT.Message("D" + msg);
	message.destinationName = w2b;
	message.retained = false;

	//Send the message
	client.send(message);
}

// -----------------------------------------------------------------------------------
//
function set_speed(){
	//TODO TODO TODO
	//TODO TODO TODO
	var msg = document.forms["speed_msg"]["speed_value"].value;
	console.log('Message parse:',   msg);

	//Create the message object
	message = new Paho.MQTT.Message("E" + msg);
	message.destinationName = w2b;
	message.retained = false;

	//Send the message
	client.send(message);
}


// -----------------------------------------------------------------------------------
// This function uses JSON.parse to extract data from the MQTT websocket payload.
// The TM4C composes the JSON packet and sends it to the ESP8266 which transmits
// it to this routine.
//
function onMessageArrived(message) {
	console.log( "Incoming Message",  message.destinationName, message.payloadString);

	/*
	// Check that the message is sent to the right Topic
	// Should always be true since we only subscribed to one topic
	if (message.destinationName == sub_b2w){

		//The TM4C sent all the data in a JSON format to the topic
		const obj = JSON.parse(message.payloadString);

		//Take the data labeled A in the JSON object and assign it to U_left_local
		U_left_local           =  eval(obj.A); 
		U_right_local           =  eval(obj.B); 
		error_left_local           =  eval(obj.C); 
		error_right_local           =  eval(obj.D); 
		rpm_left_local			= eval(obj.E);
		rpm_right_local           =  eval(obj.F); 
		kp1_val = eval(obj.G);
		kp2_val = eval(obj.H);
		ki1_val = eval(obj.I);
		ki2_val = eval(obj.J);
		xstar_val = eval(obj.K);
	
		//TODO TODO TODO

		console.log("U_left:\t", U_left_local);
		console.log("U_right:\t", U_right_local);
		console.log("E_left:\t", error_left_local);
		console.log("E_right:\t", error_right_local);
		console.log("X_left:\t", rpm_left_local);
		console.log("X_right:\t", rpm_right_local);

		Update_Page();
		
	} 
		*/
	
}



