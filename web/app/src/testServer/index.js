import Koa from 'koa';
import IO from 'koa-socket';

import immutableObjectPath from 'object-path-immutable'
import protobufs from "../utils/protoBufLoader";

const app = new Koa();
const io = new IO();

io.attach(app);



let shifterState = {
  modes: {
    current: "Sockets work",
    light: {
      name: "Am Anfang war das Licht",
      subMode: protobufs.MS4.Modes.Light.LightMode.NORMAL,
      color: {
        R: 100, G: 200, B: 300
      }
    }
  },
}

io.on('getState', ctx => {
  console.log('[MS3K] getState')
  io.broadcast('ms3ks.complete', { shifterState });
})

io.on('setState', (ctx, { newState }) => {
  console.log('[MS3K] setState', newState)
  shifterState = newState
})

io.on('setSubState', (ctx, { path, newState }) => {
  shifterState = immutableObjectPath.set(obj, path)
  console.log('[MS3K] setSubState', path, newState, shifterState)
})

io.on('getSubState', (ctx, { path }) => {
  console.log('[MS3K] getSubState', path)
  shifterState = immutableObjectPath.set(obj, path)
})






io.on('connection', ctx => {
  console.log('[server] connected');
});

let usernames = [];
io.on('disconnect', ctx => {
  const { username } = ctx.socket;
  if (username) {
    console.log(`[server] disconnected: ${username}`);
    usernames = usernames.filter(u => u !== username)
  }
});

io.on('login', (ctx, { username }) => {
  console.log(`[server] login: ${username}`);
  usernames.push(username);
  ctx.socket.username = username;

  io.broadcast('users.login', { username });
});

io.on('logout', ctx => {
  const { username } = ctx.socket;
  if (username) {
    console.log(`[server] logout: ${username}`);
    usernames = usernames.filter(u => u !== username)
    delete ctx.socket['username'];

    io.broadcast('users.logout', { username });
  }
});

let messages = [];
io.on('message', (ctx, { text }) => {
  console.log(`[server] message: ${text}`);
  const message = {
    id: messages.length,
    text,
    username: ctx.socket.username,
  };
  messages.push(message);

  io.broadcast('messages.new', { message });
});

app.listen(8123, () => {
  console.log('[server] ready');
});
