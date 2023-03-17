var coap = require('coap');

function randomInt(min,max) {
return (Math.floor(Math.random()*(max-min) + min));
}

var portNumber=5683;
coap.createServer(function (req,res) {
console.info('CoAP device got a request from %s', req.url);
if(req.headers['Accept'] != 'application/json') {
res.code='4.06';
return res.end();
}

switch(req.url) {
case "/aqi":
displayOutput(res, {'AQI(Air Quality Index)':randomInt(0,1000)});
break;
case "/temperature":
displayOutput(res, {'Temperature':randomInt(-10,50)});
break;
case "/humidity":
displayOutput(res, {'Humidity':randomInt(0,100)});
break;
default:
displayOutput(res);
}
}).listen(portNumber);
console.log('CoAP Server is started at port Number 5683');

function displayOutput (res,content) {
if(content) {
res.setOption('Content-Format','application/json');
res.code='2.05';
res.end (JSON.stringify(content));
} else {
res.code='4.04';
res.end();
}
}