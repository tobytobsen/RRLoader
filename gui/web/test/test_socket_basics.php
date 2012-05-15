<?php

/**
 * Basic Socket Tests
 *
 * NOTE: start the test-server first!
 * -> server.php / server.bat in the `etc` folder.
 */

namespace rrl;

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

// quit
$sock->write('quit');
assert($sock->read() === 'good bye');

// close
$sock->close();

print 'done';
