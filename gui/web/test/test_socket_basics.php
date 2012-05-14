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

$sock = new Socket;

$sock->write('hello');
assert($sock->read() === 'hello back!');

$sock->write('quit');
assert($sock->read() === 'good bye');

$sock->close();

print 'done';
