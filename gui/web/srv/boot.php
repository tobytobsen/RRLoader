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

println('Starting server at localhost:8088');
println('Document-Root is: ' . $doc);
println();
println('Do not close this Terminal/Console!');
println(); 

`$bin -S localhost:8088 -t "$doc"`;

// ------------------------

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

function println($msg = '') {
  print $msg . PHP_EOL;
}
