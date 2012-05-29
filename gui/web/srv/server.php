#!/usr/bin/php
<?php

/**
 * This is a WebSocket Server
 * Copyright 2012 droptable/murdoc <murdoc@raidrush.org>
 */
 
namespace rrl;
 
// RFC 6455 Support (IETF Draft 17)
// Gecko 11 / Chrome 16

$srv = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
socket_bind($srv, 'localhost', 8089);
socket_listen($srv);
socket_set_nonblock($srv);

for ($cpl = [];;) {
  $r = [ $srv ];
  $w = [ $srv ];
  $e = [ $srv ];

  if (socket_select($r, $w, $e, 0) === 1
   && ($clt = socket_accept($srv)) !== false) {
    socket_getpeername($clt, $ip);
    
    print '[ new connection: ' . $ip . ' ]' . PHP_EOL;
    handshake($clt);
    
    $cpl[] = $clt;
  }
  
  foreach ($cpl as $i => $clt) {      
    $r = [ $clt ];
    $w = [];
    $e = [];
    
    if (socket_select($r, $w, $e, 0) != 1)
      continue;
    
    $buf = socket_read($clt, 4096, PHP_BINARY_READ);
    
    if (false === $buf || $buf === '') {
      @socket_close($clt);    
      array_splice($cpl, $i, 1); 
      
      print '[ connection lost ]' . PHP_EOL;
      continue;
    }
    
    print '-> got input from client: "' . $buf . '"' . PHP_EOL;
    
    $r = [];
    $w = [ $clt ];
    $e = [];
    
    if (socket_select($r, $w, $e, 0) != 1)
      continue;
    
    print '-> writing response to client ... ' . PHP_EOL;
    
    switch ($buf) {
      case 'hello':
        socket_write($clt, 'hello!', 5);
        break;
    }
  }
  
  usleep(500000);
  # print '[ wakeup ]' . PHP_EOL;
}

// ----------------------

# GET / HTTP/1.1
# Host: localhost:8089
# DNT: 1
# Connection: keep-alive, Upgrade
# Sec-WebSocket-Version: 13
# Origin: null
# Sec-WebSocket-Key: eyZVr2W0VG60O4DhiOMNDw==
# Pragma: no-cache
# Cache-Control: no-cache
# Upgrade: websocket

function handshake($clt) {
  $req = socket_read($clt, 4096, PHP_BINARY_READ);
  // 258EAFA5-E914-47DA-95CA-C5AB0DC85B11

  if (!preg_match('/^Sec-WebSocket-Key: (.*)\r$/m', $req, $key))
    return;
    
  $key = $key[1] . '258EAFA5-E914-47DA-95CA-C5AB0DC85B11';
  $key = base64_encode(sha1($key));

  $buf = "HTTP/1.1 101 Switching Protocols\r\n"
    . "Upgrade: websocket\r\n"
    . "Connection: Upgrade\r\n"
    . "Sec-WebSocket-Accept: $key\r\n"
    . "Sec-WebSocket-Protocol: rrl\r\n\r\n";
    
  socket_write($clt, $buf, strlen($buf));
}

