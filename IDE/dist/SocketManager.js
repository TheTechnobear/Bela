"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var io = require("socket.io");
// all connected sockets
var ide_sockets;
function init(server) {
    ide_sockets = io(server).of('/IDE');
    ide_sockets.on('connection', connection);
}
exports.init = init;
function connection(socket) {
    init_message(socket);
}
/*async function init_message(socket: SocketIO.Socket){
    let message: Init_Message;
    message.projects = await project_manager.listProjects();
    message.examples = await project_manager.listExamples();
//	message.settings = await ide_settings.read();
//	message.boot_project = await IDE.boot_project();
//	message.xenomai_version = await IDE.xenomai_version();
//	message.status = await process_manager.status();
    console.log('init message');
    console.dir(message, {depth:null});
    socket.emit('init', message);
}*/

//# sourceMappingURL=data:application/json;charset=utf8;base64,eyJ2ZXJzaW9uIjozLCJzb3VyY2VzIjpbIlNvY2tldE1hbmFnZXIudHMiXSwibmFtZXMiOltdLCJtYXBwaW5ncyI6Ijs7QUFBQSw4QkFBZ0M7QUFJaEMsd0JBQXdCO0FBQ3hCLElBQUksV0FBK0IsQ0FBQztBQUVwQyxjQUFxQixNQUFtQjtJQUN2QyxXQUFXLEdBQUcsRUFBRSxDQUFDLE1BQU0sQ0FBQyxDQUFDLEVBQUUsQ0FBQyxNQUFNLENBQUMsQ0FBQztJQUNwQyxXQUFXLENBQUMsRUFBRSxDQUFDLFlBQVksRUFBRSxVQUFVLENBQUMsQ0FBQztBQUMxQyxDQUFDO0FBSEQsb0JBR0M7QUFFRCxvQkFBb0IsTUFBdUI7SUFDMUMsWUFBWSxDQUFDLE1BQU0sQ0FBQyxDQUFDO0FBQ3RCLENBQUM7QUFFRDs7Ozs7Ozs7Ozs7R0FXRyIsImZpbGUiOiJTb2NrZXRNYW5hZ2VyLmpzIiwic291cmNlc0NvbnRlbnQiOlsiaW1wb3J0ICogYXMgaW8gZnJvbSAnc29ja2V0LmlvJztcbmltcG9ydCAqIGFzIGh0dHAgZnJvbSAnaHR0cCc7XG5pbXBvcnQgKiBhcyBwcm9qZWN0X21hbmFnZXIgZnJvbSAnLi9Qcm9qZWN0TWFuYWdlcic7XG5cbi8vIGFsbCBjb25uZWN0ZWQgc29ja2V0c1xubGV0IGlkZV9zb2NrZXRzOiBTb2NrZXRJTy5OYW1lc3BhY2U7XG5cbmV4cG9ydCBmdW5jdGlvbiBpbml0KHNlcnZlcjogaHR0cC5TZXJ2ZXIpe1xuXHRpZGVfc29ja2V0cyA9IGlvKHNlcnZlcikub2YoJy9JREUnKTtcblx0aWRlX3NvY2tldHMub24oJ2Nvbm5lY3Rpb24nLCBjb25uZWN0aW9uKTtcbn1cblxuZnVuY3Rpb24gY29ubmVjdGlvbihzb2NrZXQ6IFNvY2tldElPLlNvY2tldCl7XG5cdGluaXRfbWVzc2FnZShzb2NrZXQpO1xufVxuXG4vKmFzeW5jIGZ1bmN0aW9uIGluaXRfbWVzc2FnZShzb2NrZXQ6IFNvY2tldElPLlNvY2tldCl7XG5cdGxldCBtZXNzYWdlOiBJbml0X01lc3NhZ2U7XG5cdG1lc3NhZ2UucHJvamVjdHMgPSBhd2FpdCBwcm9qZWN0X21hbmFnZXIubGlzdFByb2plY3RzKCk7XG5cdG1lc3NhZ2UuZXhhbXBsZXMgPSBhd2FpdCBwcm9qZWN0X21hbmFnZXIubGlzdEV4YW1wbGVzKCk7XG4vL1x0bWVzc2FnZS5zZXR0aW5ncyA9IGF3YWl0IGlkZV9zZXR0aW5ncy5yZWFkKCk7XG4vL1x0bWVzc2FnZS5ib290X3Byb2plY3QgPSBhd2FpdCBJREUuYm9vdF9wcm9qZWN0KCk7XG4vL1x0bWVzc2FnZS54ZW5vbWFpX3ZlcnNpb24gPSBhd2FpdCBJREUueGVub21haV92ZXJzaW9uKCk7XG4vL1x0bWVzc2FnZS5zdGF0dXMgPSBhd2FpdCBwcm9jZXNzX21hbmFnZXIuc3RhdHVzKCk7XG5cdGNvbnNvbGUubG9nKCdpbml0IG1lc3NhZ2UnKTtcblx0Y29uc29sZS5kaXIobWVzc2FnZSwge2RlcHRoOm51bGx9KTtcblx0c29ja2V0LmVtaXQoJ2luaXQnLCBtZXNzYWdlKTtcbn0qL1xuIl19