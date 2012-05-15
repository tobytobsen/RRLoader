<?php

/**
 * Basic Socket Tests
 *
 * NOTE: start the test-server first!
 * -> server.php / server.bat in the `etc` folder.
 */

namespace rrl;

set_time_limit(0);
error_reporting(E_ALL | E_STRICT | E_DEPRECATED);
header('Content-Type: text/plain; Charset=UTF-8');

require '../lib/socket.php';

$sock = new Socket([ 'host' => 'localhost', 'port' => 50100 ]);

// write -> read
$sock->write('hello');
assert($sock->read() === 'hello back!');

// read bytes
$sock->write('hello');
assert($sock->read(5) === 'hello');
assert($sock->read() === ' back!');

// request more bytes as available
$sock->write('hello');
assert($sock->read(255) === 'hello back!');

// read specific ammount of bytes
$sock->write('bytes-short');
assert($sock->read_bytes('s')[1] === 1337);

$sock->write('bytes-string');
$len = $sock->read_bytes('s')[1];
$str = $sock->read_bytes($len, 'a*')[1];
assert($len === 4);
assert($str === 'test');

$sock->write('bytes-string');
$data = unpack('s1/a*2', $sock->read_bytes(6));
assert($data[1] === 4);
assert($data[2] === 'test');

// quit
$sock->write('quit');
assert($sock->read() === 'good bye');

// close
$sock->close();

print 'done';
