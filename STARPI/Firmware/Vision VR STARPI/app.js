// importando dependencias necesarias
var express = require("express");
var app = new express();

//iniciamos el servidor express
var http = require("http").Server(app);
//iniciamos la libreia socketio
var io = require("socket.io")(http);

// asignacion del puerto
var port = process.env.PORT || 3000;

// ruta para los archivos estaticos a cargar
app.use(express.static(__dirname + "/public" ));

//Redirige a index html porque es archivo estatico
app.get('/', function(req,res){
	res.redirect('index.html');
});

//Escuchar la conexion de un usuario
//io.on escucha cuando hay conexion es que hay un socket y es cliente

io.on('connection',function(socket){

//socket sabe cuando hay petición de stream y manda una imagen, vamos a mandar imágenes por segundo
	socket.on('stream',function(image){
		//Este emite y el otro recive
		socket.broadcast.emit('stream',image);
	});

});
//mensaje al iniciar el servicio
http.listen(port, function(){
	console.log('Servidor escuchando a traves del puerto %s',port);
})