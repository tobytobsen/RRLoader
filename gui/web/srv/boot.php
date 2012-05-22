#!/usr/bin/php
<?php

/**
 * RRLoader WebUI Standalone Server
 * Copyright 2012 - droptable/murdoc <murdoc@raidrush.org>
 */

namespace rrl;

print <<<END_HEADER
   ___  ___  __               __           _      __     __  __  __ ____
  / _ \/ _ \/ /  ___ ___ ____/ /___ ____  | | /| / /___ / / / / / //  _/
 / , _/ , _/ /__/ _ | _ `/ _  // -_) __/  | |/ |/ // -_) _ | /_/ /_/ /  
/_/|_/_/|_/____/\___|_,_/\_,_/ \__/_/     |__/|__/ \__/_.__|____//___/  
                                                                        

END_HEADER;

$ini = __DIR__ . '/boot.ini';
$ini = file_exists($ini) ? parse_ini_file($ini, true) : [];

$bin = find_php($ini);
$doc = realpath(__DIR__ . '/..');

list ($host, $port) = get_args($ini);
  
if (!$host) $host = 'localhost';
if (!$port || $port <= 1023) $port = 8088;
  
print <<<END_INFO


Starting Webserver at $host:$port
Document-Root is: $doc

You can now access the WebUI via:
http://$host:$port/

Do not close this Terminal/Console!


END_INFO;

`$bin -S $host:$port -t "$doc"`;

// ------------------------

function get_args(array &$ini) {
  $host_ini = !empty($ini['WEBUI']['host']) ? $ini['WEBUI']['host'] : null;
  $port_ini = !empty($ini['WEBUI']['port']) ? $ini['WEBUI']['port'] : null;
  
  if (!isset($_SERVER['argv'][1]))
    return [$host_ini, (int) $port_ini];
    
  $argv = $_SERVER['argv'][1];
  
  if ($argv === '?')
    exit('usage: boot [ <host>:<port> | <host> | :<port> ]' . PHP_EOL);
  
  if (strpos($argv, ']:')) {
    list ($host, $port) = explode(']:', $argv, 2);
    return [substr($host, 1), ((int) $port) ?: $port_ini];
  }
  
  if (($p = strpos($argv, ':')) === false)
    return [$argv, (int) $port_ini];
    
  if ($p === 0) // :<port> notation
    return [$host_ini, ((int) substr($argv, 1)) ?: $port_ini];
    
  $host = substr($argv, 0, $p);
  $port = substr($argv, $p + 1);
  
  return [$host, ((int) $port) ?: $port_ini];
}

function check_cli($cli) {
  $err = (PHP_OS === 'WINNT') ? '2>nul' : '2>/dev/null';
  $res = `$cli -v $err`;
  
  if (!preg_match('/PHP (\d+(?:\.\d+)*)/', $res, $match))
    return false;
  
  return version_compare($match[1], '5.4') > -1;
}

function find_php(array &$ini) {
  // check ini first
  if (!empty($ini['PHP_CLI']['path'])) {
    $cli = $ini['PHP_CLI']['path'];
    if (check_cli($cli)) return $cli;
  }
  
  foreach (['php', $_SERVER['PHP_CLI']] as $path)
    if (check_cli($path)) goto cli_found;
  
  print 'Missing PHP-CLI (>= 5.4), please give us a path to your PHP install-folder:'
    . PHP_EOL . '> ';
  
  $dirs = DIRECTORY_SEPARATOR;
  $path = str_replace(array('\\', '/'), $dirs, trim(fgets(STDIN)));
  
  print PHP_EOL;
  
  if (substr($path, -8) !== ($bin = "{$dirs}php.exe"))
    $path .= $bin;   

  // remove unecessary slashes
  $path = preg_replace('/(?:' . preg_quote($dirs, '/') . '){2,}/', $dirs, $path);
  
  if (file_exists($path) && check_cli($path)) 
    goto cli_found;
   
  exit('PHP-CLI was not found on your System, please add it to your PATH Variable!');
  
  cli_found:
  alter_cli_path($ini, $path);
  
  return $path;
}

function alter_cli_path(array &$ini, $value) {
  $s = 'PHP_CLI';
  $n = 'path';
  
  if (!isset($ini[$s]) || !is_array($ini[$s]))
    $ini[$s] = [];
    
  $ini[$s][$n] = $value;
  
  // replace value in file and save it
  $path = __DIR__ . DIRECTORY_SEPARATOR . 'boot.ini';
  $sect = false;
  $done = false;
  $buff = '';
  
  ini_set('auto_detect_line_endings', true);
  
  foreach (file($path, FILE_IGNORE_NEW_LINES) as $line) {
    if ($sect === true && preg_match('/^path\s*=/', $line)) {
      $buff .= 'path = ' . $value . PHP_EOL;
      $done = true;
      continue;
    }
    
    $buff .= $line . PHP_EOL;
    
    if (!$done && !$sect)
      $sect = trim($line) === '[PHP_CLI]';
  } 
  
  file_put_contents($path, $buff);
}

