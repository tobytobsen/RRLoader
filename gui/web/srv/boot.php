#!/usr/bin/php
<?php

namespace rrl;

print <<<END_HEADER
   ___  ___  __               __           _      __     __  __  __ ____
  / _ \/ _ \/ /  ___ ___ ____/ /___ ____  | | /| / /___ / / / / / //  _/
 / , _/ , _/ /__/ _ | _ `/ _  // -_) __/  | |/ |/ // -_) _ | /_/ /_/ /  
/_/|_/_/|_/____/\___|_,_/\_,_/ \__/_/     |__/|__/ \__/_.__|____//___/  
                                                                        

END_HEADER;
  
if (version_compare(PHP_VERSION , '5.4.0', '>=') == 0)
  exit('You need at least PHP 5.4.0');
  
if (PHP_OS != 'WINNT')
  exit('WIP');

if (!($bin = find_php())) 
  exit('Please add PHP-CLI to your PATH variable!');
 
$doc = realpath(__DIR__ . '/..');

list ($host, $port) = get_args();

if (!$host) 
  $host = 'localhost';

if (!$port || $port <= 1023)
  $port = 8088;

print <<<END_INFO
Starting server at $host:$port
Document-Root is: $doc

Do not close this Terminal/Console!


END_INFO;

`$bin -S $host:$port -t "$doc"`;

// ------------------------

function get_args() {
  if (!isset($_SERVER['argv'][1]))
    return [null, null];
    
  $argv = $_SERVER['argv'][1];
  
  if ($argv === '?')
    exit('usage: boot [ <host>:<port> | <host> | :<port> ]' . PHP_EOL);
  
  if (strpos($argv, ']:')) {
    list ($host, $port) = explode(']:', $argv, 2);
    return [$host . ']', (int) $port];
  }
  
  if (($p = strpos($argv, ':')) === false)
    return [$argv, null];
    
  if ($p === 0)
    return [null, (int) substr($argv, 1)];
    
  $host = substr($argv, 0, $p);
  $port = substr($argv, $p + 1);
  
  return [$host, (int) $port];
}

function find_php() {
  $res = `php -v`;
  
  if (stristr($res, 'PHP 5')) 
    return 'php';
  
  foreach (['php', 'xampp/php', 'wamp/php'] as $p)
    foreach (['C', 'D', 'E', 'F'] as $h)
      if (file_exists($path = $h . ':/' . $p . '/php.exe'))
        return $path;
  
  return null;
}
