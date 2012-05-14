<?php

/**
 * Simple socket API for the RRLoader Daemon
 * Copyright 2012 - droptable/murdoc <murdoc@raidrush.org>
 */
 
namespace rrl;

use \Exception;
use \ErrorException;
use \InvalidArgumentException;

class Socket 
{
  const DEFAULT_HOST = 'localhost',
        DEFAULT_PORT = 1234 // WIP
        
  protected $sock, $conf;
  
  // @throws \InvalidArgumentException
  public function __construct(array $conf = [])
  {
    $conf += [ 'host' => self::DEFAULT_HOST, 'port' => self::DEFAULT_PORT ];
    
    if (!is_string($conf['host']) || empty($conf['host'])
     || !is_integer($conf['port']) || $conf['port'] <= 1023) {
      throw new InvalidArgumentException(
        '`host` must not be empty and `port` must be of type integer > 1023. '
        . 'if you don\'t know the correct values for these variables, '
        . 'try to use the built-in default values.'
      );
    }
    
    $this->conf = &$conf;
    
    // -------------------------
    
    $this->connect();
  }
  
  // @throws \ErrorException
  protected function connect()
  {
    // ...
  }
}

// goal:
// $api = new Socket;
// print $api->send('hello')->read_sync(4096); # something
