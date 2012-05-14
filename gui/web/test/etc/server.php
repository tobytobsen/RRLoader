#!/usr/bin/php
<?php

/**
 * simple server to test the socket-lib
 * 
 * note: this server is a single-process server
 * response can take up to 1 second.
 */

/*

S = SERVER
C = CLIENT

BEGIN
  S accepts C 
  C writes something (input) to S

  CASE "hello" as input
    S writes "hello back" to C
  
  CASE "quit" as input
    S writes "good bye" to C
    S closes connection to C
    
  DEFAULT
    S writes "noop" to C
END

*/ 
 
set_time_limit(0);

$srv = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);

socket_bind($srv, 'localhost', 50100);
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
    $cpl[] = $clt;
  }
  
  for ($i = 0, $l = count($cpl); $i < $l; ++$i) {      
    $r = [ $cpl[$i] ];
    $w = [];
    $e = [];
    
    if (socket_select($r, $w, $e, 0) != 1)
      continue;
    
    $buf = socket_read($cpl[$i], 4096, PHP_BINARY_READ);
    
    if (false === $buf || $buf === '') {
      @socket_close($cpl[$i]);    
      array_splice($cpl, $i, 1); 
      
      print '[ connection lost ]' . PHP_EOL;
      break;
    }
    
    print '-> got input from client: "' . $buf . '"' . PHP_EOL;
    
    $r = [];
    $w = [ $cpl[$i] ];
    $e = [];
    
    if (socket_select($r, $w, $e, 0) != 1)
      continue;
    
    print '-> writing response to client ... ' . PHP_EOL;
    
    switch ($buf) {
      case 'hello':
        socket_write($cpl[$i], 'hello back!', 11);
        break;
        
      case 'quit':
        socket_write($cpl[$i], 'good bye', 8);
        socket_close($cpl[$i]);
        array_splice($cpl, $i, 1);
        
        print '[ connection closed ]' . PHP_EOL;
        break 2;
        
      default:
        socket_write($cpl[$i], 'noop', 4);
    }
  }
  
  sleep(1);
}
